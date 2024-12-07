using Test: @testset, @test

test = """
       190: 10 19
       3267: 81 40 27
       83: 17 5
       156: 15 6
       7290: 6 8 6 15
       161011: 16 10 13
       192: 17 8 14
       21037: 9 7 18 13
       292: 11 6 16 20
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    return map(lines) do line
        (result, first, rest...) = parse.(Int, split(line, r":? "))
        (result, first, rest)
    end
end

q1(input) = solve(input, [*, +])

q2(input) = solve(input, [*, +, concat])

concat(a::Int, b::Int) = a * 10 ^ ndigits(b) + b

function solve(input, ops)
    return sum([eq[1] for eq in input if solvable(ops, eq)])
end

function solvable(ops, (result, first, rest))
    opsi = fill(0, length(rest))
    part = [first]
    while !isempty(part)
        i = length(part)
        if i > length(rest) || part[end] > result
            part[end] == result && return true
            pop!(part)
        else
            opsi[i] = (opsi[i] + 1) % (length(ops) + 1)
            if opsi[i] == 0
                pop!(part)
            else
                push!(part, ops[opsi[i]](part[end], rest[i]))
            end
        end
    end
    return false
end

if !isinteractive()
    @testset begin
        lines = split(test, '\n'; keepempty = false)
        input = parse_input(lines)
        @test q1(input) == 3749
        @test q2(input) == 11387
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
