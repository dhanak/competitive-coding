using Test: @testset, @test

test = """        ...#
        .#..
        #...
        ....
...#.......#
........#...
..#....#....
..........#.
        ...#....
        .....#..
        .#......
        ......#.

10R5L5R10L4R5L5"""

function parse_input(lines)
    # parse board
    board = lines[1:end - 2]
    w = maximum(length, board)
    board = map(board) do line
        return [collect(line); fill(' ', w - length(line))]
    end .|> permutedims |> Base.splat(vcat)
    # parse path
    path = lines[end]
    lengths = split(path, r"[LR]")
    turns = split(path, r"[0-9]+")[2:end]
    @assert path == join(map(*, lengths, turns), "")
    lengths = parse.(Int, lengths)
    turns = map(t -> t == "L" ? left : t == "R" ? right : identity, turns)
    path = vcat(map(vcat, lengths, turns)...)
    return (; board, path)
end

right(d) = d % 4 + 1
left(d) = (d + 2) % 4 + 1

abstract type AbstractMove end

const CI = CartesianIndex

function solve(move::AbstractMove; board, path)::Int
    p = CI(1, findfirst(!isspace, board[1, :]))
    d = 1
    @assert board[p] == '.'
    for m in path
        if m isa Function
            d = m(d)
        else
            for _ in 1:m
                (p2, d2) = move(p, d)
                board[p2] == '#' && break
                @assert board[p2] == '.'
                (p, d) = (p2, d2)
            end
        end
    end
    return 1000p.I[1] + 4p.I[2] + d - 1
end

Base.rem(a::CI, b::CI) = CI(a.I .% b.I)

struct SimpleMove <: AbstractMove
    tb::Vector
    lr::Vector
end

function SimpleMove(board::Matrix)
    edges(v) = findall(!isspace, v)[[1, end]]
    return SimpleMove(edges.(eachcol(board)), edges.(eachrow(board)))
end

function (move::SimpleMove)(p, d)
    w(v, a, b) = (v - a + (b - a + 1)) % (b - a + 1) + a
    (r, c) = p.I
    if d ∈ [1, 3]
        c = w(c + (2 - d), move.lr[r]...)
    else
        r = w(r + (3 - d), move.tb[c]...)
    end
    return (CI(r, c), d)
end

struct CubeMove <: AbstractMove
    map::Dict
end

function CubeMove(l::Integer, layout::Vector, rules::Vector)
    l1 = l + 1
    m = []
    flip = [3, 4, 1, 2]
    for (a, ae, b, be, r) in rules, i in 1:l
        j = r ? l1 - i : i
        leave = [CI(i, l1), CI(l1, i), CI(i, 0), CI(0, i)]
        enter = [CI(j, l), CI(l, j), CI(j, 1), CI(1, j)]
        append!(m, [
            (layout[a] + leave[ae], ae) => (layout[b] + enter[be], flip[be])
            (layout[b] + leave[be], be) => (layout[a] + enter[ae], flip[ae])
        ])
    end
    return CubeMove(Dict(m))
end

function CubeMove(board::Matrix)
    l = gcd(size(board)...)
    if l == 4
        layout = [                    CI( 0, 2l),
                  CI(l, 0), CI(l, l), CI( l, 2l),
                                      CI(2l, 2l), CI(2l, 3l)]
        rules = [(1, 1, 6, 1, true),
                 (1, 3, 3, 4, false),
                 (1, 4, 2, 4, true),
                 (2, 2, 5, 2, true),
                 (2, 3, 6, 2, true),
                 (3, 2, 5, 3, true),
                 (4, 1, 6, 4, true)]
    else
        layout = [           CI( 0, l), CI(0, 2l),
                             CI( l, l),
                  CI(2l, 0), CI(2l, l),
                  CI(3l, 0)]
        rules = [(1, 3, 4, 3, true),
                 (1, 4, 6, 3, false),
                 (2, 1, 5, 1, true),
                 (2, 2, 3, 1, false),
                 (2, 4, 6, 2, false),
                 (3, 3, 4, 4, false),
                 (5, 2, 6, 1, false)]
    end
    return CubeMove(l, layout, rules)
end

function (move::CubeMove)(p, d)
    p1 = p + [CI(0, 1), CI(1, 0), CI(0, -1), CI(-1, 0)][d]
    return get(move.map, (p1, d), (p1, d))
end

q1(v) = solve(SimpleMove(v.board); v...)
q2(v) = solve(CubeMove(v.board); v...)

@testset begin
    v = parse_input(split(test, '\n'))
    @test q1(v) == 6032
    @test q2(v) == 5031
end

@time begin
    v = parse_input(readlines("day22.in"))
    println("Q1: ", q1(v))
    println("Q2: ", q2(v))
end
