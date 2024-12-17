#pragma once
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <algorithm>
#include <queue>

#include "Core/Constexpr/ConstexprGeometry.h"
#include "Core/Constexpr/ConstexprCollections.h"

/*
How to use:
    AStarParameters params {
        .map = lines,
        .start = begin,
        .end = end
    };
    std::optional<std::vector<RowCol>> path = AStarMin<10'000>(params);

Advanced Usage
namespace Year2024Day16 {
    struct State {
        RowCol Pos;
        Facing Dir;
        u32 Cost;
    };
}
namespace Constexpr {
    template<>
	struct Hasher<Year2025Day1::State> {
		constexpr size_t operator()(const Year2025Day1::State& state) const {
			return Hasher<RowCol>()(state.Pos);
		}
	};
}
SOLUTION(2024, 16) {
    using Grid = std::vector<std::string>;
    size_t Solve(const Grid& lines) {
        auto costFunc = [](const State& from, const State& to) -> size_t {
            return from.Dir == to.Dir ? 1 : 1001;
        };
        auto doneFunc = [](const State& pos, const State& goal) -> bool {
            return pos.Pos == goal.Pos;
        };
        auto heuristic = [](const State& from, const State& to) -> size_t {
            return MDistance(from.Pos, to.Pos);
        };
        auto neighbors = [](const Grid& map, const State& pos) -> std::vector<State> {
            auto n = GetDirectNeighbors(pos, GetLimits<RowCol>(map));
            return n
                | std::views::filter([&](const RowCol rc) { return map[rc.Row][rc.Col] == '.'; })
                | std::ranges::to<std::vector>();
        };
        auto onMoveFunc = [](State& from, State& to) {
            if(from.Cost % 2 == 0) {
                to.Cost *= 2;
            } else {
                to.Cost /= 2;
            }
        };

        State start = {RowCol{0, 0}, Facing::Right, 0};
        State end = {RowCol{lines.size() - 1, lines[0].size() - 1}, Facing::Left, 0};

        AStarParameters<Grid, State> params {
            .map = lines,
            .start = start,
            .end = end,
            .costFunc = costFunc,
            .doneFunc = doneFunc,
            .hFunc = heuristic,
            .nFunc = neighbors,
            .moveFunc = onMoveFunc
        };
        std::optional<std::vector<State>> path = AStarMin<100'000>(params);
        if(path.has_value()) {
            return path.back().Cost;
        }
        return 0;
    }
}
*/

template<typename T>
using CostFunc = size_t(*)(const T&, const T&);
template<typename T>
using DoneFunc = bool(*)(const T&, const T&);
template<typename T>
using HFunc = size_t(*)(const T&, const T&);
template<typename T, typename Map>
using NFunc = std::vector<T>(*)(const Map&, const T&);
template<typename T>
using MoveFunc = void(*)(T&, T&);

template<typename T, typename Map>
struct AStarParameters {
    Map map{};
    T start{};
    T end{};
    std::optional<CostFunc<T>> costFunc{};
	std::optional<DoneFunc<T>> doneFunc{};
	std::optional<HFunc<T>> hFunc{};
	std::optional<NFunc<T, Map>> nFunc{};
	std::optional<MoveFunc<T>> moveFunc{};
};

namespace AStarPrivate {
    template<typename T>
    struct MinimalPath {
        size_t Known = std::numeric_limits<size_t>::max();
        size_t Forcast = std::numeric_limits<size_t>::max();
        T Val{};

        constexpr MinimalPath() {}
        constexpr explicit MinimalPath(T t) : Val(t) {}

        constexpr bool operator<(const MinimalPath& other) const {
            return other.Forcast != Forcast 
                ? other.Forcast < Forcast 
                : Known < other.Known;
        }
    };

    template<typename T>
    struct MaximalPath {
        size_t Known = 0;
        size_t Forcast = 0;
        T Val{};

