#include <fstream>
#include <sstream>
#include <vector>

#include "font_manager.h"

namespace projectfarm::graphics
{
    bool FontManager::Initialize()
    {
        this->LogMessage("Initializing fonts...");

        if (!this->LoadListedFonts())
        {
            this->LogMessage("Failed to load listed fonts.");
            return false;
        }

        this->LogMessage("Initialized fonts.");

        return true;
    }

    bool FontManager::LoadFont(uint16_t size, const std::string& name)
    {
        std::shared_ptr<Font> font = std::make_shared<Font>();
        font->SetLogger(this->_logger);

        auto fontTexturePath = this->_dataProvider->ResolveFileName(
                projectfarm::shared::DataProviderLocations::ClientFonts, name + ".png");

        auto fontDetailsPath = this->_dataProvider->ResolveFileName(
                projectfarm::shared::DataProviderLocations::ClientFonts, name + ".txt");

        if (!font->Load(fontTexturePath, fontDetailsPath, size, name))
        {
            this->LogMessage("Failed to load font: " + fontTexturePath.u8string());
            return false;
        }

        auto fontName = std::string(name);
        if (this->_fonts.find(fontName) != this->_fonts.end())
        {
            this->LogMessage("The font: '" + fontName + "' already exists.");
            return false;
        }

        this->_fonts[fontName] = font;

        return true;
    }

    void FontManager::Shutdown()
    {
        for (const auto&[_, font] : this->_fonts)
        {
            if (font)
            {
                font->Destroy();
            }
        }

        this->_fonts.clear();
    }

    bool FontManager::LoadListedFonts()
    {
        auto fontListPath = this->_dataProvider->ResolveFileName(projectfarm::shared::DataProviderLocations::ClientFonts, "fontlist.txt");

        std::ifstream fp(fontListPath);

        if (!fp.is_open())
        {
            this->LogMessage("Failed to open fontlist file: " + fontListPath.string());
            return false;
        }

        std::string line;
        std::string token;

        while (std::getline(fp, line))
        {
            std::istringstream ss(line);
            std::vector<std::string> result;

            while (std::getline(ss, token, ','))
            {
                result.emplace_back(token);
            }

            if (result.size() != 3)
            {
                this->LogMessage("Invalid line in fontlist file:");
                this->LogMessage(line);
                return false;
            }

            // we no longer care about the actual font filename, just the font name

            auto fontSize = static_cast<uint16_t>(std::strtol(result[1].c_str(), nullptr, 10));

            std::string name = result[2];

            if (!this->LoadFont(fontSize, name))
            {
                this->LogMessage("Failed to load the font with name: " + name);
                return false;
            }
        }

        return true;
    }
}