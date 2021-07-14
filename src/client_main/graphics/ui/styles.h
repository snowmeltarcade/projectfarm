#ifndef PROJECTFARM_STYLES_H
#define PROJECTFARM_STYLES_H

#include <vector>

#include "css/css.h"
#include "data/consume_data_provider.h"

namespace projectfarm::graphics::ui
{
    class Styles final : public shared::ConsumeDataProvider
    {
    public:
        Styles() = default;
        ~Styles() = default;

        [[nodiscard]]
        bool Initialize() noexcept;

        void Shutdown();

        [[nodiscard]]
        std::optional<shared::css::CSSClass> GetBySelectorAndType(std::string_view selector,
                                                                  shared::css::CSSSelectorTypes type) const noexcept;

    private:
        std::vector<shared::css::CSSDocument> _defaultStyles;
    };
}

#endif
