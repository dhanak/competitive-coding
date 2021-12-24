function parse_input(io::IO)
    monad = []
    while !eof(io)
        line = readline(io)
        isempty(line) && continue
        (opc, args...) = split(line, ' ')
        opc = getfield(@__MODULE__, Symbol(opc))
        tgt = Symbol(args[1])
        src = length(args) == 1 ? nothing : args[2][1] ∈ "xyzw" ?
            Symbol(args[2]) : parse(Int, args[2])
        push!(monad, (opc, tgt, src))
    end
    return monad
end

mutable struct ALU
    x
    y
    z
    w
    ALU(x = 0, y = 0, z = 0, w = 0) = new(x, y, z, w)
end

Base.getindex(alu::ALU, v::Integer) = v
Base.getindex(alu::ALU, r::Symbol) = getfield(alu, r)
Base.setindex!(alu::ALU, v, r::Symbol) = setfield!(alu, r, v)

inp(tgt::Symbol, ::Nothing, alu::ALU, in::Vector) = alu[tgt] = popfirst!(in)
add(tgt::Symbol, src, alu::ALU, _) = alu[tgt] = alu[tgt] + alu[src]
mul(tgt::Symbol, src, alu::ALU, _) = alu[tgt] = alu[tgt] * alu[src]
div(tgt::Symbol, src, alu::ALU, _) = alu[tgt] = alu[tgt] ÷ alu[src]
mod(tgt::Symbol, src, alu::ALU, _) = alu[tgt] = alu[tgt] % alu[src]
eql(tgt::Symbol, src, alu::ALU, _) = alu[tgt] = alu[tgt] == alu[src] ? 1 : 0

function run(code::AbstractVector, input::Int, alu::ALU = ALU())::Int
    input = parse.(Int, split(string(input), ""))
    for (opc, tgt, src) in code
        if opc == inp && isempty(input)
            @warn "terminate early"
            return alu
        end
        opc(tgt, src, alu, input)
    end
    isempty(input) || @warn "unparsed input $(join(input))"
    return alu.z
end

monad = open(parse_input, "day24.in", "r")

# for calculations, see day24-decoded.txt

@assert run(monad, 93997999296912) == 0 # Q1, largest model number
@assert run(monad, 81111379141811) == 0 # Q2, smallest model number
