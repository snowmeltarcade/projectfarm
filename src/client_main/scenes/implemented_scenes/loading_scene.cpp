#include <SDL.h>
#include <sstream>

#include "loading_scene.h"
#include "engine/game.h"
#include "networking/packet_factory.h"
#include "networking/packets/server_client_load_world.h"
#include "graphics/ui/texture.h"
#include "graphics/ui/label.h"
#include "world_scene.h"

namespace projectfarm::scenes::implemented_scenes
{
	void LoadingScene::UpdateInput()
	{
		auto input = this->GetSceneManager()->GetGame()->GetKeyboardInput();

		if (input->IsKeyDown_Keycode(SDLK_ESCAPE))
		{
			this->Quit();
			return;
		}
	}

	void LoadingScene::ProcessPacket(const std::shared_ptr<shared::networking::Packet>& packet)
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
            // we may get this packet if this client is loading a new world
        }
	}

	bool LoadingScene::ValidatePacket(const std::shared_ptr<shared::networking::Packet>& packet) const
	{
		auto isValid = false;

		const auto packetType = packet->GetPacketType();
		
		if (packetType == shared::networking::PacketTypes::ServerClientLoadWorld ||
            packetType == shared::networking::PacketTypes::ServerClientEntityUpdate)
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

	void LoadingScene::HandleServerClientLoadWorldPacket(const std::shared_ptr<shared::networking::Packet>& packet)
	{
        const auto serverClientLoadWorld =
                std::static_pointer_cast<shared::networking::packets::ServerClientLoadWorldPacket>(packet);

        auto worldToLoad = serverClientLoadWorld->GetWorldToLoad();

        this->_logger->LogMessage("Received packet to load this world: " + worldToLoad);

        this->LoadNewWorld(worldToLoad);
	}

	bool LoadingScene::Initialize()
	{
	    this->LogMessage("Initializing loading scene scene.");

		if (!this->SetupUI())
        {
		    this->LogMessage("Failed to setup UI.");
		    return false;
        }

        if (auto iter = this->_loadParameters.find(WorldScene::LoadParameter_WorldName);
            iter != this->_loadParameters.end())
        {
            auto worldName = iter->second;
            if (!worldName.empty())
            {
                this->_worldToLoad = worldName;
            }
        }

        this->LogMessage("Initialized loading scene scene.");

		return true;
	}

	void LoadingScene::PrepareRender()
    {
	    this->_renderManager->AddUI(this->_ui);
    }
	
	void LoadingScene::Tick()
	{
	    if (!this->_isLoadingNewScene && !this->_worldToLoad.empty())
        {
            this->LoadNewWorld(this->_worldToLoad);
        }
	}

	void LoadingScene::Destroy()
    {
        this->_ui->Shutdown();
    }

	void LoadingScene::Quit()
    {
        this->_shouldQuit = true;

        this->Destroy();
    }

	bool LoadingScene::SetupUI()
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

        if (!this->_ui->LoadFromFile("loading_screen"))
        {
            this->LogMessage("Failed to load ui from file: loading_screen");
            return false;
        }

        return true;
	}

    void LoadingScene::LoadNewWorld(const std::string& worldName) noexcept
    {
        this->GetSceneManager()->AddLoadSceneParameter(WorldScene::LoadParameter_WorldName, worldName);
        this->GetSceneManager()->QueueLoadScene(projectfarm::scenes::SceneTypes::WorldScene);

        this->_isLoadingNewScene = true;
    }
}
