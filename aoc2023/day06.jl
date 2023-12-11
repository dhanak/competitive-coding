using Test: @testset, @test

test = """
       Time:      7  15   30
       Distance:  9  40  200
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    return map(line -> split(line)[2:end], lines)
end

function q1((times, dists); solve = poly)
    return mapreduce(s2i(solve), *, times, dists)
end

function q2((times, dists); solve = poly)
    (t, d) = join.((times, dists))
    return s2i(solve)(t, d)
end

function s2i(f::Function)::Function
    return (args...) -> f(parse.(Int, args)...)
end

function naive(t::Int, d::Int)::Int
    return count(1:t) do h
        return (t - h) * h > d
    end
end

function poly(t::Int, d::Int)::Int
    # find number of integer h values that satisfy: h^2 - ht + d < 0
    (a, b) = roots(1, -t, d)
    return ceil(Int, b) - floor(Int, a) - 1
end

function roots(a::Number, b::Number, c::Number)
    base = -b / 2a
    disc = √(b ^ 2 - 4 * a * c) / 2a
    return (base - disc, base + disc)
end

if !isinteractive()
    @testset begin
        lines = filter!(!isempty, split(test, '\n'))
        input = parse_input(lines)
        @test q1(input) == 288
        @test q2(input) == 71503
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
