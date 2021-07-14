#ifndef GAME_H
#define GAME_H

#include <memory>

#include "graphics/graphics.h"
#include "scenes/scene_manager.h"
#include "network_client/network_client.h"
#include "networking/networking.h"
#include "keyboard_input.h"
#include "mouse_input.h"
#include "data/data_provider.h"
#include "system_arguments.h"
#include "system.h"
#include "graphics/font_manager.h"
#include "action_manager.h"
#include "time/timer.h"
#include "debug_information.h"
#include "client_config.h"
#include "player.h"
#include "entities/character_appearance_library.h"
#include "crypto/crypto_provider.h"
#include "scripting/script_system.h"
#include "engine/scripting/client_script_factory.h"
#include "math/random_engine.h"

namespace pf = projectfarm;

namespace projectfarm::engine
{
	class Game final : public std::enable_shared_from_this<Game>
	{
	public:
		Game()
		{
			this->_graphics = std::make_shared<pf::graphics::Graphics>();
			this->_keyboardInput = std::make_shared<pf::engine::KeyboardInput>();
            this->_mouseInput = std::make_shared<pf::engine::MouseInput>();
			this->_sceneManager = std::make_shared<pf::scenes::SceneManager>();
			this->_networkClient = std::make_shared<pf::network_client::NetworkClient>();
			this->_fontManager = std::make_shared<pf::graphics::FontManager>();
			this->_actionManager = std::make_shared<pf::engine::ActionManager>();
            this->_timer = std::make_shared<pf::shared::time::Timer>();
            this->_debugInformation = std::make_shared<pf::engine::DebugInformation>();
            this->_characterAppearanceLibrary = std::make_shared<pf::engine::entities::CharacterAppearanceLibrary>();
            this->_cryptoProvider = std::make_shared<pf::shared::crypto::CryptoProvider>();
            this->_scriptSystem = std::make_shared<pf::shared::scripting::ScriptSystem>();
            this->_scriptFactory = std::make_shared<pf::engine::scripting::ClientScriptFactory>();
            this->_randomEngine = std::make_shared<projectfarm::shared::math::RandomEngine>();
		}
		~Game() = default;

		Game(const Game&) = delete;
		Game(Game&&) = delete;

		[[nodiscard]] std::shared_ptr<Game> GetPtr() noexcept
		{
			return this->shared_from_this();
		}

		void Run(int argc, char* argv[]);

		[[nodiscard]] const std::shared_ptr<pf::graphics::Graphics>& GetGraphics() const noexcept
		{
			return this->_graphics;
		}

		[[nodiscard]] const std::shared_ptr<pf::engine::KeyboardInput>& GetKeyboardInput() const noexcept
		{
			return this->_keyboardInput;
		}

        [[nodiscard]] const std::shared_ptr<pf::engine::MouseInput>& GetMouseInput() const noexcept
        {
            return this->_mouseInput;
        }

        [[nodiscard]] const std::shared_ptr<pf::shared::DataProvider>& GetDataProvider() const noexcept
        {
            return this->_dataProvider;
        }

        [[nodiscard]] const std::shared_ptr<pf::graphics::FontManager>& GetFontManager() const noexcept
        {
            return this->_fontManager;
        }

        [[nodiscard]] const std::shared_ptr<pf::engine::ActionManager>& GetActionManager() const noexcept
        {
            return this->_actionManager;
        }

        [[nodiscard]] const std::shared_ptr<pf::engine::DebugInformation>& GetDebugInformation() const noexcept
        {
            return this->_debugInformation;
        }

        [[nodiscard]] const std::shared_ptr<pf::shared::crypto::CryptoProvider>& GetCryptoProvider() const noexcept
        {
            return this->_cryptoProvider;
        }

        [[nodiscard]] const std::shared_ptr<pf::engine::entities::CharacterAppearanceLibrary>& GetCharacterAppearanceLibrary() const noexcept
        {
		    return this->_characterAppearanceLibrary;
        }

        [[nodiscard]] Player& GetPlayer() noexcept
        {
		    return this->_player;
        }

        void ReconfirmPixelSizes() noexcept;

	private:
		std::shared_ptr<pf::graphics::Graphics> _graphics;
		std::shared_ptr<pf::scenes::SceneManager> _sceneManager;
		std::shared_ptr<pf::engine::KeyboardInput> _keyboardInput;
        std::shared_ptr<pf::engine::MouseInput> _mouseInput;
		std::shared_ptr<pf::network_client::NetworkClient> _networkClient;
		std::shared_ptr<pf::shared::DataProvider> _dataProvider;
		std::shared_ptr<pf::graphics::FontManager> _fontManager;
		std::shared_ptr<pf::engine::ActionManager> _actionManager;
        std::shared_ptr<pf::shared::time::Timer> _timer;
        std::shared_ptr<pf::engine::DebugInformation> _debugInformation;
        std::shared_ptr<pf::engine::entities::CharacterAppearanceLibrary> _characterAppearanceLibrary;
        std::shared_ptr<pf::shared::crypto::CryptoProvider> _cryptoProvider;
        std::shared_ptr<pf::shared::scripting::ScriptSystem> _scriptSystem;
        std::shared_ptr<pf::engine::scripting::ClientScriptFactory> _scriptFactory;
        std::shared_ptr<pf::shared::math::RandomEngine> _randomEngine;

		pf::shared::networking::Networking _networking;

		pf::engine::SystemArguments _systemArguments;
		pf::engine::ClientConfig _clientConfig;

		System _system;

		Player _player;

		bool Initialize(int argc, char* argv[]);

		void MainLoop();

		void Shutdown();

		void UpdateEngineEvents();

		void QuitGame();

		void SceneLoaded();
        
        void LowerMemoryPressure();
        
        void EnterLowerActivityState();
        void ResumeFromLowerActivityState();
        
        bool _isInLowerActivityState {false};
	};
}

#endif
