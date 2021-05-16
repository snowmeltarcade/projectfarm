#ifndef PROJECTFARM_ACTION_ANIMATIONS_MANAGER_H
#define PROJECTFARM_ACTION_ANIMATIONS_MANAGER_H

#include <string>
#include <unordered_map>
#include <filesystem>
#include <vector>

#include "logging/consume_logger.h"
#include "data/consume_data_provider.h"
#include "math/consume_random_engine.h"

namespace projectfarm::engine::entities
{
    class ActionAnimationsManager : public shared::ConsumeLogger,
                                    public shared::ConsumeDataProvider,
                                    public shared::math::ConsumeRandomEngine
    {
    public:
        ActionAnimationsManager() = default;
        ~ActionAnimationsManager() override = default;

        [[nodiscard]] bool Load() noexcept;

        [[nodiscard]] std::string GetAppearance(const std::string& type, const std::string& part) const noexcept;

    private:
        // type -> part -> names
        std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>> _typePartMap;

        [[nodiscard]] bool AddToLibrary(const std::string& name, const std::filesystem::path& path) noexcept;
    };
}

#endif
