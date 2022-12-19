using LinearAlgebra
using Test: @testset, @test

test = """Blueprint 1: Each ore robot costs 4 ore. Each clay robot costs 2 ore. Each obsidian robot costs 3 ore and 14 clay. Each geode robot costs 2 ore and 7 obsidian.
Blueprint 2: Each ore robot costs 2 ore. Each clay robot costs 3 ore. Each obsidian robot costs 3 ore and 8 clay. Each geode robot costs 3 ore and 12 obsidian."""

line_re =r"^Blueprint ([0-9]+): Each ore robot costs ([0-9]+) ore. Each clay robot costs ([0-9]+) ore. Each obsidian robot costs ([0-9]+) ore and ([0-9]+) clay. Each geode robot costs ([0-9]+) ore and ([0-9]+) obsidian.$"

function parse_input(lines)
    return map(enumerate(lines)) do (i, line)
        m = match(line_re, line)
        @assert m !== nothing "Unexpected line '$line'!"
        c = parse.(Int, m.captures)
        @assert c[1] == i "Unexpected blueprint $c[1]!"
        return [c[2] c[3] c[4] c[6]
                0    0    c[5] 0
                0    0    0    c[7]
                0    0    0    0]
    end
end

function assess(blueprint::Matrix, rounds::Int)::Int
    raw = 1:size(blueprint, 1) - 1
    max_mat = maximum(blueprint; dims = 2)
    last_round = [([1, 0, 0, 0], [0, 0, 0, 0])]
    prods = eachcol(I(size(blueprint, 2)))
    for round in 1:rounds
        next_round = []
        for (robots, materials) in last_round
            for (prod, need) in zip(prods, eachcol(blueprint))
                # if we have all the materials, and won't have too many robots
                # (don't need more robots than the maximum number of materials
                # needed to manufacture any robot)...
                if all(need .<= materials) &&
                    all((max_mat .>= robots + prod)[raw])
                    # then manufacture one such robot in a new state
                    push!(next_round, (robots + prod,
                                       materials - need + robots))
                end
            end
            # if it's still meaningful to accumulate more materials...
            if any(materials .< max_mat .* sign.(robots))
                # then add no-manufacture as a new state
                push!(next_round, (robots, materials + robots))
            end
        end

        # now prune the list of new states aggressively
        empty!(last_round)
        sums = [sum.(a) for a in next_round]
        by_r = sortperm(sums; by = first)
        by_m = sortperm(sums; by = last)
        for (i, a) in enumerate(next_round)
            jr = searchsorted(by_r, i; by = j -> sums[j][1])
            jm = searchsorted(by_m, i; by = j -> sums[j][2])
            js = by_r[jr] ∪ by_m[jm]
            # all other states with the same total number of robots/materials:
            all(js .=> next_round[js]) do (j, b)
                # - must have fewer robots or materials in some stock, and
                # - shouldn't have more geodes -- NOTE this is over-pruning!
                return i == j || (!worse(a, b) && a[1][end] >= b[1][end])
            end && push!(last_round, a)
        end
    end
    return maximum(last ∘ last, last_round)
end

worse(a, b) = all(vcat(a...) .<= vcat(b...))

q1(blueprints) = sum(enumerate(blueprints)) do (i, bp)
    return i * assess(bp, 24)
end

q2(blueprints) = prod(blueprints[1:min(3, end)]) do bp
    return assess(bp, 32)
end

@testset begin
    v = parse_input(split(test, '\n'))
    @test q1(v) == 1 * 9 + 2 * 12
    @test q2(v) == 56 * 62
end

v = parse_input(readlines("day19.in"))
println("Q1: ", q1(v))
println("Q2: ", q2(v))
