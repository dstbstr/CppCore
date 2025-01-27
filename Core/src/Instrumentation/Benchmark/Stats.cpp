#include "Core/Instrumentation/Benchmark/Stats.h"
#include <ostream>

std::ostream& Stats::operator<<(std::ostream& stream) {
	stream << std::format(
		"Mean: {:.2f}\n"
		"Median: {:.2f}\n"
		"Min: {:.2f}\n"
		"Max: {:.2f}\n"
		"StdDev: {:.2f}\n"
		"75%: {:.2f}\n"
		"90%: {:.2f}\n"
		"99%: {:.2f}",
		Mean, Median, Min, Max, StdDev, Percent75, Percent90, Percent99);
	return stream;
}
