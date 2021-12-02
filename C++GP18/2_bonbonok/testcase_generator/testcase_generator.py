import argparse
import random


# configs
NUM_OF_LEVELS = 1
LEVEL_PREFIX = "lev_"

MIN_NUM_OF_PIECES = 5
MAX_NUM_OF_PIECES = 8

MIN_BOARD_SIZE = 6
MAX_BOARD_SIZE = 8

MIN_PIECE_SIZE = 1
MAX_PIECE_SIZE = 2

MOD_VALUE = 3


class Position:

    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __str__(self):
        return '({},{})'.format(self.x, self.y)


class Piece:

    # values: 2d array of values (0 means empty cell)
    def __init__(self, values):
        self.y = len(values)
        self.x = len(values[0])

        self.cells = values

    def print_as_task(self, out_stream):
        out_stream.write('{} {}\n'.format(self.y, self.x))
        for i in range(self.y):
            out_stream.write(' '.join([str(n) for n in self.cells[i]]))
            out_stream.write('\n')

    def __str__(self):
        return '({}*{})\n'.format(self.x, self.y) + '\n'.join([''.join(str(v) for v in row) for row in self.cells])


class Board:

    def __init__(self, x, y, modulo):
        self.x = x
        self.y = y

        self.mod_value = modulo

        self.pieces = []
        self.positions = []

        self.board = [[0 for _ in range(self.x)] for _ in range(self.y)]

    def add_piece(self, piece):
        self.pieces.append(piece)

    def generate_random_position(self):
        self.board = [[random.randint(1, 2*3*4*5*6*7*8*9) % self.mod_value for _ in range(self.x)] for _ in range(self.y)]

    def print_as_task(self, out_stream):
        out_stream.write('{} {} {}\n'.format(self.y, self.x, self.mod_value))
        for i in range(self.y):
            out_stream.write(' '.join([str(n) for n in self.board[i]]))
            out_stream.write('\n')

        out_stream.write('{}\n'.format(len(self.pieces)))
        for piece in self.pieces:
            piece.print_as_task(out_stream)


def generate_random_piece(modulo):
    x = random.randint(MIN_PIECE_SIZE, MAX_PIECE_SIZE)
    y = random.randint(MIN_PIECE_SIZE, MAX_PIECE_SIZE)

    while True:
        values = [[random.randint(1, 2*3*4*5*6*7*8*9) % modulo for _ in range(x)] for _ in range(y)]

        # TODO: better detection that all elements are zero
        sum_row = [sum(item) for item in values]
        if sum(sum_row) > 0:
            break

    return Piece(values)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-num_of_levels',     type=int, help="Number of levels to generate")
    parser.add_argument('-level_prefix',      type=str, help='Prefix for generated levels (for file names)')
    parser.add_argument('-min_board_size',    type=int, help="Minimum board size (both directions)")
    parser.add_argument('-max_board_size',    type=int, help="Maximum board size (both directions)")
    parser.add_argument('-min_num_of_pieces', type=int, help="Minimum number of pieces to generate per level")
    parser.add_argument('-max_num_of_pieces', type=int, help="Maximum number of pieces to generate per level")
    parser.add_argument('-min_piece_size',    type=int, help="Minimum size of a piece (both directions)")
    parser.add_argument('-max_piece_size',    type=int, help="Maximum size of a piece (both directions)")
    parser.add_argument('-modulo',            type=int, help="Modulo value")
    parser.add_argument('-random_seed',       type=int, help="Random seed")
    args = parser.parse_args()

    if args.num_of_levels:
        NUM_OF_LEVELS = args.num_of_levels

    if args.level_prefix:
        LEVEL_PREFIX = args.level_prefix

    if args.min_board_size:
        MIN_BOARD_SIZE = args.min_board_size

    if args.max_board_size:
        MAX_BOARD_SIZE = args.max_board_size

    if args.min_num_of_pieces:
        MIN_NUM_OF_PIECES = args.min_num_of_pieces

    if args.max_num_of_pieces:
        MAX_NUM_OF_PIECES = args.max_num_of_pieces

    if args.min_piece_size:
        MIN_PIECE_SIZE = args.min_piece_size

    if args.max_piece_size:
        MAX_PIECE_SIZE = args.max_piece_size

    if args.modulo:
        MOD_VALUE = args.modulo

    if args.random_seed:
        random.seed(args.random_seed)

    for level_index in range(NUM_OF_LEVELS):
        with open("{}{}.in".format(LEVEL_PREFIX, level_index), "w") as file:
            boardX = random.randint(MIN_BOARD_SIZE, MAX_BOARD_SIZE)
            boardY = random.randint(MIN_BOARD_SIZE, MAX_BOARD_SIZE)
            board = Board(boardX, boardY, MOD_VALUE)

            numOfPieces = random.randint(MIN_NUM_OF_PIECES, MAX_NUM_OF_PIECES)
            for _ in range(numOfPieces):
                board.add_piece(generate_random_piece(MOD_VALUE))

            board.generate_random_position()

            board.print_as_task(file)


