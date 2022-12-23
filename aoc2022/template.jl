using Test: @testset, @test

test = """
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    return
end

q1(v) = 0
q2(v) = 0

@testset begin
    v = parse_input(split(test, '\n'))
    @test q1(v) == 0
    @test q2(v) == 0
end

@time begin
    v = parse_input(readlines("day.in"))
    println("Q1: ", q1(v))
    println("Q2: ", q2(v))
end
