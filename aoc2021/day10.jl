using Statistics: median

test = """
    [({(<(())[]>[[{[]{<()<>>
    [(()[<>])]({[<{<<[]>>(
    {([(<{}[<>[]}>{[]{[(<()>
    (((({<>}<{<{<>}{[]{[]{}
    [[<[([]))<([[{}[[()]]]
    [{[{({}]{}}([{[{{{}}([]
    {<[[]]>}<{[{[{[]{()[[[]
    [<(<(<(<{}))><([]([]()
    <{([([[(<>()){}]>(<<{{
    <{([{{}}[<[[[<>{}]]]>[]]
    """

const PAIRS = Dict('(' => ')', '[' => ']', '{' => '}', '<' => '>')
const CORRUPT_SCORES = Dict(')' => 3, ']' => 57, '}' => 1197, '>' => 25137)
const MISSING_SCORES = Dict(')' => 1, ']' => 2, '}' => 3, '>' => 4)

function parse_input(input::AbstractString)::Vector{String}
    return split(input, r"\s+"; keepempty = false)
end

function score(line::AbstractString)
    open = []
    for c in line
        if c in keys(PAIRS)
            push!(open, PAIRS[c])
        else
            pop!(open) == c || return (CORRUPT_SCORES[c], 0)
        end
    end
    s = foldr((c, s) -> 5s + MISSING_SCORES[c], open; init = 0)
    return (0, s)
end

q1(lines::AbstractVector)::Int = lines .|> score .|> first |> sum

function q2(lines::AbstractVector)::Int
    return filter(!iszero, last.(score.(lines))) |> median |> Int
end

let lines = parse_input(test)
    @assert q1(lines) == 26397
    @assert q2(lines) == 288957
end

lines = parse_input(read("day10.in", String))
println("Q1: ", q1(lines))
println("Q2: ", q2(lines))
