#pragma once

#include <concepts>
#include <string_view>

template<typename T>
concept Numeric = requires(T t) {
	t + t;
	t - t;
	t * t;
	t / t;
};

template<typename T>
concept Integral = Numeric<T> && requires(T t) {
	t % t;
};

template<typename T>
concept Signed = Numeric<T> && requires {
	requires T(-1) < T(0);
};

template<typename T>
concept Unsigned = !Signed<T>;

template<typename T>
concept StringLike = std::is_convertible_v<T, std::string_view>;
