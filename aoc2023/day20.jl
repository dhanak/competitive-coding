using Test: @testset, @test

test1 = """
        broadcaster -> a, b, c
        %a -> b
        %b -> c
        %c -> inv
        &inv -> a
        """

test2 = """
        broadcaster -> a
        %a -> inv, con
        &inv -> b
        %b -> con
        &con -> output
        """

Base.@kwdef mutable struct Mod
    name::Symbol
    type::Symbol
    out::Vector{Symbol} = Symbol[]
    state::Union{Nothing, Bool, Dict{Symbol, Bool}} = nothing
end

function parse_input(lines::AbstractVector{<: AbstractString})
    mods = map(lines) do line
        (typename, out...) = split(line, r" -> |, ")
        (type, name) =
            typename[1] == '%' ? (:flipflop,    Symbol(typename[2:end])) :
            typename[1] == '&' ? (:conjunction, Symbol(typename[2:end])) :
            typename == "broadcaster" ? (:broadcaster, :broadcaster) :
            @assert false "Unkown module $(typename)!"
        out = Symbol.(out)
        return name => Mod(; name, type, out)
    end |> Dict
    for (name, mod) in mods
        if mod.type == :flipflop
            mod.state = false
        elseif mod.type == :conjunction
            mod.state = Dict(iname => false
                             for (iname, imod) in mods
                             if name ∈ imod.out)
        end
    end
    return mods
end

function q1(mods)
    mods = deepcopy(mods)
    signals = []
    for _ in 1:1000
        append!(signals, button!(mods))
    end
    return count(first, signals) * count(!first, signals)
end

function button!(mods)
    signals = [(false, :btn, :broadcaster)]
    for (signal, snd, rcv) in signals
        mod = get(mods, rcv, nothing)
        if mod === nothing
            # do nothing
        elseif mod.type == :flipflop && !signal
            mod.state = !mod.state
            send!(signals, mod, mod.state)
        elseif mod.type == :conjunction
            mod.state[snd] = signal
            send!(signals, mod, !all(values(mod.state)))
        elseif mod.type == :broadcaster
            send!(signals, mod, signal)
        end
    end
    return signals
end

function send!(signals, mod::Mod, signal::Bool)
    append!(signals, [(signal, mod.name, out) for out in mod.out])
end

function q2(mods)
    l1 = findfirst(mod -> mod.out == [:rx], mods)
    @assert mods[l1].type == :conjunction
    return map(findall(mod -> l1 ∈ mod.out, mods)) do l2
        i = 1
        mods_ = deepcopy(mods)
        while (true, l2, l1) ∉ button!(mods_)
            i += 1
        end
        return i
    end |> lcm
end

if !isinteractive()
    @testset begin
        lines = filter!(!isempty, split(test1, '\n'))
        input = parse_input(lines)
        @test q1(input) == 32000000
        lines = filter!(!isempty, split(test2, '\n'))
        input = parse_input(lines)
        @test q1(input) == 11687500
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
