#include <string>
#include <sstream>

#include "world_scene.h"
#include "engine/game.h"
#include "networking/packet_factory.h"
#include "networking/packets/server_client_load_world.h"
#include "networking/packets/server_client_entity_update.h"
#include "networking/packets/server_client_player_joined_world.h"
#include "networking/packets/server_client_player_left_world.h"
#include "networking/packets/server_client_remove_entity_from_world.h"
#include "networking/packets/server_client_character_set_details.h"
#include "networking/packets/client_server_chatbox_message.h"
#include "networking/packets/server_client_chatbox_message.h"
#include "engine/action_input_sources/action_input_source_keyboard.h"
#include "time/clock.h"

using namespace std::literals;

namespace projectfarm::scenes::implemented_scenes
{
    void WorldScene::UpdateInput()
    {
        auto input = this->GetSceneManager()->GetGame()->GetKeyboardInput();

        if (input->IsKeyPressed_Keycode(SDLK_ESCAPE))
        {
            this->_ui->SendMessage("close_current", {});
        }

        if (input->IsKeyPressed_Keycode(SDLK_RETURN) ||
            input->IsKeyPressed_Keycode(SDLK_RETURN2))
        {
            this->LogMessage("Enter pressed.......");
            this->_ui->HandleKeyPress_Enter();
        }

        this->_ui->HandleKeyPress();
    }

    void WorldScene::ProcessPacket(const std::shared_ptr<shared::networking::Packet>& packet)
    {
        auto sceneManager = this->GetSceneManager();

        if (!this->ValidatePacket(packet))
        {
            return;
        }

        const auto packetType = packet->GetPacketType();

        if (packetType == shared::networking::PacketTypes::ServerClientLoadWorld)
        {
            this->HandleServerClientLoadWorldPacket(packet);
        }
        else if (packetType == shared::networking::PacketTypes::ServerClientEntityUpdate)
        {
            this->HandleServerClientEntityUpdatePacket(packet);
        }
        else if (packetType == shared::networking::PacketTypes::ServerClientPlayerJoinedWorld)
        {
            this->HandleServerClientPlayerJoinedWorldPacket(packet);
        }
        else if (packetType == shared::networking::PacketTypes::ServerClientPlayerLeftWorld)
        {
            this->HandleServerClientPlayerLeftWorldPacket(packet);
        }
        else if (packetType == shared::networking::PacketTypes::ServerClientRemoveEntityFromWorld)
        {
            this->HandleServerClientRemoveEntityFromWorldPacket(packet);
        }
        else if (packetType == shared::networking::PacketTypes::ServerClientCharacterSetDetails)
        {
            this->HandleServerClientCharacterSetDetailsPacket(packet);
        }
        else if (packetType == shared::networking::PacketTypes::ServerClientChatboxMessage)
        {
            this->HandleServerClientChatboxMessagePacket(packet);
        }
    }

    bool WorldScene::ValidatePacket(const std::shared_ptr<shared::networking::Packet>& packet) const
    {
        auto isValid = false;

        const auto packetType = packet->GetPacketType();

        if (packetType == shared::networking::PacketTypes::ServerClientLoadWorld ||
            packetType == shared::networking::PacketTypes::ServerClientEntityUpdate ||
            packetType == shared::networking::PacketTypes::ServerClientPlayerJoinedWorld ||
            packetType == shared::networking::PacketTypes::ServerClientPlayerLeftWorld ||
            packetType == shared::networking::PacketTypes::ServerClientRemoveEntityFromWorld ||
            packetType == shared::networking::PacketTypes::ServerClientCharacterSetDetails ||
            packetType == shared::networking::PacketTypes::ServerClientChatboxMessage)
        {
            isValid = true;
        }

        if (!isValid)
        {
            std::stringstream ss;
            ss << "Received invalid packet: ";
            ss << static_cast<int>(packetType);

            this->LogMessage(ss.str());
        }

        return isValid;
    }

