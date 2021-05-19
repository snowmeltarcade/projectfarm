#include <iostream>
#include <memory>

#include "engine/platform.h"
#if !defined(IS_IOS)
#include "version.h"
#endif
#include "engine/game.h"

int main(int argc, char* argv[])
{
	std::cout << "Starting game client..." << std::endl;
#if !defined(IS_IOS)
    std::cout << "Project Name: " << PROJECT_NAME << std::endl;
    std::cout << "Project Version: " << PROJECT_VERSION << std::endl;
#endif

	try
	{
		std::shared_ptr<projectfarm::engine::Game> game = std::make_shared<projectfarm::engine::Game>();

		game->Run(argc, argv);
	}
	catch (const std::exception& ex)
	{
		std::cout << "Something seriously bad just happened:" << std::endl;
		std::cout << ex.what() << std::endl;
	}

	std::cout << "Exiting game client." << std::endl;

	return 0;
}
