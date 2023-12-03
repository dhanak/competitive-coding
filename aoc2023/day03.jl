using Test: @testset, @test

test = """
       467..114..
       ...*......
       ..35..633.
       ......#...
       617*......
       .....+.58.
       ..592.....
       ......755.
       ...%.*....
       .664.598..
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    return hcat((only.(split(line, "")) for line in lines)...) |> permutedims
end

function neighbors(ci::CartesianIndex)
    return setdiff((ci - one(ci)):(ci + one(ci)), [ci])
end

function unique_numbers(schematic::Matrix{Char},
                        idxs::AbstractVector{<: CartesianIndex}
                       )::Vector{<: CartesianIndices}
    candidates = filter!(idxs ∩ keys(schematic)) do i
        return isdigit(schematic[i])
    end
    return map(candidates) do i
        (r, c) = i.I
        s = findlast(!isdigit, ['.'; schematic[r, 1:c]])
        e = findfirst(!isdigit, [schematic[r, c:end]; '.']) + c - 2
        @assert all(isdigit, schematic[r, s:e])
        return CartesianIndex(r, s):CartesianIndex(r, e)
    end |> unique!
end

function get_numbers(schematic::Matrix{Char},
                     idxs::Vector{<: CartesianIndices}
                    )::Vector{Int}
    return map(idxs) do i
        parse(Int, join(schematic[i], ""))
    end
end

function q1(schematic::Matrix{Char})::Int
    symbols = findall(∉(['0':'9'; '.']), schematic)
    candidates = mapreduce(neighbors, union, symbols)
    numbers = unique_numbers(schematic, candidates)
    return sum(get_numbers(schematic, numbers))
end

function q2(schematic::Matrix{Char})::Int
    return sum(findall(==('*'), schematic)) do gear
        numbers = unique_numbers(schematic, neighbors(gear))
        return length(numbers) == 2 ? prod(get_numbers(schematic, numbers)) : 0
    end
end

isinteractive() || begin
    @testset begin
        lines = filter!(!isempty, split(test, '\n'))
        input = parse_input(lines)
        @test q1(input) == 4361
        @test q2(input) == 467835
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
