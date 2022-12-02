test = """
    A Y
    B X
    C Z
"""

function parse_input(input::AbstractString)
    game = first.(split(input, r"\s+"; keepempty = false))
    return zip(findfirst.(game[1:2:end], "ABC"),
               findfirst.(game[2:2:end], "XYZ"))
end

function q1(game)
    return sum(game) do (opp, own)
        outcome = own == opp ? 3 : own == opp % 3 + 1 ? 6 : 0
        return own + outcome
    end
end

function q2(game)
    return sum(game) do (opp, outcome)
        own = outcome == 1 ? (opp + 1) % 3 + 1 : # lose
            outcome == 2 ? opp :                 # draw
            opp % 3 + 1                          # win
        return own + (outcome - 1) * 3
    end
end

let v = parse_input(test)
    @assert q1(v) == 15
    @assert q2(v) == 12
end

v = parse_input(read("day02.in", String))
println("Q1: ", q1(v))
println("Q2: ", q2(v))
