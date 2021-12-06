using Combinatorics: combinations

test = """
    1721
    979
    366
    299
    675
    1456
    """

function q(values::AbstractVector{<: Integer};
           n::Integer = 2,
           total::Integer = 2020)
    for comb in combinations(values, n)
        if sum(comb) == total
            return prod(comb)
        end
    end
end

let values = parse.(Int, split(test, r"\s+"; keepempty= false))
    @assert q(values; n = 2) == 514579
    @assert q(values; n = 3) == 241861950
end

values = parse.(Int, readlines("day01.in"))
println("Q1: ", q(values; n = 2))
println("Q2: ", q(values; n = 3))
