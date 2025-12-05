using Test: @testset, @test

using aoc2025

test = """
3-5
10-14
16-20
12-18

1
5
8
11
17
32
"""

function parse_input(lines::AbstractVector{<:AbstractString})
    (fresh, available) = blocks(lines)
    fresh = map(fresh) do line
        (a, b) = split(line, '-')
        parse(Int, a) => parse(Int, b)
    end
    available = parse.(Int, available)
    return (fresh, available)
end

function q1((fresh, available))::Int
    return count(available) do ingredient
        return any(fresh) do (a, b)
            ingredient ∈ a:b
        end
    end
end

function q2((fresh, _))::Int
    all_fresh = reduce(add_interval, fresh; init = [])
    return sum(length ∘ Base.splat(range), all_fresh)
end

if !isinteractive()
    @testset begin
        local lines = split(test, '\n')
        local input = parse_input(lines)
        @test q1(input) == 3
        @test q2(input) == 14
    end

    @time begin
        local lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        local input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
