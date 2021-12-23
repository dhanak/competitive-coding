using Base: splat
using DataStructures: BinaryHeap

test = """
    #############
    #...........#
    ###B#C#B#D###
      #A#D#C#A#
      #########
    """

# solution for Q1 test
s1 = [(:C, :AB, 1), (:B, :C, 1), (:B, :BC, 1), (:AB, :B, 2), (:A, :B, 1),
      (:D, :CD, 1), (:D, :right, 1), (:CD, :D, 2), (:BC, :D, 1), (:right, :A, 1)]

# solution for Q2 test
s2 = [(:D, :right, 2), (:D, :left, 2), (:C, :right, 1), (:C, :CD, 1),
      (:C, :left, 1), (:B, :C, 3), (:B, :C, 2), (:B, :BC, 1), (:B, :AB, 1),
      (:BC, :B, 4), (:CD, :B, 3), (:right, :B, 2), (:D, :C, 1), (:D, :right, 1),
      (:AB, :D, 4), (:A, :B, 1), (:A, :D, 3), (:A, :AB, 1), (:left, :A, 3),
      (:left, :A, 2), (:AB, :D, 2), (:right, :A, 1), (:right, :D, 1)]

puzzle = """
    #############
    #...........#
    ###D#B#A#C###
      #B#D#A#C#
      #########
    """

extra_lines = """
          #D#C#B#A#
          #D#B#A#C#
        """

function parse_input(input::AbstractString; extra::Bool = false)
    lines = split(input, '\n'; keepempty = false)
    @assert lines[1] == "#############"
    @assert lines[2] == "#...........#"
    @assert lines[5] == "  #########"
    if extra
        lines = [lines[1:3]
                 split(extra_lines, '\n'; keepempty = false)
                 lines[4:5]]
    end
    lines = [split(line[3:11], "") for line in lines[3:end - 1]] |>
        splat(hcat) |> permutedims
    @assert all(==("#"), lines[1:end, 1:2:9])
    lines = lines[1:end, 2:2:8] .|> Symbol |> Matrix{Any}

    return (left  = Any[nothing, nothing],
            A     = lines[:, 1],
            AB    = Any[nothing],
            B     = lines[:, 2],
            BC    = Any[nothing],
            C     = lines[:, 3],
            CD    = Any[nothing],
            D     = lines[:, 4],
            right = Any[nothing, nothing])
end

const COSTS = Dict(:A => 1, :B => 10, :C => 100, :D => 1000)
const DISTANCES = let d = Dict((:A, :left)  => 2,
                               (:A, :AB)    => 2,
                               (:A, :B)     => 4,
                               (:A, :BC)    => 4,
                               (:A, :C)     => 6,
                               (:A, :CD)    => 6,
                               (:A, :D)     => 8,
                               (:A, :right) => 8,
                               (:B, :left)  => 4,
                               (:B, :AB)    => 2,
                               (:B, :BC)    => 2,
                               (:B, :C)     => 4,
                               (:B, :CD)    => 4,
                               (:B, :D)     => 6,
                               (:B, :right) => 6,
                               (:C, :left)  => 6,
                               (:C, :AB)    => 4,
                               (:C, :BC)    => 2,
                               (:C, :CD)    => 2,
                               (:C, :D)     => 4,
                               (:C, :right) => 4,
                               (:D, :left)  => 8,
                               (:D, :AB)    => 6,
                               (:D, :BC)    => 4,
                               (:D, :CD)    => 2,
                               (:D, :right) => 2)
    merge!(d, Dict((b, a) => c for ((a, b), c) in d))
end
const PATHS = let paths = Dict(),
    places = [:left, :A, :AB, :B, :BC, :C, :CD, :D, :right]

    for from in places, to in places
        (a, b) = string.(places[clamp.(sort(indexin([from, to], places)), 2, 8)])
        path = []
        while a != b && last(a) != first(b)
            a = "$(last(a))$(last(a) + 1)"
            push!(path, Symbol(a))
        end
        paths[(from, to)] = path
    end
    paths
end

iscomfy(b, t) = all(v -> v === nothing || v == t, b[t])
Base.isempty(b, t) = all(isnothing, b[t])
Base.isempty(b, from, to) = all(h -> isempty(b, h), PATHS[(from, to)])
top(b, t) = b[t][findfirst(!isnothing, b[t])]

function moves(b)::Vector
    ms = []
    for room in [:A, :B, :C, :D]
        !iscomfy(b, room) || continue
        # from room to hallway
        for hall in [:left, :AB, :BC, :CD, :right]
            isempty(b, room, hall) && append!(ms, moves(b, room, hall))
        end
        # from room to room - not strictly necessary, but this way we can
        # verify the solver with the official solutions
        t = top(b, room)
        iscomfy(b, t) && isempty(b, room, t) && push!(ms, move(b, room, t))
    end
    for hall in [:left, :AB, :BC, :CD, :right]
        !isempty(b, hall) || continue
        # from hallway to room
        t = top(b, hall)
        iscomfy(b, t) && isempty(b, hall, t) && push!(ms, move(b, hall, t))
    end
    return ms
end

function moves(b, from, to)
    ms = []
    to ∈ [:left, :right] && b[to][1] == nothing &&
        push!(ms, move(b, from, to, 1))
    isempty(b, to) && push!(ms, move(b, from, to))
    return ms
end

function move(b, from, to, j = nothing)
    i = findfirst(!isnothing, b[from])
    @assert i !== nothing "Cannot move from $from to $to:\n$b"
    if j === nothing
        j = findfirst(!isnothing, b[to])
        j = j === nothing ? length(b[to]) : j - 1
    end
    t = b[from][i]
    ch = (; from => replace(b[from], i, nothing),
          to => replace(b[to], j, t))
    m = merge(b, ch)
    return ((DISTANCES[(from, to)] + i + j - 2) * COSTS[t], m, (from, to, j))
end

function Base.replace(v::AbstractVector{<: T}, i::Integer, x::T) where T
    return [v[1:i - 1]; x; v[i + 1:end]]
end

function isfinal(b)
    return isempty(b, :left) && isempty(b, :AB) && isempty(b, :BC) &&
        isempty(b, :CD) && isempty(b, :right) &&
        iscomfy(b, :A) && iscomfy(b, :B) && iscomfy(b, :C) && iscomfy(b, :D)
end

function search(b)
    h = BinaryHeap(Base.By(first), [(0, b, [])])
    closed = Set{UInt64}()
    while true
        (c, b, p) = pop!(h)
        hb = hash(b)
        hb ∈ closed && continue
        isfinal(b) && return (cost = c, path = p)
        push!(closed, hb)
        for (cm, bm, m) in moves(b)
            push!(h, (c + cm, bm, [p; m]))
        end
    end
end

function check(b, path, cost)
    i = 1
    path = copy(path)
    while !isempty(path)
        (from, to, j) = popfirst!(path)
        m = move(b, from, to, j)
        @assert m ∈ moves(b) "#$i $b $from $to[$j]"
        (c, b, _) = move(b, from, to, j)
        cost -= c
        i += 1
    end
    @assert cost == 0 "bad cost"
end

let b1 = parse_input(test),
    b2 = parse_input(test; extra = true)

    check(b1, s1, 12521)
    check(b2, s2, 44169)
    @assert search(b1).cost == 12521
    @assert search(b2).cost == 44169
end

b1 = parse_input(puzzle)
b2 = parse_input(puzzle; extra = true)
println("Q1: ", search(b1).cost)
println("Q2: ", search(b2).cost)
