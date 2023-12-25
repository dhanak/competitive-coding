using Combinatorics
using Graphs: SimpleDiGraph, SimpleGraph, Edge, vertices, weights
using GraphsFlows: BoykovKolmogorovAlgorithm, mincut
using Test: @testset, @test

test = """
       jqt: rhn xhk nvd
       rsh: frs pzl lsr
       xhk: hfx
       cmg: qnr nvd lhk bvb
       rhn: xhk bvb hfx
       bvb: xhk hfx
       pzl: lsr hfx nvd
       qnr: nvd
       ntq: jqt hfx bvb xhk
       nvd: lhk
       lsr: lhk
       rzs: qnr cmg lsr rsh
       frs: qnr lhk lsr
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    edges = mapreduce(vcat, lines) do line
        (from, to...) = Symbol.(split(line, r":? "))
        return tuple.(from, to)
    end
    nodes = unique!([first.(edges); last.(edges)])
    nodes = Dict(reverse.(enumerate(nodes)))
    return [Edge(nodes[a], nodes[b]) for (a, b) in edges] |>
        SimpleGraph |> SimpleDiGraph
end

function q1(g)
    w = weights(g)
    algo = BoykovKolmogorovAlgorithm()
    for (a, b) in combinations(vertices(g), 2)
        (p1, p2, cut) = mincut(g, a, b, w, algo)
        cut == 3 && return length(p1) * length(p2)
    end
    @assert false
end

if !isinteractive()
    @testset begin
        lines = filter!(!isempty, split(test, '\n'))
        input = parse_input(lines)
        @test q1(input) == 54
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
    end
end
