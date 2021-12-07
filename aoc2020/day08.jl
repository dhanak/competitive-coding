test = """
    nop +0
    acc +1
    jmp +4
    acc +3
    jmp -3
    acc -99
    acc +1
    jmp -4
    acc +6
    """

function parse_program(prog::AbstractString)::Vector
    return split(prog, "\n"; keepempty = false) .|> parse_cmd
end

function parse_cmd(cmd::AbstractString)::Tuple
    (opc, adr) = split(cmd, " ")
    return (getfield(@__MODULE__, Symbol(opc)), parse(Int, adr))
end

struct State
    ip::Int
    acc::Int
end

State() = State(1, 0)

step(prog::AbstractVector, state::State) = step(prog[state.ip], state)
step((opc, adr), state::State) = opc(state, adr)

nop(state::State, adr::Int)::State = State(state.ip + 1, state.acc)
acc(state::State, adr::Int)::State = State(state.ip + 1, state.acc + adr)
jmp(state::State, adr::Int)::State = State(state.ip + adr, state.acc)

function run(prog::AbstractVector)::Tuple
    len = length(prog)
    visited = fill(false, len)
    state = State()
    while state.ip <= len && !visited[state.ip]
        visited[state.ip] = true
        state = step(prog, state)
    end
    return (state.ip > len, state.acc)
end

function swap(prog::V, i::Int)::V where {V <: AbstractVector}
    swp((opc, arg)) = (opc == nop ? jmp : nop, arg)
    return [prog[1:i - 1]; swp(prog[i]); prog[i + 1:end]]
end

function q2(prog::AbstractVector)::Int
    for i in 1:length(prog)
        prog[i][1] != acc || continue
        (ok, a) = run(swap(prog, i))
        ok && return a
    end
end

let prog = parse_program(test)
    @assert run(prog) == (false, 5)
    @assert q2(prog) == 8
end

prog = parse_program(read("day08.in", String))
println("Q1: ", run(prog) |> last)
println("Q2: ", q2(prog))
