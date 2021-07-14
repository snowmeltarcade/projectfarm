#ifndef PROJECTFARM_PLOTS_H
#define PROJECTFARM_PLOTS_H

#include <vector>
#include <memory>
#include <filesystem>

#include "data/consume_data_provider.h"
#include "graphics/consume_tile_set_pool.h"
#include "plot.h"

namespace projectfarm::engine::world
{
    class Plots final : public graphics::ConsumeTileSetPool,
                        public shared::ConsumeDataProvider
    {
    public:
        Plots() = default;
        ~Plots() override = default;

        static constexpr uint16_t EmptyIndex { std::numeric_limits<uint16_t>::max() };

        [[nodiscard]] bool Load(const std::filesystem::path& worldName);
        void Shutdown() noexcept;

        [[nodiscard]] uint16_t GetIndexByName(const std::string& name) const noexcept;

        [[nodiscard]] const std::shared_ptr<Plot>& GetByIndex(uint16_t index) const noexcept
        {
            return this->_plots[index];
        }

        [[nodiscard]] const std::vector<std::shared_ptr<Plot>>& GetPlots() const noexcept
        {
            return this->_plots;
        }

    private:
        std::vector<std::shared_ptr<Plot>> _plots;
    };
}

#endif
