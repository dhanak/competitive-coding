using Test: @testset, @test

using aoc2025

test1 = """
aaa: you hhh
you: bbb ccc
bbb: ddd eee
ccc: ddd eee fff
ddd: ggg
eee: out
fff: out
ggg: out
hhh: ccc fff iii
iii: out
"""

test2 = """
svr: aaa bbb
aaa: fft
fft: ccc
bbb: tty
tty: ccc
ccc: ddd eee
ddd: hub
hub: fff
eee: dac
dac: fff
fff: ggg hhh
ggg: out
hhh: out
"""

function parse_input(lines::AbstractVector{<:AbstractString})
    return Dict(
        let (device, outputs) = split(line, ": ")
                Symbol(device) => Symbol.(split(outputs, ' '))
        end for line in lines
    )
end

function q1(devices)::Int
    return paths((_, d) -> d, devices, :you => 1)
end

function q2(devices)::Int
    # [direct path, via fft, via dac, via both]
    return paths(devices, :svr => [1, 0, 0, 0]) do dev, d
        dev == :fft && return [0, d[1], 0, d[3]]
        dev == :dac && return [0, 0, d[1], d[2]]
        return d
    end |> last
end

function paths(
        visit::Function,
        devices::AbstractDict,
        init::Pair{Symbol, T},
    )::T where {T}
    D = Dict(init)
    queue = [first(init)]
    while !isempty(queue)
        dev = popfirst!(queue)
        dev == :out && continue
        out = devices[dev]
        d = visit(dev, pop!(D, dev))
        mergewith!(+, D, Dict(o => d for o in out))
        union!(queue, out)
    end
    return D[:out]
end

if !isinteractive()
    @testset begin
        local input1 = parse_input(split(test1, '\n'; keepempty = false))
        local input2 = parse_input(split(test2, '\n'; keepempty = false))
        @test q1(input1) == 5
        @test q2(input2) == 2
    end

    @time begin
        local lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        local input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
