test = """
    light red bags contain 1 bright white bag, 2 muted yellow bags.
    dark orange bags contain 3 bright white bags, 4 muted yellow bags.
    bright white bags contain 1 shiny gold bag.
    muted yellow bags contain 2 shiny gold bags, 9 faded blue bags.
    shiny gold bags contain 1 dark olive bag, 2 vibrant plum bags.
    dark olive bags contain 3 faded blue bags, 4 dotted black bags.
    vibrant plum bags contain 5 faded blue bags, 6 dotted black bags.
    faded blue bags contain no other bags.
    dotted black bags contain no other bags.
    """

color(name::AbstractString)::Symbol = Symbol(replace(name, ' ' => '_'))

function parse_rules(rules::AbstractString)
    return split(rules, "\n"; keepempty = false) .|> parse_rule |> Dict
end

function parse_rule(rule::AbstractString)
    @assert rule[end] == '.'
    rule = rule[1:end - 1]
    (container, contained) = split(rule, " bags contain ")
    if contained == "no other bags"
        contained = []
    else
        contained = map(split(contained, ", ")) do ctd
            m = match(r"^([0-9]+) (.+) bags?$", ctd)
            @assert m !== nothing
            return (parse(Int, m.captures[1]), color(m.captures[2]))
        end
    end
    return color(container) => contained
end

function contains((_, contained)::Pair, color::Symbol)::Bool
    return any(==(color) ∘ last, contained)
end

function q1(rules::AbstractDict, color::Symbol)::Int
    containers = [color]
    n = 1
    while n <= length(containers)
        new = [first(rule) for rule in rules if contains(rule, containers[n])]
        union!(containers, new)
        n += 1
    end
    return n - 2
end

function q2(rules::AbstractDict, color::Symbol)::Int
    return reduce(rules[color]; init = 0) do t, (cnt, ctd)
        return t + cnt + cnt * q2(rules, ctd)
    end
end

let rules = parse_rules(test)
    @assert q1(rules, :shiny_gold) == 4
    @assert q2(rules, :shiny_gold) == 32
end

rules = parse_rules(read("day07.in", String))
println("Q1: ", q1(rules, :shiny_gold))
println("Q2: ", q2(rules, :shiny_gold))
