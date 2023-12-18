using Test: @testset, @test

test = """
       R 6 (#70c710)
       D 5 (#0dc571)
       L 2 (#5713f0)
       D 2 (#d2c081)
       R 2 (#59c680)
       D 2 (#411b91)
       L 5 (#8ceee2)
       U 2 (#caa173)
       L 1 (#1b58a2)
       U 2 (#caa171)
       R 2 (#7807d2)
       U 3 (#a77fa3)
       L 2 (#015232)
       U 2 (#7a21e3)
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    return map(lines) do line
        (d1, l1, c) = split(line)
        c = strip(c, collect("#()"))
        (d2, l2) = ("RDLU"[c[6] - '0' + 1], parse(Int, c[1:5]; base = 16))
        return ((Symbol(d1), parse(Int, l1)), (Symbol(d2), l2))
    end
end

q1(input) = area(first.(input))

q2(input) = area(last.(input))

CI = CartesianIndex{2}

move = Dict(:R => CI(0, 1), :D => CI(1, 0), :L => CI(0, -1), :U => CI(-1, 0))
shift = Dict(:R => CI(1, 0), :D => CI(0, 0), :L => CI(0, 0), :U => CI(0, 1))

function area(dig::AbstractVector{Tuple{Symbol, Int}})
    path = [CI(1, 1)]
    for (d, l) in dig
        push!(path, path[end] + move[d] * l)
    end
    sa = sign(area(path))
    grown = map(path, [dig[end]; dig[1:end - 1]], dig) do p, (d1, _), (d2, _)
        return p + sa * (shift[d1] + shift[d2])
    end
    return sa * area(grown)
end

function area(path::AbstractVector{CI})
    return mapreduce(+, path[1:end], [path[2:end]; path[1]]) do p1, p2
        (y1, x1) = p1.I
        (y2, x2) = p2.I
        return (x2 - x1) * (y1 + y2) ÷ 2
    end
end

if !isinteractive()
    @testset begin
        lines = filter!(!isempty, split(test, '\n'))
        input = parse_input(lines)
        @test q1(input) == 62
        @test q2(input) == 952408144115
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
