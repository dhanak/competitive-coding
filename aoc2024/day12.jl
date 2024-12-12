using Test: @testset, @test

using aoc2024

test = """
       RRRRIICCFF
       RRRRIICCCF
       VVRRRCCFFF
       VVRCCCJFFF
       VVVVCJJCFE
       VVIVCCJJEE
       VVIIICJJEE
       MIIIIIJJEE
       MIIISIJEEE
       MMMISSJEEE
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    return grow(permutedims(hcat(collect.(lines)...)), ' ')
end

q1(garden) = measure(garden, perimeter)

q2(garden) = measure(garden, sides)

function measure(garden::Matrix{Char}, f::Function)::Int
    (h, w) = size(garden)
    counted = Set{CI}()
    return sum(CartesianIndices((2:h - 1, 2:w - 1))) do I
        I ∈ counted && return 0
        plot = flood(garden, I)
        union!(counted, plot)
        return length(plot) * f(plot)
    end
end

function flood(garden::Matrix{Char}, I::CI)::Vector{CI}
    reached = [I]
    for J in reached, K in J .+ neighbors4
        garden[K] == garden[J] && K ∉ reached && push!(reached, K)
    end
    return sort!(reached)
end

function perimeter(plot::Vector{CI})::Int
    return sum(plot) do I
        return count(∉(plot), I .+ neighbors4)
    end
end

function sides(plot::Vector{CI})::Int
    fences = []                 # fence bottom and right endpoints
    for I in plot, (d, J) in enumerate(I .+ neighbors4)
        J ∈ plot && continue    # interior edge, skip
        (a, b) = isodd(d) ? I.I : reverse(I.I)
        filter!(!=((d, a, b - 1)), fences)
        push!(fences, (d, a, b))
    end
    return length(fences)
end

if !isinteractive()
    @testset begin
        lines = split(test, '\n'; keepempty = false)
        input = parse_input(lines)
        @test q1(input) == 1930
        @test q2(input) == 1206
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
