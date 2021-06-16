#include <SDL.h>

#include "game.h"
#include "device_capabilities.h"
#include "scenes/implemented_scenes/authenticate_scene.h"

namespace projectfarm::engine
{
	void Game::Run(int argc, char* argv[])
	{
		this->_logger->LogMessage("Starting game...");

		if (!this->Initialize(argc, argv))
        {
            this->Shutdown();
            return;
        }

		this->MainLoop();

		this->Shutdown();

		this->_logger->LogMessage("Exiting game.");
	}

	bool Game::Initialize(int argc, char* argv[])
	{
		this->_logger->LogMessage("Initializing game...");

        this->_cryptoProvider->SetLogger(this->_logger);
        if (!this->_cryptoProvider->Initialize())
        {
            this->_logger->LogMessage("Failed to setup crypto provider.");
            return false;
        }

        this->_systemArguments.SetLogger(this->_logger);
        if (!this->_systemArguments.SetArguments(argc, argv))
        {
            this->_logger->LogMessage("Failed to set arguments.");
            return false;
        }

		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			this->_logger->LogMessage("Failed to initialize SDL.");
			this->_logger->LogMessage(SDL_GetError());
			return false;
		}

		this->_dataProvider = std::make_shared<pf::shared::DataProvider>(this->_systemArguments.GetBinaryPath());
		this->_dataProvider->SetLogger(this->_logger);
		if (!this->_dataProvider->SetupClient())
        {
		    this->_logger->LogMessage("Failed to setup data provider.");
		    this->_dataProvider->Shutdown();
		    return false;
        }

        this->_system.SetLogger(this->_logger);
        this->_system.SetDataProvider(this->_dataProvider);
        this->_system.Initialize();

		this->_clientConfig.SetLogger(this->_logger);
		this->_clientConfig.SetDataProvider(this->_dataProvider);
		if (!this->_clientConfig.LoadConfig())
        {
		    this->_logger->LogMessage("Failed to load client config.");
		    return false;
        }

		if (this->_systemArguments.GetShouldStartServer())
		{
			this->_system.StartServer();
		}
		
		this->_graphics->SetLogger(this->_logger);
		this->_graphics->SetDebugInformation(this->GetDebugInformation());
		this->_graphics->SetDataProvider(this->GetDataProvider());
		this->_graphics->SetGame(this->shared_from_this());
		if (!this->_graphics->Initialize(this->_clientConfig.GetScreenWidth(),
		                                 this->_clientConfig.GetScreenHeight(),
		                                 this->_clientConfig.GetFullScreen(),
		                                 this->_clientConfig.GetScreenWidthInMeters()))
		{
			this->_logger->LogMessage("Failed to initialize graphics.");
			this->_graphics->Shutdown(); // ensure all created resources are released
			return false;
		}

		this->_fontManager->SetLogger(this->_logger);
		this->_fontManager->SetDataProvider(this->_dataProvider);
		if (!this->_fontManager->Initialize())
        {
		    this->_logger->LogMessage("Failed to initialize font manager.");
		    this->_fontManager->Shutdown();
		    return false;
        }

		this->_keyboardInput->SetLogger(this->_logger);
		if (!this->_keyboardInput->Initialize())
		{
			this->_logger->LogMessage("Failed to initialize input.");
			return false;
		}

		this->_actionManager->SetLogger(this->_logger);
		this->_actionManager->SetGame(this->GetPtr());
		if (!this->_actionManager->Initialize())
        {
		    this->_logger->LogMessage("Failed to initialize action manager.");
		    return false;
        }

		this->_networking.SetLogger(this->_logger);
		if (!this->_networking.Initialize())
		{
			this->_networking.LogMessage("Failed to initialize networking.");
			return false;
		}

