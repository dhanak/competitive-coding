using Test: @testset, @test

using aoc2024

test = """
       x00: 1
       x01: 1
       x02: 1
       y00: 0
       y01: 1
       y02: 0

       x00 AND y00 -> z00
       x01 XOR y01 -> z01
       x02 OR y02 -> z02
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    br = only(findall(isempty, lines))
    initial = map(lines[1:br - 1]) do line
        (name, value) = split(line, ": ")
        value = parse(Int, value)
        return name => value
    end |> sort!
    gates = map(lines[br + 1:end]) do line
        (left, c) = split(line, " -> ")
        (a, op, b) = split(left, ' ')
        op = getfield(@__MODULE__, Symbol(lowercase(op)))
        return minmax(a, b) => (op, c)
    end |> v -> sort!(v, by = first)
    return (initial, gates)
end

and(a, b) = a & b
or(a, b) = a | b

Base.isless(::typeof(and), ::typeof(and)) = false
Base.isless(::typeof(or), ::typeof(or))   = false
Base.isless(::typeof(xor), ::typeof(xor)) = false
Base.isless(::typeof(and), ::typeof(or))  = true
Base.isless(::typeof(or), ::typeof(and))  = false
Base.isless(::typeof(and), ::typeof(xor)) = true
Base.isless(::typeof(xor), ::typeof(and)) = false
Base.isless(::typeof(or), ::typeof(xor))  = true
Base.isless(::typeof(xor), ::typeof(or))  = false

function q1((initial, gates))
    values = Dict{String, Int}()
    queue = copy(initial)
    while !isempty(queue)
        (w1, v1) = pop!(queue)
        for (w2, v2) in values
            ((a, va), (b, vb)) = minmax((w1, v1), (w2, v2))
            for (_, (op, c)) in filter(==((a, b)) ∘ first, gates)
                push!(queue, c => op(va, vb))
            end
        end
        values[w1] = v1
    end
    zs = sort(collect(filter(startswith('z'), keys(values))); rev = true)
    return foldl(zs; init = 0) do z, n
        return z << 1 | values[n]
    end
end

function q2((_, gates))
    g0 = Dict(c => (op, a, b) for ((a, b), (op, c)) in gates)
    g1 = Dict(v => k for (k, v) in g0)
    @assert length(g0) == length(g1) # no duplicate gates
    swapped = []
    function swap(a, b)
        ga = g0[a]
        gb = g0[b]
        g0[a] = gb
        g0[b] = ga
        g1[ga] = b
        g1[gb] = a
        push!(swapped, minmax(a, b))
    end

    local add, carry, i, j
    zs = sort(collect(filter(startswith('z'), keys(g0))))
    for z in zs
        i = z[2:3]
        gz = g0[z]
        if z == "z00"
            good = (xor, "x00", "y00")
            gz == good || swap(z, g1[good])
        elseif z == "z01"
            a = g1[and, "x00", "y00"]
            b = g1[xor, "x01", "y01"]
            good = (xor, minmax(a, b)...)
            gz == good || swap(z, g1[good])
            (carry, add) = (a, b)
        elseif z != last(zs)
            a = g1[and, minmax(carry, add)...]
            b = g1[and, "x$j", "y$j"]
            c = g1[or, minmax(a, b)...]
            d = g1[xor, "x$i", "y$i"]
            good = (xor, minmax(c, d)...)
            if good ∉ keys(g1)
                @assert gz[1] == xor && length([c, d] ∩ gz[2:3]) == 1
                l = setdiff([c, d], gz[2:3])[]
                r = setdiff(gz[2:3], [c, d])[]
                swap(l, r)
            elseif gz != good
                swap(z, g1[good])
            end
            carry = g1[or, minmax(a, b)...]
            add = g1[xor, "x$i", "y$i"]
        else
            a = g1[and, minmax(carry, add)...]
            b = g1[and, "x$j", "y$j"]
            @assert gz == (or, minmax(a, b)...) # last gate, must work
        end
        j = i
    end
    @assert length(swapped) == 4
    return join(sort!(mapreduce(collect, vcat, swapped)), ',')
end

if !isinteractive()
    @testset begin
        lines = split(test, '\n')[1:end - 1]
        input = parse_input(lines)
        @test q1(input) == 4
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
