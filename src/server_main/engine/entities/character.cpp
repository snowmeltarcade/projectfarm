#include <fstream>
#include <nlohmann/json.hpp>

#include "character.h"
#include "utils/util.h"
#include "time/timer.h"
#include "scripting/script_types.h"
#include "scripting/function_types.h"
#include "engine/world/world.h"

namespace projectfarm::engine::entities
{
    void Character::OnTick() noexcept
    {
        this->_scriptStopwatch.Tick();

        this->ProcessBehaviourState();

        this->ProcessState();

        if (this->IsPlayer())
        {
            // persist the latest state if needed
            this->_playerTickStopwatch.Tick();
        }
    }

    // this data is being sent to the client
    std::vector<std::byte> Character::GetEntityData() const noexcept
    {
        std::vector<std::byte> data;

        pfu::WriteUInt32(data, this->_playerId);

        pfu::WriteBool(data, this->_lerpPositionChangeOnClient);
        pfu::WriteInt32(data, static_cast<int32_t>(this->_positionX * 10000.0f));
        pfu::WriteInt32(data, static_cast<int32_t>(this->_positionY * 10000.0f));

        const auto& state = this->_stateMachine->GetCurrentState();
        pfu::WriteUInt32(data, static_cast<uint32_t>(state.GetKey()));
        pfu::WriteUInt32(data, static_cast<uint32_t>(state.GetValue()));

        pfu::WriteString(data, this->_type, static_cast<uint32_t>(this->_type.size()));

        // these speeds are in m/s, so converting to cm/s and sending as ints seems more stable
        pfu::WriteUInt32(data, static_cast<uint32_t>(this->_walkSpeed * 10000.0f));
        pfu::WriteUInt32(data, static_cast<uint32_t>(this->_runSpeed * 10000.0f));

        return data;
    }

    // this data is from the client
    void Character::SetData(const std::vector<std::byte>& data) noexcept
    {
        uint32_t dataIndex {0};

        pfu::ReadUInt32(data, dataIndex); // we want to ignore the player id

        auto worldName = pfu::ReadString(data, dataIndex);

        // an old packet from a previously loaded world may have just been received
        // we don't want to process it
        if (worldName != this->_currentWorld->GetName())
        {
            return;
        }

        auto x = pfu::ReadInt32(data, dataIndex) * 0.0001f;
        auto y = pfu::ReadInt32(data, dataIndex) * 0.0001f;

        auto stateKey = static_cast<shared::entities::CharacterStates>(pfu::ReadUInt32(data, dataIndex));
        auto stateValue = static_cast<shared::entities::CharacterStateValues>(pfu::ReadUInt32(data, dataIndex));

        this->_stateMachine->ClearStates();
        this->_stateMachine->PushState({stateKey, stateValue});

        this->SetLocation(x, y);
    }

    bool Character::LoadFromFile(const std::filesystem::path& filePath) noexcept
    {
        // nlohmann::json can throw exceptions
        try
        {
            std::ifstream file(filePath);

            if (!file.is_open())
            {
                this->LogMessage("Failed to open character file: " + filePath.u8string());
                return false;
            }

            nlohmann::json jsonFile;
            file >> jsonFile;

            this->_type = jsonFile["type"].get<std::string>();
            this->_walkSpeed = jsonFile["walk_speed"].get<float>();
            this->_runSpeed = jsonFile["run_speed"].get<float>();

            this->_appearanceDetails =
            {
                jsonFile["appearance_hair"].get<std::string>(),
                jsonFile["appearance_body"].get<std::string>(),
                jsonFile["appearance_clothes_top"].get<std::string>(),
                jsonFile["appearance_clothes_bottom"].get<std::string>(),
                jsonFile["appearance_feet"].get<std::string>(),
            };

            auto scriptPathString = jsonFile["script_path"].get<std::string>();
            auto scriptPath = this->_dataProvider->NormalizePath(scriptPathString);

            this->_script = this->_scriptSystem->CreateScript(shared::scripting::ScriptTypes::Character, scriptPath);
            if (!this->_script)
            {
                this->LogMessage("Failed to load character script: " + scriptPath.u8string());
                return false;
            }

            this->_script->SetObjectInternalField(this);

            if (!this->_script->CallFunction(shared::scripting::FunctionTypes::Init, {}))
            {
                this->LogMessage("Failed to call the 'init' function.");
                return false;
            }
        }
        catch (const std::exception& ex)
        {
            this->LogMessage("Failed to read character file: " + filePath.u8string() +
                             "with exception: " + ex.what());

            return false;
        }

        return true;
    }

