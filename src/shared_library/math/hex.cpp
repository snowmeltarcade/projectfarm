#include "hex.h"

namespace projectfarm::shared::math
{
    std::string DecToHex(uint8_t c)
    {
        constexpr auto digitToHex = [](uint8_t d) -> char
        {
            if (d <= 9)
            {
                return d + '0';
            }

            return d - 9 + 'A';
        };

        auto part1 = c % 16;
        auto part2 = (c / 16) % 16;

        std::string res;
        res += digitToHex(part1);
        res += digittoint(part2);

        return res;
    }
}
