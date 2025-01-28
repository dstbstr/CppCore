#ifdef _WIN32
#define _WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <Psapi.h>
#include "Core/Instrumentation/Benchmark/Stats.h"

namespace ResourceImpl {
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
		struct CpuData {
			ULARGE_INTEGER Cpu{};
			ULARGE_INTEGER SysCpu{};
			ULARGE_INTEGER UserCpu{};
			FILETIME Time{};
			FILETIME SysTime{};
			FILETIME UserTime{};

			static CpuData Get(HANDLE process) {
				CpuData data;
				GetSystemTimeAsFileTime(&data.Time);
				memcpy(&data.Cpu, &data.Time, sizeof(FILETIME));
				if (!GetProcessTimes(process, &data.Time, &data.Time, &data.SysTime, &data.UserTime)) throw "Wat?";
				memcpy(&data.SysCpu, &data.SysTime, sizeof(FILETIME));
				memcpy(&data.UserCpu, &data.UserTime, sizeof(FILETIME));
				return data;
			}
		};

		CpuMonitor(StatHolder& stats) : Stats(stats) {
			SYSTEM_INFO sysInfo;
			GetSystemInfo(&sysInfo);
			m_ProcCount = sysInfo.dwNumberOfProcessors;
			m_ProcessHandle = GetCurrentProcess();

			m_Previous = CpuData::Get(m_ProcessHandle);
		}
		
		void Update() {
			auto current = CpuData::Get(m_ProcessHandle);
			auto percent = static_cast<f64>((current.SysCpu.QuadPart - m_Previous.SysCpu.QuadPart) +
				(current.UserCpu.QuadPart - m_Previous.UserCpu.QuadPart));
			percent /= (current.Cpu.QuadPart - m_Previous.Cpu.QuadPart);
			percent /= m_ProcCount;
			m_Previous = current;

			Stats.Add(percent);
		}

		StatHolder& Stats;
	private:
		CpuData m_Previous;
		int m_ProcCount {0};
		HANDLE m_ProcessHandle;
	};
}

#endif