#ifndef PROJECTFARM_CHARACTER_SCRIPT_OBJECT_H
#define PROJECTFARM_CHARACTER_SCRIPT_OBJECT_H

#include <memory>
#include <v8.h>

#include "engine/entities/character.h"

namespace projectfarm::engine::scripting
{
    class CharacterScriptObject final
    {
    public:
        CharacterScriptObject() = delete;
        ~CharacterScriptObject() = delete;

        static v8::Local<v8::Object> GetObjectTemplateInstance(v8::Isolate* isolate,
                                                               entities::Character* character) noexcept;

    private:
        static v8::Persistent<v8::ObjectTemplate> _characterTemplate;

        static v8::Local<v8::ObjectTemplate> GetObjectTemplate(v8::Isolate* isolate) noexcept;

        static void PositionXGetter(v8::Local<v8::String> property,
                                    const v8::PropertyCallbackInfo<v8::Value>& info);
        static void PositionXSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value,
                                    const v8::PropertyCallbackInfo<void>& info);

        static void PositionYGetter(v8::Local<v8::String> property,
                                    const v8::PropertyCallbackInfo<v8::Value>& info);
        static void PositionYSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value,
                                    const v8::PropertyCallbackInfo<void>& info);

        static void TypeGetter(v8::Local<v8::String> property,
                               const v8::PropertyCallbackInfo<v8::Value>& info);
        static void TypeSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value,
                               const v8::PropertyCallbackInfo<void>& info);
    };
}

#endif
