#include "styles.h"

namespace projectfarm::graphics::ui
{
    bool Styles::Initialize() noexcept
    {
        auto paths = this->_dataProvider->GetDefaultCSSLocations();

        for (const auto& [_, path] : paths)
        {
            auto doc = shared::css::LoadFromFile(path);
            if (!doc)
            {
                this->LogMessage("Failed to load CSS file: " + path.u8string());
                return false;
            }

            this->_defaultStyles.emplace_back(std::move(*doc));
        }

        return true;
    }

    void Styles::Shutdown()
    {
        this->_defaultStyles.clear();
    }
}
