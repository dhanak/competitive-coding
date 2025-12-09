using Test: @testset, @test

using aoc2025

test = """
7,1
11,1
11,7
9,7
9,5
2,5
2,3
7,3
"""

function parse_input(lines::AbstractVector{<:AbstractString})
    return map(lines) do line
        return Tuple(parse.(Int, split(line, ',')))
    end
end

area(a, b)::Int = prod(abs.(b .- a) .+ (1, 1))

function q1(tiles::AbstractVector)::Int
    N = length(tiles)
    return maximum(
        area(tiles[i], tiles[j])
            for i in 1:(N - 1)
            for j in (i + 1):N
    )
end

function q2(tiles::AbstractVector)::Int
    N = length(tiles)
    rects = [
        (tiles[i], tiles[j])
            for i in 1:(N - 1)
            for j in (i + 1):N
    ]
    sort!(rects; by = Base.splat(area), rev = true)
    poly = grow_poly(tiles; by = 0.5)
    i = findfirst(rect -> rect_in_poly(rect, poly), rects)
    return area(rects[i]...)
end

function rect_in_poly((a, c), poly::AbstractVector)::Bool
    b = (c[1], a[2])
    d = (a[1], c[2])
    return segment_in_poly(a => b, poly) &&
        segment_in_poly(b => c, poly) &&
        segment_in_poly(c => d, poly) &&
        segment_in_poly(d => a, poly)
end

function segment_in_poly((a, b), poly::AbstractVector)::Bool
    return all(keys(poly)) do i
        (c, d) = (poly[i], poly[i % end + 1])
        x = intersection((a, b), (c, d))
        x == nothing ||
            !(0 < point_on_segment(x, (a, b)) < 1) ||
            !(0 < point_on_segment(x, (c, d)) < 1)
    end
end

if !isinteractive()
    @testset begin
        local lines = split(test, '\n'; keepempty = false)
        local input = parse_input(lines)
        @test q1(input) == 50
        @test q2(input) == 24
    end

    @time begin
        local lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        local input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
