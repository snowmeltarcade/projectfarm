#include "warp.h"
#include "api/logging/logging.h"

namespace projectfarm::engine::world::action_tile_actions
{
    bool Warp::Run() noexcept
    {
        auto world = this->_character->GetCurrentWorld();

        if (this->_character->GetCurrentWorld()->GetName() == this->_worldName)
        {
            auto actionTiles = world->GetActionTilesByProperty("type", this->_destinationTileType);
            if (actionTiles.empty())
            {
                shared::api::logging::Log("Failed to find warp destination tile type for world: " + this->_worldName);
                return false;
            }

            // TODO: For now just get the first tile
            for (const auto& [islandIndex, tiles] : actionTiles)
            {
                auto tile = tiles[0];

                auto islands = world->GetIslands();
                auto [wx, wy] = islands[islandIndex]->GetWorldPositionFromTileCoordinate(tile.X, tile.Y);

                this->_character->Warp(wx, wy);
            }
        }
        else
        {
            if (this->_character->GetPlayerId() > 0)
            {
                if (!world->TransferPlayerToWorld(this->_character->GetPlayerId(),
                                                  this->_character->GetEntityId(),
                                                  this->_worldName))
                {
                    shared::api::logging::Log("Failed to transfer player with id: " + std::to_string(this->_character->GetPlayerId()) +
                                     " to world: " + this->_worldName);
                    return false;
                }
            }
            else
            {
                if (!world->TransferCharacterToWorld(this->_character, this->_worldName, this->_destinationTileType))
                {
                    shared::api::logging::Log("Failed to transfer character with entity id: " +
                                     std::to_string(this->_character->GetEntityId()) +
                                     " to world: " + this->_worldName);

                    return false;
                }
            }
        }

        return true;
    }
}