    void Character::ScriptMove(const std::string& action, float startingX, float startingY, float distance) noexcept
    {
        this->ScriptMoveTo(action, startingX + distance, startingY);
    }

    void Character::ScriptMoveTo(const std::string& action, float x, float y) noexcept
    {
        auto isWalking = action == "w";

        this->_behaviourStateMachine->PushState(isWalking ? Character::BehaviourStateMoveToWalk
                                                          : Character::BehaviourStateMoveToRun);

        this->_moveToDestinationX = x;
        this->_moveToDestinationY = y;
    }

    void Character::Warp(float worldX, float worldY) noexcept
    {
        this->_positionX = worldX;
        this->_positionY = worldY;

        // any state should be decided by what happens at the new location
        this->_behaviourStateMachine->ClearStates();
        this->_stateMachine->ClearStates();

        this->_forceSendToOwningPlayer = true;
        this->_lerpPositionChangeOnClient = false;
    }

    void Character::ProcessBehaviourState() noexcept
    {
        const auto& currentState = this->_behaviourStateMachine->GetCurrentState();

        if (currentState == Character::BehaviourStateIdle)
        {
            return;
        }
        else if (currentState == Character::BehaviourStateMoveToWalk)
        {
            if (this->ProcessMoveTo(true))
            {
                this->_behaviourStateMachine->CancelState(currentState.GetKey());
            }
        }
        else if (currentState == Character::BehaviourStateMoveToRun)
        {
            if (this->ProcessMoveTo(false))
            {
                this->_behaviourStateMachine->CancelState(currentState.GetKey());
            }
        }
    }

    void Character::ProcessState() noexcept
    {
        const auto& currentState = this->_stateMachine->GetCurrentState();

        auto frameTime = this->GetTimer()->GetLastFrameDurationInSeconds();
        auto walkSpeed = this->_walkSpeed * frameTime;
        auto runSpeed = this->_runSpeed * frameTime;

        if (currentState == Character::StateIdle)
        {
            return;
        }
        else if (currentState == Character::StateWalkUp)
        {
            this->Move(0.0f, -walkSpeed);
        }
        else if (currentState == Character::StateWalkDown)
        {
            this->Move(0.0f, walkSpeed);
        }
        else if (currentState == Character::StateWalkLeft)
        {
            this->Move(-walkSpeed, 0.0f);
        }
        else if (currentState == Character::StateWalkRight)
        {
            this->Move(walkSpeed, 0.0f);
        }
        else if (currentState == Character::StateWalkUpLeft)
        {
            this->Move(-walkSpeed, -walkSpeed);
        }
        else if (currentState == Character::StateWalkUpRight)
        {
            this->Move(walkSpeed, -walkSpeed);
        }
        else if (currentState == Character::StateWalkDownLeft)
        {
            this->Move(-walkSpeed, walkSpeed);
        }
        else if (currentState == Character::StateWalkDownRight)
        {
            this->Move(walkSpeed, walkSpeed);
        }
        else if (currentState == Character::StateRunUp)
        {
            this->Move(0.0f, -runSpeed);
        }
        else if (currentState == Character::StateRunDown)
        {
            this->Move(0.0f, runSpeed);
        }
        else if (currentState == Character::StateRunLeft)
        {
            this->Move(-runSpeed, 0.0f);
        }
        else if (currentState == Character::StateRunRight)
        {
            this->Move(runSpeed, 0.0f);
        }
        else if (currentState == Character::StateRunUpLeft)
        {
            this->Move(-runSpeed, -runSpeed);
        }
        else if (currentState == Character::StateRunUpRight)
        {
            this->Move(runSpeed, -runSpeed);
        }
        else if (currentState == Character::StateRunDownLeft)
        {
            this->Move(-runSpeed, runSpeed);
        }
        else if (currentState == Character::StateRunDownRight)
        {
            this->Move(runSpeed, runSpeed);
        }
        else
        {
            this->LogMessage("Unknown character state: " +
                             std::to_string(static_cast<uint8_t>(currentState.GetKey())) +
                             ":" + std::to_string(static_cast<uint8_t>(currentState.GetValue())));
        }
    }

