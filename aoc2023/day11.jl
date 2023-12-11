using Combinatorics: combinations
using Test: @testset, @test

test = """
       ...#......
       .......#..
       #.........
       ..........
       ......#...
       .#........
       .........#
       ..........
       .......#..
       #...#.....
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    chart = mapreduce(collect, hcat, lines) |> permutedims
    exp_rows = findall(no_galaxy, eachrow(chart))
    exp_cols = findall(no_galaxy, eachcol(chart))
    galaxies = findall(==('#'), chart)
    return (galaxies, exp_rows, exp_cols)
end

no_galaxy(row) = all(==('.'), row)

function solve((galaxies, exp_rows, exp_cols), mul::Integer)
    return sum(combinations(galaxies, 2)) do (a, b)
        (ar, ac) = a.I
        (br, bc) = b.I
        return abs(ar - br) + abs(ac - bc) +
            (mul - 1) * count(between(ar, br), exp_rows) +
            (mul - 1) * count(between(ac, bc), exp_cols)
    end
end

between(a, b) = x -> min(a, b) <= x <= max(a, b)

if !isinteractive()
    @testset begin
        lines = filter!(!isempty, split(test, '\n'))
        input = parse_input(lines)
        @test solve(input, 2) == 374
        @test solve(input, 10) == 1030
        @test solve(input, 100) == 8410
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", solve(input, 2))
        println("Q2: ", solve(input, 1_000_000))
    end
end
