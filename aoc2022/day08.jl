test = """
    30373
    25512
    65332
    33549
    35390
    """

function parse_input(input::AbstractString)
    lines = split(input, r"\s+"; keepempty = false)
    rows = [collect(line) .- '0' for line in lines]
    return permutedims(hcat(rows...))
end

function visible(forest::Matrix, i::CartesianIndex)::Bool
    shorter = <(forest[i])
    (r, c) = i.I
    return all(shorter, forest[1:r - 1, c]) ||
        all(shorter, forest[r, 1:c - 1]) ||
        all(shorter, forest[r + 1:end, c]) ||
        all(shorter, forest[r, c + 1:end])
end

function scenic_score(forest::Matrix, i::CartesianIndex)::Int
    (r, c) = i.I
    return scenic_score(forest[r:-1:1, c]) *
        scenic_score(forest[r:end, c]) *
        scenic_score(forest[r, c:-1:1]) *
        scenic_score(forest[r, c:end])
end

function scenic_score(line::Vector)::Int
    return something(findfirst(>=(line[1]), line[2:end]), length(line) - 1)
end

q1(forest::Matrix)::Int = count(i -> visible(forest, i), keys(forest))

q2(forest::Matrix)::Int = maximum(i -> scenic_score(forest, i), keys(forest))

let v = parse_input(test)
    @assert q1(v) == 21
    @assert scenic_score(v, CartesianIndex(2, 3)) == 4
    @assert scenic_score(v, CartesianIndex(4, 3)) == 8
    @assert q2(v) == 8
end

v = parse_input(read("day08.in", String))
println("Q1: ", q1(v))
println("Q2: ", q2(v))
