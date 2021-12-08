using Base.Iterators: repeated

test = """
    L.LL.LL.LL
    LLLLLLL.LL
    L.L.L..L..
    LLLL.LL.LL
    L.LL.LL.LL
    L.LLLLL.LL
    ..L.L.....
    LLLLLLLLLL
    L.LLLLLL.L
    L.LLLLL.LL
    """

is_floor(spot) = spot == '.'
is_empty(spot) = spot == 'L'
is_occupied(spot) = spot == '#'

function seats(string::AbstractString)::Matrix
    rows = split(string, r"\s+"; keepempty = false)
    seats = map(only, hcat(split.(rows, "")...)) |> permutedims
end

function neighbors(seats::M, ci::CartesianIndex)::M where {M <: AbstractMatrix}
    return neighbors(seats, ci.I...)
end

function neighbors(seats::M, r::Int, c::Int)::M where {M <: AbstractMatrix}
    return seats[(r - 1:r + 1) ∩ (1:end),
                 (c - 1:c + 1) ∩ (1:end)]
end

function visible(seats::AbstractMatrix, ci::CartesianIndex)::Vector
    return visible(seats, ci.I...)
end

function visible(seats::AbstractMatrix, r::Int, c::Int)::Vector
    (R, C) = size(seats)
    (cs, rs) = repeated.((c, r))
    rays = [(r - 1:-1:1, cs), (r + 1:R, cs), # vertical
            (rs, c - 1:-1:1), (rs, c + 1:C), # horizontal
            (r - 1:-1:1, c - 1:-1:1), (r + 1:R, c + 1:C), # diagonal
            (r - 1:-1:1, c + 1:C), (r + 1:R, c - 1:-1:1)]
    return map(rays) do ray
        ray = seats[map(CartesianIndex, ray...)]
        i = findfirst(!is_floor, ray)
        return i === nothing ? '.' : ray[i]
    end
end

function step(seats::M, look_at::Function)::M where {M <: AbstractMatrix}
    return map(keys(seats)) do ci
        n_occ = count(is_occupied, look_at(seats, ci))
        return seats[ci] == 'L' && n_occ == 0 ? '#' :
            seats[ci] == '#' && n_occ >= 5 ? 'L' :
            seats[ci]
    end
end

function run(seats::M, look_at::Function)::M where {M <: AbstractMatrix}
    prev_seats = nothing
    while prev_seats != seats
        prev_seats = seats
        seats = step(seats, look_at)
    end
    return seats
end

function q1(seats::AbstractMatrix)::Integer
    final = run(seats, neighbors)
    return count(is_occupied, final)
end

function q2(seats::AbstractMatrix)::Integer
    final = run(seats, visible)
    return count(is_occupied, final)
end

let lounge = seats(test)
    @assert q1(lounge) == 37
    @assert q2(lounge) == 26
end

lounge = read("day11.in", String) |> seats
println("Q1: ", q1(lounge))
println("Q2: ", q2(lounge))
