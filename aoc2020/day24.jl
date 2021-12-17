test = """
    sesenwnenenewseeswwswswwnenewsewsw
    neeenesenwnwwswnenewnwwsewnenwseswesw
    seswneswswsenwwnwse
    nwnwneseeswswnenewneswwnewseswneseene
    swweswneswnenwsewnwneneseenw
    eesenwseswswnenwswnwnwsewwnwsene
    sewnenenenesenwsewnenwwwse
    wenwwweseeeweswwwnwwe
    wsweesenenewnwwnwsenewsenwwsesesenwne
    neeswseenwwswnwswswnw
    nenwswwsewswnenenewsenwsenwnesesenew
    enewnwewneswsewnwswenweswnenwsenwsw
    sweneswneswneneenwnewenewwneswswnese
    swwesenesewenwneswnwwneseswwne
    enesenwswwswneneswsenwnewswseenwsese
    wnwnesenesenenwwnenwsewesewsesesew
    nenewswnwewswnenesenwnesewesw
    eneswnwswnwsenenwnwnwwseeswneewsenese
    neswnwewnwnwseenwseesewsenwsweewe
    wseweeenwnesenwwwswnew
    """

function parse_input(input::AbstractString)
    lines = split(input, "\n"; keepempty = false)
    return lines
end

function parse_path(input::AbstractString)::Vector{Symbol}
    isempty(input) && return []
    l = input[1] ∈ ['n', 's'] ? 2 : 1
    return [Symbol(input[1:l]); parse_path(input[l + 1:end])]
end

function move(path::AbstractString;
              init::Tuple = (0, 0))::Tuple
    return move(parse_path(path); init)
end

function move(path::AbstractVector{<: Symbol};
               init::Tuple = (0, 0))::Tuple
    return reduce(path; init) do pos, dir
        return move(dir, pos)
    end
end

move(dir::Symbol, i::Tuple) = move(Val(dir), i)
move(::Val{:w},  i::Tuple) = i .- (0, 1)
move(::Val{:e},  i::Tuple) = i .+ (0, 1)
move(::Val{:nw}, i::Tuple) = i .+ (-1, mod(i[1], 2) - 1)
move(::Val{:sw}, i::Tuple) = i .+ ( 1, mod(i[1], 2) - 1)
move(::Val{:ne}, i::Tuple) = i .+ (-1, mod(i[1], 2))
move(::Val{:se}, i::Tuple) = i .+ ( 1, mod(i[1], 2))

q1(paths) = length(flip_all(paths))

function flip_all(paths::AbstractVector)::Set
    blacks = Set{Tuple}()
    for tile in move.(paths)
        (tile ∈ blacks ? delete! : push!)(blacks, tile)
    end
    return blacks
end

function q2(paths::AbstractVector; steps::Integer = 100)::Int
    blacks = flip_all(paths)
    for _ in 1:steps
        step!(blacks)
    end
    return length(blacks)
end

function neighbours(pos::Tuple)::Vector{Tuple}
    return map(dir -> move(dir, pos), [:w, :nw, :ne, :e, :se, :sw])
end

function step!(blacks::Set)
    (minrow, maxrow) = blacks .|> first |> extrema
    (mincol, maxcol) = blacks .|> last  |> extrema
    tl = CartesianIndex(minrow - 1, mincol - 1)
    br = CartesianIndex(maxrow + 1, maxcol + 1)
    whites = setdiff(Tuple.(tl:br), blacks)

    to_white = [tile for tile in blacks
                     if count(∈(blacks), neighbours(tile)) ∉ [1, 2]]
    to_black = [tile for tile in whites
                     if count(∈(blacks), neighbours(tile)) == 2]

    union!(setdiff!(blacks, to_white), to_black)
end

@assert move("nwwswee") == (0, 0)
@assert move("esew") == (1, 0)

let v = parse_input(test)
    @assert q1(v) == 10
    @assert q2(v) == 2208
end

v = parse_input(read("day24.in", String))
println("Q1: ", q1(v))
println("Q2: ", q2(v))
