#ifndef SYSTEM_ARGUMENTS_H
#define SYSTEM_ARGUMENTS_H

#include <filesystem>

namespace projectfarm::engine
{
	class SystemArguments final
	{
	public:
		SystemArguments() = default;
		~SystemArguments() = default;

		[[nodiscard]]
		bool SetArguments(int argc, char* argv[]) noexcept;

		[[nodiscard]]
		std::filesystem::path GetBinaryPath() const noexcept
		{
			return this->_binaryPath;
		}

		[[nodiscard]]
		bool GetShouldStartServer() const noexcept
		{
			return this->_shouldStartServer;
		}

        [[nodiscard]]
        const std::string& GetUserName() const noexcept
        {
            return this->_userName;
        }

        [[nodiscard]]
        const std::string& GetPassword() const noexcept
        {
            return this->_password;
        }
		
	private:
		std::filesystem::path _binaryPath;

		bool _shouldStartServer = false;

		std::string _userName;
		std::string _password;
	};
}

#endif