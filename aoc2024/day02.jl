using Test: @testset, @test

test = """
       7 6 4 2 1
       1 2 7 8 9
       9 7 6 2 1
       1 3 2 4 5
       8 6 4 4 1
       1 3 6 7 9
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    return map(lines) do line
        return parse.(Int, split(line))
    end
end

function issafe(report)
    d = diff(report)
    return abs.(unique(sign.(d))) == [1] && all(abs.(d) .<= 3)
end

q1(reports) = count(issafe, reports)

function q2(reports)
    return count(reports) do report
        return issafe(report) || any(eachindex(report)) do i
            return issafe(report[[begin:i - 1; i + 1:end]])
        end
    end
end

if !isinteractive()
    @testset begin
        lines = split(test, '\n'; keepempty = false)
        input = parse_input(lines)
        @test q1(input) == 2
        @test q2(input) == 4
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
