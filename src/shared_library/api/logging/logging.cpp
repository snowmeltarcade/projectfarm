#include <iostream>

#include "logging.h"

namespace projectfarm::shared::api::logging
{
    void Log(std::string_view message, LogLevels)
    {
        std::cout << message << '\n';
    }
}
