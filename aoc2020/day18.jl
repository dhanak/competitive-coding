# For Julia operator precedences, see
# https://raw.githubusercontent.com/JuliaLang/julia/master/src/julia-parser.scm

test = """
    1 + 2 * 3 + 4 * 5 + 6
    1 + (2 * 3) + (4 * (5 + 6))
    2 * 3 + (4 * 5)
    5 + (8 * 3 + 9 + 3 * 4 * 3)
    5 * 9 * (7 * 3 * 3 + 9 * 3 + (8 + 6 * 4))
    ((2 + 4 * 9) * (6 + 9 * 8 + 6) + 6) + 2 + 4 * 2
    """

function eval_input(io, pairs...)
    return sum([eval_expr(line, pairs...) for line in readlines(io)])
end

function eval_expr(input::AbstractString, pairs...)
    join(replace(split(input, ""), pairs...), "") |> Meta.parse |> eval
end

# + and * have equal precedence
q1(io) = eval_input(io, "*" => "⊕")
a ⊕ b = a * b

# + and * have swapped precedence
q2(io) = eval_input(io, "*" => "⊞", "+" => "⊠")
a ⊞ b = a * b
a ⊠ b = a + b

@assert q1(IOBuffer(test)) == 71 + 51 + 26 + 437 + 12240 + 13632
@assert q2(IOBuffer(test)) == 231 + 51 + 46 + 1445 + 669060 + 23340

println("Q1: ", q1("day18.in"))
println("Q2: ", q2("day18.in"))
