#include <SDL_image.h>
#include <fstream>

#include "font.h"
#include "utils/util.h"

namespace projectfarm::graphics
{
    bool Font::Load(const std::filesystem::path& texturePath,
                    const std::filesystem::path& detailsPath,
                    uint16_t size,
                    std::string_view name)
    {
        this->LogMessage("Loading font: " + texturePath.u8string());

        if (!this->LoadTexture(texturePath))
        {
            this->LogMessage("Failed to load font texture: " + texturePath.u8string());
            return false;
        }

        if (!this->LoadDetails(detailsPath))
        {
            this->LogMessage("Failed to load font details: " + detailsPath.u8string());
            return false;
        }

        this->_size = size;
        this->_name = name;

        this->LogMessage("Loaded font: " + texturePath.string());

        return true;
    }

    void Font::Destroy()
    {
        if (this->_surface)
        {
            SDL_FreeSurface(this->_surface);
            this->_surface = nullptr;
        }

        this->_details.clear();
    }

    auto Font::RenderToSurface(std::string_view text,
                               SDL_Color color,
                               uint32_t maxWidth,
                               const std::string& characterOverride,
                               bool useMarkdown,
                               uint32_t maxLines) noexcept
        -> std::pair<std::shared_ptr<SDLFreeableSurface>, RenderDetails>
    {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        uint32_t rMask = 0xff000000;
        uint32_t gMask = 0x00ff0000;
        uint32_t bMask = 0x0000ff00;
        uint32_t aMask = 0x000000ff;
#else
        uint32_t rMask = 0x000000ff;
        uint32_t gMask = 0x0000ff00;
        uint32_t bMask = 0x00ff0000;
        uint32_t aMask = 0xff000000;
#endif

        auto renderDetails = this->GetRenderDetails(text,
                                                    maxWidth,
                                                    color,
                                                    characterOverride,
                                                    useMarkdown,
                                                    maxLines);

        auto destSurface = SDL_CreateRGBSurface(0,
                                                renderDetails._width, renderDetails._height,
                                                32, rMask, gMask, bMask, aMask);

        auto y {0u};

        FontDetails source;

        for (const auto& line : renderDetails._lines)
        {
            std::string renderedLine;

            auto x {0u};

            for (const auto& part : line)
            {
                for (auto c : part._text)
                {
                    if (c == ' ')
                    {
                        x += this->_spaceWidth;
                        continue;
                    }

                    auto details = this->GetCharacterDetails(c, part._style);
                    if (!details)
                    {
                        continue;
                    }

                    source = *details;

                    SDL_Rect sourceRect
                    {
                        static_cast<int>(source._x),
                        static_cast<int>(source._y),
                        static_cast<int>(source._w),
                        static_cast<int>(source._h),
                    };

                    SDL_Rect dest
                    {
                        static_cast<int>(x),
                        static_cast<int>(y),
                        static_cast<int>(source._w),
                        static_cast<int>(source._h),
                    };

                    SDL_BlitSurface(this->_surface, &sourceRect, destSurface, &dest);

                    this->SetColor(destSurface, dest, part._color);

                    x += source._w;
                }
            }

            y += this->_lineHeight;
        }

        return { std::make_shared<SDLFreeableSurface>(destSurface), renderDetails };
    }

    uint32_t Font::PositionToCharacterIndex(std::string_view text,
                                            uint32_t posX, uint32_t posY,
                                            uint32_t maxWidth,
                                            const std::string& characterOverride,
                                            bool useMarkdown) noexcept
    {
        SDL_Point point { static_cast<int>(posX), static_cast<int>(posY) };

        auto index {0u};

        auto renderDetails = this->GetRenderDetails(text, maxWidth, {}, characterOverride, useMarkdown, 0);

        auto y {0u};

        for (const auto& line : renderDetails._lines)
        {
            std::string renderedLine;

            auto x {0u};

            for (const auto& part : line)
            {
                for (auto c : part._text)
                {
                    ++index;

                    SDL_Rect dest
                    {
                        static_cast<int>(x),
                        static_cast<int>(y),
                        static_cast<int>(0),
                        static_cast<int>(this->_lineHeight),
                    };

                    if (c == ' ')
                    {
                        x += this->_spaceWidth;
                        dest.w = this->_spaceWidth;
                    }
                    else
                    {
                        auto details = this->GetCharacterDetails(c, part._style);
                        if (!details)
                        {
                            continue;
                        }

                        dest.w = details->_w;
                        dest.h = details->_h;

                        x += details->_w;
                    }

                    if (SDL_PointInRect(&point, &dest))
                    {
                        return index;
                    }
                }
            }

            y += this->_lineHeight;
        }

        return index;
    }

