using Test: @testset, @test

test1 = """
        LLR

        AAA = (BBB, BBB)
        BBB = (AAA, ZZZ)
        ZZZ = (ZZZ, ZZZ)
        """

test2 = """
        LR

        11A = (11B, XXX)
        11B = (XXX, 11Z)
        11Z = (11B, XXX)
        22A = (22B, XXX)
        22B = (22C, 22C)
        22C = (22Z, 22Z)
        22Z = (22B, 22B)
        XXX = (XXX, XXX)
        """

I = Dict('L' => :left, 'R' => :right)

function parse_input(lines::AbstractVector{<: AbstractString})
    instructions = lines[1]
    nodes = map(lines[3:end]) do node
        (name, left, right) = split(node, r"[^\w]+"; keepempty = false)
        name => (; left, right)
    end |> Dict
    return return (instructions, nodes)
end

function q1((instr, nodes))
    node = "AAA"
    for (n, i) in enumerate(Iterators.cycle(instr))
        node = nodes[node][I[i]]
        node == "ZZZ" && return n
    end
end

function q2((instr, nodes))
    start = filter!(endswith('A'), collect(keys(nodes)))
    cycles = map(start) do node
        for (n, i) in enumerate(Iterators.cycle(instr))
            node = nodes[node][I[i]]
            node[end] == 'Z' && return n
        end
    end
    return lcm(cycles)
end

if !isinteractive()
    @testset begin
        lines = split(test1, '\n')[1:end - 1]
        input = parse_input(lines)
        @test q1(input) == 6

        lines = split(test2, '\n')[1:end - 1]
        input = parse_input(lines)
        @test q2(input) == 6
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
