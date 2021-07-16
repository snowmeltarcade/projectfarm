#ifndef UI_H
#define UI_H

#include <SDL.h>
#include <filesystem>
#include <memory>
#include <optional>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <functional>

#include "graphics/consume_graphics.h"
#include "data/consume_data_provider.h"
#include "graphics/consume_font_manager.h"
#include "scripting/consume_script_system.h"
#include "scenes/consume_scene.h"
#include "canvas.h"
#include "styles.h"

namespace projectfarm::graphics::ui
{
	class UI final : public projectfarm::graphics::ConsumeGraphics,
                     public projectfarm::shared::ConsumeDataProvider,
                     public projectfarm::graphics::ConsumeFontManager,
                     public projectfarm::shared::scripting::ConsumeScriptSystem,
                     public projectfarm::scenes::ConsumeScene,
                     public std::enable_shared_from_this<UI>
	{
	public:
        using SimpleBindingType = std::function<void(const std::string&)>;

		UI()
		{
			this->_baseCanvas = std::make_shared<Canvas>();
		}
		~UI() override = default;

		bool Initialize() noexcept;

        [[nodiscard]]
        bool LoadFromFile(const std::string& name) noexcept;

		void Shutdown() const noexcept;

		void Clear() const noexcept;

		void Render() const noexcept;

		void ReconfirmSize() noexcept;

		[[nodiscard]]
        const std::shared_ptr<Canvas>& GetBaseCanvas() const noexcept
        {
		    return this->_baseCanvas;
        }

        // no [[nodiscard]]
        bool HandleClick(uint32_t x, uint32_t y) noexcept;

        // Note: This function has nothing to do with the mouse cursor UI control.
        // This only deals with events that result from a cursor position. Please
        // use the mouse cursor UI control to have a rendered mouse cursor
        void HandleMoveMouse(uint32_t x, uint32_t y) noexcept;

        void HandleMouseUp() noexcept;

        // When the mouse is clicked and dragged
        void HandleDrag(uint32_t x, uint32_t y) noexcept;

        void HandleKeyPress() noexcept;
        void HandleKeyPress_Enter() noexcept;

        [[nodiscard]]
        bool LoadControl(const nlohmann::json& controlJson,
                         const std::shared_ptr<BaseControl>& parentControl,
                         const std::vector<std::pair<std::string, std::string>>& parameters = {},
                         const std::optional<ControlStyle>& parentStyle = {});

        [[nodiscard]]
        std::string NormalizeParameter(std::string_view parameter,
                                       const std::vector<std::pair<std::string, std::string>>& parameters) const noexcept;

        [[nodiscard]]
        nlohmann::json NormalizeJson(const nlohmann::json& json,
                                     const std::vector<std::pair<std::string, std::string>>& parameters) const noexcept;

        void EnableSimpleBindingForParameter(const std::shared_ptr<SimpleBindingType>& binding,
                                             const std::string& value) noexcept;

        void SetSimpleBinding(const std::string& name, const std::string& value) noexcept;

        [[nodiscard]]
        std::optional<std::shared_ptr<BaseControl>> FindControlById(const std::string& id) noexcept;

        void SendMessage(const std::string& key, const std::vector<std::string>& parameters) const noexcept;

        [[nodiscard]]
        const std::shared_ptr<Styles>& GetStyles() const noexcept
        {
            return this->_styles;
        }

	private:
        [[nodiscard]]
        std::shared_ptr<BaseControl> CreateControlFromType(const std::string& type) noexcept;

        [[nodiscard]]
        bool LoadControlScriptsFromJson(const std::shared_ptr<BaseControl>& control,
                                        const nlohmann::json& controlJson) noexcept;

        [[nodiscard]]
        bool SetupControlScript(const std::shared_ptr<BaseControl>& control,
                                std::string& inlineScript,
                                const nlohmann::json& controlJson,
                                const std::string& key,
                                shared::scripting::FunctionTypes functionType) noexcept;

        void UpdateFocusedControlAtPoint(uint32_t x, uint32_t y) noexcept;

		std::shared_ptr<Canvas> _baseCanvas;

		std::shared_ptr<Styles> _styles;

        std::unordered_map<std::string, std::shared_ptr<SimpleBindingType>> _simpleBindings;

        std::optional<std::shared_ptr<BaseControl>> _focusedControl;
        std::optional<std::shared_ptr<BaseControl>> _mouseOverControl;
        std::optional<std::shared_ptr<BaseControl>> _dragControl;

        uint32_t _lastMouseXPos {0};
        uint32_t _lastMouseYPos {0};
	};
}

#endif