#ifndef PROJECTFARM_UI_CONTROL_SCRIPT_H
#define PROJECTFARM_UI_CONTROL_SCRIPT_H

#include <vector>

#include <v8.h>

#include "scripting/script.h"
#include "scripting/function_types.h"

namespace projectfarm::graphics::ui
{
    class BaseControl;
    class UI;
}

namespace projectfarm::engine
{
    class KeyboardInput;
}

namespace projectfarm::scenes
{
    class Scene;
}

namespace projectfarm::engine::scripting
{
    class UIControlScript final : public shared::scripting::Script
    {
    public:
        // Be sure the update `GetNumberOfInternalFieldsNeeded()` if any of these values are updated
        static inline constexpr const int InternalFieldIndex_Control = 1;
        static inline constexpr const int InternalFieldIndex_UI = 2;
        static inline constexpr const int InternalFieldIndex_Scene = 3;
        static inline constexpr const int InternalFieldIndex_KeyboardInput = 4;

        UIControlScript() = default;
        ~UIControlScript() override = default;

        [[nodiscard]] std::vector<std::pair<shared::scripting::FunctionTypes, bool>> GetFunctions() const noexcept override;

        void SetupGlobalTemplate(v8::Local<v8::ObjectTemplate>& globalTemplate) noexcept override;

        static void GetCurrentControl(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void GetUI(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void GetScene(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void GetKeyboardInput(const v8::FunctionCallbackInfo<v8::Value>& args);

        [[nodiscard]] uint8_t GetNumberOfInternalFieldsNeeded() const noexcept override
        {
            return 4;
        }

        void SetObjectInternalFields(const std::shared_ptr<graphics::ui::BaseControl>& control,
                                     const std::shared_ptr<graphics::ui::UI>& ui,
                                     const std::shared_ptr<scenes::Scene>& scene,
                                     const std::shared_ptr<engine::KeyboardInput>& keyboardInput)
        {
            this->SetObjectInternalField(control.get(), UIControlScript::InternalFieldIndex_Control);
            this->SetObjectInternalField(ui.get(), UIControlScript::InternalFieldIndex_UI);
            this->SetObjectInternalField(scene.get(), UIControlScript::InternalFieldIndex_Scene);
            this->SetObjectInternalField(keyboardInput.get(), UIControlScript::InternalFieldIndex_KeyboardInput);
        }
    };
}

#endif
