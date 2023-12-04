using Test: @testset, @test

test = """
       Card 1: 41 48 83 86 17 | 83 86  6 31 17  9 48 53
       Card 2: 13 32 20 16 61 | 61 30 68 82 17 32 24 19
       Card 3:  1 21 53 59 44 | 69 82 63 72 16 21 14  1
       Card 4: 41 92 73 84 69 | 59 84 76 51 58  5 54 83
       Card 5: 87 83 26 28 32 | 88 30 70 12 93 22 82 36
       Card 6: 31 18 13 56 72 | 74 77 10 23 35 67 36 11
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    return map(lines) do line
        (_, win, own) = split(line, r"[:\|]")
        return map((win, own)) do numbers
            return parse.(Int, split(numbers))
        end
    end
end

function q1(cards)
    return sum(cards) do (win, own)
        total = length(win ∩ own)
        return total == 0 ? 0 : 1 << (total - 1)
    end
end

function q2(cards)
    wins = map(cards) do (win, own)
        return length(win ∩ own)
    end
    counts = fill(1, length(cards))
    for i in 1:length(wins)
        copies = (i + 1:i + wins[i]) ∩ keys(wins)
        counts[copies] .+= counts[i]
    end
    return sum(counts)
end

isinteractive() || begin
    @testset begin
        lines = filter!(!isempty, split(test, '\n'))
        input = parse_input(lines)
        @test q1(input) == 13
        @test q2(input) == 30
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
