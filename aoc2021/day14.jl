test = """
    NNCB

    CH -> B
    HH -> N
    CB -> H
    NH -> C
    HB -> C
    HC -> B
    HN -> C
    NN -> C
    BH -> H
    NC -> B
    NB -> B
    BN -> B
    BB -> N
    BC -> B
    CC -> N
    CN -> C
    """

function pair_stats(s::AbstractString)::Dict
    pairs = [s[i:i+1] for i in 1:length(s) - 1]
    return Dict([p => count(==(p), pairs) for p in unique(pairs)])
end

function parse_input(io::IO)
    template = readline(io)
    readline(io)
    # rules0: between these -> insert this
    rules0 = split.(readlines(io), " -> ")
    # rules: this pair is created -> from these pairs
    rules = Dict{String, Vector{String}}()
    for (pat, ins) in rules0
        push!(get!(rules, string(pat[1], ins), []), pat)
        push!(get!(rules, string(ins, pat[2]), []), pat)
    end
    # first and last element never changes
    return (pair_stats(template), rules, last(template))
end

function step(pairs::AbstractDict, rules::AbstractDict)
    return Dict(a => sum([get(pairs, b, 0) for b in bs]) for (a, bs) in rules)
end

function run(polymer::AbstractDict, rules::AbstractDict, n::Int)::Dict
    for _ in 1:n
        polymer = step(polymer, rules)
    end
    return Dict(k => v for (k, v) in polymer if v > 0)
end

function element_count_range(polymer::AbstractDict, tail::AbstractChar)::Int
    counts = [[first(p) => c for (p, c) in polymer]; tail => 1]
    freqs = [sum([c for (elt, c) in counts if elt == unique_elt])
             for unique_elt in unique(first.(counts))]
    (min, max) = extrema(freqs)
    return max - min
end

q1(polymer, rules, tail) = element_count_range(run(polymer, rules, 10), tail)
q2(polymer, rules, tail) = element_count_range(run(polymer, rules, 40), tail)

let (polymer, rules, tail) = parse_input(IOBuffer(test))
    @assert pair_stats("NCNBCHB") == run(polymer, rules, 1)
    @assert pair_stats("NBCCNBBBCBHCB") == run(polymer, rules, 2)
    @assert pair_stats("NBBBCNCCNBBNBNBBCHBHHBCHB") == run(polymer, rules, 3)
    @assert q1(polymer, rules, tail) == 1588
    @assert q2(polymer, rules, tail) == 2188189693529
end

open("day14.in", "r") do io
    input = parse_input(io)
    println("Q1: ", q1(input...))
    println("Q2: ", q2(input...))
end
