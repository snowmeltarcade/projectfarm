#ifndef PROJECTFARM_FONT_H
#define PROJECTFARM_FONT_H

#include <string>
#include <filesystem>
#include <cstdlib>
#include <memory>
#include <unordered_map>

#include "font_styles.h"
#include "graphics/SDL_freeable_surface.h"
#include "markdown/markdown.h"

namespace projectfarm::graphics
{
    class Font final
    {
    public:
        Font() = default;
        ~Font()
        {
            this->Destroy();
        }

        struct FontDetails
        {
            uint32_t _x {0};
            uint32_t _y {0};
            uint32_t _w {0};
            uint32_t _h {0};
        };

        struct TextRenderPart
        {
            TextRenderPart() = default;
            TextRenderPart(shared::markdown::TextPart part)
            {
                this->_text = part._text;

                if (part._type == shared::markdown::TextPartTypes::Italic)
                {
                    this->_style = FontStyles::Italic;
                }
                else if (part._type == shared::markdown::TextPartTypes::Bold)
                {
                    this->_style = FontStyles::Bold;
                }
                else if (part._type == shared::markdown::TextPartTypes::BoldItalic)
                {
                    this->_style = FontStyles::BoldItalic;
                }
                else
                {
                    this->_style = FontStyles::Normal;
                }

                this->_color.r = part._color.r;
                this->_color.g = part._color.g;
                this->_color.b = part._color.b;
                this->_color.a = part._color.a;
            }

            FontStyles _style {FontStyles::Normal};
            SDL_Color _color {0, 0, 0, 255};
            std::string _text;
        };

        struct RenderDetails
        {
            uint32_t _width {0};
            uint32_t _height {0};
            std::vector<std::vector<TextRenderPart>> _lines;
        };

        [[nodiscard]]
        bool Load(const std::filesystem::path& texturePath,
                  const std::filesystem::path& detailsPath,
                  uint16_t size,
                  std::string_view name);

        void Destroy();

        [[nodiscard]]
        auto RenderToSurface(std::string_view text,
                             SDL_Color color,
                             uint32_t maxWidth = 0,
                             const std::string& characterOverride = "",
                             bool useMarkdown = true,
                             uint32_t maxLines = 0) noexcept
                -> std::pair<std::shared_ptr<SDLFreeableSurface>, RenderDetails>;

        [[nodiscard]]
        uint32_t PositionToCharacterIndex(std::string_view text,
                                          uint32_t posX, uint32_t posY,
                                          uint32_t maxWidth = 0,
                                          const std::string& characterOverride = "",
                                          bool useMarkdown = true) noexcept;

        [[nodiscard]]
        uint16_t GetLineHeight() noexcept
        {
            return this->_lineHeight;
        }

        [[nodiscard]]
        std::optional<FontDetails> GetCharacterDetails(char c, FontStyles style) const noexcept;

    private:
        [[nodiscard]]
        bool LoadTexture(const std::filesystem::path& path) noexcept;

        [[nodiscard]]
        bool LoadDetails(const std::filesystem::path& path) noexcept;

        [[nodiscard]]
        RenderDetails GetRenderDetails(std::string_view text,
                                       uint32_t maxWidth,
                                       const SDL_Color& color,
                                       const std::string& characterOverride,
                                       bool useMarkdown,
                                       uint32_t maxLines) const noexcept;

        void SetColor(SDL_Surface* surface, const SDL_Rect& rect, const SDL_Color& color) const noexcept;

        [[nodiscard]]
        std::vector<TextRenderPart> GetTextRenderParts(std::string_view text,
                                                      SDL_Color defaultColor,
                                                      bool useMarkdown) const noexcept;

        std::string _name;
        std::filesystem::path _path;

        uint16_t _size {0};
        uint16_t _lineHeight {0};
        uint16_t _spaceWidth {0};

        SDL_Surface* _surface {nullptr};

        std::unordered_map<char, std::unordered_map<FontStyles, FontDetails>> _details;
    };
}

#endif
