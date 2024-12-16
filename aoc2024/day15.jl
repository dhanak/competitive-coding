using Test: @testset, @test

using aoc2024

test = """
       ##########
       #..O..O.O#
       #......O.#
       #.OO..O.O#
       #..O@..O.#
       #O#..O...#
       #O..O..O.#
       #.OO.O.OO#
       #....O...#
       ##########

       <vv>^<v^>v>^vv^v>v<>v^v<v<^vv<<<^><<><>>v<vvv<>^v^>^<<<><<v<<<v^vv^v>^
       vvv<<^>^v^^><<>>><>^<<><^vv^^<>vvv<>><^^v>^>vv<>v<<<<v<^v>^<^^>>>^<v<v
       ><>vv>v^v^<>><>>>><^^>vv>v<^^^>>v^v^<^^>v^^>v^<^v>v<>>v^v^<v>v^^<^^vv<
       <<v<^>>^^^^>>>v^<>vvv^><v<<<>^^^vv^<vvv>^>v<^^^^v<>^>vvvv><>>v^<<^^^^^
       ^><^><>>><>^^<<^^v>>><^<v>^<vv>>v>>>^v><>^v><<<<v>>v<v<v>vvv>^<><<>^><
       ^>><>^v<><^vvv<^^<><v<<<<<><^v<<<><<<^^<v<^^^><^>>^<v^><<<^>>^v<v^v<v^
       >^>>^v>vv>^<<^v<>><<><<v<<v><>v<^vv<<<>^^v^>^^>>><<^v>>v^v><^^>>^<>vv^
       <><^^>^^^<><vvvvv^v<v<<>^v<v>v<<^><<><<><<<^^<<<^<<>><<><^^^>^^<>^>v<>
       ^^>vv<^v^v<vv>^<><v<^v>^^^>>>^^vvv^>vvv<>>>^<^>>>>>^<<^v>^vvv<>^<><<v>
       v^^>>><<^^<>>^v^<v^vv<>v^<<>^<^v^v><^<<<><<^<v><v<>vv>>v><v^<vv<>v^<<^
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    br = findfirst(isempty, lines)
    M::Matrix{Char} = lines[1:br - 1]
    boxes = findall(==('O'), M)
    robot = findall(==('@'), M) |> only
    M[[boxes; robot]] .= '.'

    moves = map(collect(join(lines[br + 1:end]))) do c
        return neighbors4[findfirst(c, "^>v<")]
    end
    return (M, boxes, robot, moves)
end

function q1((M, boxes, robot, moves))
    M = copy(M)
    M[boxes] .= 'O'
    s = maximum(size(M))
    for d in moves
        i = findfirst(1:s) do i
            M[robot + i * d] ∈ ['.', '#']
        end
        M[robot + i * d] == '#' && continue
        for j = i:-1:1
            M[robot + j * d] = M[robot + (j - 1) * d]
            @assert M[robot + j * d] == (j == 1 ? '.' : 'O')
        end
        robot += d
    end
    return sum(findall(==('O'), M)) do b
        return (b[1] - 1) * 100 + (b[2] - 1)
    end
end

function q2((M, boxes, robot, moves))
    (M, (robot, boxes...)) = enlarge(M, [robot; boxes])
    boxes = Set(boxes)
    for d in moves
        wall = false
        push = [robot + d]
        moved_boxes = CI[]
        for p in push
            if M[p] == '#'
                wall = true
                break
            end
            b = [p - CI(0, 1), p] ∩ boxes
            isempty(b) && continue
            b = only(b)
            push!(moved_boxes, b)
            if d[1] == 0
                push!(push, p + 2d)
            else
                append!(push, [b + d, b + CI(0, 1) + d])
            end
        end
        wall && continue
        unmoved_moxes = setdiff(boxes, moved_boxes)
        boxes = unmoved_moxes ∪ map(p -> p + d, moved_boxes)
        robot += d
    end
    return sum(boxes) do b
        return (b[1] - 1) * 100 + (b[2] - 1)
    end
end

function enlarge(M, points)
    E = hcat(repeat(eachcol(M); inner = 2)...)
    points = map(p -> CI(p[1], 2p[2] - 1), points)
    return (E, points)
end

function draw(M, boxes, robot)
    M = copy(M)
    for p in boxes
        M[p] = '['
        M[p + CI(0, 1)] = ']'
    end
    M[robot] = '@'
    println.(join.(eachrow(M), ""))
    return nothing
end

if !isinteractive()
    @testset begin
        lines = split(test, '\n')
        input = parse_input(lines)
        @test q1(input) == 10092
        @test q2(input) == 9021
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
