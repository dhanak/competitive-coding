using Test: @testset, @test

test1 = "xmul(2,4)%&mul[3,7]!@^do_not_mul(5,5)+mul(32,64]then(mul(11,8)mul(8,5))"
test2 = "xmul(2,4)&mul[3,7]!^don't()_mul(5,5)+mul(32,64](mul(11,8)undo()?mul(8,5))"

function q1(line)
    re = r"mul\(([0-9]{1,3}),([0-9]{1,3})\)"
    muls = [parse.(Int, m.captures) for m in eachmatch(re, line)]
    sum(prod.(muls))
end

function q2(line)
    re = r"(mul)\(([0-9]{1,3}),([0-9]{1,3})\)|(do|don't)\(\)"
    (total, _) =
        reduce(eachmatch(re, line), init = (0, true)) do (total, enabled), m
            m.captures[1] === nothing && return (total, m.captures[4] == "do")
            !enabled && return (total, false)
            total += prod(parse.(Int, m.captures[2:3]))
            return (total, true)
        end
    return total
end

if !isinteractive()
    @testset begin
        @test q1(test1) == 161
        @test q2(test2) == 48
    end

    @time begin
        line = join(readlines(replace(@__FILE__, ".jl" => ".in")))
        println("Q1: ", q1(line))
        println("Q2: ", q2(line))
    end
end
