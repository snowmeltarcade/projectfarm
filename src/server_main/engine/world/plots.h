#ifndef PROJECTFARM_PLOTS_H
#define PROJECTFARM_PLOTS_H

#include <vector>
#include <memory>
#include <cstdint>
#include <limits>

#include "logging/consume_logger.h"
#include "data/consume_data_provider.h"
#include "plot.h"

namespace projectfarm::engine::world
{
    class Plots final : public shared::ConsumeLogger,
                        public shared::ConsumeDataProvider
    {
    public:
        Plots() = default;
        ~Plots() override = default;

        static constexpr uint16_t EmptyIndex { std::numeric_limits<uint16_t>::max() };

        [[nodiscard]] bool Load(const std::filesystem::path& worldName) noexcept;

        [[nodiscard]] uint16_t GetPlotIndexByName(std::string_view name) const noexcept;

        [[nodiscard]] const std::shared_ptr<Plot>& GetPlot(uint16_t index) const noexcept
        {
            return this->_plots[index];
        }

    private:
        std::vector<std::shared_ptr<Plot>> _plots;
    };
}

#endif
