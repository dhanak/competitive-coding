#include <cassert>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#include "common.h"
#include "redirector.h"
#include "naivesolver.h"
#include "mincostflowsolver.h"
#include "treesolver.h"

std::chrono::high_resolution_clock::time_point const g_start = std::chrono::high_resolution_clock::now();

int checkarg(std::string arg, std::vector<std::string> chks)
{
	for (int i = 0; i < static_cast<int>(chks.size()); ++i)
	{
		if (chks[i].compare(0, arg.size(), arg) == 0)
			return i;
	}
	return -1;
}

std::unique_ptr<Solver> make_solver(int solver_type, matrix m)
{
	switch (solver_type)
	{
	case 0:
		return std::make_unique<NaiveSolver>(std::move(m));
	case 1:
		return std::make_unique<MinCostFlowSolver>(std::move(m));
	case 2:
		return std::make_unique<TreeSolver>(std::move(m));
	}
	return nullptr;
}

int main(int argc, char const *argv[])
{
	int solver_type = checkarg(argv[1], {"naive", "graph", "tree"});
	if (solver_type == -1)
	{
		solver_type = 2;
	}
	else
	{
		argc--;
		argv++;
	}

	std::cerr << "Reading puzzle..." << std::endl;
	Redirector redir(argv + 1, argv + argc);
	size_t R, C;
	std::cin >> R >> C;

	matrix M(R, C);
	for (size_t r = 0; r < R; ++r)
	{
		for (size_t c = 0; c < C; ++c)
		{
			std::cin >> M[r][c];
		}
	}
	assert(0 == std::accumulate(M.begin(), M.end(), 0, [](int a, std::vector<int> const &v) {
			   return std::accumulate(v.begin(), v.end(), a);
		   }));

	std::cerr << "Solving..." << std::endl;
	std::unique_ptr<Solver> solver = make_solver(solver_type, std::move(M));
	solver->solve();
	std::cout << 0 << std::endl;

	std::cerr << "Checking solution..." << std::endl;
	assert(solver->is_solved());

	std::cerr << solver->get_cost() << std::endl;
}
