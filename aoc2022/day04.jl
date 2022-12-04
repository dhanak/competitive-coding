test = """
    2-4,6-8
    2-3,4-5
    5-7,7-9
    2-8,3-7
    6-6,4-6
    2-6,4-8
    """

function parse_input(input::AbstractString)
    lines = split(input, r"\s+"; keepempty = false)
    return [eval(Meta.parse(replace(line, '-' => ':'))) for line in lines]
end

function q1(pairs)
    return count(pairs) do (a, b)
        return a ⊆ b || b ⊆ a
    end
end

function q2(pairs)
    return count(pairs) do (a, b)
        return !isempty(a ∩ b)
    end
end

let v = parse_input(test)
    @assert q1(v) == 2
    @assert q2(v) == 4
end

v = parse_input(read("day04.in", String))
println("Q1: ", q1(v))
println("Q2: ", q2(v))
