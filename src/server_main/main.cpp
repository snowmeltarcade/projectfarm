#include <iostream>
#include <SDL.h>

#include "server/platform.h"
#if !defined(IS_IOS)
#include "version.h"
#endif
#include "server/server.h"

int main(int argc, char* argv[])
{
	std::cout << "Starting game server..." << std::endl;
#if !defined(IS_IOS)
	std::cout << "Project Name: " << PROJECT_NAME << std::endl;
    std::cout << "Project Version: " << PROJECT_VERSION << std::endl;
#endif

	try
	{
		auto server = std::make_shared<projectfarm::server::Server>();

		server->Run(argc, argv);
	}
	catch (const std::exception& ex)
	{
		std::cout << "Something seriously bad just happened:\n" << ex.what() << std::endl;
	}

	std::cout << "Exiting game server." << std::endl;

	return 0;
}
