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
Base.div(a::CI, b::Number, mode = RoundToZero) = CI(div.(a.I, b, mode)...)

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

function CubeMove(l::Integer, offset::Vector, rules::Vector)
    m = []
    flip = [3, 4, 1, 2]
    for (a, ae, b, be, r) in rules, i in 1:l
        j = r ? l + 1 - i : i
        leave = [CI(i, l + 1), CI(l + 1, i), CI(i, 0), CI(0, i)]
        enter = [CI(j, l), CI(l, j), CI(j, 1), CI(1, j)]
        append!(m, [
            (offset[a] + leave[ae], ae) => (offset[b] + enter[be], flip[be])
            (offset[b] + leave[be], be) => (offset[a] + enter[ae], flip[ae])
        ])
    end
    return CubeMove(Dict(m))
end

# rolls right, down, left and up, when start is [(i, false) for i in 1:12]
const ROLLS = let f = false, r = true
    [[(7, r), (9, f), (1, r), (12, f), # bottom
      (3, r), (10, f), (5, r), (11, f), # top
      (6, f), (2, f), (4, f), (8, f)],  # vertical sides
     [(9, r), (6, r), (10, r), (2, r),
      (11, r), (8, r), (12, r), (4, r),
      (7, r), (5, r), (3, r), (1, r)],
     [(3, r), (10, f), (5, r), (11, f),
      (7, r), (9, f), (1, r), (12, f),
      (2, f), (6, f), (8, f), (4, f)],
     [(12, r), (4, r), (11, r), (8, r),
      (10, r), (2, r), (9, r), (6, r),
      (1, r), (3, r), (5, r), (7, r)]]
end

function CubeMove(board::Matrix)
    l = gcd(size(board)...)
    I = CI(1, 1)

    # compute offset grid
    offset = filter(I:CI(l, l):CI(6l + 1, 6l + 1)) do i
        return i ∈ keys(board) && board[i] != ' '
    end .- Ref(I)

    # create neighbor and edge map
    layout = offset .÷ l .+ Ref(I)
    grid = fill(0, maximum(layout).I)
    grid[layout] .= 1:6
    neighbors = []
    edges = [[] for _ in 1:12]
    visit = [(findfirst(isone, grid), [(i, false) for i in 1:12])]
    for (i, cube) in visit
        for (j, (s, r)) in enumerate(cube[1:4])
            push!(edges[s], (grid[i], j, xor(r, j ∈ [2, 3])))
        end
        for n in 1:4
            j = i + [CI(0, 1), CI(1, 0), CI(0, -1), CI(-1, 0)][n]
            if j ∈ keys(grid) && grid[j] != 0 && j ∉ first.(visit)
                c = [(ci, xor(rr, cr)) for (ri, rr) in ROLLS[n]
                                       for (ci, cr) = Ref(cube[ri])]
                push!(visit, (j, c))
                push!(neighbors, (grid[i], grid[j]))
            end
        end
    end
    @assert all(==(2) ∘ length, edges)

    # compute neighbor edge rules
    rules = []
    for ((a, ae, ar), (b, be, br)) in edges
        if (a, b) ∉ neighbors
            push!(rules, (a, ae, b, be, xor(ar, br)))
        end
    end
    @assert length(rules) == 7

    return CubeMove(l, offset, rules)
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
