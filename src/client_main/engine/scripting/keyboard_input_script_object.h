#ifndef PROJECTFARM_KEYBOARD_INPUT_SCRIPT_OBJECT_H
#define PROJECTFARM_KEYBOARD_INPUT_SCRIPT_OBJECT_H

#include <memory>
#include <v8.h>

#include "engine/keyboard_input.h"

namespace projectfarm::engine::scripting
{
    class KeyboardInputScriptObject final
    {
    public:
        static inline constexpr const int InternalFieldIndex_KeyboardInput = 0;

        KeyboardInputScriptObject() = delete;
        ~KeyboardInputScriptObject() = delete;

        static v8::Local<v8::Object> GetObjectTemplateInstance(v8::Isolate* isolate,
                                                               engine::KeyboardInput* keyboardInput) noexcept;

    private:
        static v8::Persistent<v8::ObjectTemplate> _controlTemplate;

        static v8::Local<v8::ObjectTemplate> GetObjectTemplate(v8::Isolate* isolate) noexcept;

        static void IsKeyDown(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void IsKeyPressed(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void IsKeyboardStateEnabled(const v8::FunctionCallbackInfo<v8::Value>& args);

        static SDL_Keycode GetKeyCodeFromKey(std::string_view key) noexcept;
    };
}

#endif
