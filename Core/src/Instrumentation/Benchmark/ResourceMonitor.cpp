#include <thread>

#include "Core/Instrumentation/Benchmark/ResourceMonitor.h"

#ifdef WIN32
#include "ResourceMonitor_Windows.h"
#elif defined(__linux__)
#include "ResourceMonitor_Linux.h"
#else
#error "Benchmark monitor not implemented for this platform"
#endif

namespace {
	struct TestOnlyMonitor {
		TestOnlyMonitor(StatHolder& stats) : Stats(stats) {}
		void Update() {
			Stats.Add(static_cast<f64>(Value++));
		}

		StatHolder& Stats;
		u32 Value{ 0 };
	};

	template<typename TMonitor>
	std::thread StartMonitor(std::chrono::milliseconds interval, bool& doneFlag, StatHolder& stats) {
		return std::thread([interval, &doneFlag, &stats]() {
			TMonitor monitor{ stats };
			while (!doneFlag) {
				monitor.Update();
				std::this_thread::sleep_for(interval);
			}
		});
	}
}

void ResourceMonitor::Start() {
	if (m_MonitorThread.has_value()) throw "Started multiple times";

	switch (m_Type) {
	case MonitorType::PMem:
		m_MonitorThread = StartMonitor<ResourceImpl::PhysicalMemoryMonitor>(m_Interval, m_DoneFlag, m_Stats);
		break;
	case MonitorType::VMem:
		m_MonitorThread = StartMonitor<ResourceImpl::VirtualMemoryMonitor>(m_Interval, m_DoneFlag, m_Stats);
		break;
	case MonitorType::Cpu:
		m_MonitorThread = StartMonitor<ResourceImpl::CpuMonitor>(m_Interval, m_DoneFlag, m_Stats);
		break;
	case MonitorType::TestOnly:
		m_MonitorThread = StartMonitor<TestOnlyMonitor>(m_Interval, m_DoneFlag, m_Stats);
		break;
	}
}

ResourceMonitor::~ResourceMonitor() {
	if (m_MonitorThread.has_value()) {
		m_DoneFlag = true;
		m_MonitorThread->join();
		m_MonitorThread.reset();
	}
}