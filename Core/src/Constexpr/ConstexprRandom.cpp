#include "Core/Constexpr/ConstexprRandom.h"

constexpr bool RunTest(auto min, auto max) {
	u32 state = Constexpr::Random::_detail::GetSeed();
	auto value = Constexpr::Random::Next(min, max, state);
	return value >= min && value < max;
}

static_assert(RunTest(0, 10));