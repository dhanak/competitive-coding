using Test: @testset, @test

using aoc2024

test = """
       #####
       .####
       .####
       .####
       .#.#.
       .#...
       .....

       #####
       ##.##
       .#.##
       ...##
       ...#.
       ...#.
       .....

       .....
       #....
       #....
       #...#
       #.#.#
       #.###
       #####

       .....
       .....
       #.#..
       ###..
       ###.#
       ###.#
       #####

       .....
       .....
       .....
       #....
       #.#..
       #.#.#
       #####
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    schematics = convert.(Matrix{Char}, blocks(lines))
    locks = map(pins, filter(==('#') ∘ first, schematics))
    keys = map(s -> pins(s[end:-1:1, :]), filter(==('.') ∘ first, schematics))
    return (locks, keys)
end

function pins(schema::Matrix{Char})::Vector{Int}
    return map(eachcol(schema)) do c
        return findlast(==('#'), c) - 1
    end
end

function q1((locks, keys))
    return count(Base.product(locks, keys)) do (l, k)
        return all(<(6), l .+ k)
    end
end

if !isinteractive()
    @testset begin
        lines = split(test, '\n')
        input = parse_input(lines)
        @test q1(input) == 3
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
    end
end
