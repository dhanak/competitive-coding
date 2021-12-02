#pragma once

#include <utility>

#include "solver.h"

class NaiveSolver : public Solver
{
  private:
	static const bool bSequential = true;
	static const bool bLinear = false;

	void dump(int r, int c);
	void try_move(int r0, int c0, int r, int c);
	std::pair<int, int> find_largest() const;

  public:
	NaiveSolver(matrix M) : Solver(M) {}
	void solve() override;
};
