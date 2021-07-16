#ifndef PROJECTFARM_ISLAND_H
#define PROJECTFARM_ISLAND_H

#include <cstdint>
#include <vector>
#include <fstream>
#include <nlohmann/json.hpp>

#include "plots.h"
#include "world_change_log_entry.h"
#include "time/consume_timer.h"
#include "time/timer.h"
#include "action_tile.h"

namespace projectfarm::engine::world
{
    class Island final : public shared::time::ConsumeTimer
    {
    public:
        Island() = default;
        ~Island() override = default;

        [[nodiscard]] bool LoadFromJson(const nlohmann::json& json, const std::shared_ptr<Plots>& plots);
        [[nodiscard]] bool LoadFromBinary(std::ifstream& fs, const std::shared_ptr<Plots>& plots) noexcept;

        void Shutdown() noexcept;

        [[nodiscard]] uint32_t GetHeightInTiles() const noexcept
        {
            return this->_heightInTiles;
        }

        [[nodiscard]] const std::vector<WorldChangeLogEntry>& GetChangeLog() const noexcept
        {
            return this->_changeLog;
        }

        [[nodiscard]] uint16_t GetPlotIndexAtWorldPosition(float x, float y) const noexcept;
        [[nodiscard]] std::pair<int32_t, int32_t> GetTileIndexesFromWorldPosition(float x, float y) const noexcept;
        [[nodiscard]] std::pair<float, float> GetWorldPositionFromTileCoordinate(uint32_t x, uint32_t y) const noexcept;

        [[nodiscard]] const std::vector<ActionTile>& GetActionTiles() const noexcept
        {
            return this->_actionTiles;
        }

        [[nodiscard]] std::vector<ActionTile> GetActionTilesByProperty(const std::string& name, const std::string& value) noexcept;

        [[nodiscard]] std::string GetPropertyValueForTile(const std::string& key, uint32_t x, uint32_t y) const noexcept;

    private:
        float _positionX {0.0f};
        float _positionY {0.0f};

        uint32_t _widthInTiles {0};
        uint32_t _heightInTiles {0};

        float _tileWidthInMeters {0.0f};
        float _tileHeightInMeters {0.0f};

        float _widthInMeters {0.0f};
        float _heightInMeters {0.0f};

        std::shared_ptr<Plots> _plots;

        std::vector<WorldChangeLogEntry> _changeLog;

        std::vector<std::vector<std::vector<uint16_t>>> _layers;

        [[nodiscard]] bool LoadLayerFromJson(const nlohmann::json& json);
        [[nodiscard]] bool LoadLayerFromBinary(std::ifstream& fs) noexcept;

        void LoadActionTiles(std::ifstream& fs) noexcept;

        std::vector<ActionTile> _actionTiles;
    };
}

#endif
