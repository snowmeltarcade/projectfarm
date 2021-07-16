#include <fstream>

#include "ui.h"
#include "texture.h"
#include "label.h"
#include "mouse_cursor.h"
#include "custom.h"
#include "engine/game.h"
#include "scripting/script_system.h"
#include "api/logging/logging.h"

using namespace std::literals;

namespace projectfarm::graphics::ui
{
	bool UI::Initialize() noexcept
	{
		shared::api::logging::Log("Initializing UI...");

		this->_baseCanvas->SetGraphics(this->GetGraphics());

		this->ReconfirmSize();

		this->_styles = std::make_shared<Styles>();
		this->_styles->SetDataProvider(this->_dataProvider);
		if (!this->_styles->Initialize())
        {
		    shared::api::logging::Log("Failed to initialize styles.");
		    return false;
        }

		shared::api::logging::Log("Initialized UI.");

		return true;
	}

    bool UI::LoadFromFile(const std::string& name) noexcept
    {
        // nlohmann::json can throw exceptions
        try
        {
            auto filePath = this->_dataProvider->GetUILocationFromName(name);

            std::ifstream file(filePath);

            if (!file.is_open())
            {
                shared::api::logging::Log("Failed to open ui file: " + filePath.u8string());
                return false;
            }

            nlohmann::json jsonFile;
            file >> jsonFile;

            const auto& controls = jsonFile["controls"];

            for (const auto& control : controls)
            {
                if (!this->LoadControl(control, this->_baseCanvas))
                {
                    shared::api::logging::Log("Failed to load control: " + control.dump());
                    return false;
                }
            }

            if (!this->LoadControlScriptsFromJson(this->_baseCanvas, jsonFile))
            {
                shared::api::logging::Log("Failed to load base canvas control.");
                return false;
            }

            this->_baseCanvas->CallScriptFunction(shared::scripting::FunctionTypes::OnLoad,
                                                  {},
                                                  true);
        }
        catch (const std::exception& ex)
        {
            shared::api::logging::Log("Failed to load ui file: " + name +
                             "with exception: " + ex.what());

            return false;
        }

        return true;
    }

	void UI::Shutdown() const noexcept
	{
		shared::api::logging::Log("Shutting down UI...");

		this->Clear();

		shared::api::logging::Log("Shut down UI.");
	}

	void UI::Clear() const noexcept
	{
        this->_baseCanvas->CallScriptFunction(shared::scripting::FunctionTypes::OnShutdown,
                                              {},
                                              true);

		this->_baseCanvas->Clear();
	}

    void UI::ReconfirmSize() noexcept
    {
        const auto& graphics = this->GetGraphics();
        uint32_t screenWidth = graphics->GetCamera()->GetViewport().w;
        uint32_t screenHeight = graphics->GetCamera()->GetViewport().h;

        ControlSize size;
        size.SetParentSize(screenWidth, screenHeight);
        size.SetSizePercent(100.0f, 100.0f);
        this->_baseCanvas->SetSize(size);
    }

	void UI::Render() const noexcept
	{
	    this->_baseCanvas->Render();
	}

    bool UI::HandleClick(uint32_t x, uint32_t y) noexcept
    {
        this->UpdateFocusedControlAtPoint(x, y);

        if (this->_focusedControl &&
            *this->_focusedControl &&
            (*this->_focusedControl)->GetIsEnabled())
        {
            (*this->_focusedControl)->HandleClick(x, y);
        }

        auto clickedOnControl = this->_focusedControl &&
                                *this->_focusedControl &&
                                (*this->_focusedControl)->GetControlType() != ControlTypes::Canvas;
        return clickedOnControl;
    }

