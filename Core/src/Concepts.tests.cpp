#include "Core/Concepts.h"

#include <string>

static_assert(Numeric<int>);
static_assert(Numeric<float>);
static_assert(Numeric<double>);

// some odd-ball ones to be aware of
static_assert(Numeric<bool>);
static_assert(Numeric<char>);

static_assert(!Numeric<int[]>);
static_assert(!Numeric<std::string>);

static_assert(Integral<int>);
static_assert(!Integral<float>);

static_assert(Signed<int>);
static_assert(!Signed<uint16_t>);

static_assert(!Unsigned<int>);
static_assert(Unsigned<uint16_t>);
