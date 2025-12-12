using Test: @testset, @test

using aoc2025

function parse_input(lines::AbstractVector{<:AbstractString})
    (presents..., regions) = blocks(lines)
    presents = map(presents) do present
        shape = convert(Matrix{Char}, present[2:end])
        return count(==('#'), shape)
    end
    regions = map(regions) do region
        (size, shapes) = split(region, ": ")
        (w, h) = parse.(Int, split(size, 'x'))
        shapes = parse.(Int, split(shapes, ' '))
        return ((w, h), shapes)
    end
    return (presents, regions)
end

function q1((presents, regions))::Int
    return count(regions) do (size, counts)
        return sum(presents .* counts) <= prod(size)
    end
end

if !isinteractive()
    @time begin
        local lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        local input = parse_input(lines)
        println("Q1: ", q1(input))
    end
end
