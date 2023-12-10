using Test: @testset, @test

test1 = """
        ..F7.
        .FJ|.
        SJ.L7
        |F--J
        LJ...
        """

test2 = """
        FF7FSF7F7F7F7F7F---7
        L|LJ||||||||||||F--J
        FL-7LJLJ||||||LJL-77
        F--JF--7||LJLJ7F7FJ-
        L---JF-JLJ.||-FJLJJ7
        |F|F-JF---7F7-L7L|7|
        |FFJF7L7F-JF7|JL---7
        7-L-JL7||F7|L7F-7F7|
        L.L7LFJ|||||FJL7||LJ
        L7JLJL-JLJLJL--JLJ.L
        """

CI = CartesianIndex
(U, D, R, L) = [CI(-1, 0), CI(1, 0), CI(0, 1), CI(0, -1)]
pipes = Dict('-' => [R, L],
             '|' => [U, D],
             'F' => [R, D],
             '7' => [D, L],
             'J' => [U, L],
             'L' => [U, R],
             '.' => CI[],
             'S' => [U, R, D, L])

function parse_input(lines::AbstractVector{<: AbstractString})
    land = mapreduce(collect, hcat, lines) |> permutedims
    grid = map(of(pipes), land)
    start = findfirst(==('S'), land)
    grid[start] = filter(grid[start]) do s
        I = start + s
        return I ∈ keys(grid) && start ∈ (Ref(I) .+ grid[I])
    end
    land[start] = findfirst(==(grid[start]), pipes)
    loop = get_loop(grid, start)
    land[setdiff(keys(land), loop)] .= '.'
    return (land, loop)
end

of(collection) = Base.Fix1(getindex, collection)

function get_loop(grid, start)
    loop = [start]
    I = start + grid[start][1]
    while I != start
        push!(loop, I)
        I = only(setdiff(Ref(I) .+ grid[I], [loop[end - 1]]))
    end
    return loop
end

q1((_, loop)) = length(loop) ÷ 2

function q2((land, _))
    return reduce(eachrow(land); init = 0) do total, row
        reduce(row; init = false) do inside, c
            total += inside && (c == '.')
            # crossing any lines (on the upper edge of the cell)?
            return inside ⊻ (c ∈ "|LJ")
        end
        return total
    end
end

if !isinteractive()
    @testset begin
        lines = filter!(!isempty, split(test1, '\n'))
        input = parse_input(lines)
        @test q1(input) == 8

        lines = filter!(!isempty, split(test2, '\n'))
        input = parse_input(lines)
        @test q2(input) == 10
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
