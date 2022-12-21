using Test: @testset, @test

test = """root: pppw + sjmn
dbpl: 5
cczh: sllz + lgvd
zczc: 2
ptdq: humn - dvpt
dvpt: 3
lfqf: 4
humn: 5
ljgn: 2
sjmn: drzm * dbpl
sllz: 4
pppw: cczh / lfqf
lgvd: ljgn * ptdq
drzm: hmdt - zczc
hmdt: 32"""

function parse_input(lines)
    return map(lines) do line
        (name, expr) = split(line, ": ")
        value = tryparse(Int, expr)
        value !== nothing && return Symbol(name) => value
        (a, op, b) = Symbol.(split(expr, ' '))
        return Symbol(name) => (getfield(Base, op), a, b)
    end |> Dict
end

function evaluate(rules::Dict, f::Symbol)::Int
    e(n::Int) = n
    e(f::Symbol) = e(rules[f])
    e((op, a, b)) = op(e(a), e(b))
    return e(f)
end

inverse(::typeof(+)) = -
inverse(::typeof(-)) = +
inverse(::typeof(*)) = /
inverse(::typeof(/)) = *

q1(rules) = evaluate(rules, :root)

function q2(rules)
    rules = copy(rules)
    rev = Dict(vcat([[a => f, b => f]
                     for (f, body) in rules
                     if body isa Tuple
                     for (_, a, b) = Ref(body)]...))
    needrev = [:humn]
    for f in needrev
        g = rev[f]
        (op, a, b) = rules[g]
        rules[f] = if g == :root
            a == f ? b : a
        else
            push!(needrev, g)
            if a == f
                (inverse(op), g, b)
            elseif op ∈ [*, +]
                (inverse(op), g, a)
            else
                (op, a, g)
            end
        end
    end
    return evaluate(rules, :humn)
end

@testset begin
    rules = parse_input(split(test, '\n'))
    @test q1(rules) == 152
    @test q2(rules) == 301
end

@time begin
    rules = parse_input(readlines("day21.in"))
    println("Q1: ", q1(rules))
    println("Q2: ", q2(rules))
end
