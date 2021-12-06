test = """
    ..##.......
    #...#...#..
    .#....#..#.
    ..#.#...#.#
    .#...##..#.
    ..#.##.....
    .#.#.#....#
    .#........#
    #.##...#...
    #...##....#
    .#..#...#.#
    """

function parse_forest(rows::AbstractVector{<: AbstractString})::BitMatrix
    hcat([split(row, "") .== "#" for row in rows]...)'
end

function path_sum(forest::AbstractMatrix;
                  right::Integer,
                  down::Integer
                 )::Integer
    total = 0
    (r, c) = (1, 1)
    while r <= size(forest, 1)
        total += forest[r, c]
        c = (c + right - 1) % size(forest, 2) + 1
        r += down
    end
    return total
end

const SLOPES = [(right = 1, down = 1),
                (right = 3, down = 1),
                (right = 5, down = 1),
                (right = 7, down = 1),
                (right = 1, down = 2)]

let forest = IOBuffer(test) |> readlines |> parse_forest
    @assert path_sum(forest; right = 3, down = 1) == 7
    @assert prod([path_sum(forest; slope...) for slope in SLOPES]) == 336
end

forest = readlines("day03.in") |> parse_forest
println("Q1: ", path_sum(forest,; right = 3, down = 1))
println("Q2: ", prod([path_sum(forest; slope...) for slope in SLOPES]))
