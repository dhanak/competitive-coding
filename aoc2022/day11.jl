using Test: @testset, @test

test = """Monkey 0:
  Starting items: 79, 98
  Operation: new = old * 19
  Test: divisible by 23
    If true: throw to monkey 2
    If false: throw to monkey 3

Monkey 1:
  Starting items: 54, 65, 75, 74
  Operation: new = old + 6
  Test: divisible by 19
    If true: throw to monkey 2
    If false: throw to monkey 0

Monkey 2:
  Starting items: 79, 60, 97
  Operation: new = old * old
  Test: divisible by 13
    If true: throw to monkey 1
    If false: throw to monkey 3

Monkey 3:
  Starting items: 74
  Operation: new = old + 3
  Test: divisible by 17
    If true: throw to monkey 0
    If false: throw to monkey 1
"""

function parse_input(lines)
    return map(Iterators.partition(lines, 7)) do block
        (monkey, items, op, test, iftrue, iffalse, blank...) = block
        monkey = dropstart(monkey, "Monkey")
        items = dropstart(items, "  Starting items: ")
        op = dropstart(op, "  Operation: ")
        test = dropstart(test, "  Test: divisible by ")
        iftrue = dropstart(iftrue, "    If true: throw to monkey ")
        iffalse = dropstart(iffalse, "    If false: throw to monkey ")
        @assert all(isempty, blank)

        items = parse.(Int, split(items, ", "))
        inspect = eval(Meta.parse(replace(op, "new =" => "old ->")))
        (divisor, t, f) = parse.(Int, (test, iftrue, iffalse))
        throw = x -> (x % divisor == 0 ? t : f) + 1
        return (; items, inspect, divisor, throw)
    end
end

function dropstart(s::AbstractString, prefix::AbstractString)::String
    @assert startswith(s, prefix)
    return s[length(prefix) + 1:end]
end

function monkey_business!(monkeys::Vector, divisor::Int, rounds::Int)::Int
    inspections = fill(0, length(monkeys))
    modulus = lcm([monkey.divisor for monkey in monkeys])
    for round in 1:rounds, (i, monkey) in enumerate(monkeys)
        inspections[i] += length(monkey.items)
        for item in splice!(monkey.items, keys(monkey.items))
            worry = (monkey.inspect(item) ÷ divisor) % modulus
            target = monkey.throw(worry)
            push!(monkeys[target].items, worry)
        end
    end
    return prod(sort!(inspections; rev = true)[1:2])
end

q1(monkeys) = monkey_business!(deepcopy(monkeys), 3, 20)
q2(monkeys) = monkey_business!(deepcopy(monkeys), 1, 10000)

@testset begin
    monkeys = parse_input(split(test, '\n'))
    @test q1(monkeys) == 10605
    @test q2(monkeys) == 2713310158
end

monkeys = parse_input(readlines("day11.in"))
println("Q1: ", q1(monkeys))
println("Q2: ", q2(monkeys))