        constexpr MaximalPath() {}
        constexpr explicit MaximalPath(T t) : Val(t) {}

        constexpr bool operator<(const MaximalPath& other) const {
            return Forcast != other.Forcast 
                ? Forcast < other.Forcast 
                : other.Known < Known;
        }
    };

    template<typename T, typename State, typename SeenType = Constexpr::SmallSet<T>, typename HistoryType = Constexpr::SmallMap<T, T>, typename ForcastType = Constexpr::SmallMap<T, State>>
    constexpr std::vector<T> AStar(T start, auto costFunc, auto doneFunc, auto hFunc, auto nFunc, auto moveFunc) {       
        SeenType seen{};
        HistoryType cameFrom{};
        ForcastType state{};

        Constexpr::PriorityQueue<State> queue{};

        auto startState = State(start);
        startState.Known = 0;
        startState.Forcast = hFunc(start);
        state[start] = startState;
        cameFrom[start] = start;
        queue.push(startState);

        auto constructPath = [&cameFrom](const T& start, const T& end) {
            std::vector<T> result{ end };
            auto current = end;
            while (current != start) {
                auto next = cameFrom.at(current);
                result.push_back(next);
                current = next;
            }

            std::reverse(result.begin(), result.end());
            return result;
        };

        while (!queue.empty()) {
            auto current = queue.pop();

            if (doneFunc(current.Val)) {
                return constructPath(start, current.Val);
            }

            seen.insert(current.Val);
            for (auto neighbor : nFunc(current.Val)) {
                if (seen.contains(neighbor)) continue;

                auto known = current.Known + costFunc(current.Val, neighbor);
                if (known < state[neighbor].Known) {
                    state[neighbor].Known = known;
                    state[neighbor].Forcast = known + hFunc(neighbor);
                    State next = State(neighbor);
                    next.Known = known;
                    next.Forcast = state[neighbor].Forcast;
                    moveFunc(current.Val, next.Val);
                    cameFrom[next.Val] = current.Val;
                    queue.push(next);
                }
            }
        }

        return {};
    }

    template<typename T, typename Map>
    struct AStar2Params {
        Map map{};
        T start{};
        T end{};
        CostFunc<T> costFunc{};
        DoneFunc<T> doneFunc{};
        HFunc<T> hFunc{};
        NFunc<T, Map> nFunc{};
        MoveFunc<T> moveFunc{};
    };

    template<typename T, size_t Reserve>
	constexpr std::vector<T> ConstructPath(const Constexpr::BigMap<T, T, Reserve>& cameFrom, const T& start, const T& end) {
		std::vector<T> result{ end };
		auto current = end;
		while (current != start) {
			auto next = cameFrom.at(current);
			result.push_back(next);
			current = next;
		}
		std::reverse(result.begin(), result.end());
		return result;
	}

    template<size_t Reserve, typename State, typename T, typename Map>
    constexpr std::optional<std::vector<T>> AStar2(AStar2Params<T, Map> params) {

        Constexpr::BigSet<T, Reserve> seen{};
        Constexpr::BigMap<T, T, Reserve> cameFrom{};
        Constexpr::BigMap<T, State, Reserve> state{};

        Constexpr::PriorityQueue<State> queue{};
        auto start = params.start;

        auto startState = State(start);
        startState.Known = 0;
        startState.Forcast = params.hFunc(start, params.end);
        state[start] = startState;
        cameFrom[start] = start;
        queue.push(startState);

        while (!queue.empty()) {
            auto current = queue.pop();

            if (params.doneFunc(current.Val, params.end)) {
                return ConstructPath(cameFrom, start, current.Val);
            }

            seen.insert(current.Val);
            for (auto neighbor : params.nFunc(params.map, current.Val)) {
                if (seen.contains(neighbor)) continue;

                auto known = current.Known + params.costFunc(current.Val, neighbor);
                if (known < state[neighbor].Known) {
                    state[neighbor].Known = known;
                    state[neighbor].Forcast = known + params.hFunc(neighbor, params.end);
                    State next = State(neighbor);
                    next.Known = known;
                    next.Forcast = state[neighbor].Forcast;
                    params.moveFunc(current.Val, next.Val);
                    cameFrom[next.Val] = current.Val;
                    queue.push(next);
                }
            }
        }

        return std::nullopt;
    }

