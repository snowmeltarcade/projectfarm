#include "canvas.h"
#include "ui.h"

namespace projectfarm::graphics::ui
{
	void Canvas::Clear()
	{
        this->ClearChildren();
	}

	void Canvas::Render()
    {
	    this->RenderChildren();
    }

    void Canvas::ReadStylesDataFromJson(const nlohmann::json& controlJson,
                                        const std::shared_ptr<UI>& ui,
                                        const std::vector<std::pair<std::string, std::string>>& parentParameters)
    {
        auto normalizedJson = ui->NormalizeJson(controlJson, parentParameters);

        if (auto cssClass = normalizedJson.find("cssClass"); cssClass != normalizedJson.end())
        {
            this->_cssClass = cssClass->get<std::string>();
        }
    }

    bool Canvas::SetupFromJson(const nlohmann::json& controlJson,
                               const std::shared_ptr<UI>& ui,
                               const std::vector<std::pair<std::string, std::string>>& parameters)
    {
        auto normalizedJson = ui->NormalizeJson(controlJson, parameters);
        if (!this->SetCommonValuesFromJson(normalizedJson))
        {
            this->LogMessage("Failed to set common values.");
            return false;
        }

        return true;
    }
}