    bool Character::ProcessMoveTo(bool isWalking) noexcept
    {
        // TODO: Use A* for this algorithm
        auto dx = this->_moveToDestinationX - this->_positionX;
        auto dy = this->_moveToDestinationY - this->_positionY;

        // TODO: Set these epsilon values somewhere...
        auto epsilon = isWalking ? this->_walkSpeed : this->_runSpeed;
        epsilon *= this->GetTimer()->GetLastFrameDurationInSeconds() * 2.0f;

        if (dx >= -epsilon && dx <= epsilon)
        {
            dx = 0.0f;
        }

        if (dy >= -epsilon && dy <= epsilon)
        {
            dy = 0.0f;
        }

        if (dx == 0.0f && dy == 0.0f)
        {
            auto state = isWalking ? shared::entities::CharacterStates::Walk :
                                     shared::entities::CharacterStates::Run;

            this->_stateMachine->CancelState(state);
            return true;
        }

        auto characterState = Character::StateIdle;

        if (dx < 0.0f && dy < 0.0f)
        {
            characterState = isWalking ? Character::StateWalkUpLeft : Character::StateRunUpLeft;
        }
        else if (dx < 0.0f && dy == 0.0f)
        {
            characterState = isWalking ? Character::StateWalkLeft : Character::StateRunLeft;
        }
        else if (dx < 0.0f && dy > 0.0f)
        {
            characterState = isWalking ? Character::StateWalkDownLeft : Character::StateRunDownLeft;
        }
        else if (dx == 0.0f && dy < 0.0f)
        {
            characterState = isWalking ? Character::StateWalkUp : Character::StateRunUp;
        }
        // dx == 0.0 && dy == 0.0 handled above...
        else if (dx == 0.0f && dy > 0.0f)
        {
            characterState = isWalking ? Character::StateWalkDown : Character::StateRunDown;
        }
        else if (dx > 0.0f && dy < 0.0f)
        {
            characterState = isWalking ? Character::StateWalkUpRight : Character::StateRunUpRight;
        }
        else if (dx > 0.0f && dy == 0.0f)
        {
            characterState = isWalking ? Character::StateWalkRight : Character::StateRunRight;
        }
        else if (dx > 0.0f && dy > 0.0f)
        {
            characterState = isWalking ? Character::StateWalkDownRight : Character::StateRunDownRight;
        }
        else
        {
            this->LogMessage("Invalid dx:dy: " + std::to_string(dx) + ":" + std::to_string(dy));
            return true;
        }

        this->_stateMachine->PushState(characterState);
        return false;
    }

    void Character::Move(float xDir, float yDir) noexcept
    {
        auto world = this->GetCurrentWorld();

        auto newX = this->_positionX + xDir;
        auto newY = this->_positionY + yDir;

        auto [_, nx, ny] = world->GetTileIndexesFromWorldPosition(newX, newY);

        // must have left the map or a hole in the island perhaps
        if (nx == -1 || ny == -1)
        {
            return;
        }

        auto newPlot = world->GetPlotIndexFromWorldPosition(newX, newY);

        auto plots = world->GetPlots();
        auto plot = plots->GetPlot(newPlot);

        auto currentState = this->_stateMachine->GetCurrentState();

        auto disallowedCharacterStates = plot->GetDisallowedCharacterStates();

        auto stateIter = std::find(disallowedCharacterStates.begin(), disallowedCharacterStates.end(),
                currentState.GetKey());

        if (stateIter != disallowedCharacterStates.end())
        {
            return;
        }

        this->_positionX = newX;
        this->_positionY = newY;

        world->TriggerActionTiles(newX, newY, this->shared_from_this());
    }

