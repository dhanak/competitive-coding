import Base: isequal, isless
using Test: @testset, @test

test = """[1,1,3,1,1]
[1,1,5,1,1]

[[1],[2,3,4]]
[[1],4]

[9]
[[8,7,6]]

[[4,4],4,4]
[[4,4],4,4,4]

[7,7,7,7]
[7,7,7]

[]
[3]

[[[]]]
[[]]

[1,[2,[3,[4,[5,6,7]]]],8,9]
[1,[2,[3,[4,[5,6,0]]]],8,9]"""

function parse_input(lines)
    return map(Iterators.partition(lines, 3)) do (left, right, blank...)
        @assert all(isempty, blank)
        return [left, right] .|> Meta.parse .|> eval
    end
end

isequal(a::Int, b::Vector) = [a] == b
isequal(a::Vector, b::Int) = a == [b]
isless(a::Int, b::Vector) = [a] < b
isless(a::Vector, b::Int) = a < [b]

q1(pairs) = sum(findall(Base.splat(<), pairs))

function q2(pairs)::Int
    packets = vcat([[[2]], [[6]]], pairs...)
    return prod(invperm(sortperm(packets))[1:2])
end

@testset begin
    v = parse_input(split(test, '\n'))
    @test q1(v) == 13
    @test q2(v) == 140
end

v = parse_input(readlines("day13.in"))
println("Q1: ", q1(v))
println("Q2: ", q2(v))
