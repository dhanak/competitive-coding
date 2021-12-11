using Base: splat

test = """
    5483143223
    2745854711
    5264556173
    6141336146
    6357385478
    4167524645
    2176841721
    6882881134
    4846848554
    5283751526
    """

function parse_input(input::AbstractString)::Matrix
    return [[parse(Int, c) for c in row]
            for row in split(input, r"\s+"; keepempty = false)] |>
                splat(hcat) |> permutedims
end

function neighbors(M::AbstractMatrix, ci::CartesianIndex)::Vector
    ci1 = CartesianIndex(1, 1)
    ns0 = (ci - ci1):(ci + ci1)
    return setdiff(vec(ns0) ∩ keys(M), Ref(ci))
end

function step!(M::AbstractMatrix)::Int
    M .+= 1
    flash = findall(>(9), M)
    for f in flash
        M[neighbors(M, f)] .+= 1
        union!(flash, findall(>(9), M))
    end
    M[flash] .= 0
    return length(flash)
end

function q1(octopuses::AbstractMatrix)::Int
    oct = copy(octopuses)
    return sum(_ -> step!(oct), 1:100)
end

function q2(octopuses::AbstractMatrix)::Int
    oct = copy(octopuses)
    steps = 0
    while !all(iszero, oct)
        step!(oct)
        steps += 1
    end
    return steps
end

let octopuses = parse_input(test)
    @assert q1(octopuses) == 1656
    @assert q2(octopuses) == 195
end

octopuses = parse_input(read("day11.in", String))
println("Q1: ", q1(octopuses))
println("Q2: ", q2(octopuses))
