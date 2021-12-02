#pragma once

#include <chrono>
#include <iostream>
#include <utility>

using point = std::pair<size_t, size_t>;

inline std::ostream &operator<<(std::ostream &str, point p)
{
	return str << p.first << ',' << p.second;
}

extern std::chrono::high_resolution_clock::time_point const g_start;
