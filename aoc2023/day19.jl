using aoc2023
using Test: @testset, @test

test = """
       px{a<2006:qkq,m>2090:A,rfg}
       pv{a>1716:R,A}
       lnx{m>1548:A,A}
       rfg{s<537:gd,x>2440:R,A}
       qs{s>3448:A,lnx}
       qkq{x<1416:A,crn}
       crn{x>2662:A,R}
       in{s<1351:px,qqz}
       qqz{s>2770:qs,m<1801:hdj,R}
       gd{a>3333:R,R}
       hdj{m>838:A,pv}

       {x=787,m=2655,a=1222,s=2876}
       {x=1679,m=44,a=2067,s=496}
       {x=2036,m=264,a=79,s=2244}
       {x=2461,m=1339,a=466,s=291}
       {x=2127,m=1623,a=2188,s=1013}
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    (wfs, parts) = blocks(lines)
    wfs = map(wfs) do wf
        (name, rules...) = split(wf, collect("{,}"); keepempty = false)
        return Symbol(name) => map(rules) do rule
            forward = split(rule, ':')
            if length(forward) == 1
                return (Returns(true), :x, 0, Symbol(forward[]))
            end
            test = forward[1]
            field = Symbol(test[1])
            op = getfield(Main, Symbol(test[2]))
            value = parse(Int, test[3:end])
            return (op, field, value, Symbol(forward[2]))
        end
    end |> Dict
    parts = map(parts) do part
        return eval(Meta.parse("($(part[2:end - 1]))"))
    end
    return (wfs, parts)
end

function q1((wfs, parts))
    return sum(parts) do part
        wf = :(in)
        while wf ∉ [:A, :R]
            for (op, field, value, fwd) in wfs[wf]
                if op(part[field], value)
                    wf = fwd
                    break
                end
            end
        end
        return wf === :A ? sum(part) : 0
    end
end

function q2((wfs, _))
    total = 0
    queue = [(x = 1:4000, m = 1:4000, a = 1:4000, s = 1:4000) => :(in)]
    for (part, wf) in queue
        for (op, field, value, fwd) in wfs[wf]
            (slice, part) = split_part(part, op, field, value)
            if any(isempty, slice)
                # nothing
            elseif fwd == :A
                total += prod(length.(values(slice)))
            elseif fwd != :R
                push!(queue, slice => fwd)
            end
            if any(isempty, part)
                break
            end
        end
    end
    return total
end

function split_part(part, ::Returns, _, _)
    return (part, (x = 1:0, m = 1:0, a = 1:0, s = 1:0))
end

function split_part(part, op, field, value)
    (p1s, p2s) = splitters(op, value)
    (merge(part, (; field => part[field] ∩ p1s)),
     merge(part, (; field => part[field] ∩ p2s)))
end

splitters(::typeof(<), value) = (1:value - 1, value:4000)
splitters(::typeof(>), value) = (value + 1:4000, 1:value)

if !isinteractive()
    @testset begin
        lines = split(test, '\n')
        input = parse_input(lines)
        @test q1(input) == 19114
        @test q2(input) == 167409079868000
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
