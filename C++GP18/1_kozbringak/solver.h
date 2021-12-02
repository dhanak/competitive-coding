#pragma once

#include "matrix.h"

class Solver
{
  protected:
	matrix M;
	size_t const R;
	size_t const C;
	size_t cost = 0;

	void move(size_t r0, size_t c0, size_t r, size_t c, int v);

  public:
	Solver(matrix m) : M(std::move(m)), R(M.height()), C(M.width()) {}
	virtual ~Solver() {}
	virtual void solve() = 0;
	bool is_solved() const;
	void print() const;
	size_t get_cost() const { return cost; }
};
