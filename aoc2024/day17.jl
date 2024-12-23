using Test: @testset, @test

using aoc2024

test1 = """
        Register A: 729
        Register B: 0
        Register C: 0

        Program: 0,1,5,4,3,0
        """

test2 = """
        Register A: 2024
        Register B: 0
        Register C: 0

        Program: 0,3,5,4,3,0
        """

mutable struct State
    regs::Vector{Int}
    ip::Int
    out::Vector{Int}
    State(a::Int = 0, b::Int = 0, c::Int = 0) = new([a, b, c], 0, [])
end

function parse_input(lines::AbstractVector{<: AbstractString})
    @assert isempty(lines[4])
    regs = map(lines[1:3]) do line
        @assert startswith(line, "Register ")
        return parse(Int, split(line, ": ")[2])
    end
    @assert startswith(lines[5], "Program")
    program = parse.(Int, split(lines[5], r": |,")[2:end])
    return (regs, program)
end

function combo(st::State, op::Int)
    0 <= op <= 3 && return op
    4 <= op <= 6 && return st.regs[op - 3]
    op == 7 && error("reserved combo operand")
end

adv(st, op) = (st.regs[1] = st.regs[1] >> combo(st, op); nothing)
bxl(st, op) = (st.regs[2] = st.regs[2] ⊻ op            ; nothing)
bst(st, op) = (st.regs[2] = combo(st, op) % 8          ; nothing)
jnz(st, op) = (st.regs[1] != 0 ? (st.ip = op)          : nothing)
bxc(st, op) = (st.regs[2] = st.regs[2] ⊻ st.regs[3]    ; nothing)
out(st, op) = (push!(st.out, combo(st, op) % 8)        ; nothing)
bdv(st, op) = (st.regs[2] = st.regs[1] >> combo(st, op); nothing)
cdv(st, op) = (st.regs[3] = st.regs[1] >> combo(st, op); nothing)

const instructions = [adv, bxl, bst, jnz, bxc, out, bdv, cdv]

q1((regs, program)) = join(run(State(regs...), program), ',')

function run(st, program)
    while st.ip < length(program)
        inst = instructions[program[st.ip + 1] + 1]
        op = program[st.ip + 2]
        if inst(st, op) === nothing
            st.ip += 2
        end
    end
    return st.out
end

function q2((_, program))
    # program ends with "adv 3, out ?, jnz 0", drop jump from end
    @assert program[[(end - 5:end - 3); (end - 1:end)]] == [0, 3, 5, 3, 0]
    return q2(0, program[1:end - 2], reverse(program))
end

function q2(a, program, output)
    isempty(output) && return a
    (o, output...) = output
    for a in valid_a(a, program, o)
        a = q2(a, program, output)
        a !== nothing && return a
    end
    return nothing
end

function valid_a(a, program, o)
    a = a << 3
    return filter(a:a + 7) do a
        return run(State(a), program) == [o]
    end
end

if !isinteractive()
    @testset begin
        input1 = parse_input(split(test1, '\n'))
        @test q1(input1) == "4,6,3,5,6,3,5,2,1,0"
        input2 = parse_input(split(test2, '\n'))
        @test q2(input2) == 117440
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