		this->_networkClient->SetLogger(this->_logger);
		this->_networkClient->SetSceneManager(this->_sceneManager);
		if (!this->_networkClient->Initialize(this->_clientConfig.GetHostName(), this->_clientConfig.GetTcpPort(),
		                                      this->_clientConfig.GetClientUdpPort(), this->_clientConfig.GetServerUdpPort()))
		{
			this->_networkClient->LogMessage("Failed to initialize network client.");
			return false;
		}

		this->_characterAppearanceLibrary->SetLogger(this->_logger);
		this->_characterAppearanceLibrary->SetDataProvider(this->_dataProvider);
		if (!this->_characterAppearanceLibrary->Setup())
        {
		    this->_logger->LogMessage("Failed to setup character appearance library.");
		    return false;
        }

        this->_randomEngine->Initialize();

        this->_scriptFactory->SetLogger(this->_logger);
        this->_scriptSystem->SetScriptFactory(this->_scriptFactory);
        this->_scriptSystem->SetDataProvider(this->_dataProvider);
        this->_scriptSystem->SetLogger(this->_logger);
        this->_scriptSystem->SetRandomEngine(this->_randomEngine);
        if (!this->_scriptSystem->Initialize(this->_systemArguments.GetBinaryPath()))
        {
            this->_logger->LogMessage("Failed to initialize script system.");
            return false;
        }

		this->_sceneManager->SetLogger(this->_logger);
		this->_sceneManager->SetNetworkClient(this->_networkClient);
		this->_sceneManager->SetGame(this->GetPtr());
		this->_sceneManager->SetTimer(this->_timer);
		this->_sceneManager->SetDebugInformation(this->_debugInformation);
        this->_sceneManager->SetScriptSystem(this->_scriptSystem);
        this->_sceneManager->AddLoadSceneParameter(
                pf::scenes::implemented_scenes::AuthenticateScene::LoadParameter_UserName,
                this->_systemArguments.GetUserName());
        this->_sceneManager->AddLoadSceneParameter(
                pf::scenes::implemented_scenes::AuthenticateScene::LoadParameter_Password,
                this->_systemArguments.GetPassword());
        if (!this->_sceneManager->LoadScene(pf::scenes::SceneTypes::AuthenticateScene))
		{
			this->_logger->LogMessage("Failed to initialize loading scene.");
			return false;
		}

		this->_logger->LogMessage("Initialized game.");

