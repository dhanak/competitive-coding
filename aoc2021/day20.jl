test = """
    ..#.#..#####.#.#.#.###.##.....###.##.#..###.####..#####..#....#..#..##..###..######.###...####..#..#####..##..#.#####...##.#.#..#.##..#.#......#.###.######.###.####...#.##.##..#..#..#####.....#.#....###..#.##......#.....#..#..#..##..#...##.######.####.####.#.#...#.......#..#.#.#...####.##.#......#..#...##.#.##..#...##.#.##..###.#......#.#.......#.#.#.####.###.##...#.....####.#..#..#.##.#....##..#.####....##...##..#...#......#.#.......#.......##..####..#...#.#.#...##..#.#..###..#####........#..####......#..#

    #..#.
    #....
    ##..#
    ..#..
    ..###
    """

function parse_input(io::IO)
    cnv(c) = c == "#" ? 1 : 0
    alg = cnv.(split(readline(io), ""))
    @assert length(alg) == 512
    @assert alg[1] & alg[end] == 0
    @assert isempty(readline(io))
    lines = [cnv.(split(line, "")) for line in readlines(io)]
    image = permutedims(hcat(lines...))
    return (alg, pad(image))
end

function pad(M::AbstractMatrix; padding::Integer = 2)::AbstractMatrix
    (h, w) = size(M)
    return [fill(0, padding, w + 2padding)
            fill(0, h, padding) M fill(0, h, padding)
            fill(0, padding, w + 2padding)]
end

function str(M::AbstractMatrix)::String
    return join([join(map(v -> v == 1 ? '#' : '.', row))
                 for row in eachrow(M)], "\n")
end

function decode(M::AbstractMatrix, ci::CartesianIndex)::Int
    bits = vec(permutedims(M[ci - one(ci):ci + one(ci)]))
    return parse(Int, join(bits, ""); base = 2) + 1
end

function enhance(alg::AbstractVector, M::AbstractMatrix)::AbstractMatrix
    R = fill(M[1] == 0 ? alg[1] : alg[end], size(M) .+ (2, 2))
    for ci in CartesianIndex(2, 2):CartesianIndex(size(M) .- (1, 1))
        R[ci + one(ci)] = alg[decode(M, ci)]
    end
    return clip(R)
end

function clip(M::AbstractMatrix; padding::Integer = 2)::AbstractMatrix
    (h, w) = size(M)
    foreground = 1 - M[1]
    (top, bottom) = extrema([r for r in 1:h if foreground ∈ M[r, :]])
    (left, right) = extrema([c for c in 1:w if foreground ∈ M[:, c]])
    return M[top - padding:bottom + padding, left - padding:right + padding]
end

function q1(alg, img)
    return sum(enhance(alg, enhance(alg, img)))
end

function q2(alg, img)
    return sum(reduce((img, _) -> enhance(alg, img), 1:50; init = img))
end

let input = parse_input(IOBuffer(test))
    @assert q1(input...) == 35
    @assert q2(input...) == 3351
end

open("day20.in", "r") do io
    input = parse_input(io)
    println("Q1: ", q1(input...))
    println("Q2: ", q2(input...))
end
