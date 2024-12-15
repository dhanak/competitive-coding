using Test: @testset, @test

using aoc2024
using Combinatorics: combinations

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
    Qs = [(0, mx - 1) (mx + 1, w - 1)] .=> [(0, my - 1), (my + 1, h - 1)]
    quadrants = map(Qs) do ((xmin, xmax), (ymin, ymax))
        return count(final) do (x, y)
            return xmin <= x <= xmax && ymin <= y <= ymax
        end
    end
    return prod(quadrants)
end

function q2(robots)
    d = (w, h) = (101, 103)
    # find cycles in x and y directions when robots condense
    freqs = (fill(0, w), fill(0, h))
    for ((pa, va), (pb, vb)) in combinations(robots, 2)
        # find t where robots and b coincide (per coordinate)
        va = @. (va + d) % d
        vb = @. (vb + d) % d
        any(@. va == vb) && continue
        # a and b coincide at t, i.e.: pa + t * va = pb + t * vb
        t = @. ((d + pb - pa) * invmod(d + va - vb, d)) % d
        # count occurrence frequencies of t (per coordinate)
        map(t, freqs) do v, c
            c[v + 1] += 1
        end
    end

    # get most frequent values, i.e., where robots are the densest
    (tx, ty) = @. argmax(freqs) - 1

    # find t where tx and ty axial offsets with cycle (w, h) coincide
    # tx + a*w == ty + b*h
    ((ma, ca), (mb, cb)) = diophantine(w, -h, ty - tx)
    @assert ca > 0 && cb > 0
    k = ca ÷ abs(ma)
    a = ma * k + ca
    b = mb * k + cb
    @assert a > 0 && b > 0 && a - abs(ma) < 0 && b - abs(mb) < 0
    t = tx + a * w
    @assert t == ty + b * h

    # draw (just to be sure that we have the correct answer)
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
