#ifndef PROJECTFARM_SYSTEM_ARGUMENTS_H
#define PROJECTFARM_SYSTEM_ARGUMENTS_H

#include <filesystem>

namespace projectfarm::server
{
    class SystemArguments final
    {
    public:
        SystemArguments() = default;
        ~SystemArguments() = default;

        void SetArguments(int argc, char* argv[]);

        [[nodiscard]]
        std::filesystem::path GetBinaryPath() const
        {
            return this->_binaryPath;
        }

    private:
        std::filesystem::path _binaryPath;
    };
}

#endif