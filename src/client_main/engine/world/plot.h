#ifndef PROJECTFARM_PLOT_H
#define PROJECTFARM_PLOT_H

#include <string>
#include <cstdint>
#include <vector>
#include <nlohmann/json.hpp>

#include "logging/consume_logger.h"
#include "graphics/consume_tile_set_pool.h"
#include "graphics/tile_set.h"
#include "entities/character_states.h"

namespace projectfarm::engine::world
{
    class Plot final : public shared::ConsumeLogger,
                       public graphics::ConsumeTileSetPool
    {
    public:
        Plot() = default;
        ~Plot() override = default;

        [[nodiscard]] bool LoadFromJson(const nlohmann::json& jsonFile);

        [[nodiscard]] const std::string& GetName() const noexcept
        {
            return this->_name;
        }

        [[nodiscard]] uint32_t GetNumberOfFrames() const noexcept
        {
            return static_cast<uint32_t>(this->_animationData.size());
        }

        [[nodiscard]] uint64_t GetMilliseconds(uint32_t frame) const noexcept
        {
            return this->_animationData[frame]._milliseconds;
        }

        [[nodiscard]] uint32_t GetAbsoluteIndex(uint32_t frame) const noexcept
        {
            return this->_animationData[frame]._absoluteIndex;
        }

        [[nodiscard]] const std::string& GetTileSet(uint32_t frame) const noexcept
        {
            return this->_animationData[frame]._tileSet;
        }

        [[nodiscard]] const std::vector<shared::entities::CharacterStates>& GetDisallowedCharacterStates() const noexcept
        {
            return this->_disallowedCharacterStates;
        }

        void AddDisallowedCharacterState(shared::entities::CharacterStates state) noexcept
        {
            this->_disallowedCharacterStates.push_back(state);
        }

    private:
        struct AnimationData
        {
            uint64_t _milliseconds {0u};
            uint32_t _absoluteIndex {0u};
            std::string _tileSet;
        };

        void AddFrame(uint64_t milliseconds, uint32_t absoluteIndex,
                      const std::string& tileSet)
        {
            this->_animationData.push_back({
               milliseconds,
               absoluteIndex,
               tileSet,
            });
        }

        [[nodiscard]] bool LoadAnimationFrameFromJson(const nlohmann::json& animationJson);

        std::vector<AnimationData> _animationData;

        std::string _name;

        std::vector<shared::entities::CharacterStates> _disallowedCharacterStates;

        [[nodiscard]] bool SetDisallowedCharacterStates(const nlohmann::json& json) noexcept;
    };
}

#endif