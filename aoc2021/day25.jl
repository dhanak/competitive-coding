using Base: splat

test = """
    v...>>.vv>
    .vv>>.vv..
    >>.>v>...v
    >>v>>.>.v.
    v>v.vv.v..
    >.>>..v...
    .vv..>.>v.
    v.v..>>v.v
    ....v..v.>
    """

function parse_input(input::AbstractString)
    lines = split.(split(input, '\n'; keepempty = false), "") |>
        splat(hcat) |> permutedims
    return map(lines) do c
        return c == ">" ? 1 : c == "v" ? 2 : 0
    end
end

wrap_around(i, s) = (i - 1) % s + 1
right(C, i) = CartesianIndex(i[1], wrap_around(i[2] + 1, size(C, 2)))
down(C, i) = CartesianIndex(wrap_around(i[1] + 1, size(C, 1)), i[2])

function move!(C, dir)::Bool
    m = [right, down][dir]
    ri = findall(keys(C)) do i
        C[i] == dir && C[m(C, i)] == 0
    end
    isempty(ri) && return false
    C[ri] .= 0
    C[m.(Ref(C), ri)] .= dir
    return true
end

function step!(C)::Bool
    rm = move!(C, 1)
    dm = move!(C, 2)
    return rm || dm
end

function run(C)
    C = copy(C)
    s = 1
    while step!(C)
        s += 1
    end
    return s
end

q1(C) = run(C)

let C = parse_input(test)
    @assert q1(C) == 58
end

C = parse_input(read("day25.in", String))
println("Q1: ", q1(C))
