using Test: @testset, @test

using aoc2025

test = """
..@@.@@@@.
@@@.@.@.@@
@@@@@.@.@@
@.@@@@..@.
@@.@@@@.@@
.@@@@@@@.@
.@.@.@.@@@
@.@@@.@@@@
.@@@@@@@@.
@.@.@@@.@.
"""

function parse_input(lines::AbstractVector{<:AbstractString})
    return convert(Matrix{Char}, lines)
end

function q1(grid::Matrix{Char})::Int
    grid = grow(grid, '.')
    return sum(findall(==('@'), grid)) do i
        return count(j -> grid[j] == '@', i .+ neighbors8) < 4
    end
end

function q2(grid::Matrix{Char})::Int
    rolls = Set(findall(==('@'), grid))
    removed = 0
    while true
        remove = filter(rolls) do i
            return count(∈(rolls), i .+ neighbors8) < 4
        end
        isempty(remove) && break
        removed += length(remove)
        setdiff!(rolls, remove)
    end
    return removed
end

if !isinteractive()
    @testset begin
        local lines = split(test, '\n'; keepempty = false)
        local input = parse_input(lines)
        @test q1(input) == 13
        @test q2(input) == 43
    end

    @time begin
        local lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        local input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
