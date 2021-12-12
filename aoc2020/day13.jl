test = """939
7,13,x,x,59,x,31,19
"""

function parse_input(input::AbstractString)::Tuple
    (departure, buses) = split(input, r"\s+"; keepempty = false)
    buses = tryparse.(Int, split(buses, ','))
    return (parse(Int, departure), buses)
end

function q1(departure::Integer, buses::AbstractVector)::Int
    buses = filter(!isnothing, buses)
    bus = buses[argmin([bus - departure % bus for bus in buses])]
    return (bus - departure % bus) * bus
end

function q2(buses::AbstractVector)::Int
    return mod_lcm([(bus, rem(1 - i, bus, RoundDown))
                    for (i, bus) in enumerate(buses)
                    if bus !== nothing])
end

function mod_lcm(vs::AbstractVector)::Int
    isempty(vs) && return 0
    ((v1, m1), rest...) = vs
    r = map(rest) do (v, m)
        vp = v ÷ gcd(v, v1)
        k = findfirst(==(m), [(k * v1 + m1) % v for k in 0:vp - 1]) - 1
        return (vp, k)
    end |> mod_lcm
    return r * v1 + m1
end

let
    (departure, buses) = parse_input(test)
    @assert q1(departure, buses) == 295
    @assert q2(buses) == 1068781
end

(departure, buses) = parse_input(read("day13.in", String))
println("Q1: ", q1(departure, buses))
println("Q1: ", q2(buses))
