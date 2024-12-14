using Test: @testset, @test

using aoc2024
using Base.Iterators: partition

test = """
       Button A: X+94, Y+34
       Button B: X+22, Y+67
       Prize: X=8400, Y=5400

       Button A: X+26, Y+66
       Button B: X+67, Y+21
       Prize: X=12748, Y=12176

       Button A: X+17, Y+86
       Button B: X+84, Y+37
       Prize: X=7870, Y=6450

       Button A: X+69, Y+23
       Button B: X+27, Y+71
       Prize: X=18641, Y=10279
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    return map(partition(lines, 4)) do (a, b, p)
        (a, b) = map((a, b)) do line
            (_, x, y) = split(line, r": X\+|, Y\+")
            parse.(Int, (x, y))
        end
        (_, x, y) = split(p, r"X=|, Y=")
        return (a, b, parse.(Int, (x, y)))
    end
end

function q1(input)
    return sum(Base.splat(claw), input)
end

function q2(input)
    map(input) do (a, b, p)
        return (a, b, 10_000_000_000_000 .+ p)
    end |> q1
end

function claw((ax, ay), (bx, by), (px, py))
    try
        ((mxa, cxa), (mxb, cxb)) = diophantine(ax, bx, px) # t1
        ((mya, cya), (myb, cyb)) = diophantine(ay, by, py) # t2

        # mxa*t1 + cxa ≈ mya*t2 + cya
        #           t2 ≈ (mxa*t1 + cxa - cya)/mya

        # mxb*t1 + cxb ≈ myb*t2 + cyb ≈
        #              ≈ myb/mya*mxa*t1 + myb/mya*(cxa - cya) + cyb
        #           t1 ≈ (myb/mya*(cxa - cya) + cyb - cxb) / (mxb - myb/mya*mxa)

        t1 = round(Int, (myb/mya*(cxa - cya) + cyb - cxb) / (mxb - myb/mya*mxa))
        t2 = round(Int, (mxa*t1 + cxa - cya)/mya)
        if  mxa*t1 + cxa == mya*t2 + cya && mxb*t1 + cxb == myb*t2 + cyb
            a = mxa * t1 + cxa
            b = mxb * t1 + cxb
            return 3a + b
        else
            return 0
        end
    catch exn
        exn isa InexactError && return 0
        rethrow()
    end
end

if !isinteractive()
    @testset begin
        lines = split(test, '\n')
        input = parse_input(lines)
        @test q1(input) == 480
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
