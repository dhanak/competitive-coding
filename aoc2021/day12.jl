test1 = """
    start-A
    start-b
    A-c
    A-b
    b-d
    A-end
    b-end
    """

test2 = """
    dc-end
    HN-start
    start-kj
    dc-start
    dc-HN
    LN-dc
    HN-end
    kj-sa
    kj-HN
    kj-dc
    """

function parse_input(input::AbstractString)::Dict
    caves = Dict{AbstractString, Vector{AbstractString}}()
    for line in split(input, r"\s+"; keepempty = false)
        (a, b) = split(line, '-')
        b != "start" && push!(get!(caves, a, Symbol[]), b)
        a != "start" && push!(get!(caves, b, Symbol[]), a)
    end
    return Dict(a => sort(b) for (a, b) in caves)
end

large(s::AbstractString) = isuppercase(s[1])

function walk(caves::AbstractDict, path::AbstractVector; twice::Bool)::Int
    paths = 0
    for cave in caves[path[end]]
        if cave == "end"
            paths += 1
        elseif large(cave)
            paths += walk(caves, [path; cave]; twice)
        elseif cave ∉ path || twice
            paths += walk(caves, [path; cave]; twice = cave ∉ path && twice)
        end
    end
    return paths
end

q1(caves::AbstractDict)::Int = walk(caves, ["start"]; twice = false)
q2(caves::AbstractDict)::Int = walk(caves, ["start"]; twice = true)

let caves1 = parse_input(test1),
    caves2 = parse_input(test2)

    @assert q1(caves1) == 10
    @assert q1(caves2) == 19
    @assert q2(caves1) == 36
    @assert q2(caves2) == 103
end

caves = parse_input(read("day12.in", String))
println("Q1: ", q1(caves))
println("Q2: ", q2(caves))
