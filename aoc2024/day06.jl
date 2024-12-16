using Test: @testset, @test

using aoc2024

test = """
       ....#.....
       .........#
       ..........
       ..#.......
       .......#..
       ..........
       .#..^.....
       ........#.
       #.........
       ......#...
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    M::Matrix{Char} = lines
    return (size(M), Set(findall(==('#'), M)), only(findall(==('^'), M)))
end

q1(input) = solve(input...) .|> first |> unique |> length

function q2((dims, obstacles, guard))::Int
    candidates = solve(dims, obstacles, guard) .|> first |> unique
    return count(candidates) do p
        return solve(dims, obstacles ∪ [p], guard) == :loop
    end
end

function solve(dims, obstacles, guard)
    dir = 1
    visited = Set{Tuple{CI, Int}}()
    while guard ∈ CartesianIndices(dims)
        (guard, dir) ∈ visited && return :loop
        push!(visited, (guard, dir))
        next = guard + neighbors4[dir]
        if next ∈ obstacles
            dir = dir % 4 + 1
        else
            guard = next
        end
    end
    return visited
end

if !isinteractive()
    @testset begin
        lines = split(test, '\n'; keepempty = false)
        input = parse_input(lines)
        @test q1(input) == 41
        @test q2(input) == 6
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
