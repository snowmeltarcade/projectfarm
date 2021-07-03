#include <algorithm>

#include "label.h"
#include "graphics/graphics.h"

using namespace std::literals;

namespace projectfarm::graphics::ui
{
    bool Label::SetText(std::string_view text,
                        bool forceUpdate) noexcept
    {
        if (!forceUpdate && text == this->_text)
        {
            return true;
        }

        this->Destroy();

        if (text.empty())
        {
            this->_renderDetails = {};
        }
        else
        {
            auto font = this->_fontManager->GetFont(this->_style->Font);
            if (!font)
            {
                this->LogMessage("Failed to get font: " + this->_style->Font);
                return false;
            }

            SDL_Color sdlColor { this->_style->Color.r,
                                 this->_style->Color.g,
                                 this->_style->Color.b,
                                 this->_style->Color.a };

            auto [surface, renderDetails] = font->RenderToSurface(text,
                                                                  sdlColor,
                                                                  this->_size.GetWidth(),
                                                                  this->_characterOverride,
                                                                  this->_useMarkdown,
                                                                  this->_maxLines);
            if (!surface)
            {
                this->LogMessage("Failed to render text to surface.");
                return false;
            }

            this->_backgroundTexture = std::make_shared<graphics::Texture>();
            this->_backgroundTexture->SetLogger(this->_logger);
            this->_backgroundTexture->SetGraphics(this->GetGraphics());

            this->_backgroundTexture->SetRenderToWorldSpace(false);

            if (!this->_backgroundTexture->LoadFromSurface(surface))
            {
                this->LogMessage("Failed to load from surface.");
                return false;
            }

            this->_renderDetails = renderDetails;

            // the width will never exceed this control due to max width above,
            // so we only need to fit if the height is too big
            if (this->_fitType == FitTypes::Fit)
            {
                auto textureHeight = this->_backgroundTexture->GetTextureHeight();
                auto controlHeight = this->GetSize().GetHeight();

                if (textureHeight > controlHeight)
                {
                    auto delta = textureHeight / controlHeight;

                    this->_backgroundTexture->SetTextureWidth(this->_backgroundTexture->GetTextureWidth() / delta);
                    this->_backgroundTexture->SetTextureHeight(controlHeight);
                }
            }
            else if (this->_fitType == FitTypes::Fill)
            {
                this->_backgroundTexture->SetTextureWidth(this->GetSize().GetWidth());
                this->_backgroundTexture->SetTextureHeight(this->GetSize().GetHeight());
            }

            if (this->_scrollToBottomOnTextAdd)
            {
                // offset by the difference in height
                int32_t delta = this->_backgroundTexture->GetTextureHeight() - this->GetSize().GetHeight();
                if (delta > 0)
                {
                    this->_position.SetOffset(0, -delta);
                }
            }

            if (!this->CreateMask(this->_backgroundTexture->GetTextureWidth(),
                                  this->_backgroundTexture->GetTextureHeight()))
            {
                this->LogMessage("Failed to create mask.");
                return false;
            }

            this->_backgroundTexture->SetMaterialName("single_texture_with_mask");
            this->_backgroundTexture->AddMaterialTexture(this->_maskTexture);
        }

        this->_text = text;

        return true;
    }

    void Label::Clear()
    {
        this->Destroy();

        this->ClearChildren();
    }

    void Label::Render()
    {
        if (this->_backgroundTexture)
        {
            uint32_t x {0};
            uint32_t y {0};

            this->_position.ResolvePosition(this->shared_from_this(), x, y);

            auto w = this->_backgroundTexture->GetTextureWidth();
            auto h = this->_backgroundTexture->GetTextureHeight();

            auto [offsetX, offsetY] = this->_position.GetOffset();

            this->_backgroundTexture->SetRenderDetails(x + offsetX, y + offsetY, w, h,
                                                       graphics::RenderOriginPoints::TopLeft);

            auto renderLayer = this->GetGraphics()->BumpRenderLayer();
            this->_backgroundTexture->Render(renderLayer);
        }

        this->RenderChildren();
    }

    void Label::Destroy()
    {
        if (this->_backgroundTexture)
        {
            // this is loaded from a surface, not from the pool
            // destroy it manually
            this->_backgroundTexture->Destroy();
            this->_backgroundTexture = nullptr;
        }

        if (this->_maskSurface)
        {
            this->_maskSurface->FreeSurface();
            this->_maskSurface = nullptr;
        }
    }

