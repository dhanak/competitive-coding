using ProgressMeter: @showprogress

test = "389125467"
puzzle = "962713854"

parse_input(input::AbstractString) = parse.(Int, split(input, ""))

mutable struct Ring
    linked::AbstractVector
    current::Integer

    function Ring(v::AbstractVector)::Ring
        linked = similar(v)
        linked[v] .= [v[2:end]; v[1]]
        return new(linked, v[1])
    end
end

Base.getindex(ring::Ring, i::Integer) = ring.linked[i]
Base.setindex!(ring::Ring, v, i::Integer) = ring.linked[i] = v

function Base.convert(::Type{<: Vector},
                      ring::Ring;
                      from = ring.current
                     )::Vector
    v = similar(ring.linked)
    foldl(keys(v); init = from) do i, j
        v[j] = i
        return ring.linked[i]
    end
    return v
end

function move!(ring::Ring)::Ring
    pick = reduce(1:3; init = ([], ring.current)) do (pick, prev), _
        return let next = ring[prev]; ([pick; next], next) end
    end |> first
    dest = setdiff(predecessors(ring), pick) |> first

    ring[ring.current] = ring[last(pick)]
    ring[last(pick)] = ring[dest]
    ring[dest] = first(pick)
    next!(ring)
    return ring
end

predecessors(ring::Ring) = rem.(ring.current - 2:-1:ring.current - 5,
                                length(ring.linked),
                                RoundDown) .+ 1

next!(ring::Ring)::Ring = (ring.current = ring[ring.current]; ring)

function play(v::Vector, rounds::Integer)::Vector
    ring = Ring(v)
    @showprogress for _ in 1:rounds
        move!(ring)
    end
    return convert(Vector, ring; from = 1)
end

function q1(v::Vector, rounds::Integer = 100)::Int
    v = play(v, rounds)
    return parse(Int, join(v[2:end]))
end

function q2(v::Vector)::Int
    v = [v; range(maximum(v) + 1; step = 1, length = 1_000_000 - length(v))]
    v = play(v, 10_000_000)
    return v[2] * v[3]
end

let v = parse_input(test)
    @assert q1(v, 10) == 92658374
    @assert q1(v) == 67384529
    @assert q2(v) == 149245887792
end

v = parse_input(puzzle)
println("Q1: ", q1(v))
println("Q2: ", q2(v))
