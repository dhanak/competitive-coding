using StatsBase
using Test: @testset, @test

test = """
       32T3K 765
       T55J5 684
       KK677 28
       KTJJT 220
       QQQJA 483
       """

const CARDS = split("123456789TJQKA", "")
const HANDS = ["High card"       => [1, 1, 1, 1, 1]
               "One pair"        => [1, 1, 1, 2]
               "Two pair"        => [1, 2, 2]
               "Three of a kind" => [1, 1, 3]
               "Full house"      => [2, 3]
               "Four of a kind"  => [1, 4]
               "Five of a kind"  => [5]]

function parse_input(lines::AbstractVector{<: AbstractString})
    return map(lines) do line
        (hand, bid) = split(line)
        hand = Vector{Int}(indexin(split(hand, ""), CARDS))
        return (hand, parse(Int, bid))
    end
end

function q1(hands::AbstractVector)::Int
    return score(hands, regular_rank)
end

function regular_rank(hand)
    counts = sort!(collect(values(countmap(hand))))
    strength = findfirst(==(counts) ∘ last, HANDS)
    return [strength; hand]
end

function q2(hands::AbstractVector)::Int
    return score(hands, jolly_rank)
end

function jolly_rank(hand)
    hand = replace(hand, 11 => 1)
    Js = count(==(1), hand)
    Js == 5 && return [7; hand]
    counts = sort!(collect(values(countmap(filter(!=(1), hand)))))
    counts[end] += Js
    strength = findfirst(==(counts) ∘ last, HANDS)
    return [strength; hand]
end

function score(hands::AbstractVector, rank::Function)::Int
    hands = sort(hands; by = rank ∘ first)
    return mapreduce(Base.splat(*), +, enumerate(last.(hands)))
end

isinteractive() || begin
    @testset begin
        lines = filter!(!isempty, split(test, '\n'))
        input = parse_input(lines)
        @test q1(input) == 6440
        @test q2(input) == 5905
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
