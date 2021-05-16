#ifndef PROJECTFARM_SYSTEM_ARGUMENTS_H
#define PROJECTFARM_SYSTEM_ARGUMENTS_H

#include <filesystem>

#include "logging/consume_logger.h"

namespace projectfarm::server
{
    class SystemArguments final : public shared::ConsumeLogger
    {
    public:
        SystemArguments() = default;
        ~SystemArguments() override = default;

        void SetArguments(int argc, char* argv[]);

        [[nodiscard]] std::filesystem::path GetBinaryPath() const
        {
            return this->_binaryPath;
        }

    private:
        std::filesystem::path _binaryPath;
    };
}

#endif