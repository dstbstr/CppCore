#pragma once

#include <vector>
#include <algorithm>
#include <numeric>
#include <format>
#include <iosfwd>

#include "Core/Platform/Types.h"
#include "Core/Constexpr/ConstexprMath.h"

struct Stats {
	constexpr Stats(const std::vector<f64>& values, const std::string& unit) 
		: Unit(unit)
	{
		if (values.empty()) return;
		
		const auto sorted = [&]() {
			auto sorted = values;
			std::sort(sorted.begin(), sorted.end());
			return sorted
				| std::views::filter([](auto val) { return !std::isnan(val);})
				| std::ranges::to<std::vector>();
		}();

		auto sum = std::accumulate(sorted.begin(), sorted.end(), 0.0);
		Count = sorted.size();
		Mean = sum / Count;
		Median = sorted[Count / 2];
		Min = sorted[0];
		Max = sorted[Count - 1];
		Percent75 = sorted[static_cast<size_t>(Count * 0.75)];
		Percent90 = sorted[static_cast<size_t>(Count * 0.90)];
		Percent99 = sorted[static_cast<size_t>(Count * 0.99)];

		auto variance = std::transform_reduce(sorted.begin(), sorted.end(), 0.0, std::plus<>(),
			[&](f64 x) { return (x - Mean) * (x - Mean); }) / sorted.size();
		StdDev = Constexpr::Sqrt(variance);
	}

	std::ostream& operator<<(std::ostream& stream);

	constexpr Stats& operator*=(f64 factor) {
		Mean *= factor;
		Median *= factor;
		Min *= factor;
		Max *= factor;
		StdDev *= factor;
		Percent75 *= factor;
		Percent90 *= factor;
		Percent99 *= factor;

		return *this;
	}

	constexpr Stats& operator/=(f64 factor) {
		Mean /= factor;
		Median /= factor;
		Min /= factor;
		Max /= factor;
		StdDev /= factor;
		Percent75 /= factor;
		Percent90 /= factor;
		Percent99 /= factor;

		return *this;
	}

	f64 Mean{0};
	f64 Median{0};
	f64 Min{0};
	f64 Max{0};
	f64 StdDev{0};
	f64 Percent75{0};
	f64 Percent90{0};
	f64 Percent99{0};
	size_t Count{ 0 };
	std::string Unit{};
};

template<>
struct std::formatter<Stats> : std::formatter<std::string> {
	auto format(const Stats& stats, std::format_context& ctx) const {
		return std::formatter<std::string>::format(
			std::format(
				"Count: {}\n"
				"Mean: {:.2f}{}\n"
				"Median: {:.2f}{}\n"
				"Min: {:.2f}{}\n"
				"Max: {:.2f}{}\n"
				"StdDev: {:.2f}{}\n"
				"75%: {:.2f}{}\n"
				"90%: {:.2f}{}\n"
				"99%: {:.2f}{}",
				stats.Count, 
				stats.Mean, stats.Unit, 
				stats.Median, stats.Unit, 
				stats.Min, stats.Unit,
				stats.Max, stats.Unit,
				stats.StdDev, stats.Unit,
				stats.Percent75, stats.Unit,
				stats.Percent90, stats.Unit,
				stats.Percent99, stats.Unit
			), ctx);
	}
};

class StatHolder {
	std::vector<f64> m_Values;

public:
	constexpr void Add(f64 value) {
		m_Values.push_back(value);
	}
	constexpr Stats GetStats(const std::string& unit) {
		return Stats(m_Values, unit);
	}
};