    void UI::HandleMoveMouse(uint32_t x, uint32_t y) noexcept
    {
        if (x == this->_lastMouseXPos && y == this->_lastMouseYPos)
        {
            return;
        }

        this->_lastMouseXPos = x;
        this->_lastMouseYPos = y;

        auto mouseOverControl = this->_baseCanvas->GetFocusedControlAtPoint(x, y);

        if (mouseOverControl != this->_mouseOverControl)
        {
            // invoke the leave first
            if (this->_mouseOverControl &&
                *this->_mouseOverControl &&
                (*this->_mouseOverControl)->GetIsEnabled())
            {
                (*this->_mouseOverControl)->HandleMouseLeave(x, y);
            }

            this->_mouseOverControl = mouseOverControl;

            // now invoke the enter
            if (this->_mouseOverControl &&
                *this->_mouseOverControl &&
                (*this->_mouseOverControl)->GetIsEnabled())
            {
                (*this->_mouseOverControl)->HandleMouseEnter(x, y);
            }
        }
    }

    void UI::HandleMouseUp() noexcept
    {
        this->_dragControl = {};
    }

    void UI::HandleDrag(uint32_t x, uint32_t y) noexcept
    {
        if (x == this->_lastMouseXPos && y == this->_lastMouseYPos)
        {
            return;
        }

        auto dx = x - this->_lastMouseXPos;
        auto dy = y - this->_lastMouseYPos;

        this->_lastMouseXPos = x;
        this->_lastMouseYPos = y;

        if (!this->_dragControl)
        {
            this->_dragControl = this->_baseCanvas->GetFocusedControlAtPoint(x, y);
        }

        if (this->_dragControl &&
            *this->_dragControl &&
            (*this->_dragControl)->GetIsEnabled())
        {
            (*this->_dragControl)->HandleDrag(x, y, dx, dy);
        }
    }

    void UI::HandleKeyPress() noexcept
    {
        if (this->_focusedControl &&
            *this->_focusedControl &&
            (*this->_focusedControl)->GetIsEnabled())
        {
            (*this->_focusedControl)->HandleKeyPress();
        }
    }

    void UI::HandleKeyPress_Enter() noexcept
    {
        if (this->_focusedControl &&
            *this->_focusedControl &&
            (*this->_focusedControl)->GetIsEnabled())
        {
            (*this->_focusedControl)->HandleKeyPress_Enter();
        }
    }

    bool UI::LoadControl(const nlohmann::json& controlJson,
                         const std::shared_ptr<BaseControl>& parentControl,
                         const std::vector<std::pair<std::string, std::string>>& parameters,
                         const std::optional<ControlStyle>& parentStyle)
    {
        auto normalizedJson = this->NormalizeJson(controlJson, parameters);

        auto type = normalizedJson["type"].get<std::string>();

        auto control = this->CreateControlFromType(type);
        if (!control)
        {
            shared::api::logging::Log("Failed to create control of type: " + type);
            return false;
        }

        auto x = normalizedJson["x"].get<float>();
        auto y = normalizedJson["y"].get<float>();
        auto w = normalizedJson["w"].get<float>();
        auto h = normalizedJson["h"].get<float>();

        if (!this->LoadControlScriptsFromJson(control, normalizedJson))
        {
            shared::api::logging::Log("Failed to setup UI control script for control with json: " + controlJson.dump());
            return false;
        }

        parentControl->AddChild(control);

        auto size = parentControl->GetSize().CreateChildSize();
        size.SetSizePercent(w, h);
        control->SetSize(size);

        auto position = control->GetPosition();
        position.SetRenderOriginPoint(projectfarm::graphics::RenderOriginPoints::TopLeft);
        position.SetPositionPercent(x, y);
        control->SetPosition(position);

        control->ReadStylesDataFromJson(controlJson, this->shared_from_this(), parameters);

        if (!control->RefreshStyles(true, parentStyle))
        {
            shared::api::logging::Log("Failed to refresh styles for control: " + controlJson.dump());
            return false;
        }

        if (!control->SetupFromJson(controlJson, this->shared_from_this(), parameters))
        {
            shared::api::logging::Log("Failed to setup control with json: " + controlJson.dump());
            return false;
        }

        auto controls = controlJson.find("controls");
        if (controls != controlJson.end())
        {
            for (const auto& childControl : *controls)
            {
                if (!this->LoadControl(childControl, control, parameters, parentStyle))
                {
                    shared::api::logging::Log("Failed to load control: " + childControl.dump());
                    return false;
                }
            }
        }

        return true;
    }

