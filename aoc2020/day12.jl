test = """
    F10
    N3
    F7
    R90
    F11
    """

function parse_program(prog::AbstractString)::Vector
    return split(prog, "\n"; keepempty = false) .|> Instruction
end

struct Instruction{code}
    v::Int
    Instruction(cmd::AbstractString) = new{cmd[1]}(parse(Int, cmd[2:end]))
end

Base.show(io::IO, i::Instruction{code}) where code = print(io, "$(code)$(i.v)")

const E = [ 1,  0]
const N = [ 0, -1]
const W = [-1,  0]
const S = [ 0,  1]

struct State{mode}
    pos
    target
end

State{:ship}() = State{:ship}([0, 0], E)
State{:wp  }() = State{:wp  }([0, 0], 10E + N)

function run(program::AbstractVector; mode::Symbol)::Int
    state = foldl(|>, program; init = State{mode}())
    return state.pos .|> abs |> sum
end

(i::Instruction{'N'})(state::State) = move(state, N .* i.v)
(i::Instruction{'S'})(state::State) = move(state, S .* i.v)
(i::Instruction{'E'})(state::State) = move(state, E .* i.v)
(i::Instruction{'W'})(state::State) = move(state, W .* i.v)
(i::Instruction{'F'})(state::State) = forward(state, state.target .* i.v)
(i::Instruction{'L'})(state::State) = turn(state, i.v)
(i::Instruction{'R'})(state::State) = turn(state, -i.v)

move(state::State{:ship}, m) = forward(state, m)
move(state::State{:wp}, m) = State{:wp}(state.pos, state.target + m)
forward(state::S, m) where {S <: State} = S(state.pos + m, state.target)
turn(state::S, d) where {S <: State} = S(state.pos, rotate(state.target, d))
rotate(v, d::Number) = round.(Int, [cosd(d) sind(d); -sind(d) cosd(d)] * v)

q1(program::AbstractVector)::Int = run(program; mode = :ship)
q2(program::AbstractVector)::Int = run(program; mode = :wp)

let program = parse_program(test)
    @assert q1(program) == 25
    @assert q2(program) == 286
end

program = parse_program(read("day12.in", String))
println("Q1: ", q1(program))
println("Q2: ", q2(program))
