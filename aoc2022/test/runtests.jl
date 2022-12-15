using aoc2022
using Test

@testset "union" begin
    @test union([1:2], 4:5) == [1:2, 4:5]
    @test union([4:5], 1:2) == [1:2, 4:5]
    @test union([1:2], 3:5) == [1:5]
    @test union([1:2], 0:2) == [0:2]
    @test union([1:2, 5:6], 3:4) == [1:6]
    @test union([1:2, 7:8], 4:5) == [1:2, 4:5, 7:8]
    @test union([1:2, 7:8], 3:5) == [1:5, 7:8]
    @test union([1:2, 7:8], 4:6) == [1:2, 4:8]
end
