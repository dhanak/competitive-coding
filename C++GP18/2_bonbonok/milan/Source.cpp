#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <array>
#include <string>
#include <sstream>
#include <algorithm>
#include <random>
#include <unordered_set>

#ifdef _MSC_VER
#	include <ctime>
#else
#	include <sys/time.h>
#endif


#define BOARD_ROWS 30
#define BOARD_COLUMNS 30
#define RANDOM_SEED 123456789

using BOARD_TYPE = std::array<std::array<int, BOARD_COLUMNS>, BOARD_ROWS>;

int MODULO;
int ROWS;
int COLUMNS;
int NUM_OF_PIECES;


struct Point
{
	int x = -1;
	int y = -1;
	int value = -1;
};


struct Piece
{
	int id;

	int width;
	int height;

	std::vector<Point> points;
};


struct State
{
	BOARD_TYPE board;
	int score;


	void setBoard(const BOARD_TYPE& b)
	{
		board = b;
		score = getScore();
	}

	void reset()
	{
		for (int i = 0; i < ROWS; ++i)
		{
			for (int j = 0; j < COLUMNS; j++)
			{
				board[i][j] = 0;
			}
		}
		score = getScore();
	}

	bool isFullBoardEmpty() const
	{
		for (int i = 0; i < ROWS; ++i)
		{
			for (int j = 0; j < COLUMNS; j++)
			{
				if (board[i][j] > 0)
				{
					return false;
				}
			}
		}

		return true;
	}

	void addPiece(const Piece& piece, int x, int y)
	{
		for (const Point& point : piece.points)
		{
			board[y + point.y][x + point.x] += point.value;
			score += point.value;

			if (board[y + point.y][x + point.x] >= MODULO)
			{
				board[y + point.y][x + point.x] -= MODULO;
				score -= MODULO;
			}
		}
	}

	void removePiece(const Piece& piece, int x, int y)
	{
		for (const Point& point : piece.points)
		{
			board[y + point.y][x + point.x] -= point.value;
			score -= point.value;

			if (board[y + point.y][x + point.x] < 0)
			{
				board[y + point.y][x + point.x] += MODULO;
				score += MODULO;
			}
		}
	}

	int getScore()
	{
		int score = 0;
		for (int i = 0; i < ROWS; ++i)
		{
			for (int j = 0; j < COLUMNS; ++j)
			{
				score += board[i][j];
			}
		}
		return score;
	}

	void print() const
	{
		for (int i = 0; i < ROWS; ++i)
		{
			for (int j = 0; j < COLUMNS; j++)
			{
				std::cerr << board[i][j];
			}
			std::cerr << std::endl;
		}

		std::cerr << std::endl;
	}
};


// #########################################################################
// #                              T I M E                                  #
// #########################################################################
class TimeLimit
{
public:

	double t0;
	double dt_limit;


	TimeLimit(double limit)
	{
		t0 = time();
		dt_limit = limit;
	}

	double time() const
	{
#ifndef _MSC_VER
		timeval tv;
		gettimeofday(&tv, 0);
		return tv.tv_sec + 1e-6*tv.tv_usec;
#else
		return double(clock()) / CLOCKS_PER_SEC;
#endif
	}

	double dt() const
	{
		return time() - t0;
	}

	bool go() const
	{
		return dt() < dt_limit;
	}

	double limit() const
	{
		return dt_limit;
	}

	double left() const
	{
		return limit() - dt();
	}
};


// #########################################################################
// #                             R A N D O M                               #
// #########################################################################
struct XorShiftRand
{
	unsigned int x, y, z, w;

	XorShiftRand(int seed)
	{
		init(seed);
	}

	void init(int seed)
	{
		x = seed;
		y = 362436069;
		z = 521288629;
		w = 88675123;
	}

	unsigned int next_int(void)
	{
		unsigned int t = x ^ (x << 11);
		x = y;
		y = z;
		z = w;
		return (w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)));// & 0x7FFFFFFF;
	}

	template<class Iter>
	void random_shuffle(Iter beg, Iter end)
	{
		int n = end - beg;
		for (int i = 0; i + 1<n; ++i)
		{
			int j = i + next_int() % (n - i);
			std::swap(*(beg + i), *(beg + j));
		}
	}
};
XorShiftRand xsr(RANDOM_SEED);

#define srand(seed) xsr.init(seed)
#define rand xsr.next_int

double frand()
{
	return (rand() + 0.5) * (1.0 / 4294967296.0);
}


// #########################################################################
// #              S I M U L A T E D   A N N E A L I N G                    #
// #########################################################################
struct OptManagerSA
{
	double q0, q, dq;
	int step;
	double scurr;
	double tc;
	TimeLimit tl;


	OptManagerSA(double q1, double q2, double time_left)
		: tl(time_left)
	{
		step = 0;
		q = q0 = q1;
		dq = q2 / q0;
		tc = tl.dt();
	}

	bool accept(double newscore)
	{
		if (newscore <= scurr + 1E-9)
		{
			return true;
		}

		if ((step & 0xFF) == 0)
		{
			tc = tl.dt();
			double t = tc / (tl.limit());
			q = q0 * pow(dq, t);
		}

		double x = (scurr - newscore) / q;
		double y;
		if (-0.1 < x)
		{
			y = 1 + x;
		}
		else
		{
			y = exp(x);
		}
			
		bool ok = (frand() < y);
		return ok;
	}

