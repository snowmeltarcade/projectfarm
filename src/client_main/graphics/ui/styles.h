#ifndef PROJECTFARM_STYLES_H
#define PROJECTFARM_STYLES_H

#include <vector>

#include "css/css.h"
#include "data/consume_data_provider.h"
#include "logging/consume_logger.h"

namespace projectfarm::graphics::ui
{
    class Styles final : public shared::ConsumeDataProvider,
                         public shared::ConsumeLogger
    {
    public:
        Styles() = default;
        ~Styles() = default;

        [[nodiscard]]
        bool Initialize() noexcept;

        void Shutdown();

    private:
        std::vector<shared::css::CSSDocument> _defaultStyles;
    };
}

#endif
