#include <iostream>
#include <future>

#include "version.h"
#include "platform/platform_id.h"
#include "api/logging/logging.h"
#include "game/game.h"

using namespace projectfarm;
using namespace std::literals;

int main(int, char*[])
{
    shared::api::logging::Log("Starting application...");
#if !defined(IS_IOS)
    shared::api::logging::Log("Project Name: "s + PROJECT_NAME);
    shared::api::logging::Log("Project Version: "s + PROJECT_VERSION);
#endif

    shared::game::Game server(false, "server");
    shared::game::Game client(true, "client");

    auto serverPromise = std::async(std::launch::async, &shared::game::Game::Run, &server);
    auto clientPromise = std::async(std::launch::async, &shared::game::Game::Run, &client);

    clientPromise.wait();
    serverPromise.wait();

    shared::api::logging::Log("Exiting application...");

	return 0;
}
