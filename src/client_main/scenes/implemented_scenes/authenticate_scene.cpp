#include <SDL.h>
#include <sstream>

#include "loading_scene.h"
#include "engine/game.h"
#include "networking/packet_factory.h"
#include "networking/packets/client_server_test_udp.h"
#include "networking/packets/client_server_player_authenticate.h"
#include "networking/packets/client_server_request_hashed_password.h"
#include "networking/packets/server_client_set_player_details.h"
#include "networking/packets/server_client_load_world.h"
#include "networking/packets/server_client_send_hashed_password.h"
#include "graphics/ui/texture.h"
#include "authenticate_scene.h"
#include "world_scene.h"

using namespace std::literals;

namespace projectfarm::scenes::implemented_scenes
{
    void AuthenticateScene::UpdateInput()
    {
        auto input = this->GetSceneManager()->GetGame()->GetKeyboardInput();

        if (input->IsKeyDown_Keycode(SDLK_ESCAPE))
        {
            this->Quit();
            return;
        }

        this->_ui->HandleKeyPress();
    }

    void AuthenticateScene::ProcessPacket(const std::shared_ptr<shared::networking::Packet> &packet)
    {
        if (!this->ValidatePacket(packet))
        {
            return;
        }

        const auto packetType = packet->GetPacketType();

        if (packetType == shared::networking::PacketTypes::ServerClientLoadWorld)
        {
            this->HandleServerClientLoadWorldPacket(packet);
        }
        else if (packetType == shared::networking::PacketTypes::ServerClientSetPlayerDetails)
        {
            this->HandleServerClientSetPlayerDetailsPacket(packet);
        }
        else if (packetType == shared::networking::PacketTypes::ServerClientSendHashedPassword)
        {
            this->HandleServerClientSendHashedPasswordPacket(packet);
        }
    }

    bool AuthenticateScene::ValidatePacket(const std::shared_ptr<shared::networking::Packet> &packet) const
    {
        auto isValid = false;

        const auto packetType = packet->GetPacketType();

        if (packetType == shared::networking::PacketTypes::ServerClientLoadWorld ||
            packetType == shared::networking::PacketTypes::ServerClientSetPlayerDetails ||
            packetType == shared::networking::PacketTypes::ServerClientSendHashedPassword)
        {
            isValid = true;
        }

        if (!isValid)
        {
            std::stringstream ss;
            ss << "Received invalid packet: ";
            ss << static_cast<int>(packetType);

            this->GetSceneManager()->LogMessage(ss.str());
        }

        return isValid;
    }

    void AuthenticateScene::HandleServerClientSetPlayerDetailsPacket(const std::shared_ptr<shared::networking::Packet> &packet)
    {
        const auto serverClientSetPlayerDetails =
                std::static_pointer_cast<shared::networking::packets::ServerClientSetPlayerDetails>(packet);

        auto playerId = serverClientSetPlayerDetails->GetPlayerId();

        this->_logger->LogMessage("Setting player details. Player Id: " + std::to_string(playerId));

        auto &player = this->GetSceneManager()->GetGame()->GetPlayer();

        player.SetPlayerId(playerId);

        this->_loginStopwatch.Stop();
        this->_loggingIn = false;

        this->_performUdpTest = true;
    }

    void AuthenticateScene::HandleServerClientLoadWorldPacket(const std::shared_ptr<shared::networking::Packet> &packet)
    {
        const auto serverClientLoadWorld =
                std::static_pointer_cast<shared::networking::packets::ServerClientLoadWorldPacket>(packet);

        auto worldToLoad = serverClientLoadWorld->GetWorldToLoad();

        this->_logger->LogMessage("Received packet to load this world: " + worldToLoad);

        this->LoadNewWorld(worldToLoad);
    }

