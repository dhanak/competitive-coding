#include <cassert>
#include <cmath>
#include <deque>
#include <iostream>
#include <limits>
#include <map>
#include <tuple>
#include <vector>

#include "treesolver.h"

static boost::optional<int> operator-(boost::optional<int> const &v)
{
	return v ? boost::make_optional(-*v) : boost::none;
}

static int sgn(int v)
{
	return v > 0 ? 1 : v == 0 ? 0 : -1;
}

TreeSolver::TreeSolver(matrix M)
	: Solver(M), null(R, C)
{
	g_.resize(R, C);
	u.resize(R, C);
	parent.resize(R, C, null);
}

void TreeSolver::solve()
{
	build_initial_tree();
	optimize();
	finalize();
}

void TreeSolver::build_initial_tree()
{
	matrix b(M);
	std::vector<int> Rsum(C), Csum(R);
	for (size_t i = 0; i < R; ++i)
	{
		for (size_t j = 0; j < C; ++j)
		{
			int const bij = b[i][j];
			Rsum[i] += bij;
			Csum[j] += bij;
		}
	}
	for (size_t i = R - 1; i-- > 0;)
	{
		Rsum[i] += Rsum[i + 1];
		Csum[i] += Csum[i + 1];
	}

	for (size_t i = 0; i < R; ++i)
	{
		for (size_t j = 0; j < C; ++j)
		{
			int const bij = b[i][j];
			if (i < R - 1 && (j == C - 1 || std::abs(bij + Rsum[i + 1]) < std::abs(bij + Csum[j + 1])))
			{
				g({i, j}, {i + 1, j}, bij);
				b[i + 1][j] += bij;
				Rsum[i + 1] += bij;
			}
			else if (j < C - 1)
			{
				g({i, j}, {i, j + 1}, bij);
				b[i][j + 1] += bij;
				Csum[j + 1] += bij;
			}
		}
	}

	update_u({R / 2, C / 2});
}

void TreeSolver::optimize()
{
	point ij, kl;
	for (int c_min = argmin_c(ij, kl); c_min < 0; c_min = argmin_c(ij, kl))
	{
		//std::cerr << c_min << ':' << ij << '-' << kl << std::endl;
		if (!update_loop(find_loop(ij, kl)))
			break;

		using namespace std::chrono_literals;
		if (std::chrono::high_resolution_clock::now() - g_start > 1500ms)
			break;
	}

	int c_min = argmin_c(ij, kl);
	std::cerr << "c_min=" << c_min << std::endl;
}

void TreeSolver::finalize()
{
	std::map<std::tuple<size_t, size_t, size_t, size_t>, int> g2;

	// expand
	for (size_t i = 0; i < R; ++i)
	{
		for (size_t j = 0; j < C; ++j)
		{
			int const d = g_[i][j].down.value_or(0);
			int const r = g_[i][j].right.value_or(0);
			g2[{i, j, i + 1, j}] = d;
			g2[{i + 1, j, i, j}] = -d;
			g2[{i, j, i, j + 1}] = r;
			g2[{i, j + 1, i, j}] = -r;
		}
	}

	// merge
	for (size_t i = 0; i < R; ++i)
	{
		for (size_t j = 0; j < C; ++j)
		{
			while (1)
			{
				auto const first = g2.lower_bound({i, j, 0, 0});
				auto const last = g2.upper_bound({i, j, R, C});
				auto neg = g2.end();
				auto pos = g2.end();
				for (auto it = first; it != last; ++it)
				{
					int const v = it->second;
					if (v < 0)
					{
						neg = it;
					}
					else if (v > 0)
					{
						pos = it;
					}
				}
				if (neg == g2.end() || pos == g2.end())
					break;

				int const delta = std::min(-neg->second, pos->second);
				size_t const k = std::get<2>(neg->first);
				size_t const l = std::get<3>(neg->first);
				size_t const kk = std::get<2>(pos->first);
				size_t const ll = std::get<3>(pos->first);
				g2[{k, l, i, j}] -= delta;
				g2[{i, j, k, l}] += delta;
				g2[{i, j, kk, ll}] -= delta;
				g2[{kk, ll, i, j}] += delta;
				g2[{k, l, kk, ll}] += delta;
				g2[{kk, ll, k, l}] -= delta;
			}
		}
	}

	// export
	for (auto const &v : g2)
	{
		if (v.second > 0)
		{
			move(std::get<0>(v.first), std::get<1>(v.first), std::get<2>(v.first), std::get<3>(v.first), v.second);
		}
	}
}

void TreeSolver::update_u(point p0)
{
	std::deque<point> q;
	q.push_back(p0);
	while (!q.empty())
	{
		point const ij = q.front();
		point const pij = parent[ij];
		q.pop_front();

		for (auto kl : next(ij))
		{
			if (!good(kl) || kl == pij)
				continue;
			auto gklij = g(kl, ij);
			//std::cerr << kl << "->" << ij << '(' << gklij.value_or(0) << ')' << std::endl;
			if (gklij)
			{
				u[kl] = u[ij] + sgn(*gklij);
				parent[kl] = ij;
				q.push_back(kl);
			}
		}
	}
}

int TreeSolver::argmin_c(point &ij, point &kl) const
{
	int c_min = std::numeric_limits<int>::max();
	for (size_t i = 0; i < R; ++i)
	{
		for (size_t j = 0; j < C; ++j)
		{
			point const p{i, j};
			for (auto n : next(p))
			{
				if (!good(n))
					continue;
				int const cijkl = 1 - u[p] + u[n];
				if (cijkl < c_min)
				{
					c_min = cijkl;
					ij = p;
					kl = n;
				}
			}
		}
	}
	return c_min;
}

std::vector<point> TreeSolver::find_loop(point p0, point p1) const
{
	matrix_t<point> par(parent);
	par[p0] = null;
	point p = p0;
	for (point pn = parent[p]; pn != null; p = pn, pn = parent[p])
	{
		par[pn] = p;
	}
	std::vector<point> loop;
	for (p = p1; p != null; p = par[p])
	{
		loop.push_back(p);
	}
	assert(loop.back() == p0);
	loop.push_back(p1);
	return loop;
}

bool TreeSolver::update_loop(std::vector<point> loop)
{
	point ij0 = loop[loop.size() - 2];
	point kl0 = loop.back();
	point ij1, kl1;

	//std::cerr << ' ';
	int min_g = std::numeric_limits<int>::max();
	for (size_t n = 1; n < loop.size() - 1; ++n)
	{
		point const &ij = loop[n];
		point const &kl = loop[n - 1];
		auto gijkl = g(ij, kl);
		assert(gijkl);
		//std::cerr << ' ' << gijkl;
		if (*gijkl > 0 && *gijkl < min_g)
		{
			min_g = *gijkl;
			ij1 = ij;
			kl1 = kl;
		}
	}
	if (min_g == std::numeric_limits<int>::max())
		return false;

	//std::cerr << std::endl;
	//std::cerr << "  " << ij1 << '-' << kl1 << '=' << min_g << ';' << ij0 << '-' << kl0 << "=0" << std::endl;
	for (size_t n = 1; n < loop.size(); ++n)
	{
		point const &ij = loop[n - 1];
		point const &kl = loop[n];
		//std::cerr << ij << '-' << kl << std::endl;
		auto gijkl = g(ij, kl);
		g(ij, kl, gijkl.value_or(0) + min_g);
	}
	assert(g(ij0, kl0) == min_g);
	assert(g(ij1, kl1) == 0);
	g(ij1, kl1, boost::none);

	if (parent[ij1] == kl1)
	{
		std::swap(ij0, kl0);
		min_g = -min_g;
	}
	else
	{
		assert(parent[kl1] == ij1);
	}
	u[kl0] = u[ij0] - sgn(min_g);
	parent[kl0] = ij0;
	update_u(kl0);

	assert(parent[ij1] != kl1 && parent[kl1] != ij1);
	return true;
}

boost::optional<int> TreeSolver::g(point p0, point p1) const
{
	assert(std::abs((int)p0.first - (int)p1.first) + std::abs((int)p0.second - (int)p1.second) == 1);
	size_t i = std::min(p0.first, p1.first);
	size_t j = std::min(p0.second, p1.second);
	auto const &v = g_[i][j];
	return p0.first < p1.first ? v.down : p1.first < p0.first ? -v.down : p0.second < p1.second ? v.right : -v.right;
}

void TreeSolver::g(point p0, point p1, boost::optional<int> x)
{
	assert(std::abs((int)p0.first - (int)p1.first) + std::abs((int)p0.second - (int)p1.second) == 1);
	size_t i = std::min(p0.first, p1.first);
	size_t j = std::min(p0.second, p1.second);
	auto &v = g_[i][j];
	if (p0.first < p1.first)
		v.down = x;
	else if (p1.first < p0.first)
		v.down = -x;
	else if (p0.second < p1.second)
		v.right = x;
	else
		v.right = -x;
}

constexpr std::array<point, 4> TreeSolver::next(point p)
{
	return {{{p.first - 1, p.second}, {p.first, p.second - 1}, {p.first + 1, p.second}, {p.first, p.second + 1}}};
}
