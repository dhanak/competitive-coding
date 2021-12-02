#include <algorithm>
#include <cassert>

#include "naivesolver.h"

void NaiveSolver::solve()
{
	if (bSequential)
	{
		for (int r = 0; r < (int)R; ++r)
		{
			for (int c = 0; c < (int)C; ++c)
			{
				if (M[r][c] != 0)
				{
					dump(r, c);
				}
			}
		}
	}
	else
	{
		while (true)
		{
			std::pair<int, int> p = find_largest();
			if (M[p.first][p.second] == 0)
				break;
			dump(p.first, p.second);
		}
	}
}

void NaiveSolver::dump(int r0, int c0)
{
	int &v0 = M[r0][c0];

	if (bLinear)
	{
		for (int r = bSequential ? r0 : 0; v0 != 0 && r < (int)R; ++r)
		{
			for (int c = 0; v0 != 0 && c < (int)C; ++c)
			{
				try_move(r0, c0, r, c);
			}
		}
	}
	else
	{
		int dr = 1;
		int dc = 1;
		int r = r0;
		int c = c0 + 1;
		while (v0 != 0)
		{ // diamond shape exploration
			if (r >= 0 && r < (int)R && c >= 0 && c < (int)C)
			{
				try_move(r0, c0, r, c);
			}
			if (c == c0)
			{ // top or bottom node of diamond shape, switch direction
				dr = -dr;
			}
			else if (r == r0)
			{ // left or right node of diamond shape, switch direction
				dc = -dc;
			}
			r += dr;
			c += dc;
			if (r == r0 && (bSequential || dc == 1))
			{ // end of (half) diamond
				c = c0 + dc * (c - c0) + 1;
				dr = 1;
				dc = 1;
			}
		}
	}

	assert(v0 == 0);
}

void NaiveSolver::try_move(int r0, int c0, int r, int c)
{
	int v0 = M[r0][c0];
	int v = M[r][c];
	if (v0 * v < 0)
	{
		int dv = v0 > 0 ? std::min(v0, -v) : std::max(v0, -v);
		move(r0, c0, r, c, dv);
	}
}

std::pair<int, int> NaiveSolver::find_largest() const
{
	std::pair<int, int> p{0, 0};
	int max = std::abs(M[0][0]);
	for (int r = 0; r < (int)R; ++r)
	{
		for (int c = 0; c < (int)C; ++c)
		{
			int const v = std::abs(M[r][c]);
			if (max < v)
			{
				max = v;
				p = {r, c};
			}
		}
	}
	return p;
}
