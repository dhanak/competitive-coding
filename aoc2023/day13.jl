using aoc2023
using Test: @testset, @test

test = """
       #.##..##.
       ..#.##.#.
       ##......#
       ##......#
       ..#.##.#.
       ..##..##.
       #.#.##.#.

       #...##..#
       #....#..#
       ..##..###
       #####.##.
       #####.##.
       ..##..###
       #....#..#
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    return map(blocks(lines)) do block
        return mapreduce(hcat, block) do line
            return map(==('#'), collect(line))
        end |> permutedims
    end
end

q1(input) = solve(input, 0)

q2(input) = solve(input, 1)

function solve(input, smudges::Integer)
    return sum(input) do mirror
        (h, v) = symmetries(mirror, smudges)
        return h + 100v
    end
end

function symmetries(mirror::Matrix, smudges::Integer)
    return (symmetry(mirror, smudges), symmetry(permutedims(mirror), smudges))
end

function symmetry(mirror::Matrix, smudges::Integer)
    W = size(mirror, 2)
    for axis in 1:W - 1
        w = min(axis, W - axis)
        left = mirror[:, axis - w + 1:axis]
        right = mirror[:, axis + w:-1:axis + 1]
        sum(abs, left - right) == smudges && return axis
    end
    return 0
end

if !isinteractive()
    @testset begin
        lines = split(test, '\n')
        input = parse_input(lines)
        @test q1(input) == 405
        @test q2(input) == 400
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
