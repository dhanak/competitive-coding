test = (4, 8)
puzzle = (1, 10)

rot(v::Integer, m::Integer = 10) = mod(v - 1, m) + 1

function q1(p1::Integer, p2::Integer)::Int
    roll(i) = sum(rot.([i, i + 1, i + 2], 100))

    rolls = [[roll(i), roll(i + 3)] for i in 1:6:1000]
    fields = [rot.(fs) for fs in cumsum([[p1, p2], rolls...])]
    scores = cumsum(fields[2:end])
    last_round = findfirst(v -> any(>=(1000), v), scores)
    winner = argmax(scores[last_round])
    loser_score = winner == 1 ? scores[last_round - 1][2] : scores[last_round][1]
    total_rolls = 3 * (2 * (last_round - 1) + winner)
    return total_rolls * loser_score
end

function q2(p1::Integer, p2::Integer)::Int
    (s1, s2) = quantum_scores.((p1, p2))
    return foldl(2:size(s1, 1); init = (0, 0)) do (w1, w2), r
        # p1 reaches 21 exactly after r rounds this many times
        p1w = sum(s1[r, 21:end]) - 27 * sum(s1[r - 1, 21:end])
        # p2 stays below 21 after r rounds this many times
        p2l = sum(s2[r - 1, 1:20])
        # vice versa
        p2w = sum(s2[r, 21:end]) - 27 * sum(s2[r - 1, 21:end])
        p1l = sum(s1[r, 1:20])
        # count wins
        return (w1 + p1w * p2l, w2 + p2w * p1l)
    end |> maximum
end

function quantum_scores(start::Integer; rounds = 10, max_score = 90)
    rolls = [3 => 1, 4 => 3, 5 => 6, 6 => 7, 7 => 6, 8 => 3, 9 => 1]

    # states[r, f, s] = number of parallel states after a solo game of `r - 1`
    #   rounds ((r - 1) * 3 rolls) end on field `f`, with a score of `s`.
    states = fill(0, rounds + 1, 10, max_score + 1)
    states[1, start, 1] = 1
    for round in 1:rounds, score in 1:max_score, field in 1:min(score, 10)
        states[round + 1, field, score + 1] =
            sum([count * states[round, rot(field - roll), score + 1 - field]
                 for (roll, count) in rolls])
    end

    # clean, check and trim
    r = reshape(sum(states[2:end, :, 2:end]; dims = 2), rounds, max_score)
    @assert all(iszero, r[end, 1:20]) "Round count too low, not all games end!"
    @assert sum(r[end, :]) == 27 ^ rounds "Max score too low!"
    return r[CartesianIndex(1, 1):findlast(!iszero, r)]
end

@assert q1(test...) == 739785
@assert q2(test...) == 444356092776315

println("Q1: ", q1(puzzle...))
println("Q2: ", q2(puzzle...))
