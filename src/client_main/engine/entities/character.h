#ifndef PROJECTFARM_CHARACTER_H
#define PROJECTFARM_CHARACTER_H

#include <memory>

#include "entity.h"
#include "graphics/tile_map.h"
#include "graphics/consume_tile_set_pool.h"
#include "engine/consume_game.h"
#include "engine/actionable_entity.h"
#include "state/state_machine.h"
#include "entities/character_states.h"
#include "entities/character_state_values.h"
#include "math/lerper.h"
#include "engine/world/world.h"
#include "entities/character_appearance_details.h"

namespace projectfarm::engine::entities
{
    class CharacterAppearanceManager;

    class Character final : public Entity,
                            public shared::ConsumeDataProvider,
                            public graphics::ConsumeRenderManager,
                            public graphics::ConsumeTileSetPool,
                            public engine::ConsumeGame,
                            public engine::ActionableEntity
    {
    public:
        using StateMachineType = shared::state::StateMachine<shared::entities::CharacterStates,
                shared::entities::CharacterStateValues>;

        Character() = default;
        ~Character() override = default;

        void Render() noexcept override;

        [[nodiscard]] bool Load() noexcept override;

        void Shutdown() noexcept override;

        shared::entities::EntityTypes GetEntityType() const noexcept override
        {
            return shared::entities::EntityTypes::Character;
        }

        [[nodiscard]] std::vector<std::byte> GetEntityData() const noexcept override;

        void SetData(const std::vector<std::byte>& data) noexcept override;

        void ProcessAction(engine::ActionTypes action, bool actionAdded) noexcept override;

        [[nodiscard]] uint32_t GetPlayerId() const noexcept
        {
            return this->_playerId;
        }

        void ReconfirmPixelSizes() noexcept override;

        void SetCurrentWorld(const std::shared_ptr<engine::world::World>& world) noexcept
        {
            this->_currentWorld = world;
        }

        void SetDetails(const shared::entities::CharacterAppearanceDetails& details, bool updateTileMap = false) noexcept;

        [[nodiscard]] bool IsPlayer() const noexcept
        {
            return this->_playerId > 0;
        }

    protected:
        void OnTick() noexcept override;

    private:
        static inline const StateMachineType::StateItemType StateIdle
            {shared::entities::CharacterStates::Idle, shared::entities::CharacterStateValues::None};

        static inline const StateMachineType::StateItemType StateWalkLeft
            {shared::entities::CharacterStates::Walk, shared::entities::CharacterStateValues::Left};
        static inline const StateMachineType::StateItemType StateWalkRight
            {shared::entities::CharacterStates::Walk, shared::entities::CharacterStateValues::Right};
        static inline const StateMachineType::StateItemType StateWalkUp
            {shared::entities::CharacterStates::Walk, shared::entities::CharacterStateValues::Up};
        static inline const StateMachineType::StateItemType StateWalkDown
            {shared::entities::CharacterStates::Walk, shared::entities::CharacterStateValues::Down};

        static inline const StateMachineType::StateItemType StateWalkUpLeft
            {shared::entities::CharacterStates::Walk, shared::entities::CharacterStateValues::UpLeft};
        static inline const StateMachineType::StateItemType StateWalkUpRight
            {shared::entities::CharacterStates::Walk, shared::entities::CharacterStateValues::UpRight};
        static inline const StateMachineType::StateItemType StateWalkDownLeft
            {shared::entities::CharacterStates::Walk, shared::entities::CharacterStateValues::DownLeft};
        static inline const StateMachineType::StateItemType StateWalkDownRight
            {shared::entities::CharacterStates::Walk, shared::entities::CharacterStateValues::DownRight};

        static inline const StateMachineType::StateItemType StateRunLeft
            {shared::entities::CharacterStates::Run, shared::entities::CharacterStateValues::Left};
        static inline const StateMachineType::StateItemType StateRunRight
            {shared::entities::CharacterStates::Run, shared::entities::CharacterStateValues::Right};
        static inline const StateMachineType::StateItemType StateRunUp
            {shared::entities::CharacterStates::Run, shared::entities::CharacterStateValues::Up};
        static inline const StateMachineType::StateItemType StateRunDown
            {shared::entities::CharacterStates::Run, shared::entities::CharacterStateValues::Down};

        static inline const StateMachineType::StateItemType StateRunUpLeft
            {shared::entities::CharacterStates::Run, shared::entities::CharacterStateValues::UpLeft};
        static inline const StateMachineType::StateItemType StateRunUpRight
            {shared::entities::CharacterStates::Run, shared::entities::CharacterStateValues::UpRight};
        static inline const StateMachineType::StateItemType StateRunDownLeft
            {shared::entities::CharacterStates::Run, shared::entities::CharacterStateValues::DownLeft};
        static inline const StateMachineType::StateItemType StateRunDownRight
            {shared::entities::CharacterStates::Run, shared::entities::CharacterStateValues::DownRight};

        std::shared_ptr<graphics::TileMap> _tileMap;

        uint32_t _playerId {0};

        std::shared_ptr<StateMachineType> _stateMachine;

        void ProcessState() noexcept;

        // set to true when the first data is received from
        // the server
        bool _hasReceivedFirstData {false};

        // this is used for client prediction to help smooth out
        // any changes in location
        bool _isPositionLerping {false};
        shared::math::Lerper<float> _xPosLerper;
        shared::math::Lerper<float> _yPosLerper;

        void UpdateLerp() noexcept;

        void UpdateRenderPosition() noexcept;

        void Move(float xDir, float yDir) noexcept;

        float _positionX {0.0f};
        float _positionY {0.0f};

        float _walkSpeed {1.0f};
        float _runSpeed {1.0f};

        float _height {1.0f};
        float _width {1.0f};

        std::string _type;

        std::shared_ptr<engine::world::World> _currentWorld;

        std::unique_ptr<CharacterAppearanceManager> _characterAppearanceManager;

        void ApplyAppearanceFile(const std::string& name, uint8_t layerIndex) noexcept;

        [[nodiscard]] static std::string GetPartFromLayerIndex(uint8_t layerIndex) noexcept;
    };
}

#endif
