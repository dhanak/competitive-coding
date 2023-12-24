using Combinatorics: combinations
using NonlinearSolve: NonlinearProblem, solve
using Random: shuffle!
using StatsBase: countmap
using Test: @testset, @test

test = """
       19, 13, 30 @ -2,  1, -2
       18, 19, 22 @ -1, -1, -2
       20, 25, 34 @ -2, -2, -4
       12, 31, 28 @ -1, -2, -1
       20, 19, 15 @  1, -5, -3
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    return map(lines) do line
        (p, v) = split(line, '@')
        return map((p, v)) do c
            return parse.(Int, split(c, ','))
        end
    end
end

function q1(stones, min::Integer, max::Integer)
    return count(combinations(stones, 2)) do (a, b)
        ((ax0, ay0, _), (avx, avy, _)) = a
        ((bx0, by0, _), (bvx, bvy, _)) = b
        @assert avx != 0 && bvx != 0
        ma = avy / avx
        mb = bvy / bvx
        x = (ma * ax0 - mb * bx0 - ay0 + by0) / (ma - mb)
        y = ma * (x - ax0) + ay0
        inside = min <= x <= max && min <= y <= max
        ta = (x - ax0) / avx
        tb = (x - bx0) / bvx
        return inside && ta > 0 && tb > 0
    end
end

function q2(stones)
    stones = mapreduce(Base.splat(vcat), hcat, stones) |> permutedims
    combs = collect(combinations(1:size(stones, 1), 3))
    # Monte Carlo, otherwise it takes too long
    solutions = map(shuffle!(combs)[1:min(100, end)]) do triplet
        problem = NonlinearProblem(residual, zeros(9), stones[triplet, :])
        return solve(problem)
    end
    votes = countmap([round.(Int, s.u[1:3]) for s in solutions])
    return sum(first(argmax(last, votes)))
end

function residual(x, stones)
    px = @view x[1:3]
    vx = @view x[4:6]
    ts = @view x[7:end]
    sx = @view stones[:, 1:3]
    sv = @view stones[:, 4:6]
    return mapreduce(vcat, ts, eachrow(sx), eachrow(sv)) do t, sx, sv
        return px + vx * t - (sx + sv * t)
    end
end

if !isinteractive()
    @testset begin
        lines = filter!(!isempty, split(test, '\n'))
        input = parse_input(lines)
        @test q1(input, 7, 27) == 2
        @test q2(input) == 47
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input, 200000000000000, 400000000000000))
        println("Q2: ", q2(input))
    end
end
