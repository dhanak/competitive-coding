test = """
    1000
    2000
    3000

    4000

    5000
    6000

    7000
    8000
    9000

    10000
"""

function parse_input(input::AbstractString)
    lines = split(input, '\n') .|> strip
    breaks = findall(isempty, lines)
    groups = map([0; breaks], [breaks; length(lines) + 1]) do from, to
        return parse.(Int, lines[from + 1:to - 1])
    end
    return filter(!isempty, groups)
end

q1(groups) = maximum(sum.(groups))
q2(groups) = sum(partialsort!(sum.(groups), 1:3; rev = true))

let v = parse_input(test)
    @assert q1(v) == 24000
    @assert q2(v) == 45000
end

v = parse_input(read("day01.in", String))
println("Q1: ", q1(v))
println("Q2: ", q2(v))