    std::shared_ptr<BaseControl> UI::CreateControlFromType(const std::string& type) noexcept
    {
        std::shared_ptr<BaseControl> control;

        if (type == "canvas")
        {
            control = std::make_shared<Canvas>();
        }
        else if (type == "texture")
        {
            control = std::make_shared<Texture>();
        }
        else if (type == "label")
        {
            auto label = std::make_shared<Label>();
            label->SetFontManager(this->_fontManager);
            control = label;
        }
        else if (type == "mouseCursor")
        {
            control = std::make_shared<MouseCursor>(this->GetGraphics()->GetGame()->GetMouseInput());
        }
        else if (type == "custom")
        {
            auto custom = std::make_shared<Custom>();
            custom->SetFontManager(this->_fontManager);
            control = custom;
        }

        if (!control)
        {
            shared::api::logging::Log("Unknown control type: " + type);
            return {};
        }

        control->SetGraphics(this->GetGraphics());
        control->SetDataProvider(this->_dataProvider);
        control->SetUI(this->shared_from_this());

        return control;
    }

    std::string UI::NormalizeParameter(std::string_view parameter,
                                       const std::vector<std::pair<std::string, std::string>>& parameters) const noexcept
    {
	    if (parameter.empty() ||
	        parameter.length() < 5 || // there must be at least 5 characters for this to be a parameter
                                      // ##a## == 5 characters
	        parameter[0] != '#' ||
            parameter[1] != '#' ||
            parameter[parameter.length() - 2] != '#' ||
            parameter[parameter.length() - 1] != '#')
        {
	        return std::string(parameter);
        }

	    auto name = parameter.substr(2, parameter.length() - 4);

	    for (const auto& [key, value] : parameters)
        {
	        if (name == key)
            {
	            return value;
            }
        }

        return std::string(parameter);
    }

    nlohmann::json UI::NormalizeJson(const nlohmann::json& json,
                                     const std::vector<std::pair<std::string, std::string>>& parameters) const noexcept
    {
        auto jsonCopy = json;

        for (auto& field : jsonCopy)
        {
            if (field.is_string())
            {
                auto value = this->NormalizeParameter(field.get<std::string>(), parameters);
                field = value;
            }
        }

        return jsonCopy;
    }

    void UI::EnableSimpleBindingForParameter(const std::shared_ptr<SimpleBindingType>& binding,
                                             const std::string& value) noexcept
    {
        if (value[0] != '$' ||
            value[1] != '$' ||
            value[value.length() - 2] != '$' ||
            value[value.length() - 1] != '$')
        {
            // default values should be set by whatever is setting the UI bindings,
            // for instance, the scene
            return;
        }

        auto name = value.substr(2, value.length() - 4);

        this->_simpleBindings[name] = binding;
    }

    void UI::SetSimpleBinding(const std::string& name, const std::string& value) noexcept
    {
        auto parameterIter = this->_simpleBindings.find(name);
        if (parameterIter == this->_simpleBindings.end())
        {
            // TODO: report error?
            return;
        }

        auto binding = parameterIter->second;
        (*binding)(value);
    }

    std::optional<std::shared_ptr<BaseControl>> UI::FindControlById(const std::string& id) noexcept
    {
        if (id.empty())
        {
            return {};
        }

        auto result = this->_baseCanvas->FindControlById(id);
        return result;
    }

    void UI::SendMessage(const std::string& key, const std::vector<std::string>& parameters) const noexcept
    {
        // we only want to call this function on the base canvas itself
        std::vector<projectfarm::shared::scripting::FunctionParameter> params;

        params.push_back(projectfarm::shared::scripting::FunctionParameter { key });

        for (const auto& p : parameters)
        {
            params.push_back(projectfarm::shared::scripting::FunctionParameter { p });
        }

        this->_baseCanvas->CallScriptFunction(shared::scripting::FunctionTypes::SendMessage,
                                              params,
                                              false);
    }

