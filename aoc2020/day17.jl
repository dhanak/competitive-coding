using Base: splat

test = """
    .#.
    ..#
    ###
    """

puzzle = """
    #.#####.
    #..##...
    .##..#..
    #.##.###
    .#.#.#..
    #.##..#.
    #####..#
    ..#.#.##
    """

Base.@kwdef mutable struct PocketDim{d}
    space::Dict{CartesianIndex{d}, Bool} = Dict()
    min::CartesianIndex{d} = zero(CartesianIndex{d})
    max::CartesianIndex{d} = zero(CartesianIndex{d})
end

Base.keys(pd::PocketDim) = pd.min:pd.max
Base.values(pd::PocketDim) = pd[keys(pd)]
Base.getindex(pd::PocketDim, i::CartesianIndices) = getindex.(Ref(pd), i)
Base.getindex(pd::PocketDim, i::AbstractVector) = getindex.(Ref(pd), i)
Base.getindex(pd::PocketDim, i::CartesianIndex) = get(pd.space, i, false)

function Base.getindex(pd::PocketDim, i...)
    vs = pd[CartesianIndices(Tuple(minimum(j):maximum(j) for j in i))]
    return any(j -> j isa AbstractRange, i) ? vs : only(vs)
end

function Base.setindex!(pd::PocketDim, v, i...)
    pd.space[CartesianIndex(i...)] = v
    pd.min = min(pd.min, CartesianIndex(i...))
    pd.max = max(pd.max, CartesianIndex(i...))
end

function Base.show(io::IO, pd::PocketDim)
    for z = CartesianIndex(pd.min.I[3:end]):CartesianIndex(pd.max.I[3:end])
        println(io, join(map((n, v) -> "$n=$v", "zw", z.I), ','))
        for r in pd.min[1]:pd.max[1]
            row = pd[r, pd.min[2]:pd.max[2], z.I...]
            println(io, join(map(v -> v ? '#' : '.', row), ' '))
        end
        println(io)
    end
end

function parse_input(input::AbstractString; d::Int = 3)
    lines = split(input, r"\s+"; keepempty = false)
    plane = map.(==("#"), split.(lines, "")) |> splat(hcat) |> permutedims
    pd = PocketDim{d}()
    for k in keys(plane)
        pd[k - one(k), fill(0, d - 2)...] = plane[k]
    end
    return pd
end

function neighbors(ci::CartesianIndex)
    return setdiff((ci - one(ci)):(ci + one(ci)), [ci])
end

function step!(pd::PocketDim)::PocketDim
    change = []
    for i in (pd.min - one(pd.min)):(pd.max + one(pd.max))
        cube = pd[i]
        nbs = pd[neighbors(i)] |> sum
        if cube && !(2 <= nbs <= 3)
            push!(change, i => false)
        elseif !cube && nbs == 3
            push!(change, i => true)
        end
    end
    for (i, v) in change
        pd[i] = v
    end
    return pd
end

function run!(pd::PocketDim, cycles::Int = 6)::Int
    for _ in 1:cycles
        step!(pd)
    end
    return sum(values(pd))
end

q1(input) = run!(parse_input(input; d = 3))
q2(input) = run!(parse_input(input; d = 4))

@assert q1(test) == 112
@assert q2(test) == 848

println("Q1: ", q1(puzzle))
println("Q2: ", q2(puzzle))
