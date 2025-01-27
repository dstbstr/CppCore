#pragma once
#include <chrono>

#include "Benchmark/Stats.h"

namespace Benchmark {
	using namespace std::chrono_literals;

	template<typename TResult>
	struct Result {
		Stats RuntimeStats;
		Stats MemoryStats;
		TResult ReturnValue;
	};

	class Benchmark {
	public:
		static Benchmark OverSeconds(u32 seconds) {
			Benchmark result;
			result.m_MaxDuration = std::chrono::seconds(seconds);
			return result;
		}
		static Benchmark OverIterations(u32 iterations) {
			Benchmark result;
			result.m_MaxIterations = iterations;
			return result;
		}

		template<typename Func, typename... Args>
		Result<std::invoke_result_t<Func, Args...>> Run(Func func, Args... args) {
			using Ret = std::invoke_result_t<Func, Args...>;
			StatHolder runtimeStats;
			StatHolder memoryStats;
			using clock = std::chrono::high_resolution_clock;
			auto start = clock::now();
			auto end = start + m_MaxDuration;
			u32 iteration = 0;
			Ret resultValue;
			while(iteration < m_MaxIterations && clock::now() < end) {
				start = clock::now();
				resultValue = func(args...);
				auto micros = std::chrono::duration_cast<std::chrono::microseconds>(clock::now() - start).count();
				runtimeStats.Add(micros);
			}

			return { runtimeStats.GetStats(), memoryStats.GetStats(), resultValue };
		}
	private:
		Benchmark() = default;

		std::chrono::milliseconds m_MaxDuration{ 5min };
		u32 m_MaxIterations{ 1'000'000 };
	};
}