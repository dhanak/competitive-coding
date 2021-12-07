test = "16,1,2,0,4,2,7,1,2,14"

function p(s::AbstractString)::Vector{Int}
    return parse.(Int, split(s, ','))
end

function minimize(f, v::AbstractVector)
    (min, max) = extrema(v)
    return minimum(c -> sum(x -> f(x, c), v), min:max)
end

function q1(a, b)
    return abs(a - b)
end

function q2(a, b)
    n = abs(a - b)
    return n * (n + 1) ÷ 2
end

let crabs = p(test)
    @assert minimize(q1, crabs) == 37
    @assert minimize(q2, crabs) == 168
end

crabs = p(readline("day07.in"))
println("Q1: ", minimize(q1, crabs))
println("Q2: ", minimize(q2, crabs))
