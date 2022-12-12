using Test: @testset, @test

test = """Sabqponm
          abcryxxl
          accszExk
          acctuvwj
          abdefghi"""

function parse_input(input::AbstractString)
    lines = split(input, r"\s+"; keepempty = false)
    elevation = hcat(collect.(lines)...)
    (start, stop) = indexin(['S', 'E'], elevation)
    elevation[[start, stop]] .= ['a', 'z']
    # walk backwards, from 'E' to 'S'
    return (elevation .- 'a', stop, start)
end

function neighbors(M::Matrix, ci::CartesianIndex)::Vector{CartesianIndex}
    return [ci - CartesianIndex(1, 0),
            ci - CartesianIndex(0, 1),
            ci + CartesianIndex(1, 0),
            ci + CartesianIndex(0, 1)] ∩ keys(M)
end

function dijkstra(area::Matrix, start::CartesianIndex, stop)::Int
    queue = [(0, start)]
    for (steps, i) in queue
        stop(i => area[i]) && return steps
        for j in setdiff(neighbors(area, i), last.(queue))
            if area[i] - 1 <= area[j]
                push!(queue, (steps + 1, j))
            end
        end
    end
    @assert false "Couldn't reach stop!"
end

q1((area, start, stop)) = dijkstra(area, start, ==(stop) ∘ first)
q2((area, start, stop)) = dijkstra(area, start, iszero ∘ last)

@testset begin
    v = parse_input(test)
    @test q1(v) == 31
    @test q2(v) == 29
end

v = parse_input(read("day12.in", String))
println("Q1: ", q1(v))
println("Q2: ", q2(v))
