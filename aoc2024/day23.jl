using Test: @testset, @test

using aoc2024
using Combinatorics: combinations
using Graphs: SimpleGraph, add_edge!, maximal_cliques, neighbors

test = """
       kh-tc
       qp-kh
       de-cg
       ka-co
       yn-aq
       qp-ub
       cg-tb
       vc-aq
       tb-ka
       wh-tc
       yn-cg
       kh-ub
       ta-co
       de-co
       tc-td
       tb-wq
       wh-td
       ta-ka
       td-qp
       aq-cg
       wq-ub
       ub-vc
       de-ta
       wq-aq
       wq-vc
       wh-yn
       ka-de
       kh-ta
       co-tc
       wh-qp
       tb-vc
       td-yn
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    pairs = split.(lines, '-')
    nodes = sort!(unique(first.(pairs) ∪ last.(pairs)))
    g = SimpleGraph(length(nodes))
    for (a, b) in map(p -> indexin(p, nodes), pairs)
        add_edge!(g, a, b)
    end
    (nodes, g)
end

function q1((nodes, g))
    ts = findall(startswith('t'), nodes)
    return sum(ts) do a
        ns = [n for n in neighbors(g, a) if !startswith(nodes[n], 't') || a < n]
        return count(combinations(ns, 2)) do (b, c)
            return b ∈ neighbors(g, c)
        end
    end
end

function q2((nodes, g))
    party = argmax(length, maximal_cliques(g)) |> sort!
    return join(map(n -> nodes[n], party), ',')
end

if !isinteractive()
    @testset begin
        lines = split(test, '\n'; keepempty = false)
        input = parse_input(lines)
        @test q1(input) == 7
        @test q2(input) == "co,de,ka,ta"
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
