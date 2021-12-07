test = """
    35
    20
    15
    25
    47
    40
    62
    55
    65
    95
    102
    117
    150
    182
    127
    219
    299
    277
    309
    576
    """

function parse_input(input::AbstractString)::Vector{Int}
    return parse.(Int, split(input, r"\s+"; keepempty = false))
end

function q1(numbers::AbstractVector{<: Integer}; cycle::Integer)
    i = cycle + findfirst(cycle + 1:length(numbers)) do i
        seeds = i - cycle:i - 1
        return !any(seeds) do j
            return numbers[i] - numbers[j] ∈ numbers[setdiff(seeds, j)]
        end
    end
    return numbers[i]
end

function q2(numbers::AbstractVector{<: Integer}, target_sum::Integer)::Int
    (a, b) = (1, 1)
    rolling_sum = numbers[1]
    while rolling_sum != target_sum
        if rolling_sum < target_sum
            b += 1
            rolling_sum += numbers[b]
        else
            rolling_sum -= numbers[a]
            a += 1
        end
    end
    return numbers[a:b] |> extrema |> sum
end

let numbers = parse_input(test),
    broken = q1(numbers; cycle = 5)
    @assert broken  == 127
    @assert q2(numbers, broken) == 62
end

numbers = parse_input(read("day09.in", String))
broken = q1(numbers; cycle = 25)
println("Q1: ", broken)
println("Q2: ", q2(numbers, broken))
