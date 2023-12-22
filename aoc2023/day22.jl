using Combinatorics: combinations
using StatsBase: countmap
using Test: @testset, @test

test = """
       1,0,1~1,2,1
       0,0,2~2,0,2
       0,2,3~2,2,3
       0,0,4~0,2,4
       2,0,5~2,2,5
       0,1,6~2,1,6
       1,1,8~1,1,9
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    bricks = map(lines) do line
        (a, b) = map(split(line, '~')) do c
            return Tuple(parse.(Int, split(c, ',')))
        end
        return range.(a, b)
    end
    return settle(sort(bricks; by = c -> (c[3], c[1], c[2])))
end

function settle(bricks::Vector)
    settled = eltype(bricks)[]
    supports = [Int[] for _ in bricks]
    for (i, brick) in enumerate(bricks)
        below = findall(settled) do lower
            return !isempty(brick[1] ∩ lower[1]) &&
                !isempty(brick[2] ∩ lower[2])
        end
        z = maximum(settled[below]; init = 1) do lower
            return lower[3].stop + 1
        end
        for j in below
            z == settled[j][3].stop + 1 && push!(supports[j], i)
        end
        push!(settled, (brick[1], brick[2], brick[3] .+ (z - brick[3].start)))
    end
    return (settled, supports)
end

function q1((_, supports))
    supmap = countmap(reduce(vcat, supports))
    return count(supports) do sup
        return all(i -> supmap[i] > 1, sup)
    end
end

function q2((bricks, _))
    return sum(combinations(bricks, length(bricks) - 1)) do pile
        return sum(settle(pile)[1] .!= pile)
    end
end

if !isinteractive()
    @testset begin
        lines = filter!(!isempty, split(test, '\n'))
        input = parse_input(lines)
        @test q1(input) == 5
        @test q2(input) == 7
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
