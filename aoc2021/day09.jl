using Base: splat

test = """
    2199943210
    3987894921
    9856789892
    8767896789
    9899965678
    """

function parse_input(input::AbstractString)::Matrix
    return [[parse(Int, c) for c in row]
            for row in split(input, r"\s+"; keepempty = false)] |>
                splat(hcat) |> permutedims
end

function q1(seafloor::Matrix)::Int
    sum(getindex.(Ref(seafloor), low_points(seafloor)) .+ 1)
end

function low_points(seafloor::Matrix)::Vector
    return findall(keys(seafloor)) do i
        return minimum(neighbors(seafloor, i)) == seafloor[i]
    end
end

neighbors(seafloor::AbstractMatrix, ci) = neighbors(seafloor, ci.I...)
function neighbors(seafloor::M, r::Int, c::Int)::M where {M <: AbstractMatrix}
    return seafloor[(r - 1:r + 1) ∩ (1:end),
                    (c - 1:c + 1) ∩ (1:end)]
end

function q2(seafloor::Matrix)::Int
    basins = basin_size.(Ref(seafloor), low_points(seafloor))
    return sort!(basins; rev = true)[1:3] |> prod
end

function basin_size(seafloor::Matrix, ci::CartesianIndex)::Int
    basin = [ci]
    for ci in basin
        for cj in map(f -> f(ci), [up, down, left, right])
            peak(seafloor, cj) || union!(basin, [cj])
        end
    end
    return length(basin)
end

up(ci::CartesianIndex) = CartesianIndex(ci.I .- (1, 0))
down(ci::CartesianIndex) = CartesianIndex(ci.I .+ (1, 0))
left(ci::CartesianIndex) = CartesianIndex(ci.I .- (0, 1))
right(ci::CartesianIndex) = CartesianIndex(ci.I .+ (0, 1))
peak(m::Matrix, ci::CartesianIndex) = ci ∉ keys(m) || m[ci] == 9

let seafloor = parse_input(test)
    @assert q1(seafloor) == 15
    @assert q2(seafloor) == 1134
end

seafloor = parse_input(read("day09.in", String))
println("Q1: ", q1(seafloor))
println("Q2: ", q2(seafloor))
