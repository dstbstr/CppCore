#ifdef __linux__
#include "Core/Instrumentation/Benchmark/Stats.h"

//without having a linux environment to test with,
//I didn't want to try an implementation.
//Check these resources:
//https://stackoverflow.com/questions/669438/how-to-get-memory-usage-at-runtime-using-c
//https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process/
//https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/sys_resource.h.html

#include "sys/types.h"
#include "sys/sysinfo.h"
#include "sys/resource.h"
#include <unistd.h>
#include <fstream>

namespace ResourceImpl {
	struct PhysicalMemoryMonitor {
		PhysicalMemoryMonitor(StatHolder& stats)
			: Stats(stats)
		{
		}

		void Update() {
		}

		StatHolder& Stats;
	private:
	};

	struct VirtualMemoryMonitor {
		VirtualMemoryMonitor(StatHolder& stats)
			: Stats(stats)
		{
		}

		void Update() {
		}

		StatHolder& Stats;
	private:
	};

	struct CpuMonitor {
		CpuMonitor(StatHolder& stats) : Stats(stats) {
		}

		void Update() {
		}

		StatHolder& Stats;
	private:
	};
}
#endif