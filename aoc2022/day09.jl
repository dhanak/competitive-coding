tests = ["""R 4
            U 4
            L 3
            D 1
            R 4
            D 1
            L 5
            R 2"""
         """R 5
            U 8
            L 8
            D 3
            R 17
            D 10
            L 25
            U 20"""]

function parse_input(lines)
    return [Symbol(line[1]) => parse(Int, line[3:end])
            for line in lines
            if !isempty(line)]
end

const TUGS = Dict(:R => (0, 2), :L => (0, -2), :U => (-2, 0), :D => (2, 0))

function tug(p, Δ)
    (sΔ, aΔ)  = (sign.(Δ), abs.(Δ))
    @assert maximum(aΔ) <= 2
    # diagonal tug
    aΔ == (2, 2) && return p .+ sΔ
    # horizontal, vertical or semi-diagonal tug
    raΔ = reverse(aΔ)
    return @. p + sΔ * (aΔ - 1 * (raΔ < 2))
end

function move_rope(moves, len)
    tails = Set()
    rope = fill((0, 0), len)
    for (dir, dist) in moves, _ in 1:dist
        pull = TUGS[dir]
        for i in keys(rope)
            rope[i] = tug(rope[i], pull)
            pull = i < length(rope) ? rope[i] .- rope[i + 1] : (0, 0)
        end
        push!(tails, rope[end])
    end
    return length(tails)
end

q1(moves) = move_rope(moves, 2)
q2(moves) = move_rope(moves, 10)

let moves = parse_input.([strip.(split(test, '\n')) for test in tests])
    @assert q1(moves[1]) == 13
    @assert q2(moves[1]) == 1
    @assert q2(moves[2]) == 36
end

moves = parse_input(readlines("day09.in"))
println("Q1: ", q1(moves))
println("Q2: ", q2(moves))
