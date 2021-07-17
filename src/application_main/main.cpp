#include <iostream>

#include "version.h"
#include "platform/platform_id.h"

int main(int argc, char* argv[])
{
	std::cout << "Starting application..." << std::endl;
#if !defined(IS_IOS)
    std::cout << "Project Name: " << PROJECT_NAME << std::endl;
    std::cout << "Project Version: " << PROJECT_VERSION << std::endl;
#endif

	std::cout << "Exiting application." << std::endl;

	return 0;
}
