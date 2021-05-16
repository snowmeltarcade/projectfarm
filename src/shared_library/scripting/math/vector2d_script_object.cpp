#include "vector2d_script_object.h"
#include "scripting/script.h"

namespace projectfarm::shared::scripting::math
{
    v8::Persistent<v8::ObjectTemplate> Vector2DScriptObject::_controlTemplate;

    v8::Local<v8::Object> Vector2DScriptObject::GetObjectTemplateInstance(v8::Isolate* isolate,
                                                                          shared::math::ScriptableVector2D* vector) noexcept
    {
        auto controlTemplate = Vector2DScriptObject::GetObjectTemplate(isolate);

        auto context = isolate->GetCurrentContext();

        auto instance = controlTemplate->NewInstance(context).ToLocalChecked();
        instance->SetInternalField(Vector2DScriptObject::InternalFieldIndex_Object,
                                   v8::External::New(isolate, vector));

        return instance;
    }

    v8::Local<v8::ObjectTemplate> Vector2DScriptObject::GetObjectTemplate(v8::Isolate* isolate) noexcept
    {
        if (!Vector2DScriptObject::_controlTemplate.IsEmpty())
        {
            return Vector2DScriptObject::_controlTemplate.Get(isolate);
        }

        auto controlTemplate = v8::ObjectTemplate::New(isolate);

        controlTemplate->SetInternalFieldCount(1);

        controlTemplate->SetAccessor(shared::scripting::Script::StringToParameter("x", isolate),
                                     Vector2DScriptObject::XGetter,
                                     Vector2DScriptObject::XSetter);

        controlTemplate->SetAccessor(shared::scripting::Script::StringToParameter("y", isolate),
                                     Vector2DScriptObject::YGetter,
                                     Vector2DScriptObject::YSetter);

        Vector2DScriptObject::_controlTemplate.Reset(isolate, controlTemplate);
        return Vector2DScriptObject::_controlTemplate.Get(isolate);
    }

    void Vector2DScriptObject::XGetter(v8::Local<v8::String>,
                                       const v8::PropertyCallbackInfo<v8::Value>& info)
    {
        auto self = info.Holder();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(
                Vector2DScriptObject::InternalFieldIndex_Object));
        auto vector = static_cast<shared::math::ScriptableVector2D*>(wrap->Value());

        info.GetReturnValue().Set(vector->_object.GetX());
    }

    void Vector2DScriptObject::XSetter(v8::Local<v8::String>, v8::Local<v8::Value> value,
                                       const v8::PropertyCallbackInfo<void>& info)
    {
        auto self = info.Holder();
        auto isolate = info.GetIsolate();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(
                Vector2DScriptObject::InternalFieldIndex_Object));
        auto vector = static_cast<shared::math::ScriptableVector2D*>(wrap->Value());

        auto x = value->Int32Value(isolate->GetCurrentContext()).ToChecked();

        vector->_object.SetX(x);
    }

    void Vector2DScriptObject::YGetter(v8::Local<v8::String>,
                                       const v8::PropertyCallbackInfo<v8::Value>& info)
    {
        auto self = info.Holder();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(
                Vector2DScriptObject::InternalFieldIndex_Object));
        auto vector = static_cast<shared::math::ScriptableVector2D*>(wrap->Value());

        info.GetReturnValue().Set(vector->_object.GetY());
    }

    void Vector2DScriptObject::YSetter(v8::Local<v8::String>, v8::Local<v8::Value> value,
                                       const v8::PropertyCallbackInfo<void>& info)
    {
        auto self = info.Holder();
        auto isolate = info.GetIsolate();

        auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(
                Vector2DScriptObject::InternalFieldIndex_Object));
        auto vector = static_cast<shared::math::ScriptableVector2D*>(wrap->Value());

        auto y = value->Int32Value(isolate->GetCurrentContext()).ToChecked();

        vector->_object.SetY(y);
    }
}