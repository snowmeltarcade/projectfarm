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
        std::stringstream ss;
        ss << std::hex << std::uppercase << static_cast<uint32_t>(dec);

        auto res = ss.str();

        // pad the value with zeros
        // there are two characters per byte
        constexpr auto size = sizeof(T) * 2;

        auto diff = size - res.length();
        if (diff > 0)
        {
            std::string zeros(diff, '0');
            res = zeros + res;
        }

        return res;
    }
}

#endif
