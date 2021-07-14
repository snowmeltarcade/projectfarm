#ifndef PROJECTFARM_LOGGING_H
#define PROJECTFARM_LOGGING_H

#include "log_levels.h"

namespace projectfarm::shared::api::logging
{
    void Log(std::string_view message, LogLevels level = LogLevels::Info) noexcept;
}

#endif
