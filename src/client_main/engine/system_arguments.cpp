#include "utils/util.h"
#include "system_arguments.h"

namespace projectfarm::engine
{
	bool SystemArguments::SetArguments(int argc, char* argv[]) noexcept
	{
		this->LogMessage("Arguments:");

		for (int i = 0; i < argc; ++i)
		{
			std::string arg = argv[i];

            if (pfu::startsWith(arg, "-password"))
            {
                this->LogMessage("-password=*****");
            }
            else
            {
                this->LogMessage(arg);
            }

			if (i == 0)
			{
				std::filesystem::path binaryFullPath = arg;
				this->_binaryPath = binaryFullPath.remove_filename();
			}
			else if (pfu::startsWith(arg, "-startserver"))
			{
				this->_shouldStartServer = true;
			}
            else if (pfu::startsWith(arg, "-username"))
            {
                auto parts = pfu::split("=", arg);
                if (parts.size() != 2 || parts[1].empty())
                {
                    this->LogMessage("Please enter a username.");
                    return false;
                }

                this->_userName = parts[1];
            }
            else if (pfu::startsWith(arg, "-password"))
            {
                auto parts = pfu::split("=", arg);
                if (parts.size() != 2 || parts[1].empty())
                {
                    this->LogMessage("Please enter a password.");
                    return false;
                }

                this->_password = parts[1];
            }
		}

		// only one is empty, not both
		if (this->_userName.empty() != this->_password.empty())
        {
		    this->LogMessage("Please enter both username and password.");
		    return false;
        }

		return true;
	}
}