using Test: @testset, @test

using aoc2024
using DataStructures: PriorityQueue, dequeue!

test = """
       5,4
       4,2
       4,5
       3,0
       2,1
       6,3
       2,4
       1,5
       0,6
       3,3
       2,6
       5,1
       1,2
       5,5
       2,5
       6,5
       1,4
       0,4
       6,4
       1,1
       6,1
       1,0
       0,5
       1,6
       2,0
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    return map(lines) do line
        return CI(parse.(Int, split(line, ','))...) + CI(1, 1)
    end
end

function q1(bytes, s)
    M = fill(' ', s)
    M[bytes] .= '#'
    M = grow(M, '#')
    start = CI(2, 2)
    goal = CI(s .+ 1)
    open = [(0, start)]
    while !isempty(open)
        (cost, p) = popfirst!(open)
        p == goal && return cost
        M[p] == ' ' || continue
        M[p] = 'O'
        for n in p .+ neighbors4
            M[n] == ' ' && push!(open, (cost + 1, n))
        end
    end
    return nothing
end

# quick and easy solution that tries to solve the maze once for each block
function q2_short(bytes, n, s)
    i = findfirst(n + 1:length(bytes)) do j
        return q1(view(bytes, 1:j), s) === nothing
    end
    return join(bytes[n + i].I .- 1, ',')
end

# fast solution with twoway search and priority queue based block removal
function q2_fast(bytes, _, s)
    M = fill(0, s)
    M[bytes] .= eachindex(bytes)
    M = grow(M, -1)
    blocks = PriorityQueue{CI, Int}()
    flood!(M, blocks, -2, CI(2, 2))
    flood!(M, blocks, -3, CI(s .+ 1))
    while !isempty(blocks)
        p = dequeue!(blocks)
        ns = filter(<(-1), M[p .+ neighbors4]) |> unique! |> sort!
        ns == [-3, -2] && return join(p.I .- 2, ',')
        M[p] = 0
        flood!(M, blocks, only(ns), p)
    end
end

function flood!(M, blocks, d, p)
    queue = [p]
    while !isempty(queue)
        p = popfirst!(queue)
        M[p] != 0 && continue
        M[p] = d
        for n in p .+ neighbors4
            if M[n] == 0
                push!(queue, n)
            elseif M[n] > 0
                blocks[n] = -M[n]
            end
        end
    end
end

const q2 = q2_fast

if !isinteractive()
    @testset begin
        lines = split(test, '\n'; keepempty = false)
        input = parse_input(lines)
        s = (7, 7)
        @test q1(view(input, 1:12), s) == 22
        @test q2(input, 12, s) == "6,1"
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        s = (71, 71)
        println("Q1: ", q1(view(input, 1:1024), s))
        println("Q2: ", q2(input, 1024, s))
    end
end
