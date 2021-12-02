import os
import sys


def usage():
    print "{0} <problem-file> [solution-file]".format(sys.argv[0])
    sys.exit(0)


def read_board(problem):
    w, h = [int(x) for x in next(problem).split()]
    map = []
    for line in problem:
        map.append([int(x) for x in line.split()])

    assert(len(map) == h)
    assert(all([len(row) == w for row in map]))
    assert(sum([sum(row) for row in map]) == 0)

    return map


def check(map, plan):
    cost = 0
    for line in plan:
        line = line.strip().split()
        if line == ['0']:
            break

        assert(len(line) == 5)
        assert(all([int(x) > 0 for x in line]))

        i, j, k, l, n = [int(x)-1 for x in line]
        n += 1
        map[i][j] -= n
        map[k][l] += n
        cost += (abs(i-k) + abs(j-l)) * n

    assert(all([all([x == 0 for x in row]) for row in map]))
    print cost


def main():
    if len(sys.argv) < 2:
        usage()

    try:
        with open(sys.argv[1], 'r') as problem:
            plan = open(sys.argv[2], 'r') if len(sys.argv) > 2 else sys.stdin
            map = read_board(problem)
            check(map, plan)
            if plan is not sys.stdin:
                plan.close()
    except IOError, msg:
        print msg


if __name__ == "__main__":
    main()
