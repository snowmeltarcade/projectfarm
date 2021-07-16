#ifndef SCENE_H
#define SCENE_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "scene_types.h"
#include "networking/packet.h"
#include "networking/consume_packet_sender.h"
#include "consume_scene_manager.h"
#include "graphics/consume_graphics.h"
#include "data/consume_data_provider.h"
#include "graphics/consume_font_manager.h"
#include "graphics/consume_render_manager.h"
#include "time/consume_timer.h"
#include "engine/consume_debug_information.h"
#include "engine/consume_game.h"
#include "engine/entities/character.h"
#include "crypto/consume_crypto_provider.h"
#include "scripting/consume_script_system.h"
#include "scripting/function_parameter.h"

namespace projectfarm::scenes
{
	class SceneManager;

	class Scene : public ConsumeSceneManager,
				  public graphics::ConsumeGraphics,
                  public shared::ConsumeDataProvider,
                  public graphics::ConsumeFontManager,
                  public graphics::ConsumeRenderManager,
                  public shared::time::ConsumeTimer,
                  public engine::ConsumeDebugInformation,
                  public engine::ConsumeGame,
                  public shared::crypto::ConsumeCryptoProvider,
                  public shared::scripting::ConsumeScriptSystem,
                  public std::enable_shared_from_this<Scene>
	{
	public:
		Scene() = default;
		~Scene() override = default;

		//[[nodiscard]] virtual SceneTypes GetSceneType() const = 0;

		[[nodiscard]]
        virtual bool ShouldQuit() const = 0;

		virtual void UpdateInput() = 0;

		virtual void ProcessPacket(const std::shared_ptr<shared::networking::Packet>& packet) = 0;

		[[nodiscard]]
        virtual bool Initialize() = 0;

		virtual void PrepareRender() = 0;

		virtual void Tick() = 0;

		virtual void Destroy() = 0;

		virtual void Quit() = 0;

		virtual void OnPlayerCharacterAdd(const std::shared_ptr<engine::entities::Character>& character) noexcept = 0;

		void SetLoadParameters(const std::unordered_map<std::string, std::string>& loadParameters)
        {
		    this->_loadParameters = loadParameters;
        }

        virtual void ReconfirmPixelSizes() noexcept = 0;

        [[nodiscard]]
        virtual std::string SendUIMessage(const std::string&,
                                          const std::vector<projectfarm::shared::scripting::FunctionParameter>&) noexcept
        {
            return "";
        }

	protected:
	    std::unordered_map<std::string, std::string> _loadParameters;
	};
}

#endif
