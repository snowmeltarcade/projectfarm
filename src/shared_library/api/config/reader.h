#ifndef PROJECTFARM_READER_H
#define PROJECTFARM_READER_H

#include <filesystem>
#include <optional>

#include "item.h"

namespace projectfarm::shared::api::config
{
    std::optional<ItemRoot> Read(const std::filesystem::path& path) noexcept;
}

#endif