    void WorldScene::HandleServerClientLoadWorldPacket(const std::shared_ptr<shared::networking::Packet>& packet) const
    {
        const auto serverClientLoadWorld
            { std::static_pointer_cast<shared::networking::packets::ServerClientLoadWorldPacket>(packet) };

        auto worldToLoad = serverClientLoadWorld->GetWorldToLoad();

        this->_logger->LogMessage("Received packet to load this world: " + worldToLoad);

        this->GetSceneManager()->AddLoadSceneParameter(WorldScene::LoadParameter_WorldName, worldToLoad);
        this->GetSceneManager()->QueueLoadScene(projectfarm::scenes::SceneTypes::Loading);
    }

    void WorldScene::HandleServerClientEntityUpdatePacket(const std::shared_ptr<shared::networking::Packet>& packet)
    {
        const auto serverClientEntityUpdate
                { std::static_pointer_cast<shared::networking::packets::ServerClientEntityUpdatePacket>(packet) };

        auto entityId = serverClientEntityUpdate->GetEntityId();
        auto playerId = serverClientEntityUpdate->GetPlayerId();
        auto worldName = serverClientEntityUpdate->GetWorldName();
        auto entityType = serverClientEntityUpdate->GetEntityType();
        auto lastUpdateTime = serverClientEntityUpdate->GetTimeOfUpdate();
        auto data = serverClientEntityUpdate->GetEntityData();

        if (worldName != this->_world->GetName())
        {
            // perhaps the packet was sent around the time the world changed
            return;
        }

        if (!this->_world->UpdateEntity(entityId, playerId, entityType, lastUpdateTime, data))
        {
            //this->LogMessage("Failed to update entity: " + serverClientEntityUpdate->GetDebugData());
        }

        auto details = this->_cachedCharacterDetails.find(entityId);
        if (details != this->_cachedCharacterDetails.end())
        {
            if (auto character = this->_world->GetCharacterByEntityId(entityId); character)
            {
                (*character)->SetDetails(details->second, true);

                this->_cachedCharacterDetails.erase(entityId);
            }
        }
    }

    void WorldScene::HandleServerClientPlayerJoinedWorldPacket(const std::shared_ptr<shared::networking::Packet>& packet) const
    {
        const auto serverClientPlayerJoinedWorld
            { std::static_pointer_cast<shared::networking::packets::ServerClientPlayerJoinedWorld>(packet) };

        auto worldName = serverClientPlayerJoinedWorld->GetWorldName();

        if (worldName != this->_world->GetName())
        {
            return;
        }

        this->LogMessage("Player: " + std::to_string(serverClientPlayerJoinedWorld->GetPlayerId()) +
                         " has joined this world: " + worldName);
    }

    void WorldScene::HandleServerClientPlayerLeftWorldPacket(const std::shared_ptr<shared::networking::Packet>& packet) const
    {
        const auto serverClientPlayerLeftWorld
            { std::static_pointer_cast<shared::networking::packets::ServerClientPlayerLeftWorld>(packet) };

        auto worldName = serverClientPlayerLeftWorld->GetWorldName();

        if (worldName != this->_world->GetName())
        {
            return;
        }

        this->LogMessage("Player: " + std::to_string(serverClientPlayerLeftWorld->GetPlayerId()) +
                         " has exited this world: " + worldName);
    }

    void WorldScene::HandleServerClientRemoveEntityFromWorldPacket(const std::shared_ptr<shared::networking::Packet>& packet)
    {
        const auto serverClientRemoveEntityFromWorld
            { std::static_pointer_cast<shared::networking::packets::ServerClientRemoveEntityFromWorld>(packet) };

        auto worldName = serverClientRemoveEntityFromWorld->GetWorldName();

        if (worldName != this->_world->GetName())
        {
            return;
        }

        this->LogMessage("Removing entity with id: " + std::to_string(serverClientRemoveEntityFromWorld->GetEntityId()) +
                         " from world: " + worldName);

        this->_world->RemoveEntity(serverClientRemoveEntityFromWorld->GetEntityId());
    }

