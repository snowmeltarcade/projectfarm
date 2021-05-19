#ifndef PROJECTFARM_MOUSE_CURSOR_H
#define PROJECTFARM_MOUSE_CURSOR_H

#include <filesystem>

#include "base_control.h"
#include "graphics/texture.h"
#include "engine/consume_mouse_input.h"

namespace projectfarm::graphics::ui
{
    class MouseCursor final : public BaseControl,
                              public engine::ConsumeMouseInput
    {
    public:
        explicit MouseCursor(const std::shared_ptr<engine::MouseInput>& mouseInput)
        : ConsumeMouseInput(mouseInput)
        {
        }
        ~MouseCursor() override
        {
            this->Shutdown();
        }

        ControlTypes GetControlType() const noexcept override
        {
            return ControlTypes::MouseCursor;
        }

        [[nodiscard]]
        bool Create(const std::filesystem::path& path) noexcept;

        void Clear() override;

        void Render() override;

        void Shutdown() noexcept;

        [[nodiscard]] bool SetupFromJson(const nlohmann::json& controlJson,
                                         const std::shared_ptr<UI>& ui,
                                         const std::vector<std::pair<std::string, std::string>>& parameters) override;

    private:
        std::shared_ptr<graphics::Texture> _texture;
    };
}

#endif
