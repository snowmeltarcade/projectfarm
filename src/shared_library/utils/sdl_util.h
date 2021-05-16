#ifndef PROJECTFARM_SDL_UTIL_H
#define PROJECTFARM_SDL_UTIL_H

#include <SDL_net.h>

// we want this to be in the global namespace, as SDL itself is not in a namespace

[[nodiscard]]
bool operator== (const IPaddress& left, const IPaddress& right) noexcept;

[[nodiscard]]
bool operator!= (const IPaddress& left, const IPaddress& right) noexcept;

#endif
