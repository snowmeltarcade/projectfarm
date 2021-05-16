#ifndef PROJECTFARM_FUNCTION_TYPES_H
#define PROJECTFARM_FUNCTION_TYPES_H

namespace projectfarm::shared::scripting
{
    enum class FunctionTypes
    {
        UnknownFunction,
        Update,
        Init,
        OnLoad,
        OnShutdown,
        OnClick,
        OnMouseEnter,
        OnMouseLeave,
        OnKeyPress,
        OnKeyPress_Enter,
        OnDrag,
        SendMessage,
    };
}

#endif
