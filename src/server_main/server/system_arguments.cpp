#include "system_arguments.h"
#include "api/logging/logging.h"

namespace projectfarm::server
{
    void SystemArguments::SetArguments(int argc, char* argv[])
    {
        shared::api::logging::Log("Arguments:");
        for (int i = 0; i < argc; i++)
        {
            std::string arg = argv[i];

            shared::api::logging::Log(arg);

            if (i == 0)
            {
                std::filesystem::path binaryFullPath = arg;
                this->_binaryPath = binaryFullPath.remove_filename();
            }
        }
    }
}