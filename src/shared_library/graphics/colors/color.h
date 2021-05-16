#ifndef PROJECTFARM_COLOR_H
#define PROJECTFARM_COLOR_H

#include <cstdint>
#include <string>
#include <optional>
#include <unordered_map>

#include "math/hex.h"

namespace projectfarm::shared::graphics::colors
{
    struct Color
    {
        uint8_t r {0};
        uint8_t g {0};
        uint8_t b {0};
        uint8_t a {255};

        std::string ToString() const noexcept
        {
            return "{r: " + std::to_string(this->r) +
                    ", g: " + std::to_string(this->g) +
                    ", b: " + std::to_string(this->b) +
                    ", a: " + std::to_string(this->a) + "}";
        }

        uint32_t ToInt() const noexcept
        {
            return this->r << 24 | this->g << 16 | this->b << 8 | this->a;
        }

        void FromInt(uint32_t i) noexcept
        {
            this->r = static_cast<uint8_t>((i & 0xFF000000) >> 24);
            this->g = static_cast<uint8_t>((i & 0x00FF0000) >> 16);
            this->b = static_cast<uint8_t>((i & 0x0000FF00) >> 8);
            this->a = static_cast<uint8_t>(i & 0x000000FF);
        }
    };

    static inline const Color Black { 0, 0, 0, 255 };
    static inline const Color White { 255, 255, 255, 255 };
    static inline const Color Red { 255, 0, 0, 255 };
    static inline const Color Green { 0, 255, 0, 255 };
    static inline const Color Blue { 0, 0, 255, 255 };

    constexpr bool operator == (const Color& lhs, const Color& rhs)
    {
        return lhs.r == rhs.r &&
               lhs.g == rhs.g &&
               lhs.b == rhs.b &&
               lhs.a == rhs.a;
    }

    constexpr bool operator != (const Color& lhs, const Color& rhs)
    {
        return !(lhs == rhs);
    }

    static inline const std::unordered_map<std::string, Color> ColorNames
    {
        { "black", Black },
        { "white", White },
        { "red", Red },
        { "blue", Blue },
        { "green", Green },
    };

    constexpr std::optional<Color> FromHexString(std::string_view s)
    {
        // 9 == #aabbccdd
        if (s.length() != 9)
        {
            return {};
        }

        auto rPart = s.substr(1, 2);
        auto gPart = s.substr(3, 2);
        auto bPart = s.substr(5, 2);
        auto aPart = s.substr(7, 2);

        auto r = math::HexToDec<uint8_t>(rPart);
        auto g = math::HexToDec<uint8_t>(gPart);
        auto b = math::HexToDec<uint8_t>(bPart);
        auto a = math::HexToDec<uint8_t>(aPart);

        return {{ *r, *g, *b, *a } };
    }

    std::optional<Color> FromString(std::string_view s);
}

#endif