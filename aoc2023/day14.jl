using Test: @testset, @test

test = """
       O....#....
       O.OO#....#
       .....##...
       OO.#O....O
       .O.....O#.
       O.#..O.#.#
       ..O..#O..O
       .......O..
       #....###..
       #OO..#....
       """

const CI = CartesianIndex{2}

function parse_input(lines::AbstractVector{<: AbstractString})
    puzzle = mapreduce(collect, hcat, lines) |> permutedims |> wrap
    return (puzzle .== '#', findall(==('O'), puzzle))
end

function wrap(puzzle)
    (h, w) = size(puzzle)
    return [       fill('#', 1, w + 2)
            fill('#', h) puzzle fill('#', h)
                   fill('#', 1, w + 2)]
end

function q1((land, spheres))
    return score(size(land, 1), roll(land, spheres, :n))
end

function q2((land, spheres))
    past = Dict()
    for i in 1:typemax(Int)
        spheres = roll(land, spheres)
        j = get!(past, spheres, i)
        if i != j
            l = i - j
            k = (j - 1) + (1_000_000_000 - (j - 1)) % l
            spheres = findall(==(k), past)[]
            return score(size(land, 1), spheres)
        end
    end
end

function roll(land::BitMatrix, spheres::Vector{CI}, dir::Symbol)::Vector{CI}
    dims = (dir ∈ [:n, :s]) + 1
    return mapreduce(vcat, eachslice(keys(land); dims)) do slice
        struts = filter(i -> land[i], slice)
        mapreduce(vcat, range.(struts[1:end - 1], struts[2:end])) do r
            n = count(∈(r), spheres)
            return dir ∈ [:n, :w] ? r[2:n + 1] : r[end - n:end - 1]
        end
    end
end

function roll(land::BitMatrix, spheres::Vector{CI})::Vector{CI}
    return foldl([:n, :w, :s, :e]; init = spheres) do spheres, dir
        return roll(land, spheres, dir)
    end
end

function score(h::Integer, spheres::Vector{CI})
    return sum(spheres) do sphere
        return h - sphere[1]
    end
end

if !isinteractive()
    @testset begin
        lines = filter!(!isempty, split(test, '\n'))
        input = parse_input(lines)
        @test q1(input) == 136
        @test q2(input) == 64
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
