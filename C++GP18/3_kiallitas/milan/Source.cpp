#define _CRT_SECURE_NO_WARNINGS

#include <cstring>
#include <algorithm>
#include <iostream>
#include <vector>
#include <array>
#include <random>
#include <unordered_set>


#define TEST_NAME "32"
#define RANDOM_SEED 123456789


using BOARD_TYPE = std::array<std::array<int, 15>, 15>;
using SOLUTION_BOARD_TYPE = std::array<std::array<char, 15>, 15>;


enum DIR
{
	UP = 0,
	DOWN,
	LEFT,
	RIGHT
};

char DIR_TO_LETTER[4] = {'U', 'D', 'L', 'R'};

struct Point
{
	int x;
	int y;
};

struct Tower
{
	Point pos;
	int value;
	bool valid;
};

struct SolutionItem
{
	int towerId;
	DIR dir;
};


// #########################################################################
// #                           H A S H I N G                               #
// #########################################################################
std::default_random_engine randomStateGen(RANDOM_SEED);

struct Hashtable
{
	// [row][column][value]
	unsigned long long table[50][50][10];


	void generateZobristTable()
	{
		std::uniform_int_distribution<unsigned long long> uniform_dist(0, 18446744073709551615llu);

		for (int i = 0; i < 50; ++i)
		{
			for (int j = 0; j < 50; ++j)
			{
				for (int k = 0; k < 10; ++k)
				{
					table[i][j][k] = uniform_dist(randomStateGen);
				}
			}
		}
	}

	unsigned long long getHashValue(int row, int column, int value)
	{
		return table[row][column][value];
	}

	unsigned long long getHashValue(const BOARD_TYPE& board)
	{
		unsigned long long hash = 0;
		for (int i = 0; i < board.size(); ++i)
		{
			for (int j = 0; j < board[0].size(); ++j)
			{
				if (board[i][j] > 0)
				{
					hash ^= table[i][j][board[i][j]];
				}
			}
		}
		return hash;
	}
};

Hashtable hashTable;
std::unordered_set<unsigned long long> hashedStates;	// hash, number of moves


// #########################################################################
// #            P O S T P R O C E S S   S O L U T I O N                    #
// #########################################################################
class SolutionHandler
{
public:

	SOLUTION_BOARD_TYPE board;
	static constexpr char EMPTY = '.';
	static constexpr char TARGET = 'E';
	static constexpr char UP = '^';
	static constexpr char LEFT = '<';
	static constexpr char DOWN = 'V';
	static constexpr char RIGHT = '>';


	int rows;
	int columns;

	SolutionHandler(int r, int c)
	{
		rows = r;
		columns = c;
		for (int i = 0; i < rows; ++i)
		{
			for (int j = 0; j < columns; ++j)
			{
				board[i][j] = EMPTY;
			}
		}
	}

	void setTowers(const std::array<Tower, 100>& towers, int numOfTowers)
	{
		for (int i = 0; i < numOfTowers; ++i)
		{
			const Tower& tower = towers[i];
			board[tower.pos.y][tower.pos.x] = tower.value + 48;
		}
	}

	void setSolution(const std::array<Tower, 100>& towers, int numOfTowers, const std::vector<SolutionItem>& solution, const Point& target)
	{
		setTowers(towers, numOfTowers);

		Point dirs[4];
		dirs[DIR::UP] = {0, -1};
		dirs[DIR::DOWN] = {0, 1};
		dirs[DIR::LEFT] = {-1, 0};
		dirs[DIR::RIGHT] = {1, 0};

		char fillChars[4];
		fillChars[DIR::UP] = UP;
		fillChars[DIR::DOWN] = DOWN;
		fillChars[DIR::LEFT] = LEFT;
		fillChars[DIR::RIGHT] = RIGHT;

		for (const SolutionItem item : solution)
		{
			fill(towers[item.towerId], dirs[item.dir], towers[item.towerId].value, fillChars[item.dir]);
		}

		board[target.y][target.x] = TARGET;
	}

