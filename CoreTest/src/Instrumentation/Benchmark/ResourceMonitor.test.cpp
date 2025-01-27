#include "TestCommon.h"

#include "Core/Instrumentation/Benchmark/ResourceMonitor.h"
#include <chrono>

using namespace std::chrono_literals;
TEST(ResourceMonitor, Stats_AfterRunning_ArePopulated)
{
	auto stats = StatHolder();
	auto monitor = ResourceMonitor(MonitorType::TestOnly, 100ms, stats);
	{
		monitor.Start();
		std::this_thread::sleep_for(1s);
	}

	auto result = stats.GetStats();
	ASSERT_TRUE(result.Mean > 0.0);
	ASSERT_EQ(result.Min, 0.0);
	ASSERT_TRUE(result.Max > 0.0);
}