#include "keyboard_input_script_object.h"
#include "scripting/script.h"
#include "utils/util.h"

namespace projectfarm::engine::scripting
{
    v8::Persistent<v8::ObjectTemplate> KeyboardInputScriptObject::_controlTemplate;

    v8::Local<v8::Object> KeyboardInputScriptObject::GetObjectTemplateInstance(
            v8::Isolate* isolate,
            engine::KeyboardInput* keyboardInput) noexcept
    {
        auto controlTemplate = KeyboardInputScriptObject::GetObjectTemplate(isolate);

        auto context = isolate->GetCurrentContext();

        auto instance = controlTemplate->NewInstance(context).ToLocalChecked();
        instance->SetInternalField(KeyboardInputScriptObject::InternalFieldIndex_KeyboardInput,
                                   v8::External::New(isolate, keyboardInput));

        return instance;
    }

    v8::Local<v8::ObjectTemplate> KeyboardInputScriptObject::GetObjectTemplate(v8::Isolate* isolate) noexcept
    {
        if (!KeyboardInputScriptObject::_controlTemplate.IsEmpty())
        {
            return KeyboardInputScriptObject::_controlTemplate.Get(isolate);
        }

        auto controlTemplate = v8::ObjectTemplate::New(isolate);

        controlTemplate->SetInternalFieldCount(1);

        controlTemplate->Set(shared::scripting::Script::StringToParameter("is_key_down", isolate),
                             v8::FunctionTemplate::New(isolate, &KeyboardInputScriptObject::IsKeyDown));

        controlTemplate->Set(shared::scripting::Script::StringToParameter("is_key_pressed", isolate),
                             v8::FunctionTemplate::New(isolate, &KeyboardInputScriptObject::IsKeyPressed));

        controlTemplate->Set(shared::scripting::Script::StringToParameter("is_keyboard_state_enabled", isolate),
                             v8::FunctionTemplate::New(isolate, &KeyboardInputScriptObject::IsKeyboardStateEnabled));

        KeyboardInputScriptObject::_controlTemplate.Reset(isolate, controlTemplate);
        return KeyboardInputScriptObject::_controlTemplate.Get(isolate);
    }

    void KeyboardInputScriptObject::IsKeyDown(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        auto isolate = args.GetIsolate();

        v8::HandleScope handScope(isolate);

        auto self = args.Holder();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(
                KeyboardInputScriptObject::InternalFieldIndex_KeyboardInput));
        auto keyboardInput = static_cast<engine::KeyboardInput*>(wrap->Value());

        if (args.Length() != 1)
        {
            keyboardInput->LogMessage("Invalid number of arguments for 'IsKeyDown'.");
            return;
        }

        auto key = shared::scripting::Script::ArgumentToString(isolate, args, 0);

        auto keyCode = KeyboardInputScriptObject::GetKeyCodeFromKey(key);
        if (keyCode == SDLK_UNKNOWN)
        {
            return;
        }

        auto res = keyboardInput->IsKeyDown_Keycode(keyCode);

