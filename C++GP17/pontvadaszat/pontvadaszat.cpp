#include <algorithm>
#include <array>
#include <iomanip>
#include <iostream>
#include <queue>
#include <vector>

using namespace std;

struct die
{
	int score;
	int up, right; // next state in direction
};

array<die, 24> const states{ { // index: up, right
	{ 1,  6, 11 }, // 00: 2, 3
	{ 1, 14,  7 }, // 01: 4, 2
	{ 1, 18, 15 }, // 02: 5, 4
	{ 1, 10, 19 }, // 03: 3, 5
	{ 2,  3, 12 }, // 04: 1, 4
	{ 2,  9,  3 }, // 05: 3, 1
	{ 2, 22, 10 }, // 06: 6, 3
	{ 2, 15, 23 }, // 07: 4, 6
	{ 3,  1,  4 }, // 08: 1, 2
	{ 3, 17,  2 }, // 09: 5, 1
	{ 3, 23, 18 }, // 10: 6, 5
	{ 3,  7, 20 }, // 11: 2, 6
	{ 4,  3, 16 }, // 12: 1, 5
	{ 4,  5,  0 }, // 13: 2, 1
	{ 4, 21,  6 }, // 14: 6, 2
	{ 4, 19, 22 }, // 15: 5, 6
	{ 5,  0,  8 }, // 16: 1, 3
	{ 5, 13,  1 }, // 17: 4, 1
	{ 5, 20, 14 }, // 18: 6, 4
	{ 5, 11, 21 }, // 19: 3, 6
	{ 6,  4, 13 }, // 20: 2, 4
	{ 6,  8,  5 }, // 21: 3, 2
	{ 6, 16,  9 }, // 22: 5, 3
	{ 6, 12, 17 }, // 23: 4, 5
} };

struct node
{
	int score;
	int x, y;
	int state;
	node(int score_, int x_, int y_, int state_) : score(score_), x(x_), y(y_), state(state_) {}
	int prio() const { return score - 7*(x+y-2); }
	bool operator<(node const &o) const { return prio() < o.prio(); }
};

int main()
{
	const int N = 20;
	vector<vector<vector<int>>> t(N, vector<vector<int>>(N, vector<int>(24)));
	priority_queue<node> q;
	for (int i = 0; i < (int)states.size(); i++)
	{
		q.push(node(states[i].score, 0, 0, i));
	}

	while (!q.empty())
	{
		node n = q.top();
		q.pop();
		if (n.score <= t[n.y][n.x][n.state])
			continue;

		t[n.y][n.x][n.state] = n.score;
		int up = states[n.state].up;
		int right = states[n.state].right;
		if (n.y+1 < N)
			q.push(node(n.score + states[up].score, n.x, n.y+1, up));
		if (n.x+1 < N)
			q.push(node(n.score + states[right].score, n.x+1, n.y, right));
	}

	for (auto const &tr : t)
	{
		for (auto const &tv : tr)
		{
			cout << setw(3) << *max_element(tv.begin(), tv.end()) << ' ';
		}
		cout << '\n';
	}
}
