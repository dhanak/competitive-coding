using Combinatorics: combinations
using Base.Iterators: drop
using Test: @testset, @test

test = """
       #.#####################
       #.......#########...###
       #######.#########.#.###
       ###.....#.>.>.###.#.###
       ###v#####.#v#.###.#.###
       ###.>...#.#.#.....#...#
       ###v###.#.#.#########.#
       ###...#.#.#.......#...#
       #####.#.#.#######.#.###
       #.....#.#.#.......#...#
       #.#####.#.#.#########v#
       #.#...#...#...###...>.#
       #.#.#v#######v###.###v#
       #...#.>.#...>.>.#.###.#
       #####v#.#.###v#.#.###.#
       #.....#...#...#.#.#...#
       #.#########.###.#.#.###
       #...###...#...#...#.###
       ###.###.#.###v#####v###
       #...#...#.#.>.>.#.>.###
       #.###.###.#.###.#.#v###
       #.....###...###...#...#
       #####################.#
       """

CI = CartesianIndex{2}

moves = Dict('>' => CI(0, 1),
             '<' => CI(0, -1),
             'v' => CI(1, 0),
             '^' => CI(-1, 0))

function parse_input(lines::AbstractVector{<: AbstractString})
    return mapreduce(collect, hcat, lines) |> permutedims |> wrap
end

function wrap(puzzle)
    (h, w) = size(puzzle)
    return [       fill('#', 1, w + 2)
            fill('#', h) puzzle fill('#', h)
                   fill('#', 1, w + 2)]
end

q1(land) = solve(simplify(land; slopes = true)...)
q2(land) = solve(simplify(land; slopes = false)...)

function simplify(land; slopes::Bool)
    nodes = CI[]
    edges = Tuple{CI, CI, Int}[]
    starts = [(CI(2, 3), CI(3, 3))]
    for (jct, start) in starts
        push!(nodes, jct)
        p0 = jct
        path = [start]
        fwd = bwd = true
        for p in path
            if slopes
                forced = get(moves, land[p], nothing)
                fwd &= forced ∈ [p - p0, nothing]
                bwd &= forced ∈ [p0 - p, nothing]
            end
            pn = filter(Ref(p) .+ values(moves)) do p1
                return land[p1] != '#' && p1 != p0
            end
            if length(pn) == 1
                push!(path, pn[])
            else
                push!(nodes, p)
                fwd && push!(edges, (jct, p, length(path)))
                bwd && push!(edges, (p, jct, length(path)))
                append!(starts, [(p, p1) for p1 in pn if (p, p1) ∉ starts])
            end
            p0 = p
        end
    end
    @assert findlast(==('.'), land) ∈ nodes
    return (unique!(nodes), unique!(edges))
end

function solve(nodes, edges)
    (start, goal) = extrema(nodes)

    visited = Dict(n => false for n in nodes)
    len = 0
    best = 0

    function best_path(p)
        visited[p] = true
        for (a, b, l) in edges
            (a == p && !visited[b]) || continue
            len += l
            if b == goal
                best = max(best, len)
            else
                best_path(b)
            end
            len -= l
        end
        visited[p] = false
        return best
    end

    return best_path(start)
end

if !isinteractive()
    @testset begin
        lines = filter!(!isempty, split(test, '\n'))
        input = parse_input(lines)
        @test q1(input) == 94
        @test q2(input) == 154
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
