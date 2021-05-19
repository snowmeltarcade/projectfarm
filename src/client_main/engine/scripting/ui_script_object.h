#ifndef PROJECTFARM_UI_SCRIPT_OBJECT_H
#define PROJECTFARM_UI_SCRIPT_OBJECT_H

#include <memory>
#include <v8.h>

#include "graphics/ui/base_control.h"

namespace projectfarm::engine::scripting
{
    class UIScriptObject final
    {
    public:
        static inline constexpr const int InternalFieldIndex_UI = 0;

        UIScriptObject() = delete;
        ~UIScriptObject() = delete;

        static v8::Local<v8::Object> GetObjectTemplateInstance(v8::Isolate* isolate,
                                                               graphics::ui::UI* ui) noexcept;

    private:
        static v8::Persistent<v8::ObjectTemplate> _controlTemplate;

        static v8::Local<v8::ObjectTemplate> GetObjectTemplate(v8::Isolate* isolate) noexcept;

        static void GetControlById(const v8::FunctionCallbackInfo<v8::Value>& args);
    };
}

#endif
