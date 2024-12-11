using Test: @testset, @test

using Memoize: @memoize

function parse_input(lines::AbstractVector{<: AbstractString})
    return
end

q1(v)::Int = expand(v, 25)

q2(v)::Int = expand(v, 75)

expand(v::AbstractVector{Int}, n::Int) = sum(i -> expand(i, n), v)

@memoize function expand(i::Int, n::Int)
    n == 0 && return 1
    l = ndigits(i)
    v = if i == 0
        [i + 1]
    elseif iseven(l)
        [i ÷ 10^(l ÷ 2), i % 10^(l ÷ 2)]
    else
        [i * 2024]
    end
    return expand(v, n - 1)
end

if !isinteractive()
    @testset begin
        @test q1([125, 17]) == 55312
    end

    @time begin
        line = readlines(replace(@__FILE__, ".jl" => ".in"))[]
        input = parse.(Int, split(line, " "))
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
