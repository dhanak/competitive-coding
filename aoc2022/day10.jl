using Test: @testset, @test

function parse_input(file::AbstractString)
    return mapreduce(append!, readlines(file); init = [1]) do line
        (op, args...) = split(line, ' ')
        @assert op ∈ ["noop", "addx"]
        return op == "noop" ? [0] : [0, parse(Int, args[])]
    end |> cumsum
end

function q1(prog::Vector{Int})::Int
    interesting = 20:40:length(prog)
    return sum(interesting .* prog[interesting])
end

function q2(prog::Vector{Int})::Vector{String}
    chars = map(repeat(0:39, 6), prog) do pixel, sprite
        return sprite - 1 <= pixel <= sprite + 1 ? '#' : '.'
    end
    return join.(Iterators.partition(chars, 40), "")
end

prettify(str::String) = replace(replace(str, '.' => "  "), '#' => "##")

@testset begin
    v = parse_input("day10-test.in")
    @test q1(v) == 13140
    @test q2(v) ==
        split("""##..##..##..##..##..##..##..##..##..##..
                 ###...###...###...###...###...###...###.
                 ####....####....####....####....####....
                 #####.....#####.....#####.....#####.....
                 ######......######......######......####
                 #######.......#######.......#######.....""", '\n') .|> strip
end

v = parse_input("day10.in")
println("Q1: ", q1(v))
println("Q2:")
q2(v) .|> prettify .|> println