    void AuthenticateScene::HandleServerClientSendHashedPasswordPacket(const std::shared_ptr<shared::networking::Packet> &packet)
    {
        const auto serverClientSendHashedPasswordPacket =
                std::static_pointer_cast<shared::networking::packets::ServerClientSendHashedPasswordPacket>(packet);

        auto userName = serverClientSendHashedPasswordPacket->GetUserName();
        auto hashedPassword = serverClientSendHashedPasswordPacket->GetHashedPassword();

        if (userName != this->_userName)
        {
            this->LogMessage("Received username: " + userName + " does not match expected username: " + this->_userName);
            return;
        }

        // TODO: If the hashed password is empty, present a user registration screen
        if (hashedPassword.empty())
        {
            auto res = this->_cryptoProvider->QuickHash(this->_password);
            if (!res)
            {
                this->LogMessage("Failed to hash password.");
                return;
            }

            this->SendServerAuthenticatePacket(*res);
        }
        else if (this->_cryptoProvider->Compare(this->_password, hashedPassword))
        {
            this->LogMessage("Passwords Match :)");
            this->SendServerAuthenticatePacket(hashedPassword);
        }
        else
        {
            this->LogMessage("Passwords Don't Match :(");
        }
    }

    bool AuthenticateScene::Initialize()
    {
        this->LogMessage("Initializing loading scene scene.");

        if (!this->SetupUI())
        {
            this->LogMessage("Failed to setup UI.");
            return false;
        }

        if (auto iter = this->_loadParameters.find(AuthenticateScene::LoadParameter_UserName);
            iter != this->_loadParameters.end())
        {
            this->_userName = iter->second;
        }

        if (auto iter = this->_loadParameters.find(AuthenticateScene::LoadParameter_Password);
            iter != this->_loadParameters.end())
        {
            this->_password = iter->second;
        }

        if (!this->_userName.empty() && !this->_password.empty())
        {
            this->_shouldLogIn = true;
            this->LogMessage("Credentials provided. Logging in...");
        }

        this->LogMessage("Initialized loading scene scene.");

        return true;
    }

    void AuthenticateScene::PrepareRender()
    {
        this->_renderManager->AddUI(this->_ui);
    }

