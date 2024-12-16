using Test: @testset, @test

using aoc2024

## A somewhat faster but less compact (and less declarative) variant.

test = """
       ....#.....
       .........#
       ..........
       ..#.......
       .......#..
       ..........
       .#..^.....
       ........#.
       #.........
       ......#...
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    M::Matrix{Char} = lines
    guard = only(findall(==('^'), M))
    return (M, guard)
end

q1(input) = count(==('X'), solve(input...))

function q2((arena, guard))::Int
    return count(findall(==('X'), solve(arena, guard))) do p
        arena[p] = '#'
        loops = solve(arena, guard) == :loop
        arena[p] = '.'
        return loops
    end
end

function solve(arena, guard)
    arena = copy(arena)
    dir = 1
    visited = Set{Tuple{CI, Int}}()
    while true
        (guard, dir) ∈ visited && return :loop
        push!(visited, (guard, dir))
        arena[guard] = 'X'
        next = guard + neighbors4[dir]
        if next ∉ keys(arena)
            return arena
        elseif arena[next] == '#'
            dir = dir % 4 + 1
        else
            guard = next
        end
    end
end

if !isinteractive()
    @testset begin
        lines = split(test, '\n'; keepempty = false)
        input = parse_input(lines)
        @test q1(input) == 41
        @test q2(input) == 6
    end

    @time begin
        lines = readlines("day06.in")
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
