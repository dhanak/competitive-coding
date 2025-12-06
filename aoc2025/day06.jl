using Test: @testset, @test

using aoc2025

test = """
123 328  51 64 
 45 64  387 23 
  6 98  215 314
*   +   *   +  
"""

function parse_input(lines::AbstractVector{<:AbstractString})
    chars = mapreduce(collect, hcat, lines)
    ops = map(filter(!isspace, chars[1:end, end])) do op
        return getproperty(Base, Symbol(op))
    end
    return (permutedims(chars[1:end, 1:(end - 1)]), ops)
end

function q1((chars, ops))::Int
    numbers = map(eachrow(chars)) do row
        parts = split(join(row), ' '; keepempty = false)
        return parse.(Int, parts)
    end |> Base.splat(hcat)
    return sum(ops .=> eachrow(numbers)) do (op, problem)
        return op(problem...)
    end
end

function q2((chars, ops))::Int
    numbers = map(eachcol(chars)) do col
        number = strip(join(col))
        return isempty(number) ? nothing : parse(Int, number)
    end
    breaks = findall(isnothing, numbers)
    ranges = range.([1; breaks .+ 1], [breaks .- 1; length(numbers)])
    problems = getindex.(Ref(numbers), ranges)
    return sum(ops .=> problems) do (op, problem)
        return op(problem...)
    end
end

if !isinteractive()
    @testset begin
        local lines = split(test, '\n'; keepempty = false)
        local input = parse_input(lines)
        @test q1(input) == 4277556
        @test q2(input) == 3263827
    end

    @time begin
        local lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        local input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