    void WorldScene::HandleServerClientCharacterSetDetailsPacket(const std::shared_ptr<shared::networking::Packet>& packet)
    {
        const auto serverClientCharacterSetDetails
                { std::static_pointer_cast<shared::networking::packets::ServerClientCharacterSetDetailsPacket>(packet) };

        auto worldName = serverClientCharacterSetDetails->GetWorldName();

        if (worldName != this->_world->GetName())
        {
            return;
        }

        this->LogMessage("Updating character details with entity id: " + std::to_string(serverClientCharacterSetDetails->GetEntityId()));

        auto entityId = serverClientCharacterSetDetails->GetEntityId();

        auto details = serverClientCharacterSetDetails->GetAppearanceDetails();

        auto character = this->_world->GetCharacterByEntityId(entityId);
        if (!character)
        {
            this->_cachedCharacterDetails[entityId] = std::move(details);
            return;
        }

        (*character)->SetDetails(details, true);
    }

    void WorldScene::HandleServerClientChatboxMessagePacket(const std::shared_ptr<shared::networking::Packet>& packet)
    {
        const auto serverClientChatboxMessage
                { std::static_pointer_cast<shared::networking::packets::ServerClientChatboxMessagePacket>(packet) };

        auto username = serverClientChatboxMessage->GetUsername();
        auto message = serverClientChatboxMessage->GetMessage();
        auto serverTimeMilliseconds = serverClientChatboxMessage->GetServerTime();

        auto serverTimeString = shared::time::Clock::LocalTimeAsShortString(serverTimeMilliseconds);

        this->_ui->SendMessage("chatbox", { message, username, serverTimeString });
    }

    bool WorldScene::Initialize()
    {
        this->LogMessage("Initializing main game scene.");

        if (!this->SetupUI())
        {
            this->LogMessage("Failed to setup UI.");
            return false;
        }

        auto worldName = this->_loadParameters[WorldScene::LoadParameter_WorldName];
        if (!this->LoadWorldFile(worldName))
        {
            this->LogMessage("Failed to load the world file with name: " + worldName);
            return false;
        }

        this->TellServerWorldHasBeenLoaded();

        auto k = this->GetGame()->GetActionManager()->GetInputSourceOfType<
                engine::action_input_sources::ActionInputSourceKeyboard>();

        k->Clear();

        auto keyBinding = std::make_shared<engine::action_input_sources::KeyMapping>();
        keyBinding->AddBinding(SDLK_s, engine::ActionTypes::MoveDown);
        keyBinding->AddBinding(SDLK_w, engine::ActionTypes::MoveUp);
        keyBinding->AddBinding(SDLK_d, engine::ActionTypes::MoveRight);
        keyBinding->AddBinding(SDLK_a, engine::ActionTypes::MoveLeft);
        keyBinding->AddBinding({ SDLK_w, SDLK_a }, engine::ActionTypes::MoveUpLeft);
        keyBinding->AddBinding({ SDLK_w, SDLK_d }, engine::ActionTypes::MoveUpRight);
        keyBinding->AddBinding({ SDLK_s, SDLK_a }, engine::ActionTypes::MoveDownLeft);
        keyBinding->AddBinding({ SDLK_s, SDLK_d }, engine::ActionTypes::MoveDownRight);
        keyBinding->AddBinding({ SDLK_s, SDLK_LSHIFT }, engine::ActionTypes::MoveDownFast);
        keyBinding->AddBinding({ SDLK_w, SDLK_LSHIFT }, engine::ActionTypes::MoveUpFast);
        keyBinding->AddBinding({ SDLK_d, SDLK_LSHIFT }, engine::ActionTypes::MoveRightFast);
        keyBinding->AddBinding({ SDLK_a, SDLK_LSHIFT }, engine::ActionTypes::MoveLeftFast);
        keyBinding->AddBinding({ SDLK_w, SDLK_a, SDLK_LSHIFT }, engine::ActionTypes::MoveUpLeftFast);
        keyBinding->AddBinding({ SDLK_w, SDLK_d, SDLK_LSHIFT }, engine::ActionTypes::MoveUpRightFast);
        keyBinding->AddBinding({ SDLK_s, SDLK_a, SDLK_LSHIFT }, engine::ActionTypes::MoveDownLeftFast);
        keyBinding->AddBinding({ SDLK_s, SDLK_d, SDLK_LSHIFT }, engine::ActionTypes::MoveDownRightFast);

        k->PushKeyMapping(keyBinding);

        this->ReconfirmPixelSizes();

        this->LogMessage("Initialized main game scene.");

        return true;
    }

