#ifndef PROJECTFARM_HEX_H
#define PROJECTFARM_HEX_H

#include <cmath>
#include <cstdint>
#include <optional>
#include <string>
#include <sstream>

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

    // TODO: In C++20 set this to `constexpr`
    template <typename T>
    /*constexpr*/ std::string DecToHex(T dec)
    {
        constexpr auto digitToHex = [](uint8_t d) -> char
        {
            if (d <= 9)
            {
                return d + '0';
            }

            return d - 10 + 'A';
        };

        std::stringstream ss;

        uint8_t c {0};

        while ((c = (dec >>= 1)) != 0)
        {
            auto part1 = c % 16;
            auto part2 = (c / 16) % 16;

            ss << digitToHex(part2);
            ss << digitToHex(part1);
        }

        constexpr auto size = sizeof(T);

        auto res = ss.str();

        // pad with 0's
        auto diff = size - res.size();
        if (diff > 0)
        {
            // there are two hex characters per byte
            std::string zeros("0", diff * 2);
            res = zeros + res;
        }

        return res;
    }
}

#endif
