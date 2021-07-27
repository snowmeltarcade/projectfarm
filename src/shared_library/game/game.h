#ifndef PROJECTFARM_GAME_H
#define PROJECTFARM_GAME_H

#include <memory>
#include <vector>
#include <string>

#include "world/world.h"
#include "api/windowing/windows/iwindow.h"

namespace projectfarm::shared::game
{
    class Game final
    {
    public:
        Game(bool isClient,
             std::string name,
             std::unique_ptr<api::windowing::windows::IWindow> window) noexcept
            : _isClient(isClient),
              _name(std::move(name)),
              _window(std::move(window))
        {}
        ~Game() noexcept
        {
            this->Shutdown();
        }
        Game(const Game&) = delete;
        Game(Game&&) = default;

        [[nodiscard]]
        bool Initialize() noexcept;

        [[nodiscard]]
        bool RunFrame() noexcept;

        [[nodiscard]]
        const std::string& GetName() const noexcept
        {
            return this->_name;
        }

    private:
        bool _isClient {true};
        std::string _name;

        std::unique_ptr<api::windowing::windows::IWindow> _window;

        std::vector<world::World> _worlds;
        std::vector<std::future<void>> _worldPromises;

        [[nodiscard]]
        bool InitializeWindow() noexcept;

        void Shutdown() noexcept;

        [[nodiscard]]
        bool AddWorlds() noexcept;

        void Log(const std::string& message) const noexcept;
    };
}

#endif
