#ifndef PROJECTFARM_ENGINE_H
#define PROJECTFARM_ENGINE_H

#include <vector>
#include <utility>

#include "game.h"

namespace projectfarm::shared::game
{
    class Engine final
    {
    public:
        Engine(std::vector<Game>&& games)
            : _games(std::forward<std::vector<Game>>(games))
        {
        }
        ~Engine() noexcept
        {
            this->Shutdown();
        }
        Engine(const Engine&) = delete;
        Engine(Engine&&) = default;

        [[nodiscard]]
        bool Init() noexcept;

        [[nodiscard]]
        bool Run() noexcept;

    private:
        std::vector<Game> _games;

        void Shutdown() noexcept;
    };
}

#endif
