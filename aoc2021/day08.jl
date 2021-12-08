using Base: splat

test = """
be cfbegad cbdgef fgaecd cgeb fdcge agebfd fecdb fabcd edb | fdgacbe cefdb cefbgd gcbe
edbfga begcd cbg gc gcadebf fbgde acbgfd abcde gfcbed gfec | fcgedb cgb dgebacf gc
fgaebd cg bdaec gdafb agbcfd gdcbef bgcad gfac gcb cdgabef | cg cg fdcagb cbg
fbegcd cbd adcefb dageb afcb bc aefdc ecdab fgdeca fcdbega | efabcd cedba gadfec cb
aecbfdg fbg gf bafeg dbefa fcge gcbea fcaegb dgceab fcbdga | gecf egdcabf bgf bfgea
fgeab ca afcebg bdacfeg cfaedg gcfdb baec bfadeg bafgc acf | gebdcfa ecba ca fadegcb
dbcfg fgd bdegcaf fgec aegbdf ecdfab fbedc dacgb gdcebf gf | cefg dcbef fcge gbcadfe
bdfegc cbegaf gecbf dfcage bdacg ed bedf ced adcbefg gebcd | ed bcgafe cdgba cbgef
egadfb cdbfeg cegd fecab cgb gbdefca cg fgcdab egfdb bfceg | gbdfcae bgc cg cgb
gcafb gcf dcaebfg ecagb gf abcdeg gaef cafbge fdbac fegbdc | fgae cfgab fg bagce
"""

function parse_input(input::AbstractString)::Vector{Tuple}
    return map(split(input, "\n"; keepempty = false)) do line
        return split.(split(line, " | "), ' '; keepempty = false) |> Tuple
    end
end

function q1(specs::AbstractVector)::Int
    return sum(specs) do (_, display)
        return count(∈([2, 3, 4, 7]) ∘ length, display)
    end
end


function decode(digits::AbstractVector, display::AbstractVector)::Int
    all_digits = [digits; display]
    l(n...) = s -> length(s) ∈ n
    d(n...) = filter(l(n...), all_digits)
    i(v...) = intersect(d(v...)...)
    u(v...) = union(d(v...)...)
    Δ(vs...) = setdiff(vs...)

    # 1: 2segs, 7: 3segs, 4: 4segs, 2, 3, 5: 5segs, 0, 6, 9: 6segs, 8: 7segs
    segments = [Δ(i(3, 5, 6, 7), u(2, 4)),    # in all but 1, 4
                Δ(i(4, 6), u(2, 3), i(5, 7)), # in 0, 6, 9, not in 1, 7
                Δ(i(2, 3, 4, 7), i(5, 6)),    # in all but 5, 6
                Δ(i(4, 5, 7), u(2, 3), i(6)), # in 2, 3, 4, 5, 8, not in 1, 7
                Δ(i(7), u(2, 3, 4), i(5, 6)), # in 8, not in 1, 4, 7
                Δ(i(2, 3, 4, 6, 7), i(5)),    # in all but 2
                Δ(i(5, 6, 7), i(2, 3, 4))]    # in all but 1, 4, 7
    # resolve remaining unambiguities
    segments = map(segments) do seg
        length(seg) == 1 && return only(seg)
        unambiguous = union(filter(l(1), segments)...)
        return Δ(seg, unambiguous) |> only
    end

    s(v...) = segments[[v...]] |> sort! |> v -> join(v, "")
    mapping = Dict(s(1, 2, 3, 5, 6, 7)    => 0,
                   s(3, 6)                => 1,
                   s(1, 3, 4, 5, 7)       => 2,
                   s(1, 3, 4, 6, 7)       => 3,
                   s(2, 3, 4, 6)          => 4,
                   s(1, 2, 4, 6, 7)       => 5,
                   s(1, 2, 4, 5, 6, 7)    => 6,
                   s(1, 3, 6)             => 7,
                   s(1, 2, 3, 4, 5, 6, 7) => 8,
                   s(1, 2, 3, 4, 6, 7)    => 9)

    v(d) = mapping[join(sort(split(d, "")), "")]
    return only([1000 100 10 1] * [v.(display)...])
end

q2(specs::AbstractVector)::Int = sum(splat(decode), specs)

let specs = parse_input(test)
    @assert q1(specs) == 26
    @assert q2(specs) == 61229
end

specs = parse_input(read("day08.in", String))
println("Q1: ", q1(specs))
println("Q2: ", q2(specs))
