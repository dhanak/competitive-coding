using Test: @testset, @test

using aoc2024
using DataStructures: BinaryMinHeap

test = """
       ###############
       #.......#....E#
       #.#.###.#.###.#
       #.....#.#...#.#
       #.###.#####.#.#
       #.#.#.......#.#
       #.#.#####.###.#
       #...........#.#
       ###.#.#####.#.#
       #...#.....#.#.#
       #.#.#.###.#.#.#
       #.....#...#.#.#
       #.###.#.#.#.#.#
       #S..#.....#...#
       ###############
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    M::Matrix{Char} = lines
    (s, e) = [findfirst(==(c), M) for c in "SE"]
    M[[s, e]] .= '.'
    return (M, s, e)
end

q1((M, start, goal)) = first(solve(M, start, goal))

function q2((M, start, goal))::Int
    (_, paths) = solve(M, start, goal)
    queue = collect(filter(==(goal) ∘ first, keys(paths)))
    for p in queue
        (_, parents) = get(paths, p, (0, []))
        append!(queue, parents)
    end
    return queue .|> first |> unique |> length
end

const Node = Tuple{CI, Int}

function solve(M, start, goal)
    best_cost = Inf
    paths = Dict{Node, Tuple{Int, Vector{Node}}}()
    open = BinaryMinHeap([(0, (start, 2), (start, 0))])
    while !isempty(open)
        (cost, node, parent) = pop!(open)
        best_cost < cost && return (best_cost, paths)
        (path_cost, parents) = get(paths, node, (Inf, Node[]))
        path_cost >= cost && (paths[node] = (cost, push!(parents, parent)))
        path_cost <= cost && continue
        (p, d) = node
        if p == goal
            best_cost = cost
        else
            for dn in [d, (d + 2) % 4 + 1, d % 4 + 1]
                pn = p + neighbors4[dn]
                costn = cost + (d == dn ? 1 : 1001)
                M[pn] == '.' && push!(open, (costn, (pn, dn), node))
            end
        end
    end
end

if !isinteractive()
    @testset begin
        lines = split(test, '\n'; keepempty = false)
        input = parse_input(lines)
        @test q1(input) == 7036
        @test q2(input) == 45
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
