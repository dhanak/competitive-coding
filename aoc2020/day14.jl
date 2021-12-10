using Combinatorics: combinations

test1 = """
    mask = XXXXXXXXXXXXXXXXXXXXXXXXXXXXX1XXXX0X
    mem[8] = 11
    mem[7] = 101
    mem[8] = 0
    """

test2 = """
    mask = 000000000000000000000000000000X1001X
    mem[42] = 100
    mask = 00000000000000000000000000000000X0XX
    mem[26] = 1
    """

function read_input(input)
    return map(readlines(input)) do line
        (opc, args...) = split(line, r"\[|\]? = "; keepempty = false)
        return parse_command(Val(Symbol(opc)), args...)
    end
end

parse_command(::Val{:mask}, mask) = mask
parse_command(::Val{:mem}, args...) = parse.(Int, args)

function run(execute::Function, prog::AbstractVector)::Dict
    return reduce(execute, prog; init = ("", Dict())) |> last
end

q1(prog::AbstractVector) = run(q1, prog) |> values |> sum
q1((_, mem), m::AbstractString) = (m, mem)
function q1((m, mem), (a, v)::Tuple)
    (or, and) = parse.(Int, replace.(Ref(m), 'X' .=> ['0', '1']); base = 2)
    mem[a] = (v & and) | or
    return (m, mem)
end

q2(prog::AbstractVector) = run(q2, prog) |> values |> sum
q2((_, mem), m::AbstractString) = (m, mem)
function q2((m, mem), (a, v)::Tuple)
    or = parse(Int, replace(m, 'X' => '0'); base = 2)
    and = parse(Int, replace(replace(m, '0' => '1'), 'X' => '0'); base = 2)
    a = (a & and) | or
    floats = [2 << (length(m) - i - 1) for i in findall(==('X'), m)]
    as = [reduce(|, [a; fs]) for fs in [[], combinations(floats)...]]
    setindex!.(Ref(mem), v, as)
    return (m, mem)
end

@assert q1(read_input(IOBuffer(test1))) == 165
@assert q2(read_input(IOBuffer(test2))) == 208

prog = read_input("day14.in")
println("Q1: ", q1(prog))
println("Q2: ", q2(prog))
