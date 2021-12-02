#include <algorithm>
#include <cassert>
#include <cstdio>
#include <unordered_set>
#include <vector>

using namespace std;

size_t K, N;
vector<char> lab;
unordered_set<size_t> future;

enum DIRECTION
{
	UP_LEFT,
	UP,
	UP_RIGHT,
	DOWN_LEFT,
	DOWN,
	DOWN_RIGHT,

	DIRECTION_COUNT
};

inline size_t step(size_t i, DIRECTION d)
{
	size_t const m = i % (2*N);
	switch (d)
	{
	case UP_LEFT:    return m==0     ? -1 : m<N ? i-N-1 : i-N;
	case UP:         return i-2*N;
	case UP_RIGHT:   return m==2*N-1 ? -1 : m<N ? i-N   : i-N+1;
	case DOWN_LEFT:  return m==0     ? -1 : m<N ? i+N-1 : i+N;
	case DOWN:       return i+2*N;
	case DOWN_RIGHT: return m==2*N-1 ? -1 : m<N ? i+N   : i+N+1;
	default: assert(false); return -1;
	}
}

void walk(size_t i, DIRECTION d)
{
	for (; i < lab.size() && lab[i] != 'W'; i = step(i, d))
	{
		future.insert(i);
		if (lab[i] == 'M')
			break;
	}
}

int main()
{
	unordered_set<size_t> present;

	bool ok = scanf("%zu %zu", &K, &N) == 2;
	assert(ok);
	
	size_t const S = K*2*N;
	lab.reserve(S);
	present.reserve(S);
	future.reserve(S);
	
	int c;
	while ((c = getchar()) != EOF)
	{
		if (c == 'W' || c == 'C' || c == 'M')
		{
			lab.push_back(c);
		}
	}
	assert(lab.size() == S);

	// initialize
	for (size_t i = 0; i < K; i++)
	{
		walk(i*2*N, UP_RIGHT);
		walk(i*2*N, DOWN_RIGHT);
		walk((i+1)*2*N-1, UP_LEFT);
		walk((i+1)*2*N-1, DOWN_LEFT);
	}
	for (size_t i = 1; i < 2*N-1; i++)
	{
		walk(i, DOWN);
		walk(i+(K-1)*2*N, UP);
		if (i < N)
		{
			walk(i, DOWN_LEFT);
			walk(i, DOWN_RIGHT);
		}
		else
		{
			walk(i+(K-1)*2*N, UP_LEFT);
			walk(i+(K-1)*2*N, UP_RIGHT);
		}
	}

	// walk
	size_t stepcnt = 0;
	while (!future.empty())
	{
		stepcnt++;
		present.clear();
		present.swap(future);
		for (size_t i : present)
		{
			lab[i] = 'W';
		}
		for (size_t i : present)
		{
			for (int d = 0; d < DIRECTION_COUNT; ++d)
			{
				walk(step(i, DIRECTION(d)), DIRECTION(d));
			}
		}
	}

	// print result
	printf("%zu %zu\n", present.size(), stepcnt);
	vector<size_t> present_sorted(present.begin(), present.end());
	sort(present_sorted.begin(), present_sorted.end());
	for (size_t i : present_sorted)
	{
		printf("%zu %zu\n", i / (2*N) + 1, i % (2*N) + 1);
	}
	
	return 0;
}
