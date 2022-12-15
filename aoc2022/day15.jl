using aoc2022
using Test: @testset, @test

test = """Sensor at x=2, y=18: closest beacon is at x=-2, y=15
Sensor at x=9, y=16: closest beacon is at x=10, y=16
Sensor at x=13, y=2: closest beacon is at x=15, y=3
Sensor at x=12, y=14: closest beacon is at x=10, y=16
Sensor at x=10, y=20: closest beacon is at x=10, y=16
Sensor at x=14, y=17: closest beacon is at x=10, y=16
Sensor at x=8, y=7: closest beacon is at x=2, y=10
Sensor at x=2, y=0: closest beacon is at x=2, y=10
Sensor at x=0, y=11: closest beacon is at x=2, y=10
Sensor at x=20, y=14: closest beacon is at x=25, y=17
Sensor at x=17, y=20: closest beacon is at x=21, y=22
Sensor at x=16, y=7: closest beacon is at x=15, y=3
Sensor at x=14, y=3: closest beacon is at x=15, y=3
Sensor at x=20, y=1: closest beacon is at x=15, y=3"""

function parse_input(lines)
    return map(lines) do line
        @assert startswith(line, "Sensor at ") &&
            occursin(": closest beacon is at ", line) "Invalid line \"$line\"!"
        (sensor, beacon) = split(line,
                                 r"Sensor at |: closest beacon is at ";
                                 keepempty = false)
        return map(parse_coordinate, (; sensor, beacon))
    end
end

function parse_coordinate(coord::AbstractString)
    m = match(r"^x=([-0-9]+), y=([-0-9]+)$", coord)
    @assert m !== nothing "Invalid coordinate \"$coord\"!"
    return parse.(Int, (m[1], m[2]))
end

manhattan(a, b)::Int = sum(abs.(a .- b))

function q1(v, r::Int)::Int
    row = UnitRange{Int}[]
    occupied = Set{Int}()
    for (; sensor, beacon) in v
        d = manhattan(sensor, beacon)
        (x, y) = sensor
        Δx = d - abs(y - r)
        row = row ∪ (x - Δx:x + Δx)
        r == y && push!(occupied, x)
        r == beacon[2] && push!(occupied, beacon[1])
    end
    return length(union(row...)) - length(occupied)
end

function q2(v, s::Int)::Int
    rows = [UnitRange{Int}[] for _ in 0:s]
    for (; sensor, beacon) in v
        d = manhattan(sensor, beacon)
        (x, y) = sensor
        for r in (y - d:y + d) ∩ (0:s)
            Δx = d - abs(y - r)
            rows[r + 1] = rows[r + 1] ∪ ((x - Δx:x + Δx) ∩ (0:s))
        end
    end
    y = findfirst(row -> length(row) != 1 || row[] != 0:s, rows)
    @assert y !== nothing "All rows are full!"
    free = setdiff(0:s, rows[y]...)
    @assert length(free) == 1 "More than one free position in row $y!"
    return free[] * 4_000_000 + y - 1
end

@testset begin
    v = parse_input(split(test, '\n'))
    @test q1(v, 10) == 26
    @test q2(v, 20) == 56000011
end

v = parse_input(readlines("day15.in"))
println("Q1: ", q1(v, 2_000_000))
println("Q2: ", q2(v, 4_000_000)) # takes a few seconds, but hey, it works
