using Test: @testset, @test

using Base.Iterators: partition

test = "2333133121414131402"

function parse_input(line::AbstractString)::Vector{NTuple{3, Int}}
    parts = partition(collect(line * "0") .- '0', 2)
    files = []
    reduce(parts; init = (0, 0)) do (p, i), (f, s)
        push!(files, (i, p, p + f))
        return (p + f + s, i + 1)
    end
    return files
end

function q1(files)::Int
    files = copy(files)
    i = 2
    while i <= length(files)
        (a, b) = (files[i - 1][3], files[i][2])
        space = b - a
        if space == 0
            i += 1
        else
            (fi, c, d) = files[end]
            if c < d - space
                # split
                insert!(files, i, (fi, a, b))
                files[end] = (fi, c, d - space)
            else
                # move
                insert!(files, i, (fi, a, a + d - c))
                pop!(files)
            end
        end
    end
    return score(files)
end

function q2(files)::Int
    files = copy(files)
    i = length(files)
    while i > 1
        (fi, c, d) = files[i]
        j = findfirst(2:i) do j
            space = files[j][2] - files[j - 1][3]
            return space >= d - c
        end
        if j === nothing
            i -= 1
        else
            j = (2:i)[j]
            p = files[j - 1][3]
            insert!(files, j, (fi, p, p + d - c))
            deleteat!(files, i + 1)
        end
    end
    return score(files)
end

function score(files)::Int
    return sum(files) do (i, a, b)
        return i * ((a + b - 1) * (b - a) ÷ 2)
    end
end

if !isinteractive()
    @testset begin
        input = parse_input(test)
        @test q1(input) == 1928
        @test q2(input) == 2858
    end

    @time begin
        line = only(readlines(replace(@__FILE__, ".jl" => ".in")))
        input = parse_input(line)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