    bool WorldScene::LoadWorldFile(const std::string& worldName)
    {
        this->LogMessage("Loading world: " + worldName);

        this->_world = std::make_shared<engine::world::World>();
        this->_world->SetLogger(this->_logger);
        this->_world->SetDataProvider(this->_dataProvider);
        this->_world->SetGraphics(this->GetGraphics());
        this->_world->SetRenderManager(this->_renderManager);
        this->_world->SetTileSetPool(this->GetGraphics()->GetTileSetPool());
        this->_world->SetTimer(this->GetTimer());
        this->_world->SetGame(this->GetGame());
        this->_world->SetScene(this->shared_from_this());

        if (!this->_world->Load(worldName))
        {
            this->LogMessage("Failed to load world: " + worldName);
            return false;
        }

        this->LogMessage("Loaded world: " + this->_world->GetName());

        return true;
    }

    void WorldScene::PrepareRender()
    {
        if (!this->_renderManager->AddRenderable(0,
                                                 std::static_pointer_cast<graphics::Renderable>(this->_world)))
        {
            this->LogMessage("Failed to add world as a renderable.");
            return;
        }

        this->_renderManager->AddUI(this->_ui);
    }

    void WorldScene::Tick()
    {
        this->_world->Tick();

        this->UpdateUI();

        this->UpdateDebugInfo();
    }

    void WorldScene::Destroy()
    {
        this->_ui->Shutdown();

        this->_world->Shutdown();
    }

    void WorldScene::Quit()
    {
        this->_shouldQuit = true;
    }

    bool WorldScene::SetupUI()
    {
        this->_ui = std::make_shared<projectfarm::graphics::ui::UI>();

        this->_ui->SetLogger(this->_logger);
        this->_ui->SetGraphics(this->GetGraphics());
        this->_ui->SetDataProvider(this->_dataProvider);
        this->_ui->SetFontManager(this->_fontManager);
        this->_ui->SetScriptSystem(this->_scriptSystem);
        this->_ui->SetScene(this->shared_from_this());
        if (!this->_ui->Initialize())
        {
            this->LogMessage("Failed to initialize UI.");
            return false;
        }

        if (!this->_ui->LoadFromFile("world_screen"))
        {
            this->LogMessage("Failed to load ui from file: world_screen");
            return false;
        }

        return true;
    }

    void WorldScene::UpdateDebugInfo()
    {
        if (!this->_computeDebugInfo)
        {
            return;
        }

        std::stringstream ss;

        ss << "FPS: " << this->GetTimer()->GetFPS() << std::endl;
        ss << "Draw Calls: " << this->GetDebugInformation()->GetLastFrameDrawCalls() << std::endl;
        ss << "Drawn Tiles: " << this->GetDebugInformation()->GetLastFrameTilesDrawn() << std::endl;
        ss << "Pooled Textures: " << this->GetDebugInformation()->GetNumberOfTexturesLoaded();

        auto debugInfo = ss.str();

        this->_ui->SetSimpleBinding("debug_info", debugInfo);
    }

    void WorldScene::TellServerWorldHasBeenLoaded()
    {
        const auto clientServerWorldLoadedPacket = shared::networking::PacketFactory::CreatePacket(
                shared::networking::PacketTypes::ClientServerWorldLoaded);

		this->GetSceneManager()->SendPacketToServer(clientServerWorldLoadedPacket);
    }

    void WorldScene::OnPlayerCharacterAdd(const std::shared_ptr<engine::entities::Character>& character) noexcept
    {
        auto ae = std::static_pointer_cast<engine::ActionableEntity>(character);

        this->GetSceneManager()->GetGame()->GetActionManager()->RegisterActionableEntity(ae);

        auto k = this->GetSceneManager()->GetGame()->GetActionManager()->GetInputSourceOfType<
                engine::action_input_sources::ActionInputSourceKeyboard>();

        // TODO: remove any old characters??
        k->PushActionableEntity(ae);
    }