    void Character::SetScriptUpdateInterval(uint32_t interval) noexcept
    {
        this->_scriptStopwatch.Reset();
        this->_scriptStopwatch.SetTargetMilliseconds(interval);

        auto onTick = [this]()
        {
          if (!this->_script->CallFunction(shared::scripting::FunctionTypes::Update, {}))
          {
              this->LogMessage("Failed to call update function.");
              return;
          }
        };

        this->_scriptStopwatch.SetOnTick(onTick);

        this->_scriptStopwatch.Start();
    }

    void Character::SetPlayerId(uint32_t playerId) noexcept
    {
        this->_playerId = playerId;

        if (!this->IsPlayer())
        {
            return;
        }

        this->_playerTickStopwatch.Reset();
        // TODO: Have this value in a config setting perhaps?
        this->_playerTickStopwatch.SetTargetMilliseconds(1000);

        auto onTick = [this]()
        {
          if (!this->PersistPlayerState())
          {
              this->LogMessage("Failed to persist player state with player id: " + std::to_string(this->_playerId));
              return;
          }
        };

        this->_playerTickStopwatch.SetOnTick(onTick);

        this->_playerTickStopwatch.Start();
    }

    bool Character::PersistPlayerState() const noexcept
    {
        if (!this->IsPlayer())
        {
            this->LogMessage("This character is not a player with entity id: " + std::to_string(this->_entityId));
            return false;
        }

        // TODO: This calculation needs to be DRY as it is used in the `SetData` and `GetData`
        // functions in both the server and client and here and where the position is read from
        // the player database
        auto xPos = static_cast<int32_t>(this->_positionX * 10000.0f);
        auto yPos = static_cast<int32_t>(this->_positionY * 10000.0f);

        if (!this->_dataManager->UpdatePlayerState(this->_playerId, xPos, yPos))
        {
            this->LogMessage("Failed to update player state with player id: " + std::to_string(this->_playerId));
            return false;
        }

        return true;
    }

    bool Character::ConfirmAppearance(const std::shared_ptr<ActionAnimationsManager>& actionAnimationsManager) noexcept
    {
        const auto& type = this->GetCharacterType();

        if (this->_appearanceDetails.Hair == "random")
        {
            this->_appearanceDetails.Hair = actionAnimationsManager->GetAppearance(type, "hair");
        }

        if (this->_appearanceDetails.Body == "random")
        {
            this->_appearanceDetails.Body = actionAnimationsManager->GetAppearance(type, "body");
        }

        if (this->_appearanceDetails.ClothesTop == "random")
        {
            this->_appearanceDetails.ClothesTop = actionAnimationsManager->GetAppearance(type, "clothes_top");
        }

        if (this->_appearanceDetails.ClothesBottom == "random")
        {
            this->_appearanceDetails.ClothesBottom = actionAnimationsManager->GetAppearance(type, "clothes_bottom");
        }

        if (this->_appearanceDetails.Feet == "random")
        {
            this->_appearanceDetails.Feet = actionAnimationsManager->GetAppearance(type, "feet");
        }

        this->LogMessage("Updating appearance: " + std::to_string(this->_playerId));

        if (this->IsPlayer())
        {
            this->LogMessage("Player updating appearance: " + std::to_string(this->_playerId));

            if (!this->_dataManager->UpdatePlayerAppearanceDetails(this->_playerId, this->_appearanceDetails))
            {
                this->LogMessage("Failed to update player appearance details with player id: " + std::to_string(this->_playerId));
                return false;
            }
        }

        return true;
    }
}