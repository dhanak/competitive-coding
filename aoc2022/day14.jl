using Test: @testset, @test

test = "498,4 -> 498,6 -> 496,6\n503,4 -> 502,4 -> 502,9 -> 494,9"

function parse_input(lines)
    paths = [[CartesianIndex(eval(Meta.parse(coord)))
              for coord in split(line, " -> ")]
             for line in lines]
    rocks = [vcat(map(sortedrange, path[1:end - 1], path[2:end])...)
             for path in paths] |> Base.splat(vcat) |> Set
    return rocks
end

sortedrange(a, b) = vec(collect(a <= b ? (a:b) : (b:a)))

const DROP_DIRECTIONS = [CartesianIndex(0, 1),
                         CartesianIndex(-1, 1),
                         CartesianIndex(1, 1)]

function drop(bounds, filled, from)
    pos = from
    while pos ∈ bounds
        next = findfirst(∉(filled), Ref(pos) .+ DROP_DIRECTIONS)
        next === nothing && return pos
        pos = pos + DROP_DIRECTIONS[next]
    end
    return nothing
end

function fill_cave(rocks; has_floor::Bool)::Int
    if has_floor
        y = maximum(rocks)[2] + 2
        rocks = rocks ∪ (CartesianIndex(500 - y, y):CartesianIndex(500 + y, y))
    end
    source = CartesianIndex(500, 0)
    bounds = range(extrema(rocks ∪ [source])...)
    filled = copy(rocks)
    while true
        rest_at = drop(bounds, filled, source)
        rest_at === nothing && break # flows into the void
        push!(filled, rest_at)
        rest_at === source && break # stops at the top
    end
    @debug '\n' * cave_to_string(bounds, rocks, setdiff(filled, rocks))
    return length(filled) - length(rocks)
end

function cave_to_string(bounds, rocks, sand = [])::String
    (tl, dr) = extrema(bounds)
    tl -= CartesianIndex(1, 1)
    cave = fill('.', (dr - tl).I)
    cave[rocks .- Ref(tl)] .= '#'
    cave[sand .- Ref(tl)] .= 'o'
    return join(join.(eachcol(cave), ""), "\n")
end

q1(rocks)::Int = fill_cave(rocks; has_floor = false)
q2(rocks)::Int = fill_cave(rocks; has_floor = true)

@testset begin
    rocks = parse_input(split(test, '\n'))
    @test q1(rocks) == 24
    @test q2(rocks) == 93
end

rocks = parse_input(readlines("day14.in"))
println("Q1: ", q1(rocks))
println("Q2: ", q2(rocks))
