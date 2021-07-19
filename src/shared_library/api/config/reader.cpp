#include <string>
#include <fstream>

#include "reader.h"
#include "nlohmann/json.hpp"
#include "api/logging/logging.h"
#include "utils/util.h"

using namespace std::literals;

namespace projectfarm::shared::api::config
{
    std::vector<Item> ItemsFromJson(const nlohmann::json& json) noexcept
    {
        std::vector<Item> items;

        for (const auto& j : json.items())
        {
            Item item;

            item.Key = j.key();

            auto& value = j.value();
            if (value.size() > 1)
            {
                item.Children = ItemsFromJson(value);
            }
            else
            {
                if (value.is_array())
                {
                    item.Children = ItemsFromJson(value);
                }
                else if (value.is_string())
                {
                    item.Value = value.get<std::string>();
                }
                else if (value.is_boolean())
                {
                    item.Value = value.get<bool>() ? "true" : "false";
                }
                else if (value.is_number_float())
                {
                    // `std::to_string` will pad floats with '0'
                    item.Value = pfu::rtrim(std::to_string(value.get<float>()), '0');
                }
                else
                {
                    item.Value = std::to_string(value.get<int>());
                }
            }

            items.emplace_back(std::move(item));
        }

        return items;
    }

    std::optional<ItemRoot> Read(const std::filesystem::path& path) noexcept
    {
        std::ifstream fs(path);
        if (!fs.is_open())
        {
            logging::Log("Failed to open config file: " + path.u8string());
            return {};
        }

        try
        {
            nlohmann::json json;
            json << fs;

            fs.close();

            auto items = ItemsFromJson(json);
            return { { items } };
        }
        catch (const std::exception& ex)
        {
            fs.close();

            logging::Log("Failed to read config file: `" + path.u8string() +
                "` with error: "s + ex.what());
            return {};
        }
    }
}
