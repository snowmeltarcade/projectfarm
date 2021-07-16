#include "styles.h"
#include "api/logging/logging.h"

using namespace projectfarm::shared::css;

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
                shared::api::logging::Log("Failed to load CSS file: " + path.u8string());
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

    std::optional<CSSClass> Styles::GetBySelectorAndType(std::string_view selector,
                                                         shared::css::CSSSelectorTypes type) const noexcept
    {
        if (selector.empty())
        {
            // don't log as this will be called by all UI elements even if they are
            // not expecting a result
            return {};
        }

        // we want to get all classes with the passed selector then merge them,
        // starting with the first defined and ending with the last
        std::vector<std::reference_wrapper<const CSSClass>> allClasses;

        // find the classes
        for (const auto& doc : this->_defaultStyles)
        {
            for (const auto& c : doc.Classes)
            {
                allClasses.push_back(c);
            }
        }

        auto findMatchedClasses = [selector, type](const auto& c)
        {
            const auto& selectors = c.get().Selectors;
            auto found = std::find_if(selectors.begin(), selectors.end(),
                                      [selector, type](const auto& s){ return s.Name == selector &&
                                                                              s.Type == type; });
            return found != selectors.end();
        };

        auto matchedClasses = std::find_if(allClasses.begin(), allClasses.end(),
                                           findMatchedClasses);

        if (matchedClasses == allClasses.end())
        {
            // don't log as this will be called by all UI elements checking for their ids,
            // even if they are not expecting a result
            return {};
        }

        // merge the classes
        CSSClass mergedCSSClass;

        mergedCSSClass = matchedClasses[0];

        return mergedCSSClass;
    }
}
