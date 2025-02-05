#pragma once

#include <vector>
#include <array>
#include <string>
#include <ranges>

#include "Core/Concepts.h"

namespace Constexpr {
    constexpr size_t Strlen(const char* str) {
        return std::char_traits<char>::length(str);
    }

    constexpr bool IsDigit(char c) {
        return c >= '0' && c <= '9';
    }

    template<Integral T>
    constexpr std::string ToString(T val) {
        bool negate = false;
        if constexpr (Signed<T>) {
            if (val < 0) {
                negate = true;
                val = -val;
            }
        }

        std::string result;
        while (val >= 10) {
            result.push_back(static_cast<char>('0' + (val % 10)));
            val /= 10;
        }
        result.push_back(static_cast<char>('0' + (val % 10)));
        if (negate) {
            result.push_back('-');
        }

        std::reverse(result.begin(), result.end());
        return result;
    }

    constexpr std::string ToString(std::string_view val) {
        return std::string(val);
    }
	constexpr std::string ToString(const std::string& val) {
		return val;
	}

    constexpr auto Split(std::string_view input, std::string_view delimiter, bool keepEmpties = false) {
        return input
            | std::views::split(delimiter)
            | std::views::filter([keepEmpties](auto sv) { return keepEmpties || !sv.empty(); })
			| std::views::transform([](auto sv) { return std::string_view(&*sv.begin(), std::ranges::distance(sv)); })
            | std::ranges::to<std::vector>();
    }

    constexpr size_t HexToDec(char c) {
        if (c >= '0' && c <= '9') {
            return c - '0';
        }
        else if (c >= 'a' && c <= 'f') {
            return (c - 'a') + 10;
        }
        else if (c >= 'A' && c <= 'F') {
            return (c - 'A') + 10;
        }
        else {
            throw "Bad Input";
        }
    }

    constexpr size_t ParseHex(std::string_view input) {
        size_t result = 0;
        size_t multiplier = 1;

        for (size_t i = input.size() - 1; i > 0; i--) {
            result += HexToDec(input[i]) * multiplier;
            multiplier *= 16;
        }
        
        return result + (HexToDec(input[0]) * multiplier);
    }

    template<typename T>
    constexpr bool ParseNumber(std::string_view input, T& result) {
        T place = 1;
        size_t pos = input[0] == '-' || input[0] == '+' ? 1 : 0;
        result = 0;
        for (size_t i = input.size() - 1; i != pos; i--) {
            if (input[i] < '0' || input[i] > '9') {
                return false;
            }
            result += static_cast<T>(input[i] - '0') * place;
            place *= 10;
        }
        if (input[pos] < '0' || input[pos] > '9') return false;

        result += (input[pos] - '0') * place;
        if constexpr (Signed<T>) {
            if (input[0] == '-') {
                result = -result;
            }
        }
        else {
            if (input[0] == '-') {
                return false;
            }
        }

        return true;
    }

    template<typename T>
    constexpr bool ParseNumber(char c, T& result) {
        result = c - '0';
        return result > 0 && result < 10;
    }
    constexpr void ReplaceAll(std::string& original, const std::string& toFind, const std::string& replacement) {
        size_t index = 0;
        size_t length = toFind.length();

        while (true) {
            index = original.find(toFind, index);
            if (index == toFind.npos) break;
            original.replace(index, length, replacement);
        }
    }

    constexpr std::string ReplaceAllCopy(const std::string& original, const std::string& toFind, const std::string& replacement) {
        std::string result = original;
        ReplaceAll(result, toFind, replacement);
        return result;
    }

    constexpr void ReplaceOne(std::string& original, const std::string& toFind, const std::string& replacement) {
        size_t index = original.find(toFind, 0);
        if (index != original.npos) {
            original.replace(index, toFind.length(), replacement);
        }
    }

    constexpr std::string ReplaceOneCopy(const std::string& original, const std::string& toFind, const std::string& replacement) {
        std::string result = original;
        ReplaceOne(result, toFind, replacement);
        return result;
    }
    constexpr void RemoveAllOf(std::string& original, char toFind) {
        original.erase(std::remove(original.begin(), original.end(), toFind), original.end());
    }

    constexpr void RemoveAllOf(std::string& original, const std::string& toFind) {
        for (auto c : toFind) {
            RemoveAllOf(original, c);
        }
    }

    constexpr std::string RemoveAllOfCopy(const std::string& original, char toFind) {
        std::string result = original;
        RemoveAllOf(result, toFind);
        return result;
    }

    constexpr std::string RemoveAllOfCopy(std::string_view original, char toFind) {
        auto result = std::string(original);
        RemoveAllOf(result, toFind);
        return result;
    }

    constexpr std::string RemoveAllOfCopy(const std::string& original, const std::string& toFind) {
        std::string result = original;
        RemoveAllOf(result, toFind);
        return result;
    }

    constexpr std::string RemoveAllOfCopy(std::string_view original, const std::string& toFind) {
        auto result = std::string(original);
        RemoveAllOf(result, toFind);
        return result;
    }

    constexpr bool StartsWith(std::string_view string, std::string_view prefix) {
		return string.rfind(prefix, 0) == 0;
    }

    constexpr bool EndsWith(std::string_view string, std::string_view suffix) {
        return string.rfind(suffix) == string.length() - suffix.length() && string.length() >= suffix.length();
    }

    namespace StrUtilsTests {
        bool Run();
    }
}