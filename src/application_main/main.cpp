#include <iostream>
#include <future>

#include "version.h"
#include "platform/platform_id.h"
#include "api/logging/logging.h"
#include "game/game.h"
#include "api/windowing/system.h"
#include "api/windowing/windows/window.h"

using namespace projectfarm::shared;
using namespace projectfarm::shared::api;
using namespace std::literals;

int main(int, char*[])
{
    logging::Log("Starting application...");
#if !defined(IS_IOS)
    logging::Log("Project Name: "s + PROJECT_NAME);
    logging::Log("Project Version: "s + PROJECT_VERSION);
#endif

    if (!api::windowing::system::Init())
    {
        logging::Log("Failed to init windowing system.");
        return 1;
    }

    game::Game server(false, "server", std::make_unique<windowing::windows::Window>());
    game::Game client(true, "client", std::make_unique<windowing::windows::Window>());

    auto serverPromise = std::async(std::launch::async, &game::Game::Run, &server);
    auto clientPromise = std::async(std::launch::async, &game::Game::Run, &client);

    clientPromise.wait();
    serverPromise.wait();

    api::windowing::system::Shutdown();

    logging::Log("Exiting application...");

	return 0;
}
