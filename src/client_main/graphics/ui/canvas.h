#ifndef CANVAS_H
#define CANVAS_H

#include <memory>

#include "base_control.h"

namespace projectfarm::graphics::ui
{
    class Canvas final : public BaseControl
	{
	public:
		Canvas() = default;
		~Canvas() override = default;

        ControlTypes GetControlType() const noexcept override
        {
            return ControlTypes::Canvas;
        }

        void Clear() override;

        void Render() override;

        [[nodiscard]] bool SetupFromJson(const nlohmann::json& controlJson,
                                         const std::shared_ptr<UI>& ui,
                                         const std::vector<std::pair<std::string, std::string>>& parameters) override;
	};
}

#endif