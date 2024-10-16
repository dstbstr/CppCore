#include "TestCommon.h"
#include "Core/Threading/Tasks.h"

#include <thread>
#include <chrono>
#include <array>
#include <functional>

using namespace std::chrono_literals;

std::function<void()> MakeTask(size_t delayTime, bool* doneFlag = nullptr) {
	return [doneFlag, delayTime]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(delayTime));
		if (doneFlag) *doneFlag = true;
	};
}

TEST(WhenAll, WhenAll_WithNoTasks_ReturnsValidFuture) {
	auto future = WhenAll(std::vector<std::function<void()>>{});
	ASSERT_TRUE(future.valid());
	future.wait();
}

TEST(WhenAll, WhenAll_WithSingleTask_RunsTask) {
	bool doneFlag = false;
	auto future = WhenAll(std::vector{ MakeTask(1, &doneFlag) });
	future.wait();
	ASSERT_TRUE(doneFlag);
}

TEST(WhenAll, WhenAll_WithManyTasks_RunsAllTasks) {
	const size_t taskCount = 50;
	std::array<bool, taskCount> doneFlags = {};
	std::array<std::function<void()>, taskCount> tasks = {};

	for (size_t i = 0; i < taskCount; i++) {
		tasks[i] = MakeTask(i % 7, &doneFlags[i]);
	}

	auto future = WhenAll(tasks);
	future.wait();
	for(const auto& doneFlag : doneFlags) {
		ASSERT_TRUE(doneFlag);
	}
}

TEST(WhenAll, WhenAll_WithProgressCallback_CallsCallback) {
	size_t callCount = 0;
	auto OnProgress = [&callCount](size_t, size_t, double) {
		callCount++;
	};

	auto future = WhenAll(std::vector{ MakeTask(1) }, OnProgress);
	future.wait();
	ASSERT_EQ(callCount, 1);
}

std::function<size_t()> MakeResultTask(size_t result, size_t delayTime) {
	return [result, delayTime]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(delayTime));
		return result;
	};
}

TEST(WhenAll, WhenAll_WithEmptyResultTask_ReturnsValidFuture) {
	auto future = WhenAll(std::vector<std::function<size_t()>>{});
	ASSERT_TRUE(future.valid());
	future.wait();
}

TEST(WhenAll, WhenAll_WithResult_ReturnsVectorOfResults) {
	std::vector<std::function<size_t()>> tasks{};
	std::vector<size_t> expectedResults{};
	for(size_t i = 0ull; i < 50; i++) {
		tasks.emplace_back(MakeResultTask(i, i % 10));
		expectedResults.emplace_back(i);
	}
	auto future = WhenAll(tasks);
	future.wait();
	auto results = future.get();
	ASSERT_THAT(results, ::testing::UnorderedElementsAreArray(expectedResults));
}