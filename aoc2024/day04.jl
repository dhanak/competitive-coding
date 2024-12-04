using Test: @testset, @test
using Statistics: mean

test = """
       MMMSXXMASM
       MSAMXMSMSA
       AMXSXMAAMM
       MSAMASMSMX
       XMASAMXAMM
       XXAMMXXAMA
       SMSMSASXSS
       SAXAMASAAA
       MAMMMXMMMM
       MXMXAXMASX
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    return mapreduce(line -> split(line, ""), hcat, lines) |> permutedims
end

function q1(A)
    return sum(length, [occurs(A, "XMAS"),
                        occurs(permutedims(A), "XMAS"),
                        occurs(rot45(A), "X M A S"),
                        occurs(rot45(rotl90(A)), "X M A S")])
end

function occurs(A, text)
    return mapreduce(vcat, enumerate(eachrow(A))) do (r, row)
        return mapreduce(vcat, [text, reverse(text)]) do t
            return tuple.(r, findall(t, join(row)))
        end
    end
end

function rot45(A)
    (h, w) = size(A)
    B = fill(" ", h + w - 1, h + w - 1)
    for y = 1:h, x = 1:w
        B[y + x - 1, h - y + x] = A[y, x]
    end
    return B
end

function q2(A)
    (h, w) = size(A)
    l = map(occurs(rot45(A), "M A S")) do (y, xs)
        x = Int(mean(extrema(xs)))
        return ((h + y - x + 1) ÷ 2, (y + x - h + 1) ÷ 2)
    end
    r = map(occurs(rot45(rotl90(A)), "M A S")) do (y, xs)
        x = Int(mean(extrema(xs)))
        return ((y + x - h + 1) ÷ 2, h - (h + y - x + 1) ÷ 2 + 1)
    end
    length(l ∩ r)
end

if !isinteractive()
    @testset begin
        lines = split(test, '\n'; keepempty = false)
        input = parse_input(lines)
        @test q1(input) == 18
        @test q2(input) == 9
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
