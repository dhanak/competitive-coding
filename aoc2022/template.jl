test = """
    """

function parse_input(input::AbstractString)
    lines = split(input, r"\s+"; keepempty = false)
    return
end

q1(v) = 0
q2(v) = 0

let v = parse_input(test)
    @assert q1(v) == 0
    @assert q2(v) == 0
end

v = parse_input(read("day00.in", String))
println("Q1: ", q1(v))
println("Q2: ", q2(v))