	void fill(const Tower& tower, const Point& dir, int numOfSteps, char fillChar)
	{
		int filled = 0;
		int x = tower.pos.x + dir.x;
		int y = tower.pos.y + dir.y;

		while (filled < tower.value && x >= 0 && y >= 0 && x < columns && y < rows)
		{
			if (board[y][x] == EMPTY)
			{
				board[y][x] = fillChar;
				filled++;
			}

			x += dir.x;
			y += dir.y;
		}
	}

	void printBoard()
	{
		for (int i = 0; i < rows; ++i)
		{
			for (int j = 0; j < columns; ++j)
			{
				std::cout << board[i][j];
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
};


// #########################################################################
// #                           S O L V E R                                 #
// #########################################################################
int rows, columns;
int numOfTowers;
Point target;

int towerCountInRows[20];
int towerCountInColumns[20];


void printBoard(const BOARD_TYPE& board)
{
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < columns; ++j)
		{
			std::cout << board[i][j];
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}


int fillUp(BOARD_TYPE& board, const Tower& tower, std::array<Point, 9>& cells, unsigned long long& hashValue)
{
	int y = tower.pos.y;
	int filled = 0;

	bool foundTower = towerCountInColumns[tower.pos.x] > 0;

	while (y > 0 && filled < tower.value)
	{
		y--;
		if (board[y][tower.pos.x] == 0)
		{
			board[y][tower.pos.x] = 1;
			cells[filled++] = {tower.pos.x, y};
			hashValue ^= hashTable.getHashValue(y, tower.pos.x, 1);

			if (towerCountInRows[y] > 0)
			{
				foundTower = true;
			}
		}
	}

	if (!foundTower)
	{
		return -filled;
	}

	return filled;
}

int fillDown(BOARD_TYPE& board, const Tower& tower, std::array<Point, 9>& cells, unsigned long long& hashValue)
{
	int y = tower.pos.y;
	int filled = 0;

	bool foundTower = towerCountInColumns[tower.pos.x] > 0;

	while (y < rows - 1 && filled < tower.value)
	{
		y++;
		if (board[y][tower.pos.x] == 0)
		{
			board[y][tower.pos.x] = 1;
			cells[filled++] = { tower.pos.x, y };
			hashValue ^= hashTable.getHashValue(y, tower.pos.x, 1);

			if (towerCountInRows[y] > 0)
			{
				foundTower = true;
			}
		}
	}

	if (!foundTower)
	{
		return -filled;
	}

	return filled;
}

int fillLeft(BOARD_TYPE& board, const Tower& tower, std::array<Point, 9>& cells, unsigned long long& hashValue)
{
	int x = tower.pos.x;
	int filled = 0;

	bool foundTower = towerCountInRows[tower.pos.y] > 0;

	while (x > 0 && filled < tower.value)
	{
		x--;
		if (board[tower.pos.y][x] == 0)
		{
			board[tower.pos.y][x] = 1;
			cells[filled++] = { x, tower.pos.y };
			hashValue ^= hashTable.getHashValue(tower.pos.y, x, 1);

			if (towerCountInColumns[x] > 0)
			{
				foundTower = true;
			}
		}
	}

	if (!foundTower)
	{
		return -filled;
	}

	return filled;
}

int fillRight(BOARD_TYPE& board, const Tower& tower, std::array<Point, 9>& cells, unsigned long long& hashValue)
{
	int x = tower.pos.x;
	int filled = 0;

	bool foundTower = towerCountInRows[tower.pos.y] > 0;

	while (x < columns - 1 && filled < tower.value)
	{
		x++;
		if (board[tower.pos.y][x] == 0)
		{
			board[tower.pos.y][x] = 1;
			cells[filled++] = { x, tower.pos.y };
			hashValue ^= hashTable.getHashValue(tower.pos.y, x, 1);

			if (towerCountInColumns[x] > 0)
			{
				foundTower = true;
			}
		}
	}

	if (!foundTower)
	{
		return -filled;
	}

	return filled;
}


void unFill(BOARD_TYPE& board, const std::array<Point, 9>& cells, const Tower& tower, int count)
{
	for (int i = 0; i < count; ++i)
	{
		board[cells[i].y][cells[i].x] = 0;
	}
}



std::vector<SolutionItem> solution;
int totalEvaluations = 0;
int collisionCount = 0;
bool solveRecursive(BOARD_TYPE& board, std::array<Tower, 100>& towers, unsigned long long hashValue, int depth)
{
	// check for solution
	if (board[target.y][target.x] > 0)
	{
		std::cerr << "Solution found!" << std::endl;
		std::cerr << totalEvaluations << std::endl;
		return true;
	}

	// check for visited state
	if (hashedStates.find(hashValue) != hashedStates.end())
	{
		collisionCount++;
		return false;
	}
	else
	{
		hashedStates.insert(hashValue);
	}

	totalEvaluations++;

	if (totalEvaluations % 1000000 == 0)
	{
		//std::cout << totalEvaluations << std::endl;
		std::cerr << totalEvaluations << " " << collisionCount << std::endl;
		//printBoard(board);
	}

	if (depth >= numOfTowers) return false;

	// go through all the towers
	for (int i = 0; i < numOfTowers; ++i)
	{
		Tower& tower = towers[i];

		if (!tower.valid) continue;

		tower.valid = false;

		// bela's rectangle trick (4 areas)
		// if any of the quarter plane is empty, we can skip the corresponding 2 directions
		int topLeftCount = 0;
		int topRightCount = 0;
		int bottomLeftCount = 0;
		int bottomRightCount = 0;

		for (int j = 0; j < numOfTowers; ++j)
		{
			if (!towers[j].valid) continue;

			if (towers[j].pos.x <= tower.pos.x && towers[j].pos.y <= tower.pos.y) topLeftCount++;
			if (towers[j].pos.x <= tower.pos.x && towers[j].pos.y >= tower.pos.y) bottomLeftCount++;
			if (towers[j].pos.x >= tower.pos.x && towers[j].pos.y <= tower.pos.y) topRightCount++;
			if (towers[j].pos.x >= tower.pos.x && towers[j].pos.y >= tower.pos.y) bottomRightCount++;
		}

		if (target.x <= tower.pos.x && target.y <= tower.pos.y) topLeftCount++;
		if (target.x <= tower.pos.x && target.y >= tower.pos.y) bottomLeftCount++;
		if (target.x >= tower.pos.x && target.y <= tower.pos.y) topRightCount++;
		if (target.x >= tower.pos.x && target.y >= tower.pos.y) bottomRightCount++;

		board[tower.pos.y][tower.pos.x] = 1;
		hashValue ^= hashTable.getHashValue(tower.pos.y, tower.pos.x, tower.value + 1);
		hashValue ^= hashTable.getHashValue(tower.pos.y, tower.pos.x, 1);

		towerCountInColumns[tower.pos.x]--;
		towerCountInRows[tower.pos.y]--;

		// fill board to all the 4 directions
		std::array<Point, 9> usedCells;
		

		unsigned long long originalHashValue = hashValue;

		if (topLeftCount > 0 && topRightCount > 0)
		{
			int filledCount = fillUp(board, tower, usedCells, hashValue);
			if (filledCount > 0 || board[target.y][target.x] > 0)
			{
				bool res = solveRecursive(board, towers, hashValue, depth + 1);
				if (res)
				{
					solution.push_back({ i, DIR::UP });
					return true;
				}
				unFill(board, usedCells, tower, filledCount);
			}
			else
			{
				unFill(board, usedCells, tower, -filledCount);
			}
			hashValue = originalHashValue;
		}

		if (bottomLeftCount > 0 && bottomRightCount > 0)
		{
			int filledCount = fillDown(board, tower, usedCells, hashValue);
			if (filledCount > 0 || board[target.y][target.x] > 0)
			{
				bool res = solveRecursive(board, towers, hashValue, depth + 1);
				if (res)
				{
					solution.push_back({ i, DIR::DOWN });
					return true;
				}
				unFill(board, usedCells, tower, filledCount);
			}
			else
			{
				unFill(board, usedCells, tower, -filledCount);
			}
			hashValue = originalHashValue;
		}
		
		if (topLeftCount > 0 && bottomLeftCount > 0)
		{
			int filledCount = fillLeft(board, tower, usedCells, hashValue);
			if (filledCount > 0 || board[target.y][target.x] > 0)
			{
				bool res = solveRecursive(board, towers, hashValue, depth + 1);
				if (res)
				{
					solution.push_back({ i, DIR::LEFT });
					return true;
				}
				unFill(board, usedCells, tower, filledCount);
			}
			else
			{
				unFill(board, usedCells, tower, -filledCount);
			}
			hashValue = originalHashValue;
		}
		
		if (topRightCount > 0 && bottomRightCount > 0)
		{
			int filledCount = fillRight(board, tower, usedCells, hashValue);
			if (filledCount > 0 || board[target.y][target.x] > 0)
			{
				bool res = solveRecursive(board, towers, hashValue, depth + 1);
				if (res)
				{
					solution.push_back({ i, DIR::RIGHT });
					return true;
				}
				unFill(board, usedCells, tower, filledCount);
			}
			else
			{
				unFill(board, usedCells, tower, -filledCount);
			}
			hashValue = originalHashValue;
		}
		

		board[tower.pos.y][tower.pos.x] = tower.value + 1;
		hashValue ^= hashTable.getHashValue(tower.pos.y, tower.pos.x, 1);
		hashValue ^= hashTable.getHashValue(tower.pos.y, tower.pos.x, tower.value + 1);

		towerCountInColumns[tower.pos.x]++;
		towerCountInRows[tower.pos.y]++;

		tower.valid = true;
	}

	return false;
}


void solve(BOARD_TYPE& board, std::array<Tower, 100>& towers)
{
	totalEvaluations = 0;
	unsigned long long hashValue = hashTable.getHashValue(board);
	solveRecursive(board, towers, hashValue, 0);
}



BOARD_TYPE originalBoard;
BOARD_TYPE board;
std::array<Tower, 100> towers;


void readLevel()
{
	std::cin >> rows >> columns;

	numOfTowers = 0;
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < columns; ++j)
		{
			char c;
			std::cin >> c;

			if (c == '.')
			{
				board[i][j] = 0;
			}
			else if (c == 'E')
			{
				board[i][j] = 0;
				target = {j, i};
			}
			else
			{
				board[i][j] = c - 48 + 1;
				towers[numOfTowers++] = {{j, i}, c - 48, true};
			}
		}
	}
}


int main(int argc, char **argv)
{
	std::ios_base::sync_with_stdio(false);

	char test_name[256] = "Wrong command line parameters!";
	char inputFilePath[256], outputFilePath[256];
	inputFilePath[0] = 0;
	outputFilePath[0] = 0;

	freopen("err.txt", "wb", stderr);

	// load from input file
	if (1 < argc)
	{
		strcpy(test_name, argv[1]);
		sprintf(inputFilePath, "%s", test_name);
	}

	std::cerr << "Test case \"" << test_name << "\" started!" << std::endl;

	if (2 < argc)
	{
		strcpy(test_name, argv[2]);
		sprintf(outputFilePath, "%s", test_name);
	}

	freopen(inputFilePath, "r", stdin);
	freopen(outputFilePath, "w", stdout);
	//freopen("out.txt", "w", stdout);

	hashTable.generateZobristTable();

	readLevel();
	originalBoard = board;

	for (int i = 0; i < 20; ++i)
	{
		towerCountInRows[i] = 0;
		towerCountInColumns[i] = 0;
	}

	for (int i = 0; i < numOfTowers; ++i)
	{
		const Tower& tower = towers[i];

		towerCountInRows[tower.pos.y]++;
		towerCountInColumns[tower.pos.x]++;
	}

	solve(board, towers);

	std::reverse(solution.begin(), solution.end());

	std::cout << solution.size() << std::endl;
	for (const SolutionItem& item : solution)
	{
		std::cout << towers[item.towerId].pos.x << " " << towers[item.towerId].pos.y << " " << DIR_TO_LETTER[item.dir] << std::endl;
	}

	// simulate solution 1 by 1
	SolutionHandler solutionHandler(rows, columns);
	solutionHandler.setSolution(towers, numOfTowers, solution, target);
	solutionHandler.printBoard();

	return 0;
}