#ifndef PROJECTFARM_VECTOR2D_SCRIPT_OBJECT_H
#define PROJECTFARM_VECTOR2D_SCRIPT_OBJECT_H

#include <memory>
#include <v8.h>

#include "math/vector2d.h"

namespace projectfarm::shared::scripting::math
{
    class Vector2DScriptObject final
    {
    public:
        static inline constexpr const int InternalFieldIndex_Object = 0;

        Vector2DScriptObject() = delete;
        ~Vector2DScriptObject() = delete;

        static v8::Local<v8::Object> GetObjectTemplateInstance(v8::Isolate* isolate,
                                                               shared::math::ScriptableVector2D* vector) noexcept;

    private:
        static v8::Persistent<v8::ObjectTemplate> _controlTemplate;

        static v8::Local<v8::ObjectTemplate> GetObjectTemplate(v8::Isolate* isolate) noexcept;

        static void XGetter(v8::Local<v8::String> property,
                            const v8::PropertyCallbackInfo<v8::Value>& info);
        static void XSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value,
                            const v8::PropertyCallbackInfo<void>& info);

        static void YGetter(v8::Local<v8::String> property,
                            const v8::PropertyCallbackInfo<v8::Value>& info);
        static void YSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value,
                            const v8::PropertyCallbackInfo<void>& info);
    };
}

#endif
