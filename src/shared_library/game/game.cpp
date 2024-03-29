#include <future>
#include <filesystem>

#include "game.h"
#include "api/logging/logging.h"
#include "world/controllers/intro_credits.h"
#include "world/controllers/master_logic.h"
#include "world/ecs/systems/render.h"

using namespace std::literals;

namespace projectfarm::shared::game
{
    bool Game::Initialize() noexcept
    {
        this->Log("Initializing game...");

        this->Log("Game being created as "s + (this->_isClient ? "client" : "server"));

        if (!this->InitializeWindow())
        {
            this->Log("Failed to initialize window.");
            return false;
        }

        if (!this->AddWorlds())
        {
            this->Log("Failed to add worlds.");
            return false;
        }

        this->_worldPromises.clear();

        for (auto& world : this->_worlds)
        {
            auto promise = std::async(std::launch::async, &world::World::Run, &world);
            this->_worldPromises.emplace_back(std::move(promise));
        }

        this->Log("Initialized game.");

        return true;
    }

    bool Game::RunFrame() noexcept
    {
        this->_window->PumpMessages();

        // process anything else that needs processing, such
        // as network requests etc...

        return true;
    }

    bool Game::InitializeWindow() noexcept
    {
        if (!this->_window)
        {
            this->Log("Window is null.");
            return false;
        }

        // TODO: Get from new data provider APi
        std::filesystem::path configPath = "window.json";

        if (!this->_window->LoadFromConfig(configPath))
        {
            this->Log("Failed to load window from config: " + configPath.u8string());
            return false;
        }

        if (!this->_window->SetTitle(this->_name))
        {
            this->Log("Failed to set window title to: " + this->_name);
            return false;
        }

        return true;
    }

    void Game::Shutdown() noexcept
    {
        this->Log("Shutting down game...");

        for (auto& promise : this->_worldPromises)
        {
            promise.wait();
        }

        this->_worldPromises.clear();

        this->Log("Shut down game.");
    }

    bool Game::AddWorlds() noexcept
    {
        this->_worlds.clear();

        if (this->_isClient)
        {
            // the client only has the one world, which is setup by whatever it receives from the server
            world::World world(std::make_unique<world::controllers::IntroCredits>());

            auto renderSystem = std::make_unique<world::ecs::systems::Render>();
            world.AddECSSystem(std::move(renderSystem));

            // add cameras...
            // add render targets...

            this->_worlds.emplace_back(std::move(world));
        }
        else
        {
            // the server has all worlds
            world::World world(std::make_unique<world::controllers::MasterLogic>());

            this->_worlds.emplace_back(std::move(world));
        }

        return true;
    }

    void Game::Log(const std::string& message) const noexcept
    {
        api::logging::Log("Game (" + this->_name + "): " + message);
    }
}