    template<typename T>
    concept HasMDistance = requires(const T & from, const T & to) {
        { MDistance(from, to) } -> std::convertible_to<size_t>;
    };

    template<typename T>
    concept HasDirectNeighbors = requires(const T & pos) {
        { GetDirectNeighbors(pos) } -> std::convertible_to<std::vector<T>>;
    };

    template<typename T>
    constexpr size_t FallbackCostFunc(const T&, const T&) {
        return 1;
    }

    template<typename T>
	constexpr bool FallbackDoneFunc(const T& curr, const T& end) {
		return curr == end;
	}

    template<typename T>
    constexpr size_t FallbackHFunc(const T& from, const T& to) {
        if constexpr (HasMDistance<T>) {
            return MDistance(from, to);
        }
        else {
            throw "No hFunc provided and MDistance not available";
        }
    }

    template<typename T, typename Map>
    constexpr std::vector<T> FallbackNFunc(const Map&, const T& pos) {
        if constexpr (HasDirectNeighbors<T>) {
            return GetDirectNeighbors(pos);
        }
        else {
            throw "No nFunc provided and GetDirectNeighbors not available";
        }
    }

    template<typename T>
	constexpr void FallbackMoveFunc(T&, T&) {}
}

template<size_t Reserve, typename T, typename Map>
constexpr std::optional<std::vector<T>> AStarMin(const AStarParameters<T, Map>& params) {
    return AStarPrivate::AStar2<Reserve, AStarPrivate::MinimalPath<T>>(AStarPrivate::AStar2Params<T, Map>{
        .map = params.map,
        .start = params.start,
        .end = params.end,
        .costFunc = params.costFunc.value_or(AStarPrivate::FallbackCostFunc<T>),
        .doneFunc = params.doneFunc.value_or(AStarPrivate::FallbackDoneFunc<T>),
        .hFunc = params.hFunc.value_or(AStarPrivate::FallbackHFunc<T>),
        .nFunc = params.nFunc.value_or(AStarPrivate::FallbackNFunc<T, Map>),
        .moveFunc = params.moveFunc.value_or(AStarPrivate::FallbackMoveFunc<T>)
    });
}


template<size_t Reserve, typename T, typename Map>
constexpr std::optional<std::vector<T>> AStarMax(const AStarParameters<T, Map>& params) {
    return AStarPrivate::AStar2<Reserve, AStarPrivate::MaximalPath>(AStarPrivate::AStar2Params<T, Map>{
        .map = params.map,
        .start = params.start,
        .end = params.end,
        .costFunc = params.costFunc.value_or(AStarPrivate::FallbackCostFunc<T>),
        .doneFunc = params.doneFunc.value_or(AStarPrivate::FallbackDoneFunc<T>),
        .hFunc = params.hFunc.value_or(AStarPrivate::FallbackHFunc<T>),
        .nFunc = params.nFunc.value_or(AStarPrivate::FallbackNFunc<T>),
        .moveFunc = params.moveFunc.value_or(AStarPrivate::FallbackMoveFunc<T>)
    });
}

