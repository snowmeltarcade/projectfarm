#include "character.h"
#include "utils/util.h"
#include "engine/game.h"
#include "engine/player.h"
#include "engine/action_types.h"
#include "character_appearance_manager.h"
#include "character_appearance_library.h"

using namespace std::literals::string_literals;

namespace projectfarm::engine::entities
{
    void Character::OnTick() noexcept
    {
        this->ProcessState();

        this->UpdateLerp();

        this->_characterAppearanceManager->UpdateTileMap(this->_tileMap, this->_stateMachine);
        this->_tileMap->Tick();

        if (this->IsPlayer())
        {
            auto graphics = this->GetGraphics();
            auto camera = graphics->GetCamera();

            auto offsetX = camera->GetViewport().w * 0.5f;
            offsetX -= graphics->MetersToPixels(0.5f); // TODO: all characters are currently 1mx1m

            auto offsetY = camera->GetViewport().h * 0.5f;
            offsetY -= graphics->MetersToPixels(0.5f); // TODO: all characters are currently 1mx1m

            camera->SetPosition(graphics->MetersToPixels(this->_positionX) - offsetX,
                                graphics->MetersToPixels(this->_positionY) - offsetY);
        }
    }

    void Character::Render() noexcept
    {
        this->UpdateRenderPosition();
        this->_tileMap->SetRenderDetails(this->_renderX, this->_renderY);

        this->_tileMap->Render();
    }

    bool Character::Load() noexcept
    {
        this->_stateMachine = std::make_shared<StateMachineType>(StateIdle);

        this->_characterAppearanceManager = std::make_unique<CharacterAppearanceManager>(StateIdle);
        this->_characterAppearanceManager->SetLogger(this->_logger);
        this->_characterAppearanceManager->SetTileSetPool(this->_tileSetPool);

        this->_tileMap = std::make_shared<graphics::TileMap>();
        this->_tileMap->SetLogger(this->_logger);
        this->_tileMap->SetDataProvider(this->_dataProvider);
        this->_tileMap->SetGraphics(this->GetGraphics());
        this->_tileMap->SetRenderManager(this->_renderManager);
        this->_tileMap->SetTimer(this->GetTimer());

        auto tileWidthInPixels = static_cast<uint32_t>(this->GetGraphics()->MetersToPixels(this->_width));
        auto tileHeightInPixels = static_cast<uint32_t>(this->GetGraphics()->MetersToPixels(this->_height));

        // TODO: Hard coding the 5 layers and size is OK here?
        if (!this->_tileMap->LoadDirect(5, 1, 1,
                                        tileWidthInPixels, tileHeightInPixels))
        {
            this->LogMessage("Failed to load tilemap.");
            return false;
        }

        this->_tileMap->SetRenderToWorldSpace(this->_renderToWorldSpace);

        this->ReconfirmPixelSizes();

        // make sure we have an appearance until the server gives us one
        this->SetDetails(
                { "none", "none", "none", "none", "none" },
                true);

        return true;
    }

    void Character::Shutdown() noexcept
    {
        this->_tileMap->Shutdown();
    }

    // this data is from the server
    void Character::SetData(const std::vector<std::byte>& data) noexcept
    {
        uint32_t dataIndex {0};

        auto playerId = pfu::ReadUInt32(data, dataIndex);

        auto lerpPositionChange = pfu::ReadBool(data, dataIndex);
        auto x = pfu::ReadInt32(data, dataIndex) * 0.0001f;
        auto y = pfu::ReadInt32(data, dataIndex) * 0.0001f;

        auto stateKey = static_cast<shared::entities::CharacterStates>(pfu::ReadUInt32(data, dataIndex));
        auto stateValue = static_cast<shared::entities::CharacterStateValues>(pfu::ReadUInt32(data, dataIndex));

        this->_type = pfu::ReadString(data, dataIndex);

        this->_walkSpeed = pfu::ReadUInt32(data, dataIndex) * 0.0001f;
        this->_runSpeed = pfu::ReadUInt32(data, dataIndex) * 0.0001f;

        this->_stateMachine->ClearStates();
        this->_stateMachine->PushState({stateKey, stateValue});

        this->_playerId = playerId;

        // the first time we get a position, we want to snap into
        // place and not lerp there
        if (this->_hasReceivedFirstData && lerpPositionChange)
        {
            this->_isPositionLerping = true;
            this->_xPosLerper.Set(this->_positionX, x, 100);
            this->_yPosLerper.Set(this->_positionY, y, 100);
        }
        else
        {
            this->_isPositionLerping = false;
            // TODO: is this the right place to be doing this?
            this->_positionX = x;
            this->_positionY = y;
        }

        this->_hasReceivedFirstData = true;
    }

