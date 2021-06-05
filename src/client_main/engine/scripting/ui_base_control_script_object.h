#ifndef PROJECTFARM_UI_BASE_CONTROL_SCRIPT_OBJECT_H
#define PROJECTFARM_UI_BASE_CONTROL_SCRIPT_OBJECT_H

#include <memory>
#include <v8.h>

#include "graphics/ui/base_control.h"

namespace projectfarm::engine::scripting
{
    class UIBaseControlScriptObject final
    {
    public:
        static inline constexpr const int InternalFieldIndex_Control = 0;

        UIBaseControlScriptObject() = delete;
        ~UIBaseControlScriptObject() = delete;

        static v8::Local<v8::Object> GetObjectTemplateInstance(v8::Isolate* isolate,
                                                               graphics::ui::BaseControl* control) noexcept;

    private:
        static v8::Persistent<v8::ObjectTemplate> _controlTemplate;

        static v8::Local<v8::ObjectTemplate> GetObjectTemplate(v8::Isolate* isolate) noexcept;

        static void IdGetter(v8::Local<v8::String> property,
                             const v8::PropertyCallbackInfo<v8::Value>& info);
        static void IdSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value,
                             const v8::PropertyCallbackInfo<void>& info);

        static void TextGetter(v8::Local<v8::String> property,
                               const v8::PropertyCallbackInfo<v8::Value>& info);
        static void TextSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value,
                               const v8::PropertyCallbackInfo<void>& info);

        static void WidthGetter(v8::Local<v8::String> property,
                                const v8::PropertyCallbackInfo<v8::Value>& info);
        static void WidthSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value,
                                const v8::PropertyCallbackInfo<void>& info);

        static void HeightGetter(v8::Local<v8::String> property,
                                 const v8::PropertyCallbackInfo<v8::Value>& info);
        static void HeightSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value,
                                 const v8::PropertyCallbackInfo<void>& info);

        static void PosXGetter(v8::Local<v8::String> property,
                               const v8::PropertyCallbackInfo<v8::Value>& info);
        static void PosXSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value,
                               const v8::PropertyCallbackInfo<void>& info);

        static void PosYGetter(v8::Local<v8::String> property,
                               const v8::PropertyCallbackInfo<v8::Value>& info);
        static void PosYSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value,
                               const v8::PropertyCallbackInfo<void>& info);

        static void IsVisibleGetter(v8::Local<v8::String> property,
                                    const v8::PropertyCallbackInfo<v8::Value>& info);
        static void IsVisibleSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value,
                                    const v8::PropertyCallbackInfo<void>& info);

        static void IsEnabledGetter(v8::Local<v8::String> property,
                                    const v8::PropertyCallbackInfo<v8::Value>& info);
        static void IsEnabledSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value,
                                    const v8::PropertyCallbackInfo<void>& info);

        static void GetCustomPropertyString(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void GetCustomPropertyInt(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void GetCustomPropertyBool(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void GetCustomPropertyVector2D(const v8::FunctionCallbackInfo<v8::Value>& args);

        static void GetControlById(const v8::FunctionCallbackInfo<v8::Value>& args);

        // this returns the custom control the current control is actually in.
        // if this control is not in a custom control, `undefined` is returned
        static void GetParentCustomControl(const v8::FunctionCallbackInfo<v8::Value>& args);

        // this calls a function defined in a script attached to this control.
        // the first argument should be the function name. any remaining arguments
        // are passed as parameters to the function
        static void InvokeJavascriptFunction(const v8::FunctionCallbackInfo<v8::Value>& args);
    };
}

#endif