template<typename T, bool Big = false>
constexpr std::vector<T> AStarMin(T start,
    auto costFunc,
    auto doneFunc,
    auto hFunc,
    auto nFunc,
    auto moveFunc) {

    if constexpr (Big) {
        return AStarPrivate::AStar<
            T,
            AStarPrivate::MinimalPath<T>,
            Constexpr::BigSet<T>,
            Constexpr::BigMap<T, T>,
            Constexpr::BigMap<T, AStarPrivate::MinimalPath<T>>
        >(start, costFunc, doneFunc, hFunc, nFunc, moveFunc);
    }
    else {
        return AStarPrivate::AStar<
            T,
            AStarPrivate::MinimalPath<T>,
            Constexpr::SmallSet<T>,
            Constexpr::SmallMap<T, T>,
            Constexpr::SmallMap<T, AStarPrivate::MinimalPath<T>>
        >(start, costFunc, doneFunc, hFunc, nFunc, moveFunc);
    }
}

template<typename T, bool Big = false>
constexpr std::vector<T> AStarMin(T start, T end, auto nFunc) {
    auto costFunc = [](const T&, const T&) {return 1; };
    auto doneFunc = [&end](const T& pos) { return pos == end; };
    auto hFunc = [&end](const T& pos) { return static_cast<size_t>(MDistance(pos, end)); };
    auto moveFunc = [](T&, T&) {};

    if constexpr (Big) {
        return AStarPrivate::AStar<
            T,
            AStarPrivate::MinimalPath<T>,
            Constexpr::BigSet<T>,
            Constexpr::BigMap<T, T>,
            Constexpr::BigMap<T, AStarPrivate::MinimalPath<T>>
        >(start, costFunc, doneFunc, hFunc, nFunc, moveFunc);
    }
    else {
        return AStarPrivate::AStar<
            T,
            AStarPrivate::MinimalPath<T>,
            Constexpr::SmallSet<T>,
            Constexpr::SmallMap<T, T>,
            Constexpr::SmallMap<T, AStarPrivate::MinimalPath<T>>
        >(start, costFunc, doneFunc, hFunc, nFunc, moveFunc);
    }
}

template<typename T, bool Big = false>
constexpr std::vector<T> AStarMax(T start,
    auto costFunc,
    auto doneFunc,
    auto hFunc,
    auto nFunc,
    auto moveFunc) {
    if constexpr (Big) {
        return AStarPrivate::AStar<
            T,
            AStarPrivate::MaximalPath<T>,
            Constexpr::BigSet<T>,
            Constexpr::BigMap<T, T>,
            Constexpr::BigMap<T, AStarPrivate::MaximalPath<T>>
        >(start, costFunc, doneFunc, hFunc, nFunc, moveFunc);
    }
    else {
        return AStarPrivate::AStar<
            T,
            AStarPrivate::MaximalPath<T>,
            Constexpr::SmallSet<T>,
            Constexpr::SmallMap<T, T>,
            Constexpr::SmallMap<T, AStarPrivate::MaximalPath<T>>
        >(start, costFunc, doneFunc, hFunc, nFunc, moveFunc);
    }
}

template<typename T, bool Big = false>
constexpr std::vector<T> AStarMax(T start, T end, auto nFunc) {
    auto costFunc = [](const T&, const T&) {return 1; };
    auto doneFunc = [&end](const T& pos) { return pos == end; };
    auto hFunc = [&end](const T& pos) { return static_cast<size_t>(MDistance(pos, end)); };
    auto moveFunc = [](T&, T&) {};

    if constexpr (Big) {
        return AStarPrivate::AStar<
            T,
            AStarPrivate::MaximalPath<T>,
            Constexpr::BigSet<T>,
            Constexpr::BigMap<T, T>,
            Constexpr::BigMap<T, AStarPrivate::MaximalPath<T>>
        >(start, costFunc, doneFunc, hFunc, nFunc, moveFunc);
    }
    else {
        return AStarPrivate::AStar<
            T,
            AStarPrivate::MaximalPath<T>,
            Constexpr::SmallSet<T>,
            Constexpr::SmallMap<T, T>,
            Constexpr::SmallMap<T, AStarPrivate::MaximalPath<T>>
        >(start, costFunc, doneFunc, hFunc, nFunc, moveFunc);
    }
}