    std::optional<Font::FontDetails> Font::GetCharacterDetails(char c, FontStyles style) const noexcept
    {
        if (c == ' ')
        {
            return Font::FontDetails { 0, 0, this->_spaceWidth, 0 };
        }
        else if (c == '\n')
        {
            return Font::FontDetails { 0, 0, 0, 0 };
        }
        else
        {
            auto details = this->_details.find(c);
            if (details == this->_details.end())
            {
                return {};
            }

            return details->second.at(style);
        }
    }

    bool Font::LoadTexture(const std::filesystem::path& path) noexcept
    {
        this->_surface = IMG_Load(path.u8string().c_str());
        if (!this->_surface)
        {
            this->_logger->LogMessage("Failed to load the texture: " + path.u8string() + " with error: " +
                                      IMG_GetError());
            return false;
        }

        return true;
    }

    bool Font::LoadDetails(const std::filesystem::path& path) noexcept
    {
        std::ifstream fp(path);

        if (!fp.is_open())
        {
            this->LogMessage("Failed to open font details file: " + path.u8string());
            return false;
        }

        std::string line;

        std::getline(fp, line);
        this->_lineHeight = static_cast<uint16_t>(std::stoi(pfu::trim(line)));

        std::getline(fp, line);
        this->_spaceWidth = static_cast<uint16_t>(std::stoi(pfu::trim(line)));

        while (std::getline(fp, line))
        {
            if (pfu::startsWith(line, "//") ||
                line.empty())
            {
                continue;
            }

            auto parts = pfu::split(" ", line);
            if (parts.size() != 17)
            {
                this->LogMessage("Invalid line in font details file: " + line);
                return false;
            }

            auto character = parts[0][0]; // we only want the actual `char`, not the `std::string`

            FontDetails normal { static_cast<uint32_t>(std::stoi(parts[1])),
                                 static_cast<uint32_t>(std::stoi(parts[2])),
                                 static_cast<uint32_t>(std::stoi(parts[3])),
                                 static_cast<uint32_t>(std::stoi(parts[4])) };

            FontDetails bold { static_cast<uint32_t>(std::stoi(parts[5])),
                               static_cast<uint32_t>(std::stoi(parts[6])),
                               static_cast<uint32_t>(std::stoi(parts[7])),
                               static_cast<uint32_t>(std::stoi(parts[8])) };

            FontDetails italic { static_cast<uint32_t>(std::stoi(parts[9])),
                                 static_cast<uint32_t>(std::stoi(parts[10])),
                                 static_cast<uint32_t>(std::stoi(parts[11])),
                                 static_cast<uint32_t>(std::stoi(parts[12])) };

            FontDetails boldItalic { static_cast<uint32_t>(std::stoi(parts[13])),
                                     static_cast<uint32_t>(std::stoi(parts[14])),
                                     static_cast<uint32_t>(std::stoi(parts[15])),
                                     static_cast<uint32_t>(std::stoi(parts[16])) };

            std::unordered_map<FontStyles, FontDetails> styles;
            styles[FontStyles::Normal] = normal;
            styles[FontStyles::Bold] = bold;
            styles[FontStyles::Italic] = italic;
            styles[FontStyles::BoldItalic] = boldItalic;

            this->_details[character] = styles;
        }

        return true;
    }

