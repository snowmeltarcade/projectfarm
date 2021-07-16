#ifndef PROJECTFARM_FONT_MANAGER_H
#define PROJECTFARM_FONT_MANAGER_H

#include <map>
#include <memory>

#include <SDL.h>

#include "data/consume_data_provider.h"
#include "font.h"

namespace projectfarm::graphics
{
    class FontManager final : public projectfarm::shared::ConsumeDataProvider
    {
    public:
        FontManager() = default;
        ~FontManager() override = default;

        [[nodiscard]]
        bool Initialize();

        [[nodiscard]]
        bool LoadFont(uint16_t size, const std::string& name);

        void Shutdown();

        [[nodiscard]]
        const std::shared_ptr<Font>& GetFont(std::string_view name)
        {
            return this->_fonts[std::string(name)];
        }

    private:
        std::map<std::string, std::shared_ptr<Font>> _fonts;

        [[nodiscard]]
        bool LoadListedFonts();
    };
}

#endif
