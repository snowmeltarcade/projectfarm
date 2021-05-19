#include "world_entity.h"
#include "utils/util.h"

namespace projectfarm::engine::entities
{
    void WorldEntity::OnTick() noexcept
    {
    }

    void WorldEntity::Render() noexcept
    {
    }

    void WorldEntity::SetData(const std::vector<std::byte>& data) noexcept
    {
        auto& islands = this->_world->GetIslands();

        uint32_t dataIndex {0};

        auto islandCount = pfu::ReadUInt32(data, dataIndex);
        if (islandCount != islands.size())
        {
            this->LogMessage("Invalid island count. Expected: " + std::to_string(islands.size()) +
                             " got: " + std::to_string(islandCount));
            return;
        }

        for (const auto& island : islands)
        {
            auto numberOfLogEntries = pfu::ReadUInt32(data, dataIndex);

            auto lastUpdateTime = island->GetLastPlotUpdateTime();

            while (numberOfLogEntries--)
            {
                auto layerIndex = pfu::ReadUInt8(data, dataIndex);
                auto tileX = pfu::ReadUInt32(data, dataIndex);
                auto tileY = pfu::ReadUInt32(data, dataIndex);
                auto time = pfu::ReadUInt64(data, dataIndex);
                auto plotIndex = pfu::ReadUInt16(data, dataIndex);

                // no need to read updates already processed
                if (time <= lastUpdateTime)
                {
                    break;
                }

                island->ProcessPlotUpdate(layerIndex, tileX, tileY, time, plotIndex);
            }
        }
    }
}