        args.GetReturnValue().Set(res);
    }

    void KeyboardInputScriptObject::IsKeyPressed(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        auto isolate = args.GetIsolate();

        v8::HandleScope handScope(isolate);

        auto self = args.Holder();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(
                KeyboardInputScriptObject::InternalFieldIndex_KeyboardInput));
        auto keyboardInput = static_cast<engine::KeyboardInput*>(wrap->Value());

        if (args.Length() != 1)
        {
            keyboardInput->LogMessage("Invalid number of arguments for 'IsKeyPressed'.");
            return;
        }

        auto key = shared::scripting::Script::ArgumentToString(isolate, args, 0);

        auto keyCode = KeyboardInputScriptObject::GetKeyCodeFromKey(key);
        if (keyCode == SDLK_UNKNOWN)
        {
            return;
        }

        auto res = keyboardInput->IsKeyPressed_Keycode(keyCode);

        args.GetReturnValue().Set(res);
    }

    void KeyboardInputScriptObject::IsKeyboardStateEnabled(const v8::FunctionCallbackInfo<v8::Value> &args)
    {
        auto isolate = args.GetIsolate();

        v8::HandleScope handScope(isolate);

        auto self = args.Holder();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(
                KeyboardInputScriptObject::InternalFieldIndex_KeyboardInput));
        auto keyboardInput = static_cast<engine::KeyboardInput*>(wrap->Value());

        if (args.Length() != 1)
        {
            keyboardInput->LogMessage("Invalid number of arguments for 'IsKeyboardStateEnabled'.");
            return;
        }

        auto key = shared::scripting::Script::ArgumentToString(isolate, args, 0);

        auto keyCode = KeyboardInputScriptObject::GetKeyCodeFromKey(key);
        if (keyCode == SDLK_UNKNOWN)
        {
            return;
        }

        auto res = keyboardInput->IsKeyboardStateEnabled(keyCode);

        args.GetReturnValue().Set(res);
    }

    SDL_Keycode KeyboardInputScriptObject::GetKeyCodeFromKey(std::string_view key) noexcept
    {
        auto k = pfu::tolower(key);
        k = pfu::trim(k);

        if (k == "a") return SDLK_a;
        if (k == "b") return SDLK_b;
        if (k == "c") return SDLK_c;
        if (k == "d") return SDLK_d;
        if (k == "e") return SDLK_e;
        if (k == "f") return SDLK_f;
        if (k == "g") return SDLK_g;
        if (k == "h") return SDLK_h;
        if (k == "i") return SDLK_i;
        if (k == "j") return SDLK_j;
        if (k == "k") return SDLK_k;
        if (k == "l") return SDLK_l;
        if (k == "m") return SDLK_m;
        if (k == "n") return SDLK_n;
        if (k == "o") return SDLK_o;
        if (k == "p") return SDLK_p;
        if (k == "q") return SDLK_q;
        if (k == "r") return SDLK_r;
        if (k == "s") return SDLK_s;
        if (k == "t") return SDLK_t;
        if (k == "u") return SDLK_u;
        if (k == "v") return SDLK_v;
        if (k == "w") return SDLK_w;
        if (k == "x") return SDLK_x;
        if (k == "y") return SDLK_y;
        if (k == "z") return SDLK_z;

        if (k == "0") return SDLK_0;
        if (k == "1") return SDLK_1;
        if (k == "2") return SDLK_2;
        if (k == "3") return SDLK_3;
        if (k == "4") return SDLK_4;
        if (k == "5") return SDLK_5;
        if (k == "6") return SDLK_6;
        if (k == "7") return SDLK_7;
        if (k == "8") return SDLK_8;
        if (k == "9") return SDLK_9;

        // TODO: Test these with mobile keyboard
        if (k == "!") return SDLK_1;//SDLK_EXCLAIM;
        if (k == "@") return SDLK_2;//SDLK_AT;
        if (k == "#") return SDLK_3;//SDLK_HASH;
        if (k == "$") return SDLK_4;//SDLK_DOLLAR;
        if (k == "%") return SDLK_5;//SDLK_PERCENT;
        if (k == "^") return SDLK_6;//SDLK_CARET;
        if (k == "&") return SDLK_7;//SDLK_AMPERSAND;
        if (k == "*") return SDLK_8;//SDLK_ASTERISK;
        if (k == "(") return SDLK_9;//SDLK_LEFTPAREN;
        if (k == ")") return SDLK_0;//SDLK_RIGHTPAREN;
        if (k == "`") return SDLK_BACKQUOTE;
        if (k == "~") return SDLK_BACKQUOTE; // same as `
        if (k == "-") return SDLK_MINUS;
        if (k == "_") return SDLK_MINUS;//SDLK_UNDERSCORE;
        if (k == "=") return SDLK_EQUALS;
        if (k == "+") return SDLK_EQUALS;//SDLK_PLUS;
        if (k == "[") return SDLK_LEFTBRACKET;
        if (k == "]") return SDLK_RIGHTBRACKET;
        if (k == "{") return SDLK_LEFTBRACKET;//SDLK_KP_LEFTBRACE;
        if (k == "}") return SDLK_RIGHTBRACKET;//SDLK_KP_RIGHTBRACE;
        if (k == "\\") return SDLK_BACKSLASH;
        if (k == "|") return SDLK_BACKSLASH;//SDLK_KP_VERTICALBAR;
        if (k == ";") return SDLK_SEMICOLON;
        if (k == ":") return SDLK_SEMICOLON;//SDLK_COLON;
        if (k == "'") return SDLK_QUOTE;
        if (k == "\"") return SDLK_QUOTE;//SDLK_QUOTEDBL;
        if (k == ",") return SDLK_COMMA;
        if (k == ".") return SDLK_PERIOD;
        if (k == "<") return SDLK_COMMA;//SDLK_LESS;
        if (k == ">") return SDLK_PERIOD;//SDLK_GREATER;
        if (k == "/") return SDLK_SLASH;
        if (k == "?") return SDLK_SLASH;//SDLK_QUESTION;

        if (k == "space") return SDLK_SPACE;
        if (k == "delete") return SDLK_DELETE;
        if (k == "backspace") return SDLK_BACKSPACE;
        if (k == "enter") return SDLK_RETURN;
        if (k == "lshift") return SDLK_LSHIFT;
        if (k == "rshift") return SDLK_RSHIFT;
        if (k == "capslock") return SDLK_CAPSLOCK;
        if (k == "left") return SDLK_LEFT;
        if (k == "right") return SDLK_RIGHT;
        if (k == "up") return SDLK_UP;
        if (k == "down") return SDLK_DOWN;

        return SDLK_UNKNOWN;
    }
}