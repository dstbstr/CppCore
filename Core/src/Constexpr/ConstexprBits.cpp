#include "Core/Constexpr/ConstexprBits.h"

namespace Constexpr {
	static_assert(IsBitSet(1, 0));
	static_assert(IsBitSet(2, 1));
	static_assert(IsBitSet(3, 0));
	static_assert(IsBitSet(3, 1));

	static_assert(!IsBitSet(2, 0));
	static_assert(!IsBitSet(4, 0));
	static_assert(!IsBitSet(4, 1));

	static_assert(WithBit(0, 0) == 1);
	static_assert(WithBit(0, 1) == 2);
	static_assert(WithBit(0, 2) == 4);
	static_assert(WithBit(2, 1) == 2);
	static_assert(WithBit(4, 2) == 4);

	static_assert(WithoutBit(4, 2) == 0);
	static_assert(WithoutBit(2, 1) == 0);
	static_assert(WithoutBit(1, 0) == 0);
	static_assert(WithoutBit(3, 2) == 3);
	static_assert(WithoutBit(4, 1) == 4);
	static_assert(WithoutBit(15, 0) == 14);

	static_assert(WithToggle(0, 0) == 1);
	static_assert(WithToggle(1, 0) == 0);
	static_assert(WithToggle(0, 1) == 2);
	static_assert(WithToggle(2, 1) == 0);

	static_assert(FromBinary<int>("1011") == 11);
	static_assert(FromBinary<size_t, '#', '.'>("#.##") == 11);

}