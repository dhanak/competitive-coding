rot(n::Int) = M  -> reduce((M, _) -> rotr90(M), 1:n; init = M)
flip(M::Matrix)::Matrix = M[end:-1:1, :]

const TRANSFORMATIONS = [[rot(i) for i in 0:3];
                         [rot(i) ∘ flip for i in 0:3]]

function parse_input(file::AbstractString)
    tiles = open(file, "r") do io
        tiles = []
        while !eof(io)
            push!(tiles, parse_tile(io))
        end
        return tiles
    end

    tilemap = Dict()
    sidemap = Dict()
    for t in tiles, (tr_id, tr) in enumerate(TRANSFORMATIONS)
        t_rot_id = (id = t.id, tr = tr_id)
        t_rot = tr(t.tile)
        sides = tile_sides(t_rot)
        tilemap[t_rot_id] = (; inside = t_rot[2:end - 1, 2:end - 1], sides...)
        for side in pairs(sides)
            push!(get!(sidemap, side, NamedTuple[]), t_rot_id)
        end
    end
    return (tilemap, sidemap)
end

function parse_tile(io::IO)
    id_line = readline(io)
    id_m = match(r"^Tile ([0-9]+):$", id_line)
    @assert id_m !== nothing id_line
    id = parse(Int, id_m.captures[1])
    tile = hcat([split(readline(io), "") for _ in 1:10]...) |> permutedims
    @assert eof(io) || isempty(readline(io))
    return (; id, tile)
end

function tile_sides(tile::Matrix)
    side(v) = Symbol(join(v, ""))

    return (top    = side(tile[1, 1:end]),
            bottom = side(tile[end, 1:end]),
            left   = side(tile[1:end, 1]),
            right  = side(tile[1:end, end]))
end

up(ci::CartesianIndex) = ci - CartesianIndex(1, 0)
left(ci::CartesianIndex) = ci - CartesianIndex(0, 1)

function arrange(tiles, sides)::Matrix
    w = tiles |> keys .|> first |> unique |> length |> sqrt |> Int
    tileset = (unmatched_tops  = unmatched(tiles, :top),
               matched_bottoms = matched(tiles, :bottom),
               unmatched_lefts = unmatched(tiles, :left),
               matched_rights  = matched(tiles, :right),
               all             = keys(tiles),
               dict            = tiles)
    arr = Matrix{Any}(nothing, w, w)
    ok = arrange!(arr, keys(arr), [], tileset, sides)
    @assert ok
    println("found")
    return arr
end

unmatched(tiles, side::Symbol) = setdiff(keys(tiles), matched(tiles, side))

function matched(tiles, side::Symbol)
    opp_side::Symbol = opposite(side)
    return filter(keys(tiles)) do i
        tiles[i][side] ∈ [t[opp_side] for (j, t) in tiles if j.id != i.id]
    end
end

opposite(side::Symbol)::Symbol = opposite(Val(side))
opposite(::Val{:top}) = :bottom
opposite(::Val{:bottom}) = :top
opposite(::Val{:left}) = :right
opposite(::Val{:right}) = :left

function arrange!(arr::Matrix, cis, used, tileset, sides)::Bool
    isempty(cis) && return true
    (ci, cis...) = cis

    suitable = filter(∉(used) ∘ first, tileset.all)
    if ci[1] == 1
        intersect!(suitable, tileset.unmatched_tops)
    else
        border = tileset.dict[arr[up(ci)]].bottom
        intersect!(suitable, sides[:top => border])
    end
    if ci[1] < size(arr, 1)
        intersect!(suitable, tileset.matched_bottoms)
    end
    if ci[2] == 1
        intersect!(suitable, tileset.unmatched_lefts)
    else
        border = tileset.dict[arr[left(ci)]].right
        intersect!(suitable, sides[:left => border])
    end
    if ci[2] < size(arr, 2)
        intersect!(suitable, tileset.matched_rights)
    end

    for t in suitable
        arr[ci] = t
        arrange!(arr, cis, [used; t.id], tileset, sides) && return true
    end
    print(".")                  # backtrack
    return false
end

q1(arr) = map(t -> t.id, corners(arr)) |> prod

corners(M::Matrix)::Matrix = M[[1, end], [1, end]]

function monster(w::Int)::String
    m = ["..................#."
         "#....##....##....###"
         ".#..#..#..#..#..#..."]
    return join(m, repeat('.', w - length(m[1])))
end

function q2(arr, tiles)::Int
    return count(==('#'), mask_monsters(arr, tiles))
end

function mask_monsters(arr, tiles)::String
    images = assemble(arr, tiles)
    w = size(arr, 2) * size(first(tiles).second.inside, 2)
    m = monster(w)
    mre = Regex(m)
    image = images[only(findall(img -> count(mre, img) > 0, images))]
    mask = findall(==('#'), m)
    vimage = split(image, "")
    for i in findall(mre, image; overlap = true) .|> first
        @. vimage[i + mask - 1] = "O"
    end
    return join(vimage, "")
end

function assemble(arr, tiles)::Vector{String}
    grid = size(first(tiles).second.inside)
    image = fill("", size(arr) .* grid)
    for (i, t_id) in pairs(arr)
        tl = (i - one(i)) * grid + one(i)
        br  = i * grid
        image[tl:br] .= tiles[t_id].inside
    end
    return map(tr -> join(tr(image), ""), TRANSFORMATIONS)
end

Base.:*(ci::CartesianIndex, t::Tuple) = CartesianIndex(ci.I .* t)

let (tiles, sides) = parse_input("day20_test.in"),
    arr = arrange(tiles, sides)

    @assert q1(arr) == 20899048083289
    @assert q2(arr, tiles) == 273
end

(tiles, sides) = parse_input("day20.in")
arr = arrange(tiles, sides)
println("Q1: ", q1(arr))
println("Q2: ", q2(arr, tiles))
