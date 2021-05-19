#ifndef PROJECTFARM_SDLFREEABLESURFACE_H
#define PROJECTFARM_SDLFREEABLESURFACE_H

#include <SDL.h>

namespace projectfarm::graphics
{
    class SDLFreeableSurface final
    {
    public:
        explicit SDLFreeableSurface(SDL_Surface* surface)
        : _surface(surface)
        {}

        ~SDLFreeableSurface()
        {
            this->FreeSurface();
        }

        void FreeSurface()
        {
            SDL_FreeSurface(this->_surface);
            this->_surface = nullptr;
        }

        [[nodiscard]] SDL_Surface* GetSurface() const
        {
            return this->_surface;
        }

    private:
        SDL_Surface* _surface = nullptr;
    };
}

#endif
