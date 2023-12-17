using DataStructures
using Test: @testset, @test

test = """
       2413432311323
       3215453535623
       3255245654254
       3446585845452
       4546657867536
       1438598798454
       4457876987766
       3637877979653
       4654967986887
       4564679986453
       1224686865563
       2546548887735
       4322674655533
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    return mapreduce(hcat, lines) do line
        return map(c -> c - '0', collect(line))
    end |> permutedims
end

CI = CartesianIndex{2}

function neighbors(M::Matrix, ci::CI)
    ns = [ci - CartesianIndex(1, 0),
          ci - CartesianIndex(0, 1),
          ci + CartesianIndex(1, 0),
          ci + CartesianIndex(0, 1)]
    return filter!(∈(keys(M)) ∘ last, collect(enumerate(ns)))
end

function dijkstra(input::Matrix,
                  min_straight::Integer,
                  max_straight::Integer
                 )::Int
    visited = Set()
    start = CI(1, 1)
    stop = CI(size(input))
    open = BinaryMinHeap([(0, start, 0, 0)])
    sizehint!(open, 12 * length(input))
    while !isempty(open)
        (loss, i, d, n) = pop!(open)
        i == stop && n >= min_straight && return loss
        (i, d, n) ∈ visited && continue
        push!(visited, (i, d, n))
        for (dj, j) in neighbors(input, i)
            nj = dj == d ? n + 1 : 1
            (i == start || dj == d || n >= min_straight) &&
                nj <= max_straight &&
                (i == start || abs(dj - d) != 2) &&
                push!(open, (loss + input[j], j, dj, nj))
        end
    end
    @assert false "Couldn't reach $(stop)!"
end

q1(input) = dijkstra(input, 0, 3)

q2(input) = dijkstra(input, 4, 10)

if !isinteractive()
    @testset begin
        lines = filter!(!isempty, split(test, '\n'))
        input = parse_input(lines)
        @test q1(input) == 102
        @test q2(input) == 94
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
