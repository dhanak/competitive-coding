using Test: @testset, @test

using aoc2024

test = """
       p=0,4 v=3,-3
       p=6,3 v=-1,-3
       p=10,3 v=-1,2
       p=2,0 v=2,-1
       p=0,0 v=1,3
       p=3,0 v=-2,-2
       p=7,6 v=-1,-3
       p=3,0 v=-1,-2
       p=9,3 v=2,3
       p=7,3 v=-1,2
       p=2,4 v=2,-3
       p=9,5 v=-3,-3
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    return map(lines) do line
        (p, v) = split(line, ' ')
        return map((p, v)) do s
            (x, y) = split(s, r"=|,")[2:3]
            parse.(Int, (x, y))
        end
    end
end

function q1(robots, d)
    (w, h) = d
    t = 100
    final = map(robots) do (p, v)
        return @. (p + t * (d + v)) % d
    end
    (mx, my) = (w ÷ 2, h ÷ 2)
    quadrants = reduce(final; init = fill(0, 4)) do qs, (x, y)
        q = if y < my
            x < mx ? 1 : x > mx ? 2 : 0
        elseif y > my
            x < mx ? 3 : x > mx ? 4 : 0
        else
            0
        end
        if q > 0
            qs[q] += 1
        end
        return qs
    end
    return prod(quadrants)
end

function q2(robots)
    d = (w, h) = (101, 103)
    # I observed these values manually by dumping the first 200 pictures:
    (x0, y0) = (14, 94)
    # x0 + a*w == y0 + b*h
    ((ma, ca), (mb, cb)) = diophantine(w, -h, y0 - x0)
    @assert ca > 0 && cb > 0
    k = ca ÷ abs(ma)
    a = ma * k + ca
    b = mb * k + cb
    @assert a > 0 && b > 0 && a - abs(ma) < 0 && b - abs(mb) < 0
    t = x0 + a * w
    @assert t == y0 + b * h
    final = map(robots) do (p, v)
        return @. (p + t * (d + v)) % d
    end
    for y = 1:h
        for x = 1:w
            print((x, y) ∈ final ? '#' : ' ')
        end
        println()
    end
    return t
end

function diophantine(a, b, c)
    (d, u, v) = gcdx(a, b)
    x0 = u * Int(c / d)
    y0 = v * Int(c / d)
    return ((b ÷ d, x0), (-a ÷ d, y0))
end

if !isinteractive()
    @testset begin
        lines = split(test, '\n'; keepempty = false)
        input = parse_input(lines)
        @test q1(input, (11, 7)) == 12
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input, (101, 103)))
        println("Q2: ", q2(input))
    end
end