    void Label::ReadStylesDataFromJson(const nlohmann::json& controlJson,
                                       const std::shared_ptr<UI>& ui,
                                       const std::vector<std::pair<std::string, std::string>>& parentParameters)
    {
        auto normalizedJson = ui->NormalizeJson(controlJson, parentParameters);

        if (auto cssClass = normalizedJson.find("cssClass"); cssClass != normalizedJson.end())
        {
            this->_cssClass = cssClass->get<std::string>();
        }
    }

    bool Label::SetupFromJson(const nlohmann::json& controlJson,
                              const std::shared_ptr<UI>& ui,
                              const std::vector<std::pair<std::string, std::string>>& parameters)
    {
        auto normalizedJson = ui->NormalizeJson(controlJson, parameters);

        if (!this->SetCommonValuesFromJson(normalizedJson))
        {
            this->LogMessage("Failed to set common values.");
            return false;
        }

        auto text = normalizedJson["text"].get<std::string>();
        auto font = normalizedJson["font"].get<std::string>();

        // set this here because the default text is empty, so the font name and
        // color won't be set in `SetText`
        if (text.empty())
        {
            this->_style->Font = font;
        }

        if (auto characterOverride = normalizedJson.find("characterOverride"); characterOverride != normalizedJson.end())
        {
            this->_characterOverride = characterOverride->get<std::string>();
        }

        if (auto useMarkdown = normalizedJson.find("useMarkdown"); useMarkdown != normalizedJson.end())
        {
            // this needs to be a string in json, as this value will be a parameter,
            // so will likely be normalized, so we cannot use a pure boolean value
            this->_useMarkdown = useMarkdown->get<std::string>() == "true";
        }

        if (auto allowNewLines = normalizedJson.find("allowNewLines"); allowNewLines != normalizedJson.end())
        {
            // this needs to be a string in json, as this value will be a parameter,
            // so will likely be normalized, so we cannot use a pure boolean value
            this->_allowNewLines = allowNewLines->get<std::string>() == "true";
        }

        if (auto scrollToBottomOnTextAdd = normalizedJson.find("scrollToBottomOnTextAdd"); scrollToBottomOnTextAdd != normalizedJson.end())
        {
            // this needs to be a string in json, as this value will be a parameter,
            // so will likely be normalized, so we cannot use a pure boolean value
            this->_scrollToBottomOnTextAdd = scrollToBottomOnTextAdd->get<std::string>() == "true";
        }

        if (auto maxLines = normalizedJson.find("maxLines"); maxLines != normalizedJson.end())
        {
            this->_maxLines = maxLines->get<uint32_t>();
        }

        // we want copies of these values (other than `this`) as they will long be out of scope
        // by the time this is invoked
        auto binding = [this](const auto& s)
        {
          if (!this->SetText(s))
          {
              this->LogMessage("Failed to set text from binding with text: " + s);
          }
        };
        ui->EnableSimpleBindingForParameter(std::make_shared<UI::SimpleBindingType>(binding),
                                            text);

        if (!this->SetText(text))
        {
            this->LogMessage("Failed to set text with json: " + controlJson.dump());
            return false;
        }

        return true;
    }

    uint32_t Label::Script_GetCustomPropertyInt(std::string_view name,
                                                const std::vector<projectfarm::shared::scripting::FunctionParameter>& parameters) noexcept
    {
        if (name == "font_line_height")
        {
            return this->Script_GetCustomPropertyInt_font_line_height();
        }
        else if (name == "rendered_text_length")
        {
            return this->Script_GetCustomPropertyInt_rendered_text_length();
        }
        else if (name == "current_character_position_from_x_y_pos")
        {
            return this->Script_GetCustomPropertyInt_current_character_position_from_x_y_pos(parameters);
        }

        return 0;
    }

    bool Label::Script_GetCustomPropertyBool(std::string_view name,
                                             const std::vector<projectfarm::shared::scripting::FunctionParameter>&) noexcept
    {
        if (name == "get_use_markdown")
        {
            return this->_useMarkdown;
        }
        else if (name == "allow_new_lines")
        {
            return this->_allowNewLines;
        }

        return false;
    }

    shared::math::Vector2D Label::Script_GetCustomPropertyVector2D(std::string_view name,
                                                                   std::string_view parameter) noexcept
    {
        if (name == "character_pos")
        {
            return this->Script_GetCustomProperty_character_pos(parameter);
        }

        return {};
    }

    void Label::ApplyStyle(bool isLoading) noexcept
    {
        // the text is already set when loading
        if (!isLoading)
        {
            if (!this->SetText(this->_text,true))
            {
                this->LogMessage("Failed to set text when applying style.");
            }
        }
    }

