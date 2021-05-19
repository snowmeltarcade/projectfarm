#include "fit_types.h"
#include "utils/util.h"

namespace projectfarm::graphics::ui
{
    std::optional<FitTypes> StringToFitTypes(const std::string& s) noexcept
    {
        auto name = pfu::tolower(s);

        if (s == "fit")
        {
            return FitTypes::Fit;
        }
        else if (s == "free")
        {
            return FitTypes::Free;
        }
        else if (s == "fill")
        {
            return FitTypes::Fill;
        }
        else
        {
            return {};
        }
    }
}