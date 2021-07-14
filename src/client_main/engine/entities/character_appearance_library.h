#ifndef PROJECTFARM_CHARACTER_APPEARANCE_LIBRARY_H
#define PROJECTFARM_CHARACTER_APPEARANCE_LIBRARY_H

#include <unordered_map>
#include <string>
#include <filesystem>
#include <vector>

#include "data/consume_data_provider.h"

namespace projectfarm::engine::entities
{
    class CharacterAppearanceLibrary final : public shared::ConsumeDataProvider
    {
    public:
        CharacterAppearanceLibrary() = default;
        ~CharacterAppearanceLibrary() override = default;

        [[nodiscard]] bool Setup() noexcept;

        [[nodiscard]] const std::vector<std::filesystem::path>& GetCharacterAppearanceFilePaths(const std::string& type, const std::string& part) noexcept
        {
            return this->_library[type][part];
        }

    private:
        std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::filesystem::path>>> _library;

        [[nodiscard]] bool AddToLibrary(const std::filesystem::path& path) noexcept;
    };
}

#endif
