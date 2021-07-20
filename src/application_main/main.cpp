#include <iostream>
#include <future>

#include "version.h"
#include "platform/platform_id.h"
#include "api/logging/logging.h"
#include "game/game.h"
#include "api/windowing/window.h"

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

    game::Game server(false, "server", std::make_unique<windowing::Window>());
    game::Game client(true, "client", std::make_unique<windowing::Window>());

    auto serverPromise = std::async(std::launch::async, &game::Game::Run, &server);
    auto clientPromise = std::async(std::launch::async, &game::Game::Run, &client);

    clientPromise.wait();
    serverPromise.wait();

    logging::Log("Exiting application...");

	return 0;
}
