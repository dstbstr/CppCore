#pragma once
#include <vector>

template<typename T>
struct UnionFind {
    std::vector<T> parents;
    std::vector<size_t> sizes;
    constexpr UnionFind(size_t n)
        : parents(n)
        , sizes(n, 1) {
        for (size_t i = 0; i < n; i++) {
            parents[i] = static_cast<T>(i);
        }
    }

    constexpr T Find(T n) {
        if (parents[n] != n) {
            parents[n] = Find(parents[n]);
        }
        return parents[n];
    }

    constexpr void Join(T a, T b) {
        auto lhs = Find(a);
        auto rhs = Find(b);
        if (lhs != rhs) {
            if (sizes[lhs] > sizes[rhs]) std::swap(lhs, rhs);
            parents[lhs] = rhs;
            sizes[rhs] += sizes[lhs];
        }
    }

    constexpr size_t Size(T n) const {
        auto root = Find(n);
        return sizes[root];
    }

    constexpr size_t CountRoots() const {
        std::vector<T> roots;
        for (size_t i = 0; i < parents.size(); i++) {
            roots.push_back(Find(i));
        }
        std::sort(roots.begin(), roots.end());
        roots.erase(std::unique(roots.begin(), roots.end()), roots.end());
        return roots.size();
    }
};
