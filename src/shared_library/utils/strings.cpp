#include <algorithm>

#include "strings.h"

namespace projectfarm::shared::utils
{
    std::string GetCharactersToTrim() noexcept
    {
        const static std::string charactersToTrim = " \n\t\v\r";
        return charactersToTrim;
    }

	std::string trim(std::string_view s) noexcept
	{
		return rtrim(ltrim(s));
	}

	std::string ltrim(std::string_view s) noexcept
	{
		auto i = s.find_first_not_of(GetCharactersToTrim());

		// we must be all whitespace
		if (i == std::string::npos)
        {
		    return "";
        }

		s.remove_prefix(i);
		return std::string(s);
	}

	std::string rtrim(std::string_view s) noexcept
	{
		auto i = s.find_last_not_of(GetCharactersToTrim()) + 1;

		if (i == std::string::npos ||
			i >= s.length())
        {
            return std::string(s);
        }

		s.remove_suffix(s.length() - i);

		return std::string(s);
	}

	std::string tolower(std::string_view s) noexcept
	{
		std::string str(s);

		std::transform(str.begin(), str.end(), str.begin(),
			[](int c) { return static_cast<char>(std::tolower(c)); });

		return str;
	}

	std::string toupper(std::string_view s) noexcept
	{
		std::string str(s);

		std::transform(str.begin(), str.end(), str.begin(),
			[](int c) { return static_cast<char>(std::toupper(c)); });

		return str;
	}

    std::vector<std::string> split(std::string_view divisor, const std::string& stringToSplit) noexcept
    {
        std::vector<std::string> results;

        if (divisor.empty() || stringToSplit.empty())
        {
            return {};
        }

        size_t begin = 0u;

        while (true)
        {
            auto next = stringToSplit.find(divisor, begin);
            if (next == std::string::npos)
            {
                break;
            }

            results.push_back(stringToSplit.substr(begin, next-begin));
            begin = next + divisor.size();
        }

        if (begin <= stringToSplit.size() - 1u)
        {
            results.push_back(stringToSplit.substr(begin));
        }

        return results;
    }

    bool startsWith(std::string_view stringToCheck, std::string_view startsWith) noexcept
    {
        if (stringToCheck.empty() || startsWith.empty())
        {
            return false;
        }

        auto res = stringToCheck.substr(0, startsWith.size());
        return res == startsWith;
    }

    bool endsWith(std::string_view stringToCheck, std::string_view endsWith) noexcept
    {
        if (stringToCheck.empty() ||
            endsWith.empty() ||
            endsWith.size() > stringToCheck.size())
        {
            return false;
        }

        auto res = stringToCheck.substr(stringToCheck.size() - endsWith.size(), endsWith.size());
        return res == endsWith;
    }
}