    bool UI::LoadControlScriptsFromJson(const std::shared_ptr<BaseControl>& control,
                                        const nlohmann::json& controlJson) noexcept
    {
        if (!control->DoesSupportJavascript())
        {
            return true;
        }

        auto inlineScript = ""s;

        std::vector<std::pair<std::string, shared::scripting::FunctionTypes>> actions
        {
            { "onload", shared::scripting::FunctionTypes::OnLoad },
            { "onshutdown", shared::scripting::FunctionTypes::OnShutdown },
            { "onclick", shared::scripting::FunctionTypes::OnClick },
            { "onmouseenter", shared::scripting::FunctionTypes::OnMouseEnter },
            { "onmouseleave", shared::scripting::FunctionTypes::OnMouseLeave },
            { "onkeypress", shared::scripting::FunctionTypes::OnKeyPress },
            { "onkeypress_enter", shared::scripting::FunctionTypes::OnKeyPress_Enter },
            { "ondrag", shared::scripting::FunctionTypes::OnDrag },
            { "sendmessage", shared::scripting::FunctionTypes::SendMessage },
        };

        for (const auto& [key, functionType] : actions)
        {
            if (!this->SetupControlScript(control, inlineScript, controlJson, key, functionType))
            {
                shared::api::logging::Log("Failed to setup control script with key: '" + key +
                                 "' and json: " + controlJson.dump());
                return false;
            }
        }

        if (!inlineScript.empty())
        {
            auto script = std::static_pointer_cast<engine::scripting::UIControlScript>(
                    this->_scriptSystem->CreateScript(shared::scripting::ScriptTypes::UIControl,
                                                         inlineScript));

            if (!script)
            {
                shared::api::logging::Log("Failed to create inline UI control script with script: " + inlineScript);
                return false;
            }

            script->SetObjectInternalFields(control,
                                            this->shared_from_this(),
                                            this->GetScene(),
                                            this->GetScene()->GetGame()->GetKeyboardInput());

            control->SetInlineScript(script);
        }

        return true;
    }

    bool UI::SetupControlScript(const std::shared_ptr<BaseControl>& control,
                                std::string& internalScript,
                                const nlohmann::json& controlJson,
                                const std::string& key,
                                shared::scripting::FunctionTypes functionType) noexcept
    {
        auto it = controlJson.find(key);
        if (it != controlJson.end())
        {
            auto script = it->get<std::string>();

            if (pfu::endsWith(script, ".js"))
            {
                if (control->GetExternalScript())
                {
                    shared::api::logging::Log("External script already exists for control with key: " + key);
                    return false;
                }

                std::filesystem::path path = this->_dataProvider->NormalizePath(script);

                auto loadedScript = std::static_pointer_cast<engine::scripting::UIControlScript>(
                        this->_scriptSystem->CreateScript(shared::scripting::ScriptTypes::UIControl,
                                                              path));

                if (!loadedScript)
                {
                    shared::api::logging::Log("Failed to create external UI control script with key: '" + key +
                                     "' and path: '" + path.u8string() + "' and script: " + script);
                    return false;
                }

                loadedScript->SetObjectInternalFields(control,
                                                      this->shared_from_this(),
                                                      this->GetScene(),
                                                      this->GetScene()->GetGame()->GetKeyboardInput());

                control->SetExternalScript(loadedScript);
            }
            else if (!script.empty())
            {
                auto function = shared::scripting::ScriptSystem::GetFunctionName(functionType);
                internalScript += "function " + function + "() { " + script + "; }"s;
            }
        }

        return true;
    }

    void UI::UpdateFocusedControlAtPoint(uint32_t x, uint32_t y) noexcept
    {
        this->_focusedControl = this->_baseCanvas->GetFocusedControlAtPoint(x, y);
    }
}
