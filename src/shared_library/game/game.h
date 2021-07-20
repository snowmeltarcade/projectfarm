#ifndef PROJECTFARM_GAME_H
#define PROJECTFARM_GAME_H

#include <memory>
#include <vector>
#include <string>

#include "world/world.h"
#include "api/windowing/iwindow.h"

namespace projectfarm::shared::game
{
    class Game final
    {
    public:
        Game(bool isClient,
             std::string name,
             std::unique_ptr<api::windowing::IWindow> window) noexcept
            : _isClient(isClient),
              _name(std::move(name)),
              _window(std::move(window))
        {}
        ~Game() noexcept
        {
            this->Shutdown();
        }

        [[nodiscard]]
        bool Initialize() noexcept;

        void Run() noexcept;

    private:
        bool _isClient {true};
        std::string _name;

        std::unique_ptr<api::windowing::IWindow> _window;

        std::vector<world::World> _worlds;
        std::vector<std::future<void>> _worldPromises;

        void Shutdown() noexcept;

        [[nodiscard]]
        bool AddWorlds() noexcept;

        void Log(const std::string& message) const noexcept;
    };
}

#endif