    // this data is being sent to the server
    std::vector<std::byte> Character::GetEntityData() const noexcept
    {
        std::vector<std::byte> data;

        pfu::WriteUInt32(data, this->_playerId);

        pfu::WriteString(data, this->_currentWorld->GetName(),
                         static_cast<uint32_t>(this->_currentWorld->GetName().size()));

        pfu::WriteInt32(data, static_cast<int32_t>(this->_positionX * 10000.0f));
        pfu::WriteInt32(data, static_cast<int32_t>(this->_positionY * 10000.0f));

        const auto& state = this->_stateMachine->GetCurrentState();
        pfu::WriteUInt32(data, static_cast<uint32_t>(state.GetKey()));
        pfu::WriteUInt32(data, static_cast<uint32_t>(state.GetValue()));

        return data;
    }

    void Character::ProcessAction(engine::ActionTypes action, bool actionAdded) noexcept
    {
        auto state = Character::StateIdle;

        if (action == engine::ActionTypes::Idle)
        {
            // the state machine has a default action of idle
        }
        else if (action == engine::ActionTypes::MoveUp)
        {
            state = Character::StateWalkUp;
        }
        else if (action == engine::ActionTypes::MoveDown)
        {
            state = Character::StateWalkDown;
        }
        else if (action == engine::ActionTypes::MoveLeft)
        {
            state = Character::StateWalkLeft;
        }
        else if (action == engine::ActionTypes::MoveRight)
        {
            state = Character::StateWalkRight;
        }
        else if (action == engine::ActionTypes::MoveUpLeft)
        {
            state = Character::StateWalkUpLeft;
        }
        else if (action == engine::ActionTypes::MoveUpRight)
        {
            state = Character::StateWalkUpRight;
        }
        else if (action == engine::ActionTypes::MoveDownLeft)
        {
            state = Character::StateWalkDownLeft;
        }
        else if (action == engine::ActionTypes::MoveDownRight)
        {
            state = Character::StateWalkDownRight;
        }
        else if (action == engine::ActionTypes::MoveUpFast)
        {
            state = Character::StateRunUp;
        }
        else if (action == engine::ActionTypes::MoveDownFast)
        {
            state = Character::StateRunDown;
        }
        else if (action == engine::ActionTypes::MoveLeftFast)
        {
            state = Character::StateRunLeft;
        }
        else if (action == engine::ActionTypes::MoveRightFast)
        {
            state = Character::StateRunRight;
        }
        else if (action == engine::ActionTypes::MoveUpLeftFast)
        {
            state = Character::StateRunUpLeft;
        }
        else if (action == engine::ActionTypes::MoveUpRightFast)
        {
            state = Character::StateRunUpRight;
        }
        else if (action == engine::ActionTypes::MoveDownLeftFast)
        {
            state = Character::StateRunDownLeft;
        }
        else if (action == engine::ActionTypes::MoveDownRightFast)
        {
            state = Character::StateRunDownRight;
        }
        else
        {
            this->LogMessage("Unknown action: " + std::to_string(static_cast<uint32_t>(action)));
            return;
        }

        if (actionAdded)
        {
            this->_stateMachine->PushState(state);
        }
        else
        {
            this->_stateMachine->CancelState(state.GetKey());
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
            // TODO: play idle animations etc...
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

    void Character::UpdateLerp() noexcept
    {
        if (!this->_isPositionLerping)
        {
            return;
        }

        auto [x, xFinished] = this->_xPosLerper.Get(this->GetTimer()->GetLastFrameDurationInMilliseconds());
        auto [y, yFinished] = this->_yPosLerper.Get(this->GetTimer()->GetLastFrameDurationInMilliseconds());

        this->_positionX = x;
        this->_positionY = y;

        if (xFinished && yFinished)
        {
            this->_isPositionLerping = false;
        }
    }

    void Character::UpdateRenderPosition() noexcept
    {
        this->_renderX = this->GetGraphics()->MetersToPixels(this->_positionX);
        this->_renderY = this->GetGraphics()->MetersToPixels(this->_positionY);
    }

    void Character::ReconfirmPixelSizes() noexcept
    {
        auto tileWidthInPixels = static_cast<uint32_t>(this->GetGraphics()->MetersToPixels(this->_width));
        auto tileHeightInPixels = static_cast<uint32_t>(this->GetGraphics()->MetersToPixels(this->_height));

        this->_tileMap->SetRenderDetails(this->_renderX, this->_renderY,
                                         tileWidthInPixels, tileHeightInPixels,
                                         graphics::RenderOriginPoints::TopLeft);

        this->_tileMap->SetTileSize(tileWidthInPixels, tileHeightInPixels);
    }

    void Character::Move(float xDir, float yDir) noexcept
    {
        auto world = this->_currentWorld;

        auto newX = this->_positionX + xDir;
        auto newY = this->_positionY + yDir;

        auto [nx, ny] = world->GetTileIndexesFromWorldPosition(newX, newY);

        // must have left the map or a hole in the island perhaps
        if (nx == -1 || ny == -1)
        {
            return;
        }

        auto newPlot = world->GetPlotIndexFromWorldPosition(newX, newY);

        auto plots = world->GetPlots();
        auto plot = plots->GetByIndex(newPlot);

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
    }

    void Character::SetDetails(const shared::entities::CharacterAppearanceDetails& details, bool updateTileMap) noexcept
    {
        this->ApplyAppearanceFile(details.Hair, 4);
        this->ApplyAppearanceFile(details.Body, 3);
        this->ApplyAppearanceFile(details.ClothesTop, 2);
        this->ApplyAppearanceFile(details.ClothesBottom, 1);
        this->ApplyAppearanceFile(details.Feet, 0);

        if (updateTileMap)
        {
            this->_characterAppearanceManager->UpdateTileMap(this->_tileMap, this->_stateMachine, true);
        }
    }

    void Character::ApplyAppearanceFile(const std::string& name, uint8_t layerIndex) noexcept
    {
        if (name == "none")
        {
            this->_characterAppearanceManager->ClearCharacterAppearance(layerIndex);
            return;
        }

        std::filesystem::path path;

        if (name == "random")
        {
            auto library = this->GetGame()->GetCharacterAppearanceLibrary();

            auto type = "chicken"s;
            auto part = this->GetPartFromLayerIndex(layerIndex);
            auto paths = library->GetCharacterAppearanceFilePaths(type, part);
            if (paths.empty())
            {
                this->LogMessage("Failed to get character appearance file paths for type: '" + type + "' and part: '" + part + "'");
                return;
            }

            path = paths[0];
        }
        else
        {
            path = this->_dataProvider->GetCharacterActionAnimationLocationsFromName(name);
        }

        if (!this->_characterAppearanceManager->LoadCharacterAppearanceFile(path, layerIndex))
        {
            this->LogMessage("Failed to load character appearance file with path: " + path.u8string());
            return;
        }
    }

    std::string Character::GetPartFromLayerIndex(uint8_t layerIndex) noexcept
    {
        switch (layerIndex)
        {
            case 0: return "feet";
            case 1: return "clothes_bottom";
            case 2: return "clothes_top";
            case 3: return "body";
            case 4: return "hair";
            default: return "none";
        }
    }
}