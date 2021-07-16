#include "canvas.h"
#include "ui.h"
#include "api/logging/logging.h"

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

    bool Canvas::SetupFromJson(const nlohmann::json& controlJson,
                               const std::shared_ptr<UI>& ui,
                               const std::vector<std::pair<std::string, std::string>>& parameters)
    {
        auto normalizedJson = ui->NormalizeJson(controlJson, parameters);
        if (!this->SetCommonValuesFromJson(normalizedJson))
        {
            shared::api::logging::Log("Failed to set common values.");
            return false;
        }

        return true;
    }
}