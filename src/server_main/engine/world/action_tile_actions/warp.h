#ifndef PROJECTFARM_WARP_H
#define PROJECTFARM_WARP_H

#include <cstdint>
#include <memory>
#include <string>

#include "action_tile_action_base.h"
#include "engine/world/world.h"
#include "engine/entities/character.h"

namespace projectfarm::engine::world::action_tile_actions
{
    class Warp final : public ActionTileActionBase
    {
    public:
        Warp(std::shared_ptr<engine::entities::Character> character,
             std::string worldName,
             std::string destinationTileType)
         : _character(std::move(character)),
          _worldName(std::move(worldName)),
          _destinationTileType(std::move(destinationTileType))
        {}

        [[nodiscard]] bool Run() noexcept override;

    private:
        std::shared_ptr<engine::entities::Character> _character;

        std::string _worldName;
        std::string _destinationTileType;
    };
}

#endif