    uint32_t Label::Script_GetCustomPropertyInt_font_line_height() noexcept
    {
        auto font = this->_fontManager->GetFont(this->_style->Font);
        if (!font)
        {
            this->LogMessage("Failed to find font with name: " + this->_style->Font);
            return 0;
        }

        return font->GetLineHeight();
    }

    shared::math::Vector2D Label::Script_GetCustomProperty_character_pos(std::string_view parameter) noexcept
    {
        shared::math::Vector2D v;

        if (this->_renderDetails._lines.empty())
        {
            return v;
        }

        auto characterPosition {0u};
        try
        {
            characterPosition = static_cast<uint32_t>(std::stoi(std::string(parameter)));
        }
        catch (const std::invalid_argument& ex)
        {
            this->LogMessage("Failed to convert to character position: " + std::string(parameter) +
                             " with error: " + ex.what());

            return v;
        }

        auto font = this->_fontManager->GetFont(this->_style->Font);
        if (!font)
        {
            this->LogMessage("Failed to find font with name: " + this->_style->Font);
            return v;
        }

        std::vector<Font::TextRenderPart> lineToCalculate;

        for (const auto& line : this->_renderDetails._lines)
        {
            auto lineLength {0u};

            for (const auto& part : line)
            {
                lineLength += static_cast<uint32_t>(part._text.size());
            }

            // we need to convert to a signed type, otherwise the needed negative result
            // will wrap around to a high positive result
            if (static_cast<int32_t>(characterPosition - lineLength) <= 0)
            {
                lineToCalculate = line;
                break;
            }

            characterPosition -= lineLength;

            v.SetY(v.GetY() + font->GetLineHeight());
        }

        // if the character position is greater than all the characters in all lines,
        // default to the last line
        if (lineToCalculate.empty())
        {
            lineToCalculate = this->_renderDetails._lines[this->_renderDetails._lines.size() - 1];
        }

        // use a lambda so we can easily exit the nested loop
        auto getXPos = [&lineToCalculate, characterPosition, &font]() -> uint32_t
        {
            auto x {0u};
            auto positionIndex {0u};

            for (const auto& part : lineToCalculate)
            {
                for (auto c : part._text)
                {
                    if (positionIndex++ >= characterPosition)
                    {
                        return x;
                    }

                    if (auto details = font->GetCharacterDetails(c, part._style); details)
                    {
                        x += details->_w;
                    }
                }
            }

            return x;
        };

        v.SetX(getXPos());

        return v;
    }

    uint32_t Label::Script_GetCustomPropertyInt_rendered_text_length() noexcept
    {
        auto count = 0u;

        for (const auto& line : this->_renderDetails._lines)
        {
            for (const auto& part : line)
            {
                count += static_cast<uint32_t>(part._text.size());
            }
        }

        return count;
    }

    uint32_t Label::Script_GetCustomPropertyInt_current_character_position_from_x_y_pos(
            const std::vector<projectfarm::shared::scripting::FunctionParameter>& parameters) noexcept
    {
        if (parameters.size() != 2)
        {
            this->LogMessage("`current_character_position_from_x_y_pos` expects 2 parameters.");
            return 0;
        }

        auto x = parameters[0].GetAsUInt();
        auto y = parameters[1].GetAsUInt();

        if (!x || !y)
        {
            this->LogMessage("Invalid parameters for `current_character_position_from_x_y_pos`: x: " +
                             parameters[0].GetValue() + " y: " + parameters[1].GetValue());
            return 0;
        }

        auto font = this->_fontManager->GetFont(this->_style->Font);
        if (!font)
        {
            this->LogMessage("Failed to find font with name: " + this->_style->Font);
            return 0;
        }

        auto [ox, oy] = this->GetPosition().GetOffset();

        auto position = font->PositionToCharacterIndex(this->_text,
                                                       *x - ox, *y - oy,
                                                       this->_size.GetWidth(),
                                                       this->_characterOverride,
                                                       this->_useMarkdown);
        return position;
    }

    void Label::OnDrag(uint32_t, uint32_t, uint32_t dx, uint32_t dy) noexcept
    {
        auto pos = this->GetPosition();

        auto [ox, oy] = pos.GetOffset();

        int32_t newX = ox + dx;
        int32_t newY = oy + dy;

        // clamp the scrolling
        if (this->_backgroundTexture)
        {
            newX = std::clamp(newX, -static_cast<int32_t>(this->_backgroundTexture->GetTextureWidth()), 0);
            newY = std::clamp(newY, -static_cast<int32_t>(this->_backgroundTexture->GetTextureHeight()), 0);
        }

        pos.SetOffset(newX, newY);

        this->SetPosition(pos);
    }
}
