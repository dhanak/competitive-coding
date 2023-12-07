using Test: @testset, @test

test = """
       Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green
       Game 2: 1 blue, 2 green; 3 green, 4 blue, 1 red; 1 green, 1 blue
       Game 3: 8 green, 6 blue, 20 red; 5 blue, 4 red, 13 green; 5 green, 1 red
       Game 4: 1 green, 3 red, 6 blue; 3 green, 6 red; 3 green, 15 blue, 14 red
       Game 5: 6 red, 1 blue, 3 green; 2 blue, 1 red, 2 green
       """

colors = Dict("red" => 1, "green" => 2, "blue" => 3)

function parse_input(lines::AbstractVector{<: AbstractString})
    return map(filter(!isempty, lines)) do line
        (game, samples...) = split(line, r"[:;] *")
        game_idx = parse(Int, split(game)[2])
        return game_idx => map(samples) do sample
            return mapreduce(+, split(sample, r", *")) do cubes
                (count, color) = split(cubes)
                cubes = fill(0, 3)
                cubes[colors[color]] = parse(Int, count)
                return cubes
            end
        end
    end
end

function q1(games)
    sum(games) do (i, game)
        return i * all(game) do draw
            return all(draw .<= [12, 13, 14])
        end
    end
end

function q2(games)
    return sum(games) do (_, game)
        return reduce(game) do a, b
            return max.(a, b)
        end |> prod
    end
end

if !isinteractive()
    @testset begin
        lines = split(test, '\n')
        input = parse_input(lines)
        @test q1(input) == 8
        @test q2(input) == 2286
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
