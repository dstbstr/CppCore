#pragma once

#include <future>
#include <vector>

using ProgressCallback = std::function<void(size_t, size_t, double)>;

auto WhenAll(auto tasks, ProgressCallback progressCallback = nullptr) {
    using namespace std::chrono_literals;
    using FuncType = decltype(tasks)::value_type;
	using ReturnType = std::invoke_result_t<FuncType>;
    if(tasks.empty()) {
		if constexpr (std::is_same_v<ReturnType, void>) {
            std::promise<void> p;
			return p.get_future();
			//return std::future<void>{};
		}
		else {
            std::promise<std::vector<ReturnType>> p;
            return p.get_future();
			//return std::future<std::vector<ReturnType>>{};
		}
	}

    return std::async(std::launch::async, [tasks, progressCallback]() {
        size_t MaxConcurrency = std::max(1ull, static_cast<size_t>(std::thread::hardware_concurrency()));
        std::vector<std::future<ReturnType>> futures;
        auto copy = tasks;
        size_t finishedTasks = 0;
        size_t totalTasks = tasks.size();
        size_t currentTask = totalTasks;
        if constexpr (std::is_same_v<ReturnType, void>) {
            while (finishedTasks < totalTasks) {
                auto taskCount = std::min(currentTask, MaxConcurrency - futures.size());
                for (auto i = 0; i < taskCount; i++) {
                    futures.emplace_back(std::async(std::launch::async, copy[--currentTask]));
                }
                for (int i = static_cast<int>(futures.size()) - 1; i >= 0; i--) {
                    if (futures[i].wait_for(0ms) == std::future_status::ready) {
                        std::swap(futures[i], futures.back());
                        futures.pop_back();
                        finishedTasks++;
                        if (progressCallback) {
                            progressCallback(finishedTasks, totalTasks, static_cast<double>(finishedTasks) / totalTasks);
                        }
                    }
                }
            }
        }
        else {
            std::vector<ReturnType> results;
            results.reserve(tasks.size());

            while (finishedTasks < totalTasks) {
                auto taskCount = std::min(currentTask, MaxConcurrency - futures.size());
                for (auto i = 0; i < taskCount; i++) {
                    futures.emplace_back(std::async(std::launch::async, copy[--currentTask]));
                }
                for (int i = static_cast<int>(futures.size()) - 1; i >= 0; i--) {
                    if (futures[i].wait_for(0ms) == std::future_status::ready) {
                        results.emplace_back(futures[i].get());
                        std::swap(futures[i], futures.back());
                        futures.pop_back();
                        finishedTasks++;
                        if (progressCallback) {
                            progressCallback(finishedTasks, totalTasks, static_cast<double>(finishedTasks) / totalTasks);
                        }
                    }
                }
            }
            return results;
        }
    });
}
/*
std::future<void> WhenAll(auto tasks, ProgressCallback progressCallback = nullptr) {
	using namespace std::chrono_literals;

    return std::async(std::launch::async, [tasks, progressCallback]() {
        size_t MaxConcurrency = std::max(1ull, static_cast<size_t>(std::thread::hardware_concurrency()));
        std::vector<std::future<void>> futures;
        auto copy = tasks;
        size_t finishedTasks = 0;
        size_t totalTasks = tasks.size();
        size_t currentTask = totalTasks;
        while (finishedTasks < totalTasks) {
			auto taskCount = std::min(currentTask, MaxConcurrency - futures.size());
            for (auto i = 0; i < taskCount; i++) {
				futures.emplace_back(std::async(std::launch::async, copy[--currentTask]));
            }
            for (int i = static_cast<int>(futures.size()) - 1; i >= 0; i--) {
                if (futures[i].wait_for(0ms) == std::future_status::ready) {
                    std::swap(futures[i], futures.back());
                    futures.pop_back();
                    finishedTasks++;
                    if (progressCallback) {
                        progressCallback(finishedTasks, totalTasks, static_cast<double>(finishedTasks) / totalTasks);
                    }
                }
            }

        }
    });
}
*/

/*
auto WhenAll(auto tasks, ProgressCallback progressCallback = nullptr) {
    using namespace std::chrono_literals;
    using ReturnType = std::invoke_result_t<decltype<std::declval(tasks)>>;
    if (tasks.empty()) {
		if constexpr (std::is_same_v<ReturnType, void>) {
			return std::future<void>{};
		}
		else {
			return std::future<std::vector<ReturnType>>{};
		}
    }
    //if (tasks.empty()) return std::future<void>{};
	//using ReturnType = std::invoke_result_t<decltype(tasks[0])>;   

    return std::async(std::launch::async, [tasks, progressCallback]() {
        size_t MaxConcurrency = std::max(1ull, static_cast<size_t>(std::thread::hardware_concurrency()));

        std::vector<std::future<ReturnType>> futures;

        auto copy = tasks;
        size_t finishedTasks = 0;
        size_t totalTasks = tasks.size();
        size_t currentTask = totalTasks;
        if constexpr (std::is_same_v<ReturnType, void>) {
            while (finishedTasks < totalTasks) {
                auto taskCount = std::min(currentTask, MaxConcurrency - futures.size());
                for (auto i = 0; i < taskCount; i++) {
                    futures.emplace_back(std::async(std::launch::async, copy[--currentTask]));
                }
                for (int i = static_cast<int>(futures.size()) - 1; i >= 0; i--) {
                    if (futures[i].wait_for(0ms) == std::future_status::ready) {
                        std::swap(futures[i], futures.back());
                        futures.pop_back();
                        finishedTasks++;
                        if (progressCallback) {
                            progressCallback(finishedTasks, totalTasks, static_cast<double>(finishedTasks) / totalTasks);
                        }
                    }
                }
            }
        }
        else {
            std::vector<ReturnType> results;
            results.reserve(tasks.size());

            while (finishedTasks < totalTasks) {
                auto taskCount = std::min(currentTask, MaxConcurrency - futures.size());
                for (auto i = 0; i < taskCount; i++) {
                    futures.emplace_back(std::async(std::launch::async, copy[--currentTask]));
                }
                for (int i = static_cast<int>(futures.size()) - 1; i >= 0; i--) {
                    if (futures[i].wait_for(0ms) == std::future_status::ready) {
                        results.emplace_back(futures[i].get());
                        std::swap(futures[i], futures.back());
                        futures.pop_back();
                        finishedTasks++;
                        if (progressCallback) {
                            progressCallback(finishedTasks, totalTasks, static_cast<double>(finishedTasks) / totalTasks);
                        }
                    }
                }
            }
			return results;
        }
        
    });
}
*/

/*
template<typename Func, template<typename> class Collection> //doesn't match vector
template<typename Func, template<typename, typename> class Collection> // doesn't match array
auto WhenAll(Collection<Func> tasks, ProgressCallback callback = nullptr) {
	using ReturnType = std::invoke_result_t<Func>;
	if (tasks.empty()) {
		if constexpr (std::is_same_v<ReturnType, void>) {
			return std::future<void>{};
		}
		else {
			return std::future<std::vector<ReturnType>>{};
		}
	}
}
*/