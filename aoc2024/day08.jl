using Test: @testset, @test

using aoc2024
using Combinatorics: combinations

test = """
       ............
       ........0...
       .....0......
       .......0....
       ....0.......
       ......A.....
       ............
       ............
       ........A...
       .........A..
       ............
       ............
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    dims = (length(lines), length(lines[1]))
    antennas = mapreduce(vcat, enumerate(lines)) do (y, line)
        line = collect(line)
        xs = findall(!=('.'), line)
        return CI.(Ref(y), xs) .=> line[xs]
    end
    return (dims, antennas)
end

function q1((dims, antennas))
    bounds = CartesianIndices(dims)
    return count_antinodes(antennas) do (a, b)
        return filter(∈(bounds), [a + (a - b), b + (b - a)])
    end
end

function q2((bounds, antennas))
    return count_antinodes(antennas) do (a, b)
        resonants(bounds, a, b) ∪ resonants(bounds, b, a)
    end
end

function count_antinodes(f, antennas)
    freqs = unique(last.(antennas))
    antinodes = reduce(freqs; init = Set()) do acc, freq
        cs = [c for (c, f) in antennas if f == freq]
        return mapreduce(f, union!, combinations(cs, 2); init = acc)
    end
    return length(antinodes)
end

function resonants((h, w), a, b)
    (ay, ax) = a.I
    (dy, dx) = (a - b).I
    fit(d, p, m) = d < 0 ? (1 - p) ÷ d : d > 0 ? (m - p) ÷ d : typemax(Int)
    return [a + i * (a - b) for i in 0:min(fit(dx, ax, w), fit(dy, ay, h))]
end

if !isinteractive()
    @testset begin
        lines = split(test, '\n'; keepempty = false)
        input = parse_input(lines)
        @test q1(input) == 14
        @test q2(input) == 34
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
