#include <algorithm>
#include <cmath>
#include <iostream>

#include "solver.h"

void Solver::move(size_t r0, size_t c0, size_t r, size_t c, int v)
{
	if (v < 0)
	{
		std::swap(r0, r);
		std::swap(c0, c);
		v = -v;
	}
	if (v > 0)
	{
		M[r0][c0] -= v;
		M[r][c] += v;
		std::cout << r0 + 1 << ' ' << c0 + 1 << ' ' << r + 1 << ' ' << c + 1 << ' ' << v << std::endl;
		cost += (std::abs(static_cast<int>(r) - static_cast<int>(r0)) + std::abs(static_cast<int>(c) - static_cast<int>(c0))) * std::abs(v);
	}
}

bool Solver::is_solved() const
{
	return std::all_of(M.begin(), M.end(), [](auto &v) {
		return std::all_of(v.begin(), v.end(), [](int x) { return x == 0; });
	});
}

void Solver::print() const
{
	for (size_t r = 0; r < R; ++r)
	{
		std::cout << "> ";
		for (size_t c = 0; c < C; ++c)
		{
			std::cout << M[r][c] << ' ';
		}
		std::cout << std::endl;
	}
}
