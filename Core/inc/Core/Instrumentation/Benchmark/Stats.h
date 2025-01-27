#pragma once

#include <vector>
#include <algorithm>
#include <numeric>
#include <format>
#include <iosfwd>

#include "Core/Platform/Types.h"
#include "Core/Constexpr/ConstexprMath.h"

struct Stats {
	constexpr Stats(const std::vector<f64>& values) {
		if (values.empty()) return;

		const auto sorted = [&]() {
			auto sorted = values;
			std::sort(sorted.begin(), sorted.end());
			return sorted;
		}();

		auto sum = std::accumulate(sorted.begin(), sorted.end(), 0.0);
		Mean = sum / sorted.size();
		Median = sorted[sorted.size() / 2];
		Min = sorted[0];
		Max = sorted[sorted.size() - 1];
		Percent75 = sorted[static_cast<size_t>(sorted.size() * 0.75)];
		Percent90 = sorted[static_cast<size_t>(sorted.size() * 0.90)];
		Percent99 = sorted[static_cast<size_t>(sorted.size() * 0.99)];

		auto variance = std::transform_reduce(sorted.begin(), sorted.end(), 0.0, std::plus<>(),
			[&](f64 x) { return (x - Mean) * (x - Mean); }) / sorted.size();
		StdDev = Constexpr::Sqrt(variance);
	}

	std::ostream& operator<<(std::ostream& stream);

	f64 Mean{0};
	f64 Median{0};
	f64 Min{0};
	f64 Max{0};
	f64 StdDev{0};
	f64 Percent75{0};
	f64 Percent90{0};
	f64 Percent99{0};
};

class StatHolder {
	std::vector<f64> m_Values;

public:
	constexpr void Add(f64 value) {
		m_Values.push_back(value);
	}
	constexpr Stats GetStats() {
		return Stats(m_Values);
	}
};