		return true;
	}

	void Game::Shutdown()
	{
		this->_logger->LogMessage("Shutting down game...");

        this->_scriptSystem->Shutdown();
		this->_networkClient->Shutdown();
		this->_networking.Shutdown();
        this->_actionManager->Shutdown();
		this->_fontManager->Shutdown();
		this->_graphics->Shutdown();
		this->_system.Shutdown();

        if (this->_dataProvider)
        {
            this->_dataProvider->Shutdown();
        }

        SDL_Quit();
		
		this->_logger->LogMessage("Shut down game.");
	}

	void Game::MainLoop()
	{
	    this->_timer->Reset();
	    this->_debugInformation->Reset();

		while (true)
		{
            this->UpdateEngineEvents();
            
            if (this->_isInLowerActivityState)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }

			this->_sceneManager->Tick();
			
			const auto currentScene = this->_sceneManager->GetCurrentScene();

			// check input before we check if we should quit or not
			// this way we will not render anything during shutdown
			// which will cause weird errors
            currentScene->UpdateInput();

			if (currentScene->ShouldQuit())
            {
                currentScene->Destroy();
                break;
            }

			this->_actionManager->ProcessActions();

            currentScene->PrepareRender();

			this->_graphics->Render();

            if (!this->_sceneManager->HandleQueuedScene([this](){ this->SceneLoaded(); }))
            {
                this->GetLogger()->LogMessage("Failed to load queued scene.");
                return;
            }

            this->_graphics->EndFrame();

            this->_keyboardInput->Tick();

            this->_timer->IncrementFrame();
            this->_debugInformation->OnFrame();
		}
	}

	void Game::UpdateEngineEvents()
	{
		SDL_Event e;

		while (SDL_PollEvent(&e))
		{
            engine::UpdateDeviceCapabilities(e);

			switch (e.type)
			{
            case SDL_WINDOWEVENT:
            {
                switch (e.window.event)
                {
                    case SDL_WINDOWEVENT_RESIZED:
                    {
                        this->GetGraphics()->OnWindowResized(static_cast<uint32_t>(e.window.data1),
                                                             static_cast<uint32_t>(e.window.data2));
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }
                break;
            }
			case SDL_KEYDOWN:
			case SDL_KEYUP:
			{
				this->_keyboardInput->UpdateKeyboardKey(e.key);
				break;
			}
            case SDL_APP_TERMINATING:
            {
                this->GetLogger()->LogMessage("Event: Terminating");
                
                this->QuitGame();
                break;
            }
            case SDL_APP_LOWMEMORY:
            {
                this->GetLogger()->LogMessage("Event: Low Memory");
                
                this->LowerMemoryPressure();
                break;
            }
            case SDL_APP_WILLENTERBACKGROUND:
            {
                this->EnterLowerActivityState();
                
                this->GetLogger()->LogMessage("Event: Will Enter Background");
                break;
            }
            case SDL_APP_DIDENTERBACKGROUND:
            {
                this->GetLogger()->LogMessage("Event: Did Enter Background");
                break;
            }
            case SDL_APP_WILLENTERFOREGROUND:
            {
                this->ResumeFromLowerActivityState();
                
                this->GetLogger()->LogMessage("Event: Will Enter Foreground");
                break;
            }
            case SDL_APP_DIDENTERFOREGROUND:
            {
                this->GetLogger()->LogMessage("Event: Did Enter Foreground");
                break;
            }
			case SDL_QUIT:
			{
                this->GetLogger()->LogMessage("Event: Quit");
                
				this->QuitGame();
				break;
			}
            case SDL_MOUSEMOTION:
            {
                if (e.motion.which == SDL_TOUCH_MOUSEID)
                {
                    break;
                }

                this->_mouseInput->SetPosition(static_cast<uint32_t>(e.motion.x),
                                               static_cast<uint32_t>(e.motion.y));

                break;
            }
            case SDL_FINGERMOTION:
            {
                auto [x, y] = this->GetGraphics()->GetCamera()->GetPositionAtPercent(e.tfinger.x, e.tfinger.y);

                this->_mouseInput->SetPosition(x, y);

                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            {
                if (e.button.which == SDL_TOUCH_MOUSEID)
                {
                    //break;
                }

                if (e.button.button == SDL_BUTTON_LEFT)
                {
                    this->_mouseInput->SetButtonState(true);
                }

                break;
            }
            case SDL_MOUSEBUTTONUP:
            {
                if (e.button.which == SDL_TOUCH_MOUSEID)
                {
                    //break;
                }

                if (e.button.button == SDL_BUTTON_LEFT)
                {
                    this->_mouseInput->SetButtonState(false);
                }

                break;
            }
			default:
            {
                break;
            }
			}
		}
	}

	void Game::QuitGame()
	{
		auto scene = this->_sceneManager->GetCurrentScene();
		scene->Quit();
	}

	void Game::SceneLoaded()
    {
	    this->_graphics->SceneLoaded();
    }

    void Game::LowerMemoryPressure()
    {
        // TODO: Implement
    }

    void Game::EnterLowerActivityState()
    {
        this->_isInLowerActivityState = true;
        
        this->_networkClient->SetIsInLowerActivityState(this->_isInLowerActivityState);
    }

    void Game::ResumeFromLowerActivityState()
    {
        this->_isInLowerActivityState = false;
        
        this->_networkClient->SetIsInLowerActivityState(this->_isInLowerActivityState);
    }

    void Game::ReconfirmPixelSizes() noexcept
    {
	    this->_sceneManager->ReconfirmPixelSizes();
    }
}
