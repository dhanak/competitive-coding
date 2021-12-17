test = "target area: x=20..30, y=-10..-5"
puzzle = "target area: x=253..280, y=-73..-46"

function parse_input(input::AbstractString)
    re = r"^target area: x=(-?[0-9]+)..(-?[0-9]+), y=(-?[0-9]+)..(-?[0-9]+)$"
    m = match(re, input)
    @assert m !== nothing
    coords = parse.(Int, m.captures)
    return ((coords[3], coords[4]), (coords[1], coords[2]))
end

function q1((ymin, ymax), (xmin, xmax))
    @assert ymax < 0
    vx = round(Int, - 1/2 + sqrt(1/4 + 2xmin), RoundUp)
    vy = -ymin - 1
    @assert vx * (vx - 1) / 2 < xmin <= vx * (vx + 1) / 2
    return vy * (vy + 1) ÷ 2
end

function q2((ymin, ymax), (xmin, xmax))
    @assert xmin > 0 && ymax < 0
    vxmin = round(Int, - 1/2 + sqrt(1/4 + 2xmin), RoundUp)
    vxmax = xmax
    @assert vxmin * (vxmin - 1) / 2 < xmin <= vxmin * (vxmin + 1) / 2
    vymin = ymin
    vymax = -ymin - 1

    return count(vymin:vymax .=> (vxmin:vxmax)') do v
        return shoot(v, (ymin, ymax), (xmin, xmax))
    end
end

function shoot((vy, vx), (ymin, ymax), (xmin, xmax))::Bool
    (x, y) = (0, 0)
    while x <= xmax && y >= ymin
        x >= xmin && y <= ymax && return true
        x += vx
        y += vy
        vx = max(vx - 1, 0)
        vy -= 1
    end
    return false
end

let v = parse_input(test)
    @assert q1(v...) == 45
    @assert q2(v...) == 112
end

v = parse_input(puzzle)
println("Q1: ", q1(v...))
println("Q2: ", q2(v...))
