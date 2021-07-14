#ifndef PROJECTFARM_CHARACTER_H
#define PROJECTFARM_CHARACTER_H

#include <memory>
#include <cstdint>
#include <filesystem>
#include <atomic>

#include "entity.h"
#include "entities/entity_types.h"
#include "state/state_machine.h"
#include "entities/character_states.h"
#include "entities/character_state_values.h"
#include "entities/character_behaviour_states.h"
#include "entities/character_behaviour_state_values.h"
#include "data/consume_data_provider.h"
#include "engine/data/consume_data_manager.h"
#include "scripting/script.h"
#include "scripting/script_system.h"
#include "scripting/consume_script_system.h"
#include "action_animations_manager.h"
#include "entities/character_appearance_details.h"
#include "time/stopwatch.h"

namespace projectfarm::engine::world
{
    class World;
}

namespace projectfarm::engine::entities
{
    class Character final : public Entity,
                            public shared::ConsumeDataProvider,
                            public shared::scripting::ConsumeScriptSystem,
                            public engine::data::ConsumeDataManager,
                            public std::enable_shared_from_this<Character>
    {
    public:
        using StateMachineType = shared::state::StateMachine<shared::entities::CharacterStates,
                shared::entities::CharacterStateValues>;

        using BehaviourStateMachineType = shared::state::StateMachine<shared::entities::CharacterBehaviourStates,
                shared::entities::CharacterBehaviourStateValues>;

        Character()
        {
            this->_stateMachine = std::make_shared<StateMachineType>(StateIdle);
            this->_behaviourStateMachine = std::make_shared<BehaviourStateMachineType>(BehaviourStateIdle);
        }
        ~Character() override = default;

        [[nodiscard]] shared::entities::EntityTypes GetEntityType() const noexcept override
        {
            return shared::entities::EntityTypes::Character;
        }

        [[nodiscard]] std::vector<std::byte> GetEntityData() const noexcept override;

        void SetData(const std::vector<std::byte>& data) noexcept override;

        void Move(float xDir, float yDir) noexcept;

        void SetLocation(float x, float y) noexcept
        {
            this->_positionX = x;
            this->_positionY = y;
        }

        [[nodiscard]] std::pair<float, float> GetLocation() const noexcept
        {
            return {this->_positionX, this->_positionY};
        }

        [[nodiscard]] uint32_t GetPlayerId() const noexcept override
        {
            return this->_playerId;
        }

        void SetPlayerId(uint32_t playerId) noexcept override;

        void SetCurrentWorld(const std::shared_ptr<projectfarm::engine::world::World>& currentWorld) noexcept
        {
            this->_currentWorld = currentWorld;
        }

        [[nodiscard]] const std::shared_ptr<projectfarm::engine::world::World>& GetCurrentWorld() const noexcept
        {
            return this->_currentWorld;
        }

        [[nodiscard]] const std::string& GetCharacterType() const noexcept
        {
            return this->_type;
        }

        [[nodiscard]] const shared::entities::CharacterAppearanceDetails& GetAppearanceDetails() const noexcept
        {
            return this->_appearanceDetails;
        }

        void SetAppearanceDetails(const shared::entities::CharacterAppearanceDetails& appearanceDetails) noexcept
        {
            this->_appearanceDetails = appearanceDetails;
        }

        [[nodiscard]] bool ConfirmAppearance(const std::shared_ptr<ActionAnimationsManager>& actionAnimationsManager) noexcept;

        [[nodiscard]] bool LoadFromFile(const std::filesystem::path& filePath) noexcept;

        void SetScriptUpdateInterval(uint32_t interval) noexcept;

        void ScriptMove(const std::string& action, float startingX, float startingY, float distance) noexcept;
        void ScriptMoveTo(const std::string& action, float x, float y) noexcept;

        void Warp(float worldX, float worldY) noexcept;

        void SetLerpPositionChangeOnClient(bool lerpPositionChangeOnClient) noexcept
        {
            this->_lerpPositionChangeOnClient = lerpPositionChangeOnClient;
        }

    protected:
        void OnTick() noexcept override;

        void OnAfterBroadcastState() noexcept override
        {
            this->_lerpPositionChangeOnClient = true;
        }

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

        static inline const BehaviourStateMachineType::StateItemType BehaviourStateIdle
            {shared::entities::CharacterBehaviourStates::Idle, shared::entities::CharacterBehaviourStateValues::None};

        static inline const BehaviourStateMachineType::StateItemType BehaviourStateMoveToWalk
            {shared::entities::CharacterBehaviourStates::MoveTo, shared::entities::CharacterBehaviourStateValues::Walk};
        static inline const BehaviourStateMachineType::StateItemType BehaviourStateMoveToRun
            {shared::entities::CharacterBehaviourStates::MoveTo, shared::entities::CharacterBehaviourStateValues::Run};

        std::shared_ptr<StateMachineType> _stateMachine;
        std::shared_ptr<BehaviourStateMachineType> _behaviourStateMachine;

        void ProcessBehaviourState() noexcept;
        void ProcessState() noexcept;

        [[nodiscard]] bool ProcessMoveTo(bool isWalking) noexcept;

        [[nodiscard]] bool PersistPlayerState() const noexcept;

        float _moveToDestinationX {0.0f};
        float _moveToDestinationY {0.0f};

        float _walkSpeed {1.0f};
        float _runSpeed {1.0f};

        float _positionX {0.0f};
        float _positionY {0.0f};

        shared::entities::CharacterAppearanceDetails _appearanceDetails;

        bool _lerpPositionChangeOnClient {true};

        uint32_t _playerId {0};

        std::string _type;

        std::shared_ptr<shared::scripting::Script> _script;

        shared::time::Stopwatch _scriptStopwatch;
        shared::time::Stopwatch _playerTickStopwatch;

        std::shared_ptr<projectfarm::engine::world::World> _currentWorld;
    };
}

#endif
