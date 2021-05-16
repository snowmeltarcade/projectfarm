#include "system_arguments.h"

namespace projectfarm::server
{
    void SystemArguments::SetArguments(int argc, char* argv[])
    {
        this->_logger->LogMessage("Arguments:");
        for (int i = 0; i < argc; i++)
        {
            std::string arg = argv[i];

            this->_logger->LogMessage(arg);

            if (i == 0)
            {
                std::filesystem::path binaryFullPath = arg;
                this->_binaryPath = binaryFullPath.remove_filename();
            }
        }
    }
}