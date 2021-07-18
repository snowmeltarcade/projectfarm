#ifndef PROJECTFARM_GAME_H
#define PROJECTFARM_GAME_H

#include <vector>
#include <string>

#include "world/world.h"

namespace projectfarm::shared::game
{
    class Game final
    {
    public:
        Game(bool isClient, std::string name)
            : _isClient(isClient),
              _name(std::move(name))
        {}
        ~Game()
        {
            this->Shutdown();
        }

        [[nodiscard]]
        bool Initialize() noexcept;

        void Run() noexcept;

    private:
        bool _isClient {true};
        std::string _name;

        std::vector<world::World> _worlds;
        std::vector<std::future<void>> _worldPromises;

        void Shutdown() noexcept;

        [[nodiscard]]
        bool AddWorlds() noexcept;

        void Log(const std::string& message) const noexcept;
    };
}

#endif
