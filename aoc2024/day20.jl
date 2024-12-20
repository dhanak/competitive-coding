using Test: @testset, @test

using aoc2024

test = """
       ###############
       #...#...#.....#
       #.#.#.#.#.###.#
       #S#...#.#.#...#
       #######.#.#.###
       #######.#.#...#
       #######.#.###.#
       ###..E#...#...#
       ###.#######.###
       #...###...#...#
       #.#####.#.###.#
       #.#...#.#.#...#
       #.#.#.#.#.#.###
       #...#...#...###
       ###############
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    M::Matrix{Char} = lines
    (s, e) = [findfirst(==(c), M) for c in "SE"]
    M[[s, e]] .= '.'
    return (M, s, e)
end

function q1(input, save)
    M = walk(input)
    (h, w) = size(M)
    triplets = [[CartesianIndices((y:y, x:x+2)) for y in 2:h-1 for x in 2:w-3];
                [CartesianIndices((y:y+2, x:x)) for y in 2:h-3 for x in 2:w-1]]
    return sum(triplets) do v
        (a, b, c) = M[v]
        return a > 0 && b == -1 && c > 0 && abs(a - c) - 2 >= save
    end
end

function q2(input, save)
    M = walk(input)
    (h, w) = size(M)
    tracks = findall(>(0), M)
    track_pairs = [(a, b)
                   for a in tracks
                   for b in [vec(a + CI(1, -20):a + CI(20, 20));
                             vec(a + CI(0, 1):a + CI(0, 20))] ∩ tracks]
    return count(track_pairs) do (a, b)
        dist = sum(abs.((a - b).I))
        return dist <= 20 && abs(M[a] - M[b]) - dist >= save
    end
end

function walk((M, start, goal))::Matrix{Int}
    M = map(x -> x == '.' ? 0 : -1, M)
    queue = [goal => 1]
    while !isempty(queue)
        (p, cost) = popfirst!(queue)
        M[p] == 0 || continue
        M[p] = cost
        for n in p .+ neighbors4
            M[n] == 0 && push!(queue, n => cost + 1)
        end
    end
    return M
end

if !isinteractive()
    @testset begin
        lines = split(test, '\n'; keepempty = false)
        input = parse_input(lines)
        @test q1(input, 20) == 5
        @test q2(input, 70) == 41
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input, 100))
        println("Q2: ", q2(input, 100))
    end
end
