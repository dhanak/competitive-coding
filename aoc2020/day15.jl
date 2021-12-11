test = "0,3,6"
puzzle = "8,11,0,19,1,2"

function run(numbers::AbstractVector, rounds::Integer)::Int
    (n0, l0) = (last(numbers), length(numbers))
    nd = Dict(v => i for (i, v) in enumerate(numbers[1:end - 1]))
    return reduce(l0 + 1:rounds; init = (n0, l0)) do (n, l), _
        i = get(nd, n, l)
        nd[n] = l
        return (l - i, l + 1)
    end |> first
end

q1(numbers::AbstractVector)::Int = run(numbers, 2020)
q2(numbers::AbstractVector)::Int = run(numbers, 30_000_000)

let numbers = parse.(Int, split(test, ','))
    @assert q1(numbers) == 436
    @assert q2(numbers) == 175594
end

numbers = parse.(Int, split(puzzle, ','))
println("Q1: ", q1(numbers))
println("Q2: ", q2(numbers))
