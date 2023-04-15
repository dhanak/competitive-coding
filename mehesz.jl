@enum Heading east south west north

struct Problem
    board::Matrix{Char}
    start_heading::Heading
    avoid::Vector{Char}
    gather::Vector{Char}
    goal::Char

    function Problem(; board, start_heading, avoid, gather, goal)
        new(board .|> collect |> Base.splat(hcat) |> permutedims,
            start_heading,
            avoid |> collect,
            gather |> collect,
            goal)
    end
end

const round1 = Problem(board = ["🐣  🤖 🚪"
                                "  🤖 🐤 "
                                "🤖     "
                                "  🐥🤖  "
                                "🐔    🐝"],
                       start_heading = north,
                       avoid = "🤖",
                       gather = "🐣🐤🐥🐔",
                       goal = '🚪')

const round2 = Problem(board = ["🍇   🍒🪵"
                                " 🪤🫐 🪤 "
                                "  🪤   "
                                "🪤   🪤🍓"
                                "🍑🐝🪤 🍌🪤"],
                       start_heading = east,
                       avoid = "🪤",
                       gather = "🍇🍒🫐🍓🍑🍌",
                       goal = '🪵')

const round4 = Problem(board = ["📦    "
                                "⚡ ⚡ 📦"
                                "   ⚡⚡"
                                " ⚡  📥"
                                "  ⚡ ⚡"
                                "     "
                                " ⚡🐝  "],
                       start_heading = east,
                       avoid = "⚡",
                       gather = "📦",
                       goal = '📥')

const moves = Dict(east => CartesianIndex(0, 1),
                   south => CartesianIndex(1, 0),
                   west => CartesianIndex(0, -1),
                   north => CartesianIndex(-1, 0))

forward(p, d) = (p + moves[d], d)
backward(p, d) = (p - moves[d], d)
right(p, d) = (p, Heading((Int(d) + 1) % 4))
left(p, d) = (p, Heading((Int(d) + 3) % 4))

function solve(problem::Problem)
    start_position = findfirst(==('🐝'), problem.board)
    start = (start_position, problem.start_heading)
    gather = findall(∈(problem.gather), problem.board)
    gathered = fill(false, length(gather))
    states = [(start, gathered)]
    queue = [(0, 1)]
    for (s, i) in queue
        ((p, d), g) = states[i]
        if problem.board[p] == problem.goal && all(g)
            return s
        end

        for move in [forward, backward, right, left]
            (p1, d1) = move(p, d)
            if p1 ∉ keys(problem.board) || problem.board[p1] ∈ problem.avoid
                continue
            end
            gi = findfirst(==(p1), gather)
            g1 = gi === nothing ? g : let g1 = copy(g)
                g1[gi] = true
                g1
            end
            if ((p1, d1), g1) ∉ states
                push!(states, ((p1, d1), g1))
                push!(queue, (s + 1, length(states)))
            end
        end
    end
end
