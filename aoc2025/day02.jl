using Test: @testset, @test

using aoc2025

test = """11-22,95-115,998-1012,1188511880-1188511890,222220-222224,\
1698522-1698528,446443-446449,38593856-38593862,565653-565659,\
824824821-824824827,2121212118-2121212124"""

function parse_input(lines::AbstractVector{<:AbstractString})
    pairs = split(only(lines), ",")
    return map(pairs) do pair
        (a, b) = split(pair, "-")
        return parse(Int, a) => parse(Int, b)
    end
end

function slice(number::Integer, len::Integer)::Integer
    return number ÷ 10^(ndigits(number) - len)
end

function rep(number::Integer, count::Integer)::Integer
    p = 10^ndigits(number)
    return foldl((v, _) -> v * p + number, 1:count, init = 0)
end

function q1(ranges::AbstractVector{<:Pair})::Integer
    return sum(ranges) do (a, b)
        l = ndigits(a) ÷ 2
        if l == 0
            part = 1
            cyclic = 11
        else
            part = slice(a, l)
        end
        total = 0
        while (cyclic = rep(part, 2)) <= b
            if cyclic >= a
                total += cyclic
            end
            part += 1
        end
        return total
    end
end

function q2(ranges::AbstractVector{<:Pair})::Integer
    return sum(invalids, ranges)
end

function invalids((a, b))::Integer
    an = ndigits(a)
    bn = ndigits(b)
    an == bn && return invalids0(a, b)
    @assert an + 1 == bn "Invalid range: $a-$b"
    m = 10^an
    return invalids0(a, m - 1) + invalids0(m, b)
end

function invalids0(a::Integer, b::Integer)::Integer
    n_digits = ndigits(a)
    invalid = foldl(1:(n_digits ÷ 2); init = Int[]) do invalid, cycle_length
        n_digits % cycle_length != 0 && return invalid
        repeats = n_digits ÷ cycle_length
        cyclic = map(slice(a, cycle_length):slice(b, cycle_length)) do part
            return rep(part, repeats)
        end
        return invalid ∪ filter(id -> a <= id <= b, cyclic)
    end
    return sum(invalid; init = 0)
end

if !isinteractive()
    @testset begin
        local lines = split(test, '\n')
        local input = parse_input(lines)
        @test q1(input) == 1227775554
        @test q2(input) == 4174379265
    end

    @time begin
        local lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        local input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
