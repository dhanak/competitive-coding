#include <algorithm>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

struct point
{
	int x, y;
	bool operator==(point const &o) const { return x == o.x && y == o.y; }
	bool operator<(point const &o) const { return make_pair(y, x) < make_pair(o.y, o.x); }
};

struct car : public point
{
	int G, W, E;
	car() : point{}, G(0), W(0), E(0) {}
};

struct workplace : public point
{
	workplace(int x_, int y_) : point({ x_, y_ }) {}
};

struct garage : public point
{
	deque<int> cars;
	vector<int> D;
	int weight;
	size_t weight_age;
	int t;
	garage(int x_, int y_) : point({ x_, y_ }), weight(0), weight_age(0), t(-1) {}
};

istream &operator>>(istream &s, car &c)
{
	return s >> c.G >> c.W >> c.E;
}

namespace std
{
	template <class T1, class T2>
	struct hash<pair<T1,T2>>
	{
		size_t operator()(pair<T1,T2> const &p) const { return hash<T1>()(p.first) ^ (hash<T2>()(p.second) << 1); }
	};
	
	template <>
	struct hash<point>
	{
		size_t operator()(point const &p) const { return hash<pair<int,int>>()({p.x, p.y}); }
	};
}

enum dir { RIGHT, LEFT, DOWN, UP, WAIT };
char dirs[] = { '>', '<', 'v', '^', '*' };

vector<string> board;
vector<garage> garages;
vector<int> garage_prios;
vector<workplace> workplaces;
vector<car> cars;
unordered_set<pair<int,point>> transients; // time,point
multimap<int, pair<int,dir>> moves;		   // time -> car,direction

point move_dir(point const &p, int d)
{
	switch (d)
	{
	case RIGHT: return { p.x+1, p.y };
	case LEFT: return { p.x-1, p.y };
	case DOWN: return { p.x, p.y+1 };
	case UP: return { p.x, p.y-1 };
	default: return p;
	}
}

bool free(point const &p)
{
	int const N = board.size();
	if (p.x < 0 || p.x >= N || p.y < 0 || p.y >= N)
		return false;
	char b = board[p.y][p.x];
	return b == '*' || b == 'W';
}

bool free(int t, point const &p, int c, bool &transient)
{
	int const N = board.size();
	if (p.x < 0 || p.x >= N || p.y < 0 || p.y >= N)
		return false;
	if (p == workplaces[cars[c].W])
		return true;
	if (board[p.y][p.x] != '*')
		return false;
	if (transients.count(make_pair(t, p)) == 0)
		return true;
	
	transient = true;
	return false;
}

void calculate_distances()
{
	for (auto &g : garages)
	{
		queue<point> q;
		unordered_set<point> visited;

		int d = 0;
		int k = 1, kn = 0;
		int wn = workplaces.size();
		q.push(g);
		g.D.resize(workplaces.size());
		unordered_set<point> wsp;
		while (wn)
		{
			point const p = q.front();
			if (!visited.count(p))
			{
				visited.insert(p);
				auto const wi = lower_bound(workplaces.begin(), workplaces.end(), p);
				if (wi != workplaces.end() && *wi == p)
				{
					g.D[wi-workplaces.begin()] = d;
					wn--;
				}
				for (int d = 0; d < WAIT; d++)
				{
					point const pn = move_dir(p, d);
					if (free(pn))
					{
						q.push(pn);
						kn++;
					}
				}
			}
			q.pop();
			if (--k == 0)
			{
				k = kn;
				kn = 0;
				d++;
			}
		}
		
		//cerr << g.x << ',' << g.y << ": ";
		//for (int d : g.D) cerr << d << ' ';
		//cerr << '\n';
	}
}

void parse()
{
	int N; // size of board
	cin >> N;

	board.resize(N);
	garages.reserve(9);
	workplaces.reserve(9);
	
	for (int y = 0; y < N; y++)
	{
		string line;
		cin >> line;
		board[y] = line;
		
		for (int x = 0; x < N; x++)
		{
			switch (line[x])
			{
			case 'G': garages.emplace_back(x, y); break;
			case 'W': workplaces.emplace_back(x, y); break;
			default: break;
			}
		}
	}
	garage_prios.resize(garages.size());
	for (size_t gi = garages.size(); gi-- > 0; )
	{
		garage_prios[gi] = gi;
	}
	
	int K; // number of cars
	cin >> K;

	cars.resize(K);

	for (int ci = 0; ci < K; ci++)
	{
		car &c = cars[ci];
		cin >> c;
		(point&)c = garages[c.G];
		garages[c.G].cars.push_back(ci);
	}
}

