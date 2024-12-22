using Test: @testset, @test

using aoc2024

function parse_input(lines::AbstractVector{<: AbstractString})
    return parse.(Int, lines)
end

function next_number(n::Int)::Int
    a = mix_and_prune(n << 6, n)
    b = mix_and_prune(a >> 5, a)
    return mix_and_prune(b << 11, b)
end

mix_and_prune(a::Int, b::Int) = (a ⊻ b) % 16777216

function q1(input)
    return reduce(1:2000; init = input) do ns, _
        return next_number.(ns)
    end |> sum
end

function q2(input)
    # compute prices
    ns = input
    prices = fill(0, 2000, length(ns))
    for i in 1:2000
        prices[i, :] = ns .% 10
        ns = next_number.(ns)
    end
    # determine offsetted deltas
    Δ = diff(prices; dims = 1) .+ 9
    # identify prices of sequences in each column
    seqs = fill(0, 19 ^ 4, length(ns))
    (h, w) = size(Δ)
    for x in 1:w, y in 1:h - 3
        i = foldl((t, v) -> 19t + v, Δ[y:y + 3, x]; init = 0) + 1
        seqs[i, x] == 0 && (seqs[i, x] = prices[y + 4, x])
    end
    # return best total price
    return maximum(sum(seqs; dims = 2))
end

if !isinteractive()
    @testset begin
        @test q1([1, 10, 100, 2024]) == 37327623
        @test q2([1, 2, 3, 2024]) == 23
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
