using Test: @testset, @test

using aoc2024

test = """
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    return
end

q1(v) = 0
q2(v) = 0

if !isinteractive()
    @testset begin
        lines = split(test, '\n')
        input = parse_input(lines)
        @test q1(input) == 0
        @test q2(input) == 0
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
