using Test: @testset, @test

using aoc2025

using UnionFind

test = """
162,817,812
57,618,57
906,360,560
592,479,940
352,342,300
466,668,158
542,29,236
431,825,988
739,650,466
52,470,668
216,146,977
819,987,18
117,168,530
805,96,715
346,949,466
970,615,88
941,993,340
862,61,35
984,92,344
425,690,689
"""

function parse_input(lines::AbstractVector{<:AbstractString})
    boxes = map(lines) do line
        return parse.(Int, split(line, ','))
    end
    dists = [
        norm_sq(boxes[i] - boxes[j]) => (i, j)
            for i in 1:(length(boxes) - 1)
            for j in (i + 1):length(boxes)
    ]
    return (boxes, last.(sort!(dists)))
end

norm_sq(v::AbstractVector{<:Number}) = sum(v .^ 2)

function q1((boxes, conns), n::Integer)::Int
    uf = UnionFinder{Int}(length(boxes))
    union!(uf, conns[1:n])
    sizes = Dict(find!(uf, i) => size!(uf, i) for i in keys(boxes))
    return prod(sort!(collect(values(sizes)); rev = true)[1:3])
end

function q2((boxes, conns))::Int
    circuits = length(boxes)
    conns = copy(conns)
    uf = UnionFinder{Int}(circuits)
    (i, j) = (0, 0)
    while circuits > 1
        (i, j) = popfirst!(conns)
        if find!(uf, i) != find!(uf, j)
            circuits -= 1
            union!(uf, i, j)
        end
    end
    return boxes[i][1] * boxes[j][1]
end

if !isinteractive()
    @testset begin
        local lines = split(test, '\n'; keepempty = false)
        local input = parse_input(lines)
        @test q1(input, 10) == 40
        @test q2(input) == 25272
    end

    @time begin
        local lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        local input = parse_input(lines)
        println("Q1: ", q1(input, 1000))
        println("Q2: ", q2(input))
    end
end