    void WorldScene::ReconfirmPixelSizes() noexcept
    {
        this->_world->ReconfirmPixelSizes();
    }

    std::string WorldScene::SendUIMessage(const std::string& key,
                                          const std::vector<projectfarm::shared::scripting::FunctionParameter>& parameters) noexcept
    {
        if (key == "exit")
        {
            if (parameters.size() != 0)
            {
                this->LogMessage("Invalid number of parameters for `exit` scene message.");
                return "Invalid number of parameters.";
            }

            this->Quit();
        }
        else if (key == "compute_debug_info")
        {
            if (parameters.size() != 1)
            {
                this->LogMessage("Invalid number of parameters for `compute_debug_info` scene message.");
                return "Invalid number of parameters.";
            }

            this->_computeDebugInfo = parameters[0].GetAsString() == "true";
        }
        else if (key == "chatbox")
        {
            if (parameters.size() != 1)
            {
                this->LogMessage("Invalid number of parameters for `chatbox` scene message.");
                return "Invalid number of parameters.";
            }

            auto message = parameters[0].GetAsString();

            const auto clientServerChatboxMessagePacket = std::static_pointer_cast<shared::networking::packets::ClientServerChatboxMessagePacket>(
                    shared::networking::PacketFactory::CreatePacket(
                            shared::networking::PacketTypes::ClientServerChatboxMessage));

            clientServerChatboxMessagePacket->SetMessage(message);

            this->GetSceneManager()->SendPacketToServer(clientServerChatboxMessagePacket);
        }
        else if (key == "chatbox_command")
        {
            if (parameters.size() < 1)
            {
                this->LogMessage("Invalid number of parameters for `chatbox_command` scene message.");
                return "Invalid number of parameters.";
            }

            auto command = parameters[0].GetAsString();

            std::vector<std::string> args;
            args.reserve(parameters.size());

            for (const auto& p : parameters)
            {
                args.push_back(p.GetAsString());
            }

            auto result = this->ProcessChatboxCommand(command, args);
            return result;
        }

        return "";
    }

    void WorldScene::UpdateUI() noexcept
    {
        auto mouseInput = this->GetGame()->GetMouseInput();
        auto [x, y] = mouseInput->GetPosition();

        if (mouseInput->IsButtonClicked())
        {
            auto k = this->GetGame()->GetActionManager()->GetInputSourceOfType<
                    engine::action_input_sources::ActionInputSourceKeyboard>();

            if (this->_ui->HandleClick(x, y))
            {
                if (!this->_isUIInFocus)
                {
                    // the UI has no action map
                    auto keyBinding = std::make_shared<engine::action_input_sources::KeyMapping>();
                    k->PushKeyMapping(keyBinding);
                }

                this->_isUIInFocus = true;
            }
            else
            {
                if (this->_isUIInFocus)
                {
                    k->PopKeyMapping();
                }

                this->_isUIInFocus = false;
            }
        }

        mouseInput->ResetButtonClicked();

        if (mouseInput->IsButtonDown())
        {
            this->_ui->HandleDrag(x, y);
        }
        else
        {
            // TODO: Should this be called each frame?
            this->_ui->HandleMouseUp();

            this->_ui->HandleMoveMouse(x, y);
        }
    }

    std::string WorldScene::ProcessChatboxCommand(const std::string& command,
                                                  const std::vector<std::string>&) noexcept
    {
        if (command == "exit")
        {
            this->Quit();
            return "Quiting...";
        }
        else if (command == "debuginfoon")
        {
            this->_ui->SendMessage("show_debug_info", { "on" });

            return "Displayed debug info.";
        }
        else if (command == "debuginfooff")
        {
            this->_ui->SendMessage("show_debug_info", { "off" });

            return "Hidden debug info.";
        }

        return "Unknown parameter: "s + command;
    }
}