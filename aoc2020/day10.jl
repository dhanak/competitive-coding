test1 = """
    16
    10
    15
    5
    1
    11
    7
    19
    6
    12
    4
    """

test2 = """
    28
    33
    18
    42
    31
    14
    46
    20
    48
    47
    24
    23
    49
    45
    19
    38
    39
    11
    1
    32
    25
    35
    8
    17
    7
    9
    4
    2
    34
    10
    3
    """

function parse_input(input::AbstractString)::Vector{Int}
    return parse.(Int, split(input, r"\s+"; keepempty = false)) |> sort
end

function q1(ratings::AbstractVector{<: Integer})::Int
    ds = diff([0; ratings])
    return count(==(1), ds) * (count(==(3), ds) + 1)
end

function q2(ratings::AbstractVector{<: Integer})::Int
    n_comb = [1; fill(0, last(ratings))]
    for r in ratings
        n_comb[r + 1] = sum(n_comb[max(1, r - 2):r])
    end
    return last(n_comb)
end

let r1 = parse_input(test1),
    r2 = parse_input(test2)

    @assert q1(r1) == 35
    @assert q1(r2) == 220
    @assert q2(r1) == 8
    @assert q2(r2) == 19208
end

ratings = parse_input(read("day10.in", String))
println("Q1: ", q1(ratings))
println("Q2: ", q2(ratings))
