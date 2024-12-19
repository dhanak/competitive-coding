using Test: @testset, @test

using aoc2024

test = """
       r, wr, b, g, bwu, rb, gb, br

       brwrr
       bggr
       gbbr
       rrbgbr
       ubwu
       bwurrg
       brgr
       bbrgwb
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    patterns = split(lines[1], ", ")
    @assert lines[2] == ""
    return (patterns, lines[3:end])
end

function q1((patterns, designs))::Int
    return count(designs) do design
        return possible(design, patterns) > 0
    end
end

function q2((patterns, designs))::Int
    return sum(designs) do design
        return possible(design, patterns)
    end
end

function possible(design, patterns)::Int
    ld =  length(design)
    lps = length.(patterns)
    D = [1; fill(0, ld)]
    for i in eachindex(design), (pattern, lp) in zip(patterns, lps)
        lp > i && continue
        D[i + 1] += (design[i - lp + 1:i] == pattern) * D[i - lp + 1]
    end
    return D[end]
end

if !isinteractive()
    @testset begin
        lines = split(test, '\n')[1:end - 1]
        input = parse_input(lines)
        @test q1(input) == 6
        @test q2(input) == 16
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
