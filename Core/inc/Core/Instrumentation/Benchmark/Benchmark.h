#pragma once
#include <chrono>

#include "Core/Instrumentation/Benchmark/Stats.h"
#include "Core/Instrumentation/Benchmark/ResourceMonitor.h"

namespace Benchmark {
	using namespace std::chrono_literals;

	enum struct RuntimeResolution { Micros, Millis, Seconds	};
	constexpr std::string ToUnit(RuntimeResolution res) {
		switch (res) {
		case RuntimeResolution::Micros: return "us";
		case RuntimeResolution::Millis: return "ms";
		case RuntimeResolution::Seconds: return "s";
		}
		return "Unknown";
	}
	constexpr f64 ToFactor(RuntimeResolution res) {
		switch (res) {
		case RuntimeResolution::Micros: return 1.0;
		case RuntimeResolution::Millis: return 1 / 1'000.0;
		case RuntimeResolution::Seconds: return 1 / 1'000'000.0;
		}
		return 1.0;
	}

	template<typename TResult>
	struct Result {
		Stats RuntimeStats;
		TResult ReturnValue;
		std::unordered_map<MonitorType, Stats> MonitorStats;
	};

	struct MonitorConfig {
		MonitorType Type;
		std::chrono::milliseconds Interval;
		double Factor;
		std::string Label;
	};

	class Runner {
		friend class Builder;
		std::chrono::milliseconds m_MaxDuration{};
		std::vector<MonitorConfig> m_MonitorConfigs;
		u32 m_MaxIterations{};
		RuntimeResolution m_RuntimeResolution{ RuntimeResolution::Micros };
		Runner() = default;
	public:
		template<typename Func, typename... Args>
		Result<std::invoke_result_t<Func, Args...>> Run(Func func, Args... args) {
			using Ret = std::invoke_result_t<Func, Args...>;
			using namespace std::chrono_literals;

			StatHolder runtimeStats;
			std::vector<StatHolder> monitorStats;
			std::vector<std::unique_ptr<ResourceMonitor>> monitors;
			monitorStats.reserve(m_MonitorConfigs.size());
			monitors.reserve(m_MonitorConfigs.size());
			for(size_t i = 0u; i < m_MonitorConfigs.size(); i++) {
				const auto& config = m_MonitorConfigs[i];
				monitorStats.emplace_back();
				monitors.emplace_back(std::make_unique<ResourceMonitor>(config.Type, config.Interval, monitorStats.back()));
			}

			using clock = std::chrono::high_resolution_clock;
			auto start = clock::now();
			auto end = start + m_MaxDuration;
			u32 iteration = 0;
			Ret resultValue{};
			{
				for (auto& monitor : monitors) {
					monitor->Start();
				}
				while (iteration < m_MaxIterations && clock::now() < end) {
					start = clock::now();
					resultValue = func(args...);
					auto micros = std::chrono::duration_cast<std::chrono::microseconds>(clock::now() - start).count();
					runtimeStats.Add(static_cast<f64>(micros));
					iteration++;
				}
			}

			Result<Ret> result{
				.RuntimeStats = runtimeStats.GetStats(ToUnit(m_RuntimeResolution)),
				.ReturnValue = resultValue,
				.MonitorStats = {}
			};

			result.RuntimeStats *= ToFactor(m_RuntimeResolution);
			for (size_t i = 0u; i < m_MonitorConfigs.size(); i++) {
				const auto& config = m_MonitorConfigs[i];
				auto stats = monitorStats[i].GetStats(config.Label);
				stats *= config.Factor;
				result.MonitorStats.insert({ config.Type, stats });
			}
			return result;
		}
	};

	class Builder {
	public:
		template<typename Rep, typename Period>
		Builder& WithMaxDuration(std::chrono::duration<Rep, Period> duration) {
			m_MaxDuration = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
			return *this;
		}
		Builder& WithMaxIterations(u32 iterations) {
			m_MaxIterations = iterations;
			return *this;
		}
		Builder& WithRuntimeResoultion(RuntimeResolution res) {
			m_RuntimeResolution = res;
			return *this;
		}

		Builder& WithMonitor(MonitorConfig config) {
			m_MonitorConfigs.emplace_back(config);
			return *this;
		}

		Runner Build() {
			Runner runner;
			runner.m_MaxDuration = m_MaxDuration;
			runner.m_MaxIterations = m_MaxIterations;
			runner.m_MonitorConfigs = m_MonitorConfigs;
			runner.m_RuntimeResolution = m_RuntimeResolution;
			return runner;
		}
	
	private:
		std::chrono::milliseconds m_MaxDuration{ 5min };
		u32 m_MaxIterations{ 1'000'000 };
		std::vector<MonitorConfig> m_MonitorConfigs;
		RuntimeResolution m_RuntimeResolution{ RuntimeResolution::Micros };
	};
	/*
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
			using namespace std::chrono_literals;

			StatHolder runtimeStats;
			StatHolder memStats;
			StatHolder cpuStats;
			ResourceMonitor memMonitor(MonitorType::PMem, 100ms, memStats);
			ResourceMonitor cpuMonitor(MonitorType::Cpu, 100ms, cpuStats);

			using clock = std::chrono::high_resolution_clock;
			auto start = clock::now();
			auto end = start + m_MaxDuration;
			u32 iteration = 0;
			Ret resultValue{};
			{
				memMonitor.Start();
				cpuMonitor.Start();
				while(iteration < m_MaxIterations && clock::now() < end) {
					start = clock::now();
					resultValue = func(args...);
					auto micros = std::chrono::duration_cast<std::chrono::microseconds>(clock::now() - start).count();
					runtimeStats.Add(static_cast<f64>(micros));
					iteration++;
				}
			}

			auto runtime = runtimeStats.GetStats("us");
			auto mem = memStats.GetStats("mb");
			auto cpu = cpuStats.GetStats("%");
			mem /= 1024.0 * 1024.0;
			cpu *= 100.0;
			return { runtime, mem, cpu, resultValue };
		}
	private:
		Benchmark() = default;

		std::chrono::milliseconds m_MaxDuration{ 5min };
		u32 m_MaxIterations{ 1'000'000 };
	};
	*/
}