    Font::RenderDetails Font::GetRenderDetails(std::string_view text,
                                               uint32_t maxWidth,
                                               const SDL_Color& color,
                                               const std::string& characterOverride,
                                               bool useMarkdown,
                                               uint32_t maxLines) const noexcept
    {
        RenderDetails renderDetails;

        auto getWordLength = [this](const std::string& word, FontStyles style) -> uint32_t
        {
            auto width {0u};

            for (char c : word)
            {
                if (auto details = this->GetCharacterDetails(c, style); details)
                {
                    width += details->_w;
                }
            }

            return width;
        };

        auto parts = this->GetTextRenderParts(text, color, useMarkdown);

        std::vector<TextRenderPart> line;
        auto lineWidth {0u};

        for (auto& part : parts)
        {
            auto words = pfu::split(" ", part._text);

            std::string lineText;

            for (auto i = 0u; i < words.size(); ++i)
            {
                auto word = words[i];
                if (word.empty())
                {
                    // this must have been a space that was removed when splitting
                    // the test above
                    word = " ";
                }
                else if (i < words.size() - 1)
                {
                    word += " ";
                }

                auto wordWidth = getWordLength(word, part._style);
                if (word[0] == '\n' || // `GetTextRenderParts` above splits parts by new lines, so new lines should be at the begining of a word
                    lineWidth + wordWidth > maxWidth)
                {
                    // split the current part
                    auto partToAdd = part;
                    partToAdd._text = lineText;
                    line.push_back(partToAdd);

                    // remove the text we just split from the current part
                    part._text = part._text.substr(lineText.size());

                    // we don't want to add a blank line just because the only word
                    // in this line was too long
                    auto isEmptyLine = line.size() == 1 && line[0]._text.empty();
                    if (!isEmptyLine)
                    {
                        renderDetails._lines.push_back(line);
                    }

                    line = {};
                    lineText = "";
                    lineWidth = 0;
                }

                lineText += word;
                lineWidth += wordWidth;

                if (lineWidth > renderDetails._width)
                {
                    renderDetails._width = lineWidth;
                }
            }

            line.push_back(part);
        }

        renderDetails._lines.push_back(line);

        if (!characterOverride.empty())
        {
            for (auto& renderDetailsLine : renderDetails._lines)
            {
                for (auto& part : renderDetailsLine)
                {
                    for (auto i = 0u; i < part._text.size(); ++i)
                    {
                        part._text[i] = characterOverride[0];
                    }
                }
            }
        }

        if (maxLines > 0)
        {
            // in case `maxLines` is greater than the current number of lines
            // + 1 as the end iterator is counted, but isn't actually a line
            auto linesToRemove = std::min(static_cast<uint32_t>(renderDetails._lines.size()), maxLines + 1);

            // get the last `maxLines` lines
            renderDetails._lines = { renderDetails._lines.end() - linesToRemove, renderDetails._lines.end() };
        }

        renderDetails._height = static_cast<uint32_t>(renderDetails._lines.size() * this->_lineHeight);

        return renderDetails;
    }

    void Font::SetColor(SDL_Surface* surface, const SDL_Rect& rect, const SDL_Color& color) const noexcept
    {
        static constexpr auto pixelSize = sizeof(std::uint32_t);
        auto xOffset = rect.x * pixelSize;

        std::array<std::uint8_t, 3> line;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        line[0] = color.b;
        line[1] = color.g;
        line[2] = color.r;
#else
        line[0] = color.r;
        line[1] = color.g;
        line[2] = color.b;
#endif

        auto pixels = (std::uint8_t*)surface->pixels;
        auto pixel = &pixels[xOffset];

        auto target = rect.w * rect.h;
        auto y = rect.y;
        auto x = rect.x;

        // TODO: Clang doesn't support std::execution::par_unseq yet,
        // so for now, just use a plain loop...
        for (auto i = 0; i < target; ++i)
        {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            // ABGR
            pixel += 1;
            std::memcpy(pixel, line.data(), line.size());
            pixel += 3;
#else
            // RGBA
            std::memcpy(pixel, line.data(), line.size());
            pixel += 4;
#endif

            ++x;
            if (x >= rect.x + rect.w)
            {
                x = rect.x;
                ++y;

                pixel = &pixels[(y * surface->pitch) + xOffset];
            }
        }
    }

    std::vector<Font::TextRenderPart> Font::GetTextRenderParts(std::string_view text,
                                                               SDL_Color defaultColor,
                                                               bool useMarkdown) const noexcept
    {
        std::vector<TextRenderPart> parts;

        if (!useMarkdown)
        {
            TextRenderPart part;
            part._text = text;
            part._color = defaultColor;
            parts.emplace_back(std::move(part));
            return parts;
        }

        shared::graphics::colors::Color c
        {
            defaultColor.r, defaultColor.g, defaultColor.b, defaultColor.a,
        };

        auto result = projectfarm::shared::markdown::GetTextParts(text, c);

        for (const auto& resultPart : result)
        {
            // for rendering, we want one part per line
            auto lines = pfu::split("\n", resultPart._text);

            // the split above will not add a blank new line if one is needed
            if (resultPart._text[resultPart._text.size() - 1] == '\n') {
                lines.push_back({});
            }

            auto isFirstLine {true};

            for (const auto& line : lines)
            {
                auto splitResultPart = resultPart;
                splitResultPart._text = line;

                if (!isFirstLine)
                {
                    splitResultPart._text = "\n" + splitResultPart._text;
                }

                isFirstLine = false;

                TextRenderPart part(splitResultPart);

                parts.push_back(part);
            }
        }

        return parts;
    }
}