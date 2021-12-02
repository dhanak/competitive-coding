#pragma once

#include <array>
#include <vector>
#include <boost/optional.hpp>

#include "solver.h"

// https://bidal.sfsu.edu/~kazokada/research/okada_ieeepami06_emdl1.pdf

class TreeSolver : public Solver
{
  public:
	TreeSolver(matrix M);
	void solve() override;

  private:
	void build_initial_tree();
	void optimize();
	void finalize();

	void update_u(point p0);
	int argmin_c(point &ij, point &kl) const;
	std::vector<point> find_loop(point p0, point p1) const;
	bool update_loop(std::vector<point> loop);

	boost::optional<int> g(point p0, point p1) const;
	void g(point p0, point p1, boost::optional<int> v);
	bool good(point p) const { return p.first < R && p.second < C; }

	struct G
	{
		boost::optional<int> down, right;
	};
	matrix_t<G> g_;
	matrix u;
	matrix_t<point> parent;
	point const null;

	static constexpr std::array<point, 4>
	next(point p);
};
