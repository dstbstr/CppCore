#pragma once
#include "Core/Platform/Types.h"
#include "Core/Concepts.h"

#include <limits>

namespace Constexpr {
	namespace Random {
		namespace _detail {
			constexpr u32 GetSeed() {
				u32 result = 0;
				auto str = __TIME__;
				for (size_t i = 0u; str[i] != '\0'; i++) {
					if (str[i] == ':') continue;
					result = result * 10 + (str[i] - '0');
				}

				return result;
			}

			constexpr u32 a = 1664525;
			constexpr u32 c = 1013904223;
			constexpr u32 m = std::numeric_limits<u32>::max();

			//Linear Congruential Generator
			constexpr u32 LCG(u32 seed) {
				return (a * seed + c) % m;
			}
		}

		template<Numeric T>
		[[nodiscard]] constexpr auto Next(const T min, const T max, u32 prev) {
			auto next = _detail::LCG(prev);
			if constexpr (std::is_integral_v<T>) {
				return min + static_cast<T>(next % (max - min));
			}
			else {
				return min + (static_cast<T>(next) / static_cast<T>(std::numeric_limits<u32>::max())) * (max - min);
			}
		}
	}
}