#include <thread>
#include <filesystem>
#include <sstream>

#include "system.h"

namespace projectfarm::engine
{
    void System::Initialize() noexcept
    {
        this->_platformType = engine::GetPlatformType();

        this->LogSystemInfo();
    }

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

    void System::LogSystemInfo() noexcept
    {
        std::stringstream ss;

        ss << "System Information" << std::endl;
        ss << "==================" << std::endl;

        ss << "Platform: ";

        switch (this->_platformType)
        {
            case PlatformTypes::Windows:
            {
                ss << "Windows";
                break;
            }
            case PlatformTypes::MacOS:
            {
                ss << "MacOS";
                break;
            }
            case PlatformTypes::Linux:
            {
                ss << "Linux";
                break;
            }
            case PlatformTypes::iOS:
            {
                ss << "iOS";
                break;
            }
            case PlatformTypes::iOS_Simulator:
            {
                ss << "iOS Simulator";
                break;
            }
            case PlatformTypes::iPadOS:
            {
                ss << "iPadOS";
                break;
            }
            case PlatformTypes::iPadOS_Simulator:
            {
                ss << "iPadOS Simulator";
                break;
            }
            case PlatformTypes::tvOS:
            {
                ss << "tvOS";
                break;
            }
            case PlatformTypes::Android:
            {
                ss << "Android";
                break;
            }
            case PlatformTypes::AndroidTablet:
            {
                ss << "Android Tablet";
                break;
            }
            case PlatformTypes::Unknown:
            {
                ss << "Unknown";
                break;
            }
        }

        ss << std::endl;

        this->_logger->LogMessage(ss.str());
    }
}
