using Base: @kwdef
using Base.Iterators: cycle, take
using Test: @testset, @test

rocks = ["####",
         ".#.|###|.#.",
         "..#|..#|###",
         "#|#|#|#",
         "##|##"]

test = ">>><<><>><<<>><>>><<<>>><<<><<<>><>><<>>"

function parse_input(input::AbstractString)
    rs = map(rocks) do rock
        R = permutedims(hcat(collect.(split(rock, '|'))...))
        return replace(R, '#' => 1, '.' => 0)
    end
    ms = replace(collect(input), '>' => 1, '<' => -1)
    return (rs, ms)
end

function collide(rock, chamber, x, y)::Bool
    (x <= 0 || x + size(rock, 2) - 1 > size(chamber, 2)) && return true
    y + size(rock, 1) - 1 > size(chamber, 1) && return true
    (b, r) = (y - 1 , x - 1) .+ size(rock)
    any(>(1), rock + chamber[y:b, x:r]) && return true
    return false
end

function drop_rocks(check, rocks, moves)::Int
    j = 1
    chamber = fill(0, 4, 7)
    for (step, rock) in enumerate(cycle(rocks))
        y = something(findfirst(∋(1), eachrow(chamber)), 5) - size(rock, 1) - 3
        if y <= 0
            chamber = [fill(0, 1 - y, 7); chamber]
            y = 1
        end
        x = 3
        while true
            jet = moves[j]
            j = j % length(moves) + 1
            if !collide(rock, chamber, x + jet, y)
                x += jet
            end
            if collide(rock, chamber, x, y + 1)
                break
            end
            y += 1
        end
        (b, r) = (y - 1, x - 1) .+ size(rock)
        chamber[y:b, x:r] += rock
        h = size(chamber, 1) - findfirst(∋(1), eachrow(chamber)) + 1
        result = check(step, j, h)
        result === nothing || return result
    end
end

q1(rocks, moves) = drop_rocks(rocks, moves) do step, ji, h
    return step == 2022 ? h : nothing
end

@kwdef struct State
    heights::Vector{Int} = Int[]
    cache::Dict{Int, Tuple} = Dict{Int, Tuple}()
end

function (state::State)(s::Int, j::Int, h::Int)
    push!(state.heights, h)
    if (s - 1) % length(rocks) == 0
        (s2, h2, Δh2) = get(state.cache, j, (0, 0, 0))
        Δh = h - h2
        if Δh == Δh2
            Δs = s - s2
            l = 1_000_000_000_000
            intro = s2 - 1
            outro = (l - intro) % Δs
            return (l - intro) ÷ Δs * Δh + state.heights[intro + outro]
        end
        state.cache[j] = (s, h, Δh)
    end
    return nothing
end

q2(rocks, moves) = drop_rocks(State(), rocks, moves)

@testset begin
    (rs, ms) = parse_input(test)
    @test q1(rs, ms) == 3068
    @test q2(rs, ms) == 1514285714288
end

(rs, ms) = parse_input(strip(read("day17.in", String)))
println("Q1: ", q1(rs, ms))
println("Q2: ", q2(rs, ms))
