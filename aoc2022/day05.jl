test = """
    [D]
[N] [C]
[Z] [M] [P]
 1   2   3

move 1 from 2 to 1
move 3 from 1 to 3
move 2 from 2 to 1
move 1 from 1 to 2
"""

function parse_input(input::AbstractString)
    lines = split(input, '\n'; keepempty = false)

    # stacks
    depth = findfirst(startswith(" 1"), lines) - 1
    len = length(lines[depth])
    crates = hcat([collect(rpad(line, len)[2:4:end])
                   for line in lines[1:depth]]...)
    stacks = [filter(!=(' '), stack) for stack in eachrow(crates)]

    # moves
    moves = [let m = match(r"^move ([0-9]+) from ([0-9]+) to ([0-9]+)$", line)
                 parse.(Int, m.captures)
             end for line in lines[depth + 2:end]]

    return (stacks, moves)
end

function cratemover(f, stacks, moves)
    stacks = deepcopy(stacks)
    for (n, from, to) in moves
        crates = stacks[from][1:n]
        stacks[from] = stacks[from][n + 1:end]
        stacks[to] = [f(crates); stacks[to]]
    end
    return join(first.(stacks))
end

q1(stacks, moves) = cratemover(reverse, stacks, moves)
q2(stacks, moves) = cratemover(identity, stacks, moves)

let (stacks, moves) = parse_input(test)
    @assert q1(stacks, moves) == "CMZ"
    @assert q2(stacks, moves) == "MCD"
end

(stacks, moves) = parse_input(read("day05.in", String))
println("Q1: ", q1(stacks, moves))
println("Q2: ", q2(stacks, moves))