    void AuthenticateScene::Tick()
    {
        if (this->_isLoadingNewScene)
        {
            return;
        }

        if (this->_shouldLogIn)
        {
            if (!this->StartLoggingIn())
            {
                this->LogMessage("Failed to start logging in.");
                return;
            }
        }
        else if (this->_loggingIn)
        {
            this->_loginStopwatch.Tick();
        }
        else if (this->_performUdpTest)
        {
            this->PerformUdpTest();
        }

        auto mouseInput = this->GetGame()->GetMouseInput();
        auto [x, y] = mouseInput->GetPosition();

        if (mouseInput->IsButtonClicked())
        {
            this->_ui->HandleClick(x, y);
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

    void AuthenticateScene::Destroy()
    {
        this->_ui->Shutdown();
    }

    void AuthenticateScene::Quit()
    {
        this->_shouldQuit = true;

        this->Destroy();
    }

    bool AuthenticateScene::SetupUI()
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

        if (!this->_ui->LoadFromFile("authenticate_screen"))
        {
            this->LogMessage("Failed to load ui from file: authenticate_screen");
            return false;
        }

        this->SetSceneStatus("");

        return true;
    }

    void AuthenticateScene::LoadNewWorld(const std::string &worldName) noexcept
    {
        this->GetSceneManager()->AddLoadSceneParameter(WorldScene::LoadParameter_WorldName, worldName);
        this->GetSceneManager()->QueueLoadScene(projectfarm::scenes::SceneTypes::WorldScene);

        this->_isLoadingNewScene = true;

        this->SetSceneStatus("Loading world...");
    }

    bool AuthenticateScene::StartLoggingIn() noexcept
    {
        if (!this->_shouldLogIn)
        {
            this->LogMessage("Already logging in.");
            return false;
        }

        this->LogMessage("Starting to log in...");

        if (!this->SetupLoggingInUI())
        {
            this->LogMessage("Failed to setup logging in UI.");
            return false;
        }

        this->SendServerRequestHashedPasswordPacket();

        return true;
    }

    bool AuthenticateScene::SetupLoggingInUI() noexcept
    {
        auto loginText = "Logging in"s;
        this->SetSceneStatus(loginText);

        this->_shouldLogIn = false;

        auto loginOnTick = [this, loginText]() {
            constexpr auto numberOfFullStops = 3u;

            if (++this->_loginStopwatchCounter > numberOfFullStops)
            {
                this->_loginStopwatchCounter = 0;
            }

            auto fullstops {""s};
            for (auto i = 0u; i < this->_loginStopwatchCounter; ++i)
            {
                fullstops += ".";
            }

            for (auto i = this->_loginStopwatchCounter; i <= numberOfFullStops; ++i)
            {
                fullstops += " ";
            }

            this->SetSceneStatus(loginText);
        };

        this->_loginStopwatch.Reset();
        this->_loginStopwatch.SetOnTick(loginOnTick);
        this->_loginStopwatch.SetTargetMilliseconds(1000);

        this->_loginStopwatchCounter = 0;

        this->_loginStopwatch.Start();

        this->_loggingIn = true;

        return true;
    }

    void AuthenticateScene::SendServerAuthenticatePacket(const std::string &hashedPassword) noexcept
    {
        const auto clientServerPlayerAuthenticatePacket = std::static_pointer_cast<shared::networking::packets::ClientServerPlayerAuthenticatePacket>(shared::networking::PacketFactory::CreatePacket(
                shared::networking::PacketTypes::ClientServerPlayerAuthenticate));

        clientServerPlayerAuthenticatePacket->SetUserName(this->_userName);
        clientServerPlayerAuthenticatePacket->SetHashedPassword(hashedPassword);

        this->GetSceneManager()->SendPacketToServer(clientServerPlayerAuthenticatePacket);
    }

    void AuthenticateScene::SendServerRequestHashedPasswordPacket() noexcept
    {
        const auto clientServerRequestHashedPasswordPacket = std::static_pointer_cast<shared::networking::packets::ClientServerRequestHashedPasswordPacket>(shared::networking::PacketFactory::CreatePacket(
                shared::networking::PacketTypes::ClientServerRequestHashedPassword));

        clientServerRequestHashedPasswordPacket->SetUserName(this->_userName);

        this->GetSceneManager()->SendPacketToServer(clientServerRequestHashedPasswordPacket);
    }

    void AuthenticateScene::PerformUdpTest() noexcept
    {
        this->LogMessage("Performing UDP Test...");

        this->_udpTestCounter += this->GetTimer()->GetLastFrameDurationInMilliseconds();

        if (this->_udpTestCounter <= 100)
        {
            return;
        }

        auto clientServerTestUdp = std::static_pointer_cast<shared::networking::packets::ClientServerTestUdp>(
                shared::networking::PacketFactory::CreatePacket(shared::networking::PacketTypes::ClientServerTestUdp));

        auto playerId = this->GetSceneManager()->GetGame()->GetPlayer().GetPlayerId();

        clientServerTestUdp->SetPlayerId(playerId);

        this->GetSceneManager()->SendPacketToServer(clientServerTestUdp);
    }

    std::string AuthenticateScene::SendUIMessage(const std::string& key,
                                                 const std::vector<projectfarm::shared::scripting::FunctionParameter>& parameters) noexcept
    {
        if (key == "login")
        {
            if (parameters.size() != 2)
            {
                this->LogMessage("Invalid number of parameters for `login` scene message.");
                return "Invalid number of parameters.";
            }

            this->_userName = parameters[0].GetAsString();
            this->_password = parameters[1].GetAsString();

            this->_shouldLogIn = true;
        }

        return "";
    }
}
