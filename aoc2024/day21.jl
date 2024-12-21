using Test: @testset, @test

using aoc2024
using Memoize: @memoize

test = """
       029A
       980A
       179A
       456A
       379A
       """

const _N_ = nothing

const keypad_1 = ['7' '8' '9'
                  '4' '5' '6'
                  '1' '2' '3'
                  _N_ '0' 'A']

const keypad_2 = [_N_ '^' 'A'
                  '<' 'v' '>']

q1(codes) = solve(codes, 3)

q2(codes) = solve(codes, 26)

function type(code::AbstractString, keypad::AbstractMatrix, from::Char = 'A')
    ks = Dict(keypad[p] => p for p in keys(keypad))
    (seq, _) = reduce(code; init = ("", from)) do (s, a), b
        d = (ks[b] - ks[a]).I
        (v, h) = map(d, ['v', '>'], ['^', '<']) do x, inc, dec
            (x >= 0 ? inc : dec) ^ abs(x)
        end
        # 1. don't pass through gap
        # 2. < is more expensive (need to go farther) than everything else
        # 3. ^ and v are more expensive than >
        # Push more expensive keys first, it will be cheaper in the next round
        # (i.e., go to the far left and then come back gradually)
        arrows = keypad[ks[a][1], ks[b][2]] === nothing ? v * h :
                 keypad[ks[b][1], ks[a][2]] === nothing ? h * v :
                 '<' ∈ h ? h * v :
                 v * h
        return (s * arrows * 'A', b)
    end
    return seq
end

const shifts = let dirs = "^v<>A"
    Dict((a, b) => type(string(b), keypad_2, a) for a in dirs, b in dirs)
end

function solve(codes, N)
    return sum(codes) do code
        len = type_length(type(code, keypad_1), N)
        val = parse(Int, code[1:end - 1])
        return len * val
    end
end

function type_length(code::AbstractString, level::Int)
    return reduce(code; init = (0, 'A')) do (len, a), b
        return (len + type_length(a, b, level), b)
    end |> first
end

@memoize function type_length(from::Char, to::Char, level::Int)::Int
    return level == 1 ? 1 : type_length(shifts[from, to], level - 1)
end

if !isinteractive()
    @testset begin
        input = split(test, '\n'; keepempty = false)
        @test q1(input) == 126384
    end

    @time begin
        input = readlines(replace(@__FILE__, ".jl" => ".in"))
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
