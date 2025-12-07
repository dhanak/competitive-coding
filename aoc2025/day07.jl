using Test: @testset, @test

using aoc2025

test = """
.......S.......
...............
.......^.......
...............
......^.^......
...............
.....^.^.^.....
...............
....^.^...^....
...............
...^.^...^.^...
...............
..^...^.....^..
...............
.^.^.^.^.^...^.
...............
"""

function parse_input(lines::AbstractVector{<:AbstractString})
    M = convert(Matrix{Char}, lines)
    return (M, findfirst(==('S'), M[1, 1:end]))
end

function q1((manifold, beam))::Int
    splits = 0
    beams = [beam]
    for row in 2:size(manifold)[1]
        beams = reduce(beams; init = []) do beams, at
            p = manifold[row, at]
            if p == '.'
                return beams ∪ at
            elseif p == '^'
                splits += 1
                return beams ∪ [at - 1, at + 1]
            end
        end
    end
    return splits
end

function q2((manifold, beam))::Int
    beams = Dict(beam => 1)
    for row in 2:size(manifold)[1]
        beams = reduce(beams; init = Dict()) do beams, (at, n)
            p = manifold[row, at]
            new = if p == '.'
                Dict(at => n)
            elseif p == '^'
                Dict(at - 1 => n, at + 1 => n)
            end
            return mergewith(+, beams, new)
        end
    end
    return sum(last, beams)
end

if !isinteractive()
    @testset begin
        local lines = split(test, '\n'; keepempty = false)
        local input = parse_input(lines)
        @test q1(input) == 21
        @test q2(input) == 40
    end

    @time begin
        local lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        local input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
