#pragma once

#include <vector>
#include <array>
#include <string>
#include <ranges>

#include "Core/Constexpr/ConstexprStrUtils.h"

namespace Constexpr {
    constexpr std::string JoinVec(std::string&& delimiter, const auto& input) {
		return input
			| std::views::transform([](const auto& elem) { return Constexpr::ToString(elem); })
			| std::views::join_with(delimiter)
			| std::ranges::to<std::string>();
    }

    constexpr std::string JoinVec(char&& delimiter, const auto& input) {
        return JoinVec(std::string(1, delimiter), input);
    }

    template<size_t Rows, size_t Cols>
    constexpr size_t GetIndex(size_t row, size_t col) {
        return row * Cols + col;
    }

    template<size_t Rows, size_t Cols>
    constexpr void GetRowCol(size_t index, size_t& row, size_t& col) {
        row = index / Cols;
        col = index % Cols;
    }

    template<typename Collection>
    constexpr auto FindMin(const Collection& collection) {
		return *std::min_element(collection.begin(), collection.end());
    }

    template<typename Collection>
    constexpr auto FindMax(const Collection& collection) {
		return *std::max_element(collection.begin(), collection.end());
    }
}