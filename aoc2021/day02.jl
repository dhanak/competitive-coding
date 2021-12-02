lines = readlines("day02.in")
(f, d, a) = reduce(split.(lines); init = (0, 0, 0)) do (f, d, a), (c, v)
    v = parse(Int, v)
    if c == "forward"
        f += v
        d += a * v
    elseif c == "down"
        a += v
    elseif c == "up"
        a -= v
    end
    return (f, d, a)
end
println("Q1: ", f * a)
println("Q2: ", f * d)
