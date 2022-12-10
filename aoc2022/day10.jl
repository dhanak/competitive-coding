using Test

function parse_input(lines)
    return map(lines) do line
        parts = split(line, ' ')
        op = Symbol(parts[1])
        v = parse.(Int, parts[2:end])
        return op => v
    end |> execute
end

function execute(prog)
    return reduce(prog; init = [(1, 1)]) do states, (op, v)
        (c, x) = states[end]
        return [states; op == :noop ? (c + 1, x) : (c + 2, x + v[])]
    end
end

function q1(prog)
    interesting = 20:40:prog[end][1]
    return sum(interesting) do c
        x = prog[findlast(<=(c) ∘ first, prog)][2]
        return c * x
    end
end

function q2(prog)
    ip = 1
    chars = map(1:6 * 40) do clock
        ip += clock == prog[ip + 1][1]
        x = prog[ip][2]
        p = (clock - 1) % 40
        return x -1 <= p <= x + 1 ? '#' : '.'
    end
    return join(chars, "")
end

function wrap(str::String, w::Integer)
    str = replace(replace(str, '.' => "  "), '#' => "##")
    for i in 1:2w:length(str)
        println(str[i:i + 2w - 1])
    end
end

@testset begin
    v = parse_input(readlines("day10-test.in"))
    @test q1(v) == 13140
    @test q2(v) == "##..##..##..##..##..##..##..##..##..##..###...###...###...###...###...###...###.####....####....####....####....####....#####.....#####.....#####.....#####.....######......######......######......###########.......#######.......#######....."
end

v = parse_input(readlines("day10.in"))
println("Q1: ", q1(v))
println("Q2:")
wrap(q2(v), 40)
