test = """
    0: 4 1 5
    1: 2 3 | 3 2
    2: 4 4 | 5 5
    3: 4 5 | 5 4
    4: "a"
    5: "b"

    ababbb
    bababa
    abbbab
    aaabbb
    aaaabbb
    """

function parse_input(input::AbstractString)
    (rules, messages) = split.(split(input, "\n\n"), "\n")
    rules = parse_rule.(rules)
    @assert sort(first.(rules)) == 0:length(rules) - 1
    return (sort(rules; by = first) .|> last, messages)
end

function parse_rule(rule::AbstractString)::Pair
    (i, body) = split(rule, ": ")
    rule = body[1] == '"' ? body[2] :
        [parse.(Int, split(subrule, " ")) .+ 1
         for subrule in split(body, " | ")]
    return parse(Int, i) => rule
end

function compile(rules::AbstractVector)::Regex
    re(i::Int) = re(rules[i])
    re(rule::Char) = rule
    re(rule::Vector) = "(?:" *
        join([join(re.(sub), "") for sub in rule], "|") * ")"
    re(f::Function) = f(re)

    return Regex("^$(re(1))\$")
end

function q1(rules, msgs)::Int
    re = compile(rules)
    return count(msg -> match(re, msg) !== nothing, msgs)
end

function q2(rules, msgs, a, b)::Int
    rules = copy(rules)
    rules[9] = re -> "(?:$(re(a))+)"
    rules[12] = re -> "(?<r11>$(re(a))\\g<r11>?$(re(b)))"
    return q1(rules, msgs)
end

@assert q1(parse_input(test)...) == 2
let input = parse_input(read("day19_test.in", String))
    @assert q1(input...) == 3
    @assert q2(input..., 31, 30) == 12
end

input = parse_input(read("day19.in", String))
println("Q1: ", q1(input...))
println("Q2: ", q2(input..., 43, 32))
