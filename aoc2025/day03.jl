using Test: @testset, @test

using aoc2025

test = """
987654321111111
811111111111119
234234234234278
818181911112111
"""

function parse_input(lines::AbstractVector{<:AbstractString})
    return parse.(Int, convert(Matrix{Char}, lines))
end

function q1(M::Matrix{<:Integer})::Integer
    return sum(eachrow(M)) do row
        a = argmax(row[1:(end - 1)])
        b = a + argmax(row[(a + 1):end])
        return row[a] * 10 + row[b]
    end
end

function q2(M::Matrix{<:Integer})::Integer
    return sum(eachrow(M)) do row
        (n, _) = foldl(11:-1:0; init = (0, 0)) do (n, a), i
            b = a + argmax(row[(a + 1):(end - i)])
            return (n * 10 + row[b], b)
        end
        return n
    end
end

if !isinteractive()
    @testset begin
        local lines = split(test, '\n'; keepempty = false)
        local input = parse_input(lines)
        @test q1(input) == 357
        @test q2(input) == 3121910778619
    end

    @time begin
        local lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        local input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
