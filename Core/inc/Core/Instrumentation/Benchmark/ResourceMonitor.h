#pragma once
#include <chrono>
#include <optional>

#include "Core/Instrumentation/Benchmark/Stats.h"

enum struct MonitorType { PMem, VMem, Cpu, TestOnly };

class ResourceMonitor {
public:
	template<typename Rep, typename Period>
	ResourceMonitor(MonitorType type, std::chrono::duration<Rep, Period> interval, StatHolder& outStats)
		: m_Type(type)
		, m_Interval(std::chrono::duration_cast<std::chrono::milliseconds>(interval))
		, m_Stats(outStats)
	{}
	~ResourceMonitor();

	void Start();
private:
	MonitorType m_Type;
	std::chrono::milliseconds m_Interval;
	std::optional<std::thread> m_MonitorThread;
	bool m_DoneFlag{ false };
	StatHolder& m_Stats;
};