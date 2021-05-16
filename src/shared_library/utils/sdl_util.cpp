#include "sdl_util.h"

[[nodiscard]] bool operator== (const IPaddress& left, const IPaddress& right) noexcept
{
    return left.port == right.port &&
           left.host == right.host;
}

[[nodiscard]] bool operator!= (const IPaddress& left, const IPaddress& right) noexcept
{
    return !(left == right);
}