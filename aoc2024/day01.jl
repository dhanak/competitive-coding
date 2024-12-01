using Test: @testset, @test

test = """
       3   4
       4   3
       2   5
       1   3
       3   9
       3   3
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    return map(lines) do line
        parse.(Int, split(line))
    end |> Base.splat(hcat) |> eachrow |> collect
end

function q1((a, b))
    return sum(abs.(sort(a) .- sort(b)))
end

function q2((a, b))
    return sum(a) do v
        v * count(==(v), b)
    end
end

if !isinteractive()
    @testset begin
        lines = split(test, '\n'; keepempty = false)
        input = parse_input(lines)
        @test q1(input) == 11
        @test q2(input) == 31
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
