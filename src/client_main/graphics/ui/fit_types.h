#ifndef PROJECTFARM_FITTYPES_H
#define PROJECTFARM_FITTYPES_H

#include <string>
#include <optional>

namespace projectfarm::graphics::ui
{
    enum class FitTypes
    {
        // keep the size ratio of the parent
        Fit,

        // keep the defined size regardless of the parent size
        // enables scrolling
        Free,

        // fill the parent
        Fill,
    };

    std::optional<FitTypes> StringToFitTypes(const std::string& s) noexcept;
}

#endif