	void next_step(double newscore)
	{
		scurr = newscore;
		++step;
	}

	bool go()
	{
		//if((step&0xFF) == 0)
		return tl.go();
		//else
		//	return true;
	}

	double timeLeft()
	{
		return tl.left();
	}
};


// #########################################################################
// #                     I N P U T   /   O U T P U T                       #
// #########################################################################
class Parser
{
public:

	Parser() {}
	~Parser() {}


	BOARD_TYPE lastParsedBoard;
	std::vector<Piece> lastParsedPieces;


	bool parseBoardAndPieces()
	{
		lastParsedBoard = BOARD_TYPE();
		lastParsedPieces.clear();

		// parse board
		std::cin >> ROWS >> COLUMNS >> MODULO;

		if (BOARD_ROWS < ROWS)
		{
			//std::cerr << "Row size is too big to represent with current board!" << std::endl;
			return false;
		}

		if (BOARD_COLUMNS < COLUMNS)
		{
			//std::cerr << "Column size is too big to represent with current board!" << std::endl;
			return false;
		}

		for (int i = 0; i < ROWS; ++i)
		{
			for (int j = 0; j < COLUMNS; ++j)
			{
				char c;
				std::cin >> c;
				lastParsedBoard[i][j] = c - 48;
			}
		}

		// parse pieces
		std::cin >> NUM_OF_PIECES;

		for (int p = 0; p < NUM_OF_PIECES; ++p)
		{
			int r, c;
			std::cin >> r >> c;

			Piece piece;
			piece.id = p;
			piece.height = r;
			piece.width = c;

			for (int i = 0; i < r; ++i)
			{
				for (int j = 0; j < c; ++j)
				{
					char c;
					std::cin >> c;

					// not 0
					if (c > 48)
					{
						piece.points.push_back({ j, i, c - 48 });
					}
				}
			}

			lastParsedPieces.push_back(piece);
		}

		return true;
	}

	void writeSolution(const std::vector<Point>& solution)
	{
		for (int i = 0; i < lastParsedPieces.size(); ++i)
		{
			const Point& move = solution[i];
			std::cout << move.y+1 << " " << move.x+1 << std::endl;
		}
	}
};


// #########################################################################
// #                            S O L V E R                                #
// #########################################################################
std::vector<Point> solution;
void solve(State& board, const std::vector<Piece>& pieces)
{
	std::vector<Point> piecePositions;

	for (const auto& piece : pieces)
	{
		board.addPiece(piece, 0, 0);
		piecePositions.push_back({ 0, 0, 0 });
	}

	solution = piecePositions;
	int bestScore = board.score;

	int numOfIterations = 0;

	for (int k = 0; k < 1; ++k)
	{
		OptManagerSA sa(15, 1, 5.0);
		while (sa.go())
		{
			sa.next_step(board.score);
			numOfIterations++;

			int randPieceIndex = rand() % NUM_OF_PIECES;
			const Piece& piece = pieces[randPieceIndex];

			board.removePiece(piece, piecePositions[randPieceIndex].x, piecePositions[randPieceIndex].y);

			// try all possible positions (its bad, random choice works better)
			int pieceBestScore = 1e9;
			int pieceBestX = -1;
			int pieceBestY = -1;

			for (int i = 0; i < 10; ++i)
			{
				int randomPositionX = rand() % (COLUMNS - piece.width + 1);
				int randomPositionY = rand() % (ROWS - piece.height + 1);
				board.addPiece(piece, randomPositionX, randomPositionY);

				if (board.score < pieceBestScore)
					{
						pieceBestScore = board.score;
						pieceBestX = randomPositionX;
						pieceBestY = randomPositionY;
					}

				board.removePiece(piece, randomPositionX, randomPositionY);
			}

			board.addPiece(piece, pieceBestX, pieceBestY);

			if (sa.accept(board.score))
			{
				piecePositions[randPieceIndex] = { pieceBestX, pieceBestY, 0 };

				if (board.score < bestScore)
				{
					bestScore = board.score;
					solution = piecePositions;
					//std::cerr << numOfIterations << " " << bestScore << std::endl;
				}
			}
			else
			{
				// revert to original position
				board.removePiece(piece, pieceBestX, pieceBestY);
				board.addPiece(piece, piecePositions[randPieceIndex].x, piecePositions[randPieceIndex].y);
			}
		}

		// place all items at random positions
		for (int i = 0; i < NUM_OF_PIECES; ++i)
		{
			board.removePiece(pieces[i], piecePositions[i].x, piecePositions[i].y);

			int randomPositionX = rand() % (COLUMNS - pieces[i].width + 1);
			int randomPositionY = rand() % (ROWS - pieces[i].height + 1);
			board.addPiece(pieces[i], randomPositionX, randomPositionY);
		}

		bestScore = board.score;
	}
}

Parser parser;
State mainState;


int main()
{
	std::ios_base::sync_with_stdio(false);

	parser.parseBoardAndPieces();

	BOARD_TYPE board = parser.lastParsedBoard;
	mainState.setBoard(board);

	std::vector<Piece> pieces = parser.lastParsedPieces;

	solve(mainState, pieces);

	parser.writeSolution(solution);

	return 0;
}
