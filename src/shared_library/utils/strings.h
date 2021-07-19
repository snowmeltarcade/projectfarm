#ifndef STRINGS_H
#define STRINGS_H

#include <string>
#include <array>
#include <cstdint>
#include <vector>

namespace projectfarm::shared::utils
{
	std::string trim(std::string_view s) noexcept;
	std::string ltrim(std::string_view s) noexcept;
	std::string rtrim(std::string_view s) noexcept;

    std::string trim(std::string_view s, char c) noexcept;
    std::string ltrim(std::string_view s, char c) noexcept;
    std::string rtrim(std::string_view s, char c) noexcept;

	std::string tolower(std::string_view s) noexcept;
	std::string toupper(std::string_view s) noexcept;

	template <size_t N>
	std::array<uint8_t, N> toarray(std::string_view s) noexcept
    {
        std::array<uint8_t, N> a {};

        auto size = s.size();

        for (auto i = 0u; i < N; ++i)
        {
            a[i] = (i < size) ? s[i] : '\0';
        }

        return a;
    }

    template <size_t N>
    std::string fromarray(const std::array<uint8_t, N>& a) noexcept
    {
        std::string s;

        for (auto i = 0u; i < N; ++i)
        {
            if (a[i] == '\0')
            {
                break;
            }

            s += a[i];
        }

        return s;
    }

	[[nodiscard]] std::string GetCharactersToTrim() noexcept;

	std::vector<std::string> split(std::string_view divisor, const std::string& stringToSplit) noexcept;

    bool startsWith(std::string_view stringToCheck, std::string_view startsWith) noexcept;
    bool endsWith(std::string_view stringToCheck, std::string_view endsWith) noexcept;
}

#endif