#pragma once

#include <concepts>
#include <string_view>

namespace Constexpr {
    template<std::integral T, std::integral U>
    [[nodiscard]] constexpr bool IsBitSet(T bits, U targetBit) {
        return bits & (T(1) << targetBit);
    }

    template<std::integral T, std::integral U>
    constexpr void SetBit(T& bits, U targetBit) {
        bits |= (T(1) << targetBit);
    }

    template<std::integral T, std::integral U>
    [[nodiscard]] constexpr T WithBit(T bits, U targetBit) {
        T result = bits;
        SetBit(result, targetBit);
        return result;
    }

    template<std::integral T, std::integral U>
    constexpr void UnsetBit(T& bits, U targetBit) {
        static_assert(std::is_arithmetic_v<T> && std::is_arithmetic_v<U>);
        T mask = ~(T(1) << targetBit);
        bits &= mask;
    }

    template<std::integral T, std::integral U>
    [[nodiscard]] constexpr T WithoutBit(T bits, U targetBit) {
        T result = bits;
        UnsetBit(result, targetBit);
        return result;
    }

    template<std::integral T, std::integral U>
    constexpr void ToggleBit(T& bits, U targetBit) {
        bits ^= (T(1) << targetBit);
    }

    template<std::integral T, std::integral U>
    [[nodiscard]] constexpr T WithToggle(T bits, U targetBit) {
        T result = bits;
        ToggleBit(result, targetBit);
        return result;
    }

    template<typename T, char TrueVal = '1', char FalseVal = '0'>
    [[nodiscard]] constexpr T FromBinary(std::string_view binary) {
        T mul = 1;
        T result = 0;
        for (size_t i = binary.size() - 1; i > 0; i--) {
            result += mul * (binary[i] == TrueVal);
            mul *= 2;
        }
        result += mul * (binary[0] == TrueVal);
        return result;
    }
}