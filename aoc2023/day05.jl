using aoc2023
using Test: @testset, @test

test = """
       seeds: 79 14 55 13

       seed-to-soil map:
       50 98 2
       52 50 48

       soil-to-fertilizer map:
       0 15 37
       37 52 2
       39 0 15

       fertilizer-to-water map:
       49 53 8
       0 11 42
       42 0 7
       57 7 4

       water-to-light map:
       88 18 7
       18 25 70

       light-to-temperature map:
       45 77 23
       81 45 19
       68 64 13

       temperature-to-humidity map:
       0 69 1
       1 0 69

       humidity-to-location map:
       60 56 37
       56 93 4
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    (seeds, maps...) = blocks(lines)
    seeds = parse.(Int, split(seeds[])[2:end])
    maps = map(maps) do mapping
        @assert endswith(mapping[1], "map:")
        m = map(mapping[2:end]) do line
            (dst, src, len) = parse.(Int, split(line))
            (src, dst, len)
        end |> sort!
        # add guards
        return [(0, 0, 0); m; (typemax(Int), typemax(Int), 0)]
    end
    return (seeds, maps...)
end

function q1((seeds, maps...))
    locs = foldl(maps; init = seeds) do xs, mapping
        return map(xs) do x
            (s, d, l) = mapping[searchsortedlast(mapping, x; by = first)]
            return x < s + l ? x - s + d : x
        end
    end
    return minimum(locs)
end

function q2((seeds, maps...))
    seeds = sort!(collect(Iterators.partition(seeds, 2)))
    locs = foldl(map_ranges, maps; init = seeds)
    return locs[1][1]
end

function map_ranges(xs, mapping)
    return mapreduce(vcat, xs) do (x, lx)
        ys = NTuple{2,Int}[]
        while lx > 0
            i = searchsortedlast(mapping, x; by = first)
            (s, d, l) = mapping[i]
            (y, ly) = if x < s + l
                (x - s + d, min(s + l - x, lx))
            else
                (s1, _, _) = mapping[i + 1]
                (x, min(s1 - x, lx))
            end
            push!(ys, (y, ly))
            x += ly
            lx -= ly
        end
        return ys
    end |> sort!
end

isinteractive() || begin
    @testset begin
        lines = split(test, '\n')
        input = parse_input(lines)
        @test q1(input) == 35
        @test q2(input) == 46
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
