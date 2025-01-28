#pragma once
#include <chrono>
#include <optional>
#include <thread>

#include "Core/Instrumentation/Benchmark/Stats.h"

// TestOnly should be the last member (it's used as a count as well)
enum struct MonitorType { PMem, VMem, Cpu, TestOnly };
constexpr std::string ToString(MonitorType type) {
	switch (type) {
	case MonitorType::PMem: return "Physical Memory";
	case MonitorType::VMem: return "Virtual Memory";
	case MonitorType::Cpu: return "CPU";
	case MonitorType::TestOnly: return "Test Only";
	}
	return "Unknown";
}

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