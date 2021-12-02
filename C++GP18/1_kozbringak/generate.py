import os
import random
import sys


def usage():
    print "{0} <height> [width [range [seed]]]".format(sys.argv[0])
    sys.exit(0)


def generate(height, width, range):
    map = [[random.randint(-range, range)
            for _ in xrange(width)] for _ in xrange(height)]
    total = sum([sum(row) for row in map])
    avg = total // (height*width)
    total -= avg * (height*width)
    map = [[v - avg for v in row] for row in map]
    for _ in xrange(total):
        map[random.randint(0, height-1)][random.randint(0, width-1)] -= 1
    assert(sum([sum(row) for row in map]) == 0)
    return map


def dump(map):
    print len(map), len(map[0])
    for row in map:
        print ' '.join([str(v) for v in row])


def int_arg(n, default=None):
    try:
        return int(sys.argv[n])
    except:
        return default


def main():
    height = int_arg(1)
    width = int_arg(2, height)
    if not width:
        usage()

    range = int_arg(3, 100)
    seed = int_arg(4)
    random.seed(seed)

    map = generate(height, width, range)
    dump(map)


if __name__ == "__main__":
    main()
