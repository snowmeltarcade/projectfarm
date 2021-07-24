#include <iostream>
#include <future>
#include <vector>

#include "version.h"
#include "platform/platform_id.h"
#include "api/logging/logging.h"
#include "game/engine.h"
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

    std::vector<game::Game> games;
    games.emplace_back(false, "server", std::make_unique<windowing::windows::Window>());
    games.emplace_back(true, "client", std::make_unique<windowing::windows::Window>());

    game::Engine engine(std::move(games));

    if (!engine.Init())
    {
        logging::Log("Failed to init engine.");
        return 1;
    }

    if (!engine.Run())
    {
        logging::Log("Failed to run engine.");
        return 1;
    }

    logging::Log("Exiting application...");

	return 0;
}
