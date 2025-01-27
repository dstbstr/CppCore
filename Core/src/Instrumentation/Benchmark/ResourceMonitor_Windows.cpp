#ifdef _WIN32
#define _WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <Psapi.h>
#include <thread>

#include "Core/Instrumentation/Benchmark/ResourceMonitor.h"

namespace {
	struct PhysicalMemoryMonitor {
		PhysicalMemoryMonitor(StatHolder& stats) 
			: Stats(stats)
			, m_ProcessHandle(GetCurrentProcess())
		{}

		void Update() {
			PROCESS_MEMORY_COUNTERS_EX pmc;
			GetProcessMemoryInfo(m_ProcessHandle, reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc), sizeof(pmc));

			Stats.Add(static_cast<f64>(pmc.WorkingSetSize));
		}

		StatHolder& Stats;
	private:
		HANDLE m_ProcessHandle;
	};

	struct VirtualMemoryMonitor {
		VirtualMemoryMonitor(StatHolder& stats) 
			: Stats(stats)
			, m_ProcessHandle(GetCurrentProcess())
		{}

		void Update() {
			PROCESS_MEMORY_COUNTERS_EX pmc;
			GetProcessMemoryInfo(m_ProcessHandle, reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc), sizeof(pmc));

			Stats.Add(static_cast<f64>(pmc.PrivateUsage));
		}

		StatHolder& Stats;
	private:
		HANDLE m_ProcessHandle;
	};

	struct CpuMonitor {
		CpuMonitor(StatHolder& stats) : Stats(stats) {
			SYSTEM_INFO sysInfo;
			GetSystemInfo(&sysInfo);
			m_ProcCount = sysInfo.dwNumberOfProcessors;

			GetSystemTimeAsFileTime(&m_Ftime);
			memcpy(&m_LastCpu, &m_Ftime, sizeof(FILETIME));

			m_ProcessHandle = GetCurrentProcess();
			if (!GetProcessTimes(m_ProcessHandle, &m_Ftime, &m_Ftime, &m_Fsys, &m_Fuser)) throw "Wat?";
			memcpy(&m_LastSysCpu, &m_Fsys, sizeof(FILETIME));
			memcpy(&m_LastUserCpu, &m_Fuser, sizeof(FILETIME));
		}
		
		void Update() {
			GetSystemTimeAsFileTime(&m_Ftime);
			ULARGE_INTEGER now;
			memcpy(&now, &m_Ftime, sizeof(FILETIME));
			if (!GetProcessTimes(m_ProcessHandle, &m_Ftime, &m_Ftime, &m_Fsys, &m_Fuser)) throw "Wat?";

			ULARGE_INTEGER sys, user;
			memcpy(&sys, &m_Fsys, sizeof(FILETIME));
			memcpy(&user, &m_Fuser, sizeof(FILETIME));

			auto percent = static_cast<f64>((sys.QuadPart - m_LastSysCpu.QuadPart) + 
				(user.QuadPart - m_LastUserCpu.QuadPart));
			percent /= (now.QuadPart - m_LastCpu.QuadPart);
			percent /= m_ProcCount;

			m_LastCpu = now;
			m_LastUserCpu = user;
			m_LastSysCpu = sys;

			Stats.Add(percent);
		}

		StatHolder& Stats;
	private:
		ULARGE_INTEGER m_LastCpu {0};
		ULARGE_INTEGER m_LastSysCpu {0};
		ULARGE_INTEGER m_LastUserCpu {0};
		FILETIME m_Ftime {0};
		FILETIME m_Fsys {0};
		FILETIME m_Fuser {0};
		int m_ProcCount {0};
		HANDLE m_ProcessHandle;
	};

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
			TMonitor monitor{stats};
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
		m_MonitorThread = StartMonitor<PhysicalMemoryMonitor>(m_Interval, m_DoneFlag, m_Stats);
		break;
	case MonitorType::VMem:
		m_MonitorThread = StartMonitor<VirtualMemoryMonitor>(m_Interval, m_DoneFlag, m_Stats);
		break;
	case MonitorType::Cpu:
		m_MonitorThread = StartMonitor<CpuMonitor>(m_Interval, m_DoneFlag, m_Stats);
		break;
	}
}

ResourceMonitor::~ResourceMonitor() {
	if(m_MonitorThread.has_value()) {
		m_DoneFlag = true;
		m_MonitorThread->join();
		m_MonitorThread.reset();
	}
}

#endif