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
    return split(input, '\n') .|> strip
end

function q1(v)
    return reduce(v; init = (0, 0)) do (sum, lmax), line
        return line == "" ?
            (0, max(sum, lmax)) :
            (sum + parse(Int, line), lmax)
    end |> last
end

function q2(v)
    sums = reduce(v; init = [0]) do sums, line
        if line == ""
            return push!(sums, 0)
        else
            sums[end] += parse(Int, line)
            return sums
        end
    end
    return sum(sort!(sums)[end - 2:end])
end

let v = parse_input(test)
    @assert q1(v) == 24000
    @assert q2(v) == 45000
end

v = parse_input(read("day01.in", String))
println("Q1: ", q1(v))
println("Q2: ", q2(v))
