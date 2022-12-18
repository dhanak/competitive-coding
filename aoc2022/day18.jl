using Base: splat
using Combinatorics: combinations
using Test: @testset, @test

test = """2,2,2
1,2,2
3,2,2
2,1,2
2,3,2
2,2,1
2,2,3
2,2,4
2,2,6
1,2,5
3,2,5
2,1,5
2,3,5"""

const NEIGHBORS =  [CartesianIndex(-1, 0, 0), CartesianIndex(1, 0, 0),
                    CartesianIndex(0, -1, 0), CartesianIndex(0, 1, 0),
                    CartesianIndex(0, 0, -1), CartesianIndex(0, 0, 1)]

function parse_input(lines)
    return [CartesianIndex(parse.(Int, split(line, ','))...) for line in lines]
end

function q1(coords)
    return 6length(coords) - 2count(splat(isneighbor), combinations(coords, 2))
end

isneighbor(a, b) = sum(abs.((a - b).I)) == 1

function q2(coords)
    I = one(coords[1])
    (tl, dr) = extrema(coords)
    M = fill(' ', (dr - tl + I).I)
    M[coords .- Ref(tl - I)] .= '#'
    inside = I:dr - tl + I
    for c in inside
        M[c] == ' ' || continue
        M[c] = '.'
        open = false
        visit = [c]
        for i in visit, n in Ref(i) .+ NEIGHBORS
            if n ∉ inside
                open = true
            elseif M[n] == ' '
                M[n] = '.'
                push!(visit, n)
            end
        end
        if open
            M[visit] .= '-'
        end
    end
    return q1(coords) - q1(findall(==('.'), M))
end

@testset begin
    v = parse_input(split(test, '\n'))
    @test q1(v) == 64
    @test q2(v) == 58
end

v = parse_input(readlines("day18.in"))
println("Q1: ", q1(v))
println("Q2: ", q2(v))