enum { EMISSION, EMISSION_X_SIZE, EMISSION_SUM, DISTANCE, DISTANCE_X_EMISSION, DEEP }
	const strategy = DEEP;

void update_weights()
{
	for (auto &g : garages)
	{
		if (g.weight_age == g.cars.size())
			continue;
		g.weight_age = g.cars.size();
		car const &c = cars[g.cars.empty() ? 0 : g.cars.front()];
		switch (strategy)
		{
		case EMISSION:
			g.weight = g.cars.empty() ? 0 : c.E;
			break;
		case EMISSION_X_SIZE:
			g.weight = c.E * g.cars.size();
			break;
		case EMISSION_SUM:
			g.weight = 0;
			for (int ci : g.cars)
			{
				g.weight += cars[ci].E;
			}
			break;
		case DISTANCE:
			g.weight = g.cars.empty() ? 0 : g.D[c.W];
			break;
		case DISTANCE_X_EMISSION:
			g.weight = g.cars.empty() ? 0 : g.D[c.W] * c.E;
			break;
		case DEEP:
		{
			g.weight = 0;
			int t = 0;
			for (int ci : g.cars)
			{
				car &c = cars[ci];
				int w = (t + g.D[c.W]) * c.E;
				g.weight += w;
				t++;
			}
		}
		break;
		}
	}
}

int const MIN_DELTA_T = 12;
bool route(int ci, int t0)
{
	queue<point> q;
	unordered_map<point,int> visited;
	unordered_map<pair<int,point>, int> trail;

	int t = t0;
	int k = 1, kn = 0;
	q.push(cars[ci]);
	auto const &w = workplaces[cars[ci].W];
	while (!q.empty())
	{
		point const from = q.front();
		if (from == w)
			break;
		
		auto it = visited.find(from);
		if (it == visited.end() || it->second + MIN_DELTA_T <= t || trail[make_pair(t, from)] == WAIT)
		{
			visited[from] = t;
			bool tr = false;
			for (int d = 0; d <= WAIT; d++)
			{
				if (d == WAIT && (!tr || t == t0))
					break;
			
				point const to = move_dir(from, d);
				if (free(t+1, to, ci, tr))
				{
					trail[make_pair(t+1, to)] = d;
					q.push(to);
					kn++;
				}
			}
		}

		q.pop();
		if (--k == 0)
		{
			k = kn;
			kn = 0;
			t++;
		}
	}
	if (q.empty())
		return false;

	for (point p = w; t != t0; t--)
	{
		if (!(p == w))
		{
			transients.emplace(t, p);
		}
		int const d = trail[make_pair(t, p)];
		if (d != WAIT)
		{
			p = move_dir(p, d^1);
			moves.emplace(t-1, make_pair(ci, (dir)d));
		}
	}
	return true;
}

void solve()
{
	int K = cars.size();
	for (int t = 0; K; t++)
	{
		update_weights();
		sort(garage_prios.begin(), garage_prios.end(), [](int a, int b){
				return garages[a].weight > garages[b].weight;
			});

		for (int gi : garage_prios)
		{
			garage &g = garages[gi];
			if (g.cars.empty() || g.t >= t)
				continue;
			
			int const ci = g.cars.front();
			for (g.t = t; !route(ci, g.t); g.t++);
			g.cars.pop_front();
			K--;
		}
	}
}

void print()
{
	if (moves.empty())
		return;
	
	int const t_max = moves.rbegin()->first;
	for (int t = 0; t <= t_max; t++)
	{
		auto r = moves.equal_range(t);
		cout << distance(r.first, r.second) << '\n';
		for (; r.first != r.second; ++r.first)
			cout << r.first->second.first << ' ' << dirs[r.first->second.second] << '\n';
	}
	cout << "0\n";
}

void dump()
{
	cerr << garages.size() << " garages: ";
	for (auto const &g : garages)
	{
		cerr << g.x << ',' << g.y << '-';
		for (auto ci : g.cars)
			cerr << ci << ',';
		cerr << "   ";
	}
	cerr << '\n';
	
	cerr << workplaces.size() << " workplaces: ";
	for (auto const &w : workplaces)
		cerr << w.x << ',' << w.y << ' ';
	cerr << '\n';
	
	cerr << cars.size() << " cars: ";
	for (auto const &c : cars)
		cerr << c.x << ',' << c.y << " to " << c.W << ':' << c.E << ' ';
	cerr << '\n';
}

int main()
{
	parse();
	if (strategy >= DISTANCE)
	{
		calculate_distances();
	}
	solve();
	print();
	return 0;
}
