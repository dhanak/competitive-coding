using Test: @testset, @test

test = """
       89010123
       78121874
       87430965
       96549874
       45678903
       32019012
       01329801
       10456732
       """

const CI = CartesianIndex{2}

function parse_input(lines::AbstractVector{<: AbstractString})
    return mapreduce(hcat, lines) do line
        return map(c -> c - '0', collect(line))
    end |> permutedims
end

neighbors = [CI(-1, 0), CI(0, 1), CI(1, 0), CI(0, -1)]

function q1(M)
    points = findall(==(0), M)
    return sum(p -> length(trails(M, p)), points)
end

function q2(M)
    points = findall(==(0), M)
    return sum(p -> sum(values(trails(M, p))), points)
end

function trails(M, point)::Dict{CI, Int}
    at = Dict(point => 1)
    while !isempty(at) && M[first(at).first] != 9
        at = mapreduce(mergewith(+), at) do (p, c)
            next = filter(p .+ neighbors) do n
                return get(M, n, nothing) == M[p] + 1
            end
            return Dict(n => c for n in next)
        end
    end
    return at
end

if !isinteractive()
    @testset begin
        lines = split(test, '\n'; keepempty = false)
        input = parse_input(lines)
        @test q1(input) == 36
        @test q2(input) == 81
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
