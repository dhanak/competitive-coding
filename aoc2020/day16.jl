using Base: splat

test1 = """
    class: 1-3 or 5-7
    row: 6-11 or 33-44
    seat: 13-40 or 45-50

    your ticket:
    7,1,14

    nearby tickets:
    7,3,47
    40,4,50
    55,2,20
    38,6,12
    """

test2 = """
    class: 0-1 or 4-19
    row: 0-5 or 8-19
    seat: 0-13 or 16-19

    your ticket:
    11,12,13

    nearby tickets:
    3,9,18
    15,1,5
    5,14,9
    """

function parse_input(input::AbstractString)
    (specs, yours, nearby) = strip.(split(input, r"(your|nearby) tickets?:\n"))
    specs = [let (field, ranges...) = split(spec, r": | or "),
                 ranges = [let (a, b) = parse.(Int, split(range, '-'))
                               a:b
                           end for range in ranges]
                 field => union(ranges...)
             end for spec in split(specs, '\n')]
    yours = parse.(Int, split(yours, ','))
    nearby = [parse.(Int, split(nb, ',')) for nb in split(nearby, '\n')]
    return (specs, yours, nearby)
end

function findinvalid(ticket::AbstractVector, specs::AbstractVector)::Vector{Int}
    valid = union(last.(specs)...)
    return findall(∉(valid), ticket)
end

q1(specs, _, nearby) = sum(nb -> nb[findinvalid(nb, specs)] |> sum, nearby)

isvalid(specs) = ticket -> isempty(findinvalid(ticket, specs))

function q2(specs, yours, nearby)::Int
    fields = find_fields(specs, nearby)
    deps = findall(startswith("departure"), first.(specs))
    @assert length(deps) == 6
    return prod(yours[indexin(deps, fields)])
end

function find_fields(specs, nearby)::Vector{Int}
    nearby = filter(isvalid(specs), nearby) |> splat(hcat)
    ranges = last.(specs)
    fields = map(eachrow(nearby)) do vs
        return findall(r -> all(vs .∈ Ref(r)), ranges)
    end
    while any(>(1) ∘ length, fields)
        unamb_idx = findall(==(1) ∘ length, fields)
        unamb_vs = union(fields[unamb_idx]...)
        setdiff!.(fields[setdiff(1:end, unamb_idx)], Ref(unamb_vs))
    end
    return only.(fields)
end

let input1 = parse_input(test1),
    (specs2, _, nearby2) = parse_input(test2)

    @assert q1(input1...) == 71
    @assert find_fields(specs2, nearby2) == [2, 1, 3]
end

input = parse_input(read("day16.in", String))
println("Q1: ", q1(input...))
println("Q2: ", q2(input...))
