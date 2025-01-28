#include "Core/Instrumentation/Benchmark/Stats.h"

// All stats are 0 by default
static_assert(Stats({}).Count == 0);
static_assert(Stats({}).Mean == 0.0);
static_assert(Stats({}).Median == 0.0);
static_assert(Stats({}).Min == 0.0);
static_assert(Stats({}).Max == 0.0);
static_assert(Stats({}).StdDev == 0.0);
static_assert(Stats({}).Percent75 == 0.0);
static_assert(Stats({}).Percent90 == 0.0);
static_assert(Stats({}).Percent99 == 0.0);

// Single value stats
static_assert(Stats({ 100.0 }).Count == 1);
static_assert(Stats({ 1.0 }).Mean == 1.0);
static_assert(Stats({ 1.0 }).Median == 1.0);
static_assert(Stats({ 1.0 }).Min == 1.0);
static_assert(Stats({ 1.0 }).Max == 1.0);
static_assert(Stats({ 1.0 }).StdDev == 0.0);
static_assert(Stats({ 1.0 }).Percent75 == 1.0);
static_assert(Stats({ 1.0 }).Percent90 == 1.0);
static_assert(Stats({ 1.0 }).Percent99 == 1.0);

// Min Tests
static_assert(Stats({ 1.0, 2.0 }).Min == 1.0);
static_assert(Stats({ 2.0, 1.0 }).Min == 1.0);
static_assert(Stats({ -1.0, 2.0 }).Min == -1.0);
static_assert(Stats({ -1.0, -2.0 }).Min == -2.0);

// Max Tests
static_assert(Stats({ 1.0, 2.0 }).Max == 2.0);
static_assert(Stats({ 2.0, 1.0 }).Max == 2.0);
static_assert(Stats({ -1.0, 2.0 }).Max == 2.0);
static_assert(Stats({ -1.0, -2.0 }).Max == -1.0);

// Mean Tests
static_assert(Stats({ 1.0, 2.0 }).Mean == 1.5);
static_assert(Stats({ 1.0, 2.0, 3.0 }).Mean == 2.0);

// Median Tests
static_assert(Stats({ 1.0, 2.0 }).Median == 2.0);
static_assert(Stats({ 1.0, 2.0, 3.0 }).Median == 2.0);

// StdDev Tests
constexpr bool IsNear(f64 a, f64 b, f64 epsilon = 0.0001) {
	return Constexpr::AbsDistance(a, b) < epsilon;
}

static_assert(Stats({ 1.0, 3.0 }).StdDev == 1.0);
static_assert(IsNear(Stats({ 1.0, 2.0, 3.0 }).StdDev, Constexpr::Sqrt(2.0 / 3.0)));

// average: (2+3+5+7+13) / 5 = 6
// variance: ((2-6)^2 + (3-6)^2 + (5-6)^2 + (7-6)^2 + (13-6)^2) / 5 = 76 / 5
// stddev: sqrt(76 / 5)
static_assert(IsNear(Stats({ 2.0, 3.0, 5.0, 7.0, 13.0 }).StdDev, Constexpr::Sqrt(76.0 / 5.0)));

constexpr Stats MakeStats(u32 begin, u32 end) {
	std::vector<f64> values;
	for (u32 i = begin; i <= end; i++) {
		values.push_back(static_cast<f64>(i));
	}
	return Stats(values);
}

// Percentile Tests
static_assert(MakeStats(0, 11).Percent75 == 9.0);
static_assert(MakeStats(0, 20).Percent90 == 18.0);
static_assert(MakeStats(0, 100).Percent99 == 99.0);