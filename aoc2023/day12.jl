using Test: @testset, @test

test = """
       ???.### 1,1,3
       .??..??...?##. 1,1,3
       ?#?#?#?#?#?#?#? 1,3,1,6
       ????.#...#... 4,1,1
       ????.######..#####. 1,6,5
       ?###???????? 3,2,1
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    return map(lines) do line
        (springs, damaged) = split(line)
        return (springs, parse.(Int, split(damaged, ',')))
    end
end

function q1(input)
    return sum(solve, input)
end

function solve((springs, (d1, damaged...)))
    init = Int[fits(springs, 1, e, d1) for e in keys(springs)]
    v = foldl(damaged; init) do v, d
        return map(keys(springs)) do e
            valid = filter(s -> fits(springs, s + 2, e, d), 1:e)
            return sum(v[valid])
        end
    end
    i = something(findlast('#', springs), 1)
    return sum(v[i:end])
end

"""
Return true if a `d` long '#' sequence fits snugly at the end of `springs[a:b]`,
such that:
* it doesn't touch '#' characters at either end;
* it covers all '#' characters within the range;
* it doesn't cover any '.' characters within the range.
"""
function fits(springs::AbstractString, a::Integer, b::Integer, d::Integer)::Bool
    return (b - d + 1 >= a) &&
        get(springs, b - d, '.') != '#' &&
        get(springs, b + 1, '.') != '#' &&
        !contains(SubString(springs, a, b - d), '#') &&
        !contains(SubString(springs, b - d + 1, b), '.')
end

function q2(input)
    return map(input) do (springs, damaged)
        return (join(fill(springs, 5), '?'), repeat(damaged, 5))
    end |> q1
end

if !isinteractive()
    @testset begin
        lines = filter!(!isempty, split(test, '\n'))
        input = parse_input(lines)
        @test q1(input) == 21
        @test q2(input) == 525152
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
