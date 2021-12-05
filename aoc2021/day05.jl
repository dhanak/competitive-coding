test = """
0,9 -> 5,9
8,0 -> 0,8
9,4 -> 3,4
2,2 -> 2,1
7,0 -> 7,4
6,4 -> 2,0
0,9 -> 2,9
3,4 -> 1,4
0,0 -> 8,8
5,5 -> 8,2
"""

function read_input(io)
    lines = readlines(io)
    numbers = hcat(split.(lines, r" -> |,")...) |> permutedims
    return parse.(Int, numbers) .+ 1
end

function board(values::Matrix; diagonal::Bool = false)
    m = maximum(values)
    M = fill(0, m, m)
    for (x1, y1, x2, y2) in eachrow(values)
        if x1 == x2 || y1 == y2
            (x1, x2) = extrema((x1, x2))
            (y1, y2) = extrema((y1, y2))
            M[y1:y2, x1:x2] .+= 1
        elseif diagonal
            for (x, y) in zip(_range(x1, x2), _range(y1, y2))
                M[y, x] += 1
            end
        end
    end
    return M
end

_range(a, b) = range(a, b; step = sign(b - a))

function count_overlaps(values::Matrix; diagonal::Bool = false)
    return count(>(1), board(values; diagonal))
end

let T = read_input(IOBuffer(test))
    @assert count_overlaps(T) == 5
    @assert count_overlaps(T; diagonal = true) == 12
end

M = read_input("day05.in")
println("Q1: ", count_overlaps(M))
println("Q2: ", count_overlaps(M; diagonal = true))
