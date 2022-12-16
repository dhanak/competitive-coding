using Test: @testset, @test

test = """Valve AA has flow rate=0; tunnels lead to valves DD, II, BB
Valve BB has flow rate=13; tunnels lead to valves CC, AA
Valve CC has flow rate=2; tunnels lead to valves DD, BB
Valve DD has flow rate=20; tunnels lead to valves CC, AA, EE
Valve EE has flow rate=3; tunnels lead to valves FF, DD
Valve FF has flow rate=0; tunnels lead to valves EE, GG
Valve GG has flow rate=0; tunnels lead to valves FF, HH
Valve HH has flow rate=22; tunnel leads to valve GG
Valve II has flow rate=0; tunnels lead to valves AA, JJ
Valve JJ has flow rate=21; tunnel leads to valve II"""

line_re = r"Valve ([A-Z]{2}) has flow rate=([0-9]+); tunnels? leads? to valves? ([A-Z, ]+)"

function parse_input(lines)
    valves = map(lines) do line
        m = match(line_re, line)
        @assert m !== nothing "Unexpected line '$line'!"
        valve = Symbol(m[1])
        flow_rate = parse(Int, m[2])
        tunnels = split(m[3], ", ") .|> Symbol
        return (valve, flow_rate, tunnels)
    end |> sort!
    valve_names = first.(valves)
    return full_graph([(; flow_rate, tunnels = indexin(tunnels, valve_names))
                       for (_, flow_rate, tunnels) in valves])
end

function full_graph(graph)
    return map(graph) do (flow_rate, tunnels)
        visit = tunnels .=> 1
        for (i, d) in visit
            for j in graph[i].tunnels
                all(!=(j) ∘ first, visit) && push!(visit, j => d + 1)
            end
        end
        return (; flow_rate, paths = [i => d for (i, d) in visit
                                             if graph[i].flow_rate > 0])
    end
end

function q1(valves)::Int
    visit = [(1, Int(valves[1].flow_rate > 0), valves[1].flow_rate * 29, [1])]
    for (i, time, flow, open) in visit
        for (j, d) in filter(∉(open) ∘ first, valves[i].paths)
            t = time + d + 1
            t > 30 && continue
            f = flow + valves[j].flow_rate * (30 - t)
            all(visit) do (k, tk, fk)
                return k != j || tk > t || fk < f
            end && push!(visit, (j, t, f, [open; j]))
        end
    end
    return maximum(flow for (_, _, flow) in visit)
end

function q2(valves)::Int
    visit = [((1 => Int(valves[1].flow_rate > 0), 1 => 0),
              valves[1].flow_rate * 25,
              [1])]
    pareto = Dict{Tuple, Vector}()
    for (state, flow, open) in visit
        (a, b) = map(state) do (i, time)
            paths = [(j, t, valves[j].flow_rate * (26 - t))
                     for (j, d) in valves[i].paths
                     for t = Ref(time + d + 1)
                     if j ∉ open && t <= 26]
            return isempty(paths) ? [(i, time, 0)] : paths
        end
        for ((ia, ta, fa), (ib, tb, fb)) in Base.product(a, b)
            f = flow + (ia == ib ? max(fa, fb) : fa + fb)
            all(get!(pareto, (ia, ib), [])) do (tc, td, fp)
                return tc > ta || td > tb || fp < f
            end || continue
            push!(visit, ((ia => ta, ib => tb), f, open ∪ [ia, ib]))
            push!(pareto[(ia, ib)], (ta, tb, f))
        end
    end
    return maximum(flow for (_, flow) in visit)
end

@testset begin
    v = parse_input(split(test, '\n'))
    @test q1(v) == 1651
    @test q2(v) == 1707
end

v = parse_input(readlines("day16.in"))
println("Q1: ", q1(v))
println("Q2: ", q2(v))
