using Test: @testset, @test

test = """
       ...........
       .....###.#.
       .###.##..#.
       ..#.#...#..
       ....#.#....
       .##..S####.
       .##..#...#.
       .......##..
       .##.#.####.
       .##..##.##.
       ...........
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    garden = mapreduce(collect, hcat, lines) |> permutedims
    start = findfirst(==('S'), garden)
    garden[start] = '.'
    return (start, garden)
end

CI = CartesianIndex{2}

moves = [CI(-1, 0), CI(0, 1), CI(1, 0), CI(0, -1)]

flood((start, garden), n::Integer) = flood(garden, start, n)

function flood(garden, start::CI, n::Integer)
    steps = [(start, 0)]
    for (p, i) in steps
        i == n && break
        for m in moves
            pm = p + m
            if get(garden, pm, '#') == '.' && !any(==(pm) ∘ first, steps)
                push!(steps, (pm, i + 1))
            end
        end
    end
    return [p for (p, i) in steps if i % 2 == n % 2]
end

floodc(args...) = length(flood(args...))

q1((start, garden), n::Integer) = floodc(garden, start, n)

function q2((start, garden), steps::Integer)
    @assert all(==('.'), garden[:, start[2]])
    @assert all(==('.'), garden[start[1], :])
    @assert isodd(steps)
    s = size(garden, 1)
    @assert isodd(s) && size(garden) == (s, s)
    m = s ÷ 2
    @assert start == CI(m + 1, m + 1)
    @assert (steps - m) % s == 0
    n = (steps - m) ÷ s
    @assert iseven(n)

    odds  = floodc(garden, start, s)
    @assert odds == floodc(garden, start, s + 2)
    evens  = floodc(garden, start, s - 1)
    @assert evens == floodc(garden, start, s + 1)
    full = (n - 1) ^ 2 * odds + n ^ 2 * evens

    corners = sum([CI(1, 1), CI(1, s), CI(s, 1), CI(s, s)]) do p
        small = floodc(garden, p, m - 1)
        large = floodc(garden, p, s + m - 1)
        return n * small + (n - 1) * large
    end

    sides = sum([CI(1, m + 1), CI(m + 1, 1), CI(m + 1, s), CI(s, m + 1)]) do p
        return floodc(garden, p, s - 1)
    end

    return full + corners + sides
end

@static if isinteractive()

    function enlarge((start, garden), by::Integer)
        return (start + CI(size(garden)) * by,
                repeat(garden, 2by + 1, 2by + 1))
    end

    draw_flood((start, garden), n::Integer) = draw_flood(garden, start, n)

    function draw_flood(garden, start::CI, n::Integer)
        steps = flood(garden, start, n)
        gd = copy(garden)
        gd[steps] .= 'O'
        println(join(join.(eachrow(gd)), '\n'))
        println("Filled: ", length(steps))
    end

else

    @testset begin
        lines = filter!(!isempty, split(test, '\n'))
        input = (start, garden) = parse_input(lines)
        @test q1(input, 6) == 16
        garden[start[1], :] .= '.'
        garden[:, start[2]] .= '.'
        @test q2(input, 27) == 588
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input, 64))
        println("Q2: ", q2(input, 26501365))
    end

end
