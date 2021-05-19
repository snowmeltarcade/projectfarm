#include <fstream>
#include <regex>
#include <sstream>

#include "custom.h"
#include "graphics/graphics.h"

namespace projectfarm::graphics::ui
{
    void Custom::Clear()
    {
        this->ClearChildren();
    }

    void Custom::Render()
    {
        this->RenderChildren();
    }

    bool Custom::SetupFromJson(const nlohmann::json& controlJson,
                               const std::shared_ptr<UI>& ui,
                               const std::vector<std::pair<std::string, std::string>>& parentParameters)
    {
        auto normalizedJson = ui->NormalizeJson(controlJson, parentParameters);

        if (!this->SetCommonValuesFromJson(normalizedJson))
        {
            this->LogMessage("Failed to set common values.");
            return false;
        }

        auto name = normalizedJson["name"].get<std::string>();

        // nlohmann::json can throw exceptions
        try
        {
            auto filePath = this->_dataProvider->GetUICustomControlLocationFromName(name);

            std::ifstream file(filePath);

            if (!file.is_open())
            {
                this->LogMessage("Failed to open custom control file: " + filePath.u8string());
                return false;
            }

            nlohmann::json jsonFile;
            file >> jsonFile;

            auto parameters = this->GetControlParameters(controlJson, jsonFile, parentParameters);
            if (!parameters)
            {
                this->LogMessage("Failed to set control parameters for control file: " + filePath.u8string());
                return false;
            }

            const auto& controls = jsonFile["controls"];

            for (const auto& control : controls)
            {
                if (!ui->LoadControl(control, this->shared_from_this(), *parameters))
                {
                    this->LogMessage("Failed to load control: " + control.dump());
                    return false;
                }
            }

            this->ProcessMeta(jsonFile);
        }
        catch (const std::exception& ex)
        {
            this->LogMessage("Failed to load custom control file: " + name +
                             " with exception: " + ex.what());

            return false;
        }

        return true;
    }

    std::string Custom::Script_GetCustomPropertyString(std::string_view key,
                                                       const std::vector<projectfarm::shared::scripting::FunctionParameter>&) noexcept
    {
        if (key == "type")
        {
            return this->_type;
        }

        return "";
    }

    auto Custom::GetControlParameters(const nlohmann::json& parentControlJson,
                                      const nlohmann::json& controlJson,
                                      const std::vector<std::pair<std::string, std::string>>& parentParameters)
        -> std::optional<std::vector<std::pair<std::string, std::string>>>
    {
        std::vector<std::pair<std::string, std::string>> parameters;

        try
        {
            const auto& jsonParameters = controlJson.find("parameters");
            if (jsonParameters != controlJson.end())
            {
                for (const auto& p : jsonParameters->items())
                {
                    // see if the value was set in the parent control, else
                    // use the default value set in the control itself
                    auto key = p.key();
                    auto value = p.value().get<std::string>();

                    auto valueInParentControl = parentControlJson.find(key);
                    if (valueInParentControl != parentControlJson.end())
                    {
                        value = valueInParentControl->get<std::string>();
                    }

                    // this custom control may be within another custom control
                    // if so and the value is a parameter, use the parent custom
                    // control's value
                    if (value.size() >= 4 &&
                        value[0] == '#' && value[1] == '#' &&
                        value[value.size() - 2] == '#' && value[value.size() - 1] == '#')
                    {
                        value = value.substr(2, value.size() - 4);

                        for (const auto& [k, v] : parentParameters)
                        {
                            if (k == value)
                            {
                                value = v;
                                break;
                            }
                        }
                    }

                    parameters.push_back({ key, value });
                }
            }
        }
        catch (const std::exception& ex)
        {
            this->LogMessage("Failed to load custom control file: " + controlJson.dump() +
                             " with exception: " + ex.what());

            return {};
        }

        return parameters;
    }

    void Custom::ProcessMeta(const nlohmann::json& json) noexcept
    {
        auto meta = json.find("meta");
        if (meta == json.end())
        {
            return;
        }

        auto textControl = meta->find("textControl");
        if (textControl != meta->end())
        {
            auto controlId = textControl->get<std::string>();

            auto control = this->FindControlById(controlId);
            if (control)
            {
                this->_textControl = *control;
            }
        }

        auto type = meta->find("type");
        if (type != meta->end())
        {
            this->_type = type->get<std::string>();
        }
    }
}