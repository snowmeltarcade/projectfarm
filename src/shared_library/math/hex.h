#ifndef PROJECTFARM_HEX_H
#define PROJECTFARM_HEX_H

#include <cmath>
#include <cstdint>
#include <optional>
#include <string>

namespace projectfarm::shared::math
{
    template <typename T>
    constexpr std::optional<T> HexToDec(char c)
    {
        c = static_cast<char>(std::tolower(static_cast<int>(c)));

        auto isDigit = (c >= '0' && c <= '9');
        auto isLetter = (c >= 'a' && c <= 'f');

        auto isValid = isDigit || isLetter;

        if (!isValid)
        {
            return {};
        }

        auto res = isDigit ? c - '0' : c - 'a' + 10; // + 10 because `a` == 10
        return res;
    }

    template <typename T>
    constexpr std::optional<T> HexToDec(std::string_view s)
    {
        if (s.empty())
        {
            return {};
        }

        T res {0};

        auto index {s.length() - 1};
        for (const auto& c : s)
        {
            auto v = HexToDec<uint8_t>(c);
            if (!v)
            {
                return {};
            }

            auto base = std::pow(16u, index);
            auto value = *v;

            res += static_cast<T>(value * base);

            index--;
        }

        return res;
    }
}

#endif
