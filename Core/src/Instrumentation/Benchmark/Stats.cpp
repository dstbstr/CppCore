#include "Core/Instrumentation/Benchmark/Stats.h"
#include <ostream>

std::ostream& Stats::operator<<(std::ostream& stream) {
	stream << std::format("{}", *this);
	return stream;
}
