using Test: @testset, @test

using aoc2025

using Combinatorics
using JuMP, HiGHS

test = """
[.##.] (3) (1,3) (2) (2,3) (0,2) (0,1) {3,5,4,7}
[...#.] (0,2,3,4) (2,3) (0,4) (0,1,2) (1,2,3,4) {7,5,12,7,2}
[.###.#] (0,1,2,3,4) (0,3,4) (0,1,2,4,5) (1,2) {10,11,11,5,10,5}
"""

function parse_input(lines::AbstractVector{<:AbstractString})
    return map(lines) do line
        (lights, buttons..., joltage) = split(line, ' ')
        lights = bits_to_int(findall(==('#'), lights[2:(end - 1)]) .- 1)
        buttons = map(buttons) do button
            return parse.(Int, split(button[2:(end - 1)], ','))
        end
        joltage = parse.(Int, split(joltage[2:(end - 1)], ','))
        return (lights, buttons, joltage)
    end
end

bits_to_int(bits) = sum(2 .^ bits)

function q1(machines::AbstractVector)::Int
    return sum(Base.splat(fewest_presses), machines)
end

function fewest_presses(lights, buttons, _)::Int
    presses = collect(combinations(bits_to_int.(buttons)))
    i = findfirst(presses) do press
        return lights == reduce(xor, press)
    end
    return length(presses[i])
end

function q2(machines::AbstractVector)::Int
    return sum(Base.splat(total_presses), machines)
end

function total_presses(_, buttons, joltage)::Int
    # formulate as a MILP (mixed integer linear program)
    A = mapreduce(hcat, buttons) do button
        v = zero(joltage)
        v[button .+ 1] .= 1
        return v
    end
    n = length(buttons)
    model = Model(HiGHS.Optimizer)
    set_silent(model)
    @variable(model, 0 <= x[1:n] <= maximum(joltage), Int)
    @constraint(model, A * x == joltage)
    @objective(model, Min, sum(x))
    optimize!(model)
    return round(Int, sum(value, x))
end

if !isinteractive()
    @testset begin
        local lines = split(test, '\n'; keepempty = false)
        local input = parse_input(lines)
        @test q1(input) == 7
        @test q2(input) == 33
    end

    @time begin
        local lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        local input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
