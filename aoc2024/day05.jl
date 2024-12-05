using Test: @testset, @test

test = """
       47|53
       97|13
       97|61
       97|47
       75|29
       61|13
       75|53
       29|13
       97|29
       53|29
       61|53
       97|53
       61|29
       47|13
       75|47
       97|75
       47|61
       75|61
       47|29
       75|13
       53|13

       75,47,61,53,29
       97,61,53,29,13
       75,29,13
       75,97,47,61,53
       61,13,29
       97,13,75,29,47
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    sep = findall(isempty, lines)[]
    rules = map(lines[1:sep - 1]) do line
        return parse.(Int, split(line, "|")) |> Tuple
    end |> Set
    updates = map(lines[sep + 1:end]) do line
        return parse.(Int, split(line, ","))
    end
    return (rules, updates)
end

function q1((rules, updates))::Int
    return sum(filter(issortedby(rules), updates)) do update
        return update[Int((1 + end) / 2)]
    end
end

function q2((rules, update))::Int
    return sum(filter(!issortedby(rules), update)) do update
        return sort(update; lt = by(rules))[Int((1 + end) / 2)]
    end
end

issortedby(rules) = update -> issorted(update; lt = by(rules))

by(rules) = (a, b) -> (a, b) ∈ rules

if !isinteractive()
    @testset begin
        lines = split(test, '\n')[1:end - 1]
        input = parse_input(lines)
        @test q1(input) == 143
        @test q2(input) == 123
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
