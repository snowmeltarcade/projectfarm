#include <thread>
#include <filesystem>

#include "system.h"

namespace projectfarm::engine
{
	void System::StartServer()
	{
#if !defined(IS_IOS)
		this->LogMessage("Starting server...");
        auto devFolder = this->_dataProvider->GetDevDirectoryPath();

#ifdef WIN32
        std::string command = (devFolder / "startserver.bat").string();
#else
        std::string command = (devFolder / "startserver.sh").string();
#endif

        this->LogMessage("Running command:");
        this->LogMessage(command);
        std::system(command.c_str());

		this->_serverStarted = true;

		// give the server a couple of seconds to load
		std::this_thread::sleep_for(std::chrono::seconds(2));
		
		this->LogMessage("Started server.");
#endif
	}

	void System::StopServer()
	{
#if !defined(IS_IOS)
		this->LogMessage("Stopping server...");
        auto devFolder = this->_dataProvider->GetDevDirectoryPath();

#ifdef WIN32
        std::string command = (devFolder / "stopserver.bat").string();
#else
        std::string command = (devFolder / "stopserver.sh").string();
#endif

        this->LogMessage("Running command:");
        this->LogMessage(command);
        std::system(command.c_str());

		this->LogMessage("Stopped server.");
#endif
	}

	void System::Shutdown()
	{
		if (this->_serverStarted)
		{
			this->StopServer();
		}
	}
}
