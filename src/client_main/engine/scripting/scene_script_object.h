#ifndef PROJECTFARM_SCENE_SCRIPT_OBJECT_H
#define PROJECTFARM_SCENE_SCRIPT_OBJECT_H

#include <memory>
#include <v8.h>

#include "scenes/scene.h"

namespace projectfarm::engine::scripting
{
    class SceneScriptObject final
    {
    public:
        static inline constexpr const int InternalFieldIndex_Scene = 0;

        SceneScriptObject() = delete;
        ~SceneScriptObject() = delete;

        static v8::Local<v8::Object> GetObjectTemplateInstance(v8::Isolate* isolate,
                                                               scenes::Scene* scene) noexcept;

    private:
        static v8::Persistent<v8::ObjectTemplate> _controlTemplate;

        static v8::Local<v8::ObjectTemplate> GetObjectTemplate(v8::Isolate* isolate) noexcept;

        static void SendMessage(const v8::FunctionCallbackInfo<v8::Value>& args);
    };
}

#endif
