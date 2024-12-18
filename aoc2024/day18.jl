using Test: @testset, @test

using aoc2024

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

function q2(bytes, n, s)
    i = findfirst(n + 1:length(bytes)) do j
        return q1(view(bytes, 1:j), s) === nothing
    end
    return join(bytes[n + i].I .- 1, ',')
end

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
