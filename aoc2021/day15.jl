using DataStructures
using ProgressMeter: Progress, finish!, next!

test = """
    1163751742
    1381373672
    2136511328
    3694931569
    7463417111
    1319128137
    1359912421
    3125421639
    1293138521
    2311944581
    """

function parse_input(input::AbstractString)
    lines = split(input, r"\s+"; keepempty = false)
    return hcat([parse.(Int, split(line, "")) for line in lines]...) |>
        permutedims
end

function neighbors(M::Matrix, ci::CartesianIndex)::Vector{CartesianIndex}
    return [ci - CartesianIndex(1, 0),
            ci - CartesianIndex(0, 1),
            ci + CartesianIndex(1, 0),
            ci + CartesianIndex(0, 1)] ∩ keys(M)
end

function dijkstra(cave::Matrix, start, stop)::Int
    p = Progress(length(cave))
    handles = similar(cave)
    costs = fill(-1, size(cave))
    open = BinaryMinHeap([(0, start)])
    sizehint!(open, length(cave))
    while !isempty(open)
        (ci, i) = pop!(open)
        costs[i] < 0 || continue
        costs[i] = ci
        if i == stop
            finish!(p)
            return ci
        end
        for j in neighbors(cave, i)
            push!(open, (ci + cave[j], j))
        end
        next!(p)
    end
    @assert false "Couldn't reach $(stop)!"
end

q1(cave)::Int = dijkstra(cave, first(keys(cave)), last(keys(cave)))

function q2(cave)::Int
    big_cave = [(cave ⊕ 0) (cave ⊕ 1) (cave ⊕ 2) (cave ⊕ 3) (cave ⊕ 4)
                (cave ⊕ 1) (cave ⊕ 2) (cave ⊕ 3) (cave ⊕ 4) (cave ⊕ 5)
                (cave ⊕ 2) (cave ⊕ 3) (cave ⊕ 4) (cave ⊕ 5) (cave ⊕ 6)
                (cave ⊕ 3) (cave ⊕ 4) (cave ⊕ 5) (cave ⊕ 6) (cave ⊕ 7)
                (cave ⊕ 4) (cave ⊕ 5) (cave ⊕ 6) (cave ⊕ 7) (cave ⊕ 8)]
    return q1(big_cave)
end

(⊕)(M::Matrix, i::Int) = @. (M + i - 1) % 9 + 1

let cave = parse_input(test)
    @assert q1(cave) == 40
    @assert q2(cave) == 315
end

cave = parse_input(read("day15.in", String))
println("Q1: ", q1(cave))
println("Q2: ", q2(cave))       # slow!
