using Test: @testset, @test

using aoc2025

test = """
       L68
       L30
       R48
       L5
       R60
       L55
       L1
       L99
       R14
       L82
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    return map(lines) do line
        return (Symbol(line[1]), parse(Int, line[2:end]))
    end
end

function q1(input::AbstractVector)::Int
    p = 50
    r = 0
    for (d, n) in input
        p = mod(p + (d == :L ? -n : n), 100)
        r += p == 0
    end
    return r
end

function q2(input::AbstractVector)::Int
    p = 50
    r = 0
    for (d, n) in input
        if d == :L
            r += (n + mod(-p, 100)) ÷ 100
            p = mod(p - n, 100)
        else
            r += (n + p) ÷ 100
            p = mod(p + n, 100)
        end
    end
    return r
end

if !isinteractive()
    @testset begin
        lines = split(test, '\n'; keepempty = false)
        input = parse_input(lines)
        @test q1(input) == 3
        @test q2(input) == 6
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
