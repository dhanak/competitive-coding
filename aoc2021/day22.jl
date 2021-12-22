using ProgressMeter

test = """
    on x=-20..26,y=-36..17,z=-47..7
    on x=-20..33,y=-21..23,z=-26..28
    on x=-22..28,y=-29..23,z=-38..16
    on x=-46..7,y=-6..46,z=-50..-1
    on x=-49..1,y=-3..46,z=-24..28
    on x=2..47,y=-22..22,z=-23..27
    on x=-27..23,y=-28..26,z=-21..29
    on x=-39..5,y=-6..47,z=-3..44
    on x=-30..21,y=-8..43,z=-13..34
    on x=-22..26,y=-27..20,z=-29..19
    off x=-48..-32,y=26..41,z=-47..-37
    on x=-12..35,y=6..50,z=-50..-2
    off x=-48..-32,y=-32..-16,z=-15..-5
    on x=-18..26,y=-33..15,z=-7..46
    off x=-40..-22,y=-38..-28,z=23..41
    on x=-16..35,y=-41..10,z=-47..6
    off x=-32..-23,y=11..30,z=-14..3
    on x=-49..-5,y=-3..45,z=-29..18
    off x=18..30,y=-20..-8,z=-3..13
    on x=-41..9,y=-7..43,z=-33..15
    on x=-54112..-39298,y=-85059..-49293,z=-27449..7877
    on x=967..23432,y=45373..81175,z=27513..53682
    """

function parse_input(input::AbstractString)
    lines = split(input, r"\n"; keepempty = false)
    return parse_line.(lines)
end

const LINE_RE =
    let re(c) = Regex("(?<$(c)min>-?[0-9]+)\\.\\.(?<$(c)max>-?[0-9]+)")
        r"^(?<act>on|off) x=" * re('x') * ",y=" * re('y') * ",z=" * re('z')
    end

function parse_line(line::AbstractString)
    m = match(LINE_RE, line)
    @assert m !== nothing
    ca(c) = let a = parse(Int, m["$(c)min"]),
        b = parse(Int, m["$(c)max"])
        a:b
    end
    return (Block(ca('x'), ca('y'), ca('z')), m["act"] == "on")
end

struct Block
    x::UnitRange{Int}
    y::UnitRange{Int}
    z::UnitRange{Int}
end

Base.isempty(b::Block) = isempty(b.x) || isempty(b.y) || isempty(b.z)
Base.length(b::Block) = length(b.x) * length(b.y) * length(b.z)

function Base.intersect(a::Block, b::Block)::Block
    return Block(a.x ∩ b.x, a.y ∩ b.y, a.z ∩ b.z)
end

function Base.setdiff(a::Block, b::Block)
    return filter(x -> !isempty(x ∩ a) && isempty(x ∩ b), slices(a, b))
end

function Base.union(a::Block, b::Block)
    return filter(x -> !isempty(x ∩ a) || !isempty(x ∩ b), slices(a, b))
end

function slices(a::Block, b::Block)::Vector{Block}
    return [Block(x, y, z) for x in slice(a.x, b.x)
                           for y in slice(a.y, b.y)
                           for z in slice(a.z, b.z)]
end

function slice(a::UnitRange{<: T}, b::UnitRange{<: T}) where T
    trirange(i, j, k, l) = filter(!isempty, [i:j - 1, j:k, k + 1:l])
    a.stop < b.start && return [a, b]
    b.stop < a.start && return [b, a]
    return trirange(sort!([a.start, a.stop, b.start, b.stop])...)
end

@assert slice(-5:-2, 1:10) == [-5:-2, 1:10]
@assert slice(1:10, -5:-2) == [-5:-2, 1:10]
@assert slice(1:10, 5:6) == [1:4, 5:6, 7:10]
@assert slice(1:10, -5:3) == [-5:0, 1:3, 4:10]
@assert slice(1:10, 5:20) == [1:4, 5:10, 11:20]

unionall(as::AbstractVector{<: Block}, b::Block) = unionall(as, [b])
function unionall(as::AbstractVector{<: Block}, bs::AbstractVector{<: Block})
    isempty(bs) && return as
    (b, bs...) = bs
    i = findfirst(a -> !isempty(a ∩ b), as)
    i == nothing && return unionall([as; b], bs)
    a = splice!(as, i)
    return unionall(as, [a ∪ b; bs])
end

q1(procedure) = boot(procedure; bounds = Block(-50:50, -50:50, -50:50))
q2(procedure) = boot(procedure)

function boot(procedure; bounds = nothing)
    blocks = Block[]
    @showprogress for (region, state) in procedure
        if bounds !== nothing
            region = region ∩ bounds
            isempty(region) && continue
        end
        blocks = (state ?
            unionall(blocks, region) :
            vcat(map(block -> setdiff(block, region), blocks)...)) |> simplify!
    end
    return sum(length, blocks)
end

function simplify!(bs::V)::V where {V <: AbstractVector{<: Block}}
    i = 1
    while i < length(bs)
        j = findnext(b -> mergeable(bs[i], b), bs, i + 1)
        while j != nothing
            bs[i] = merge(bs[i], bs[j])
            deleteat!(bs, j)
            j = findnext(b -> mergeable(bs[i], b), bs, j)
        end
        i += 1
    end
    return bs
end

function mergeable(a::Block, b::Block)::Bool
    return a.x == b.x && a.y == b.y && mergeable(a.z, b.z) ||
        a.x == b.x && a.z == b.z && mergeable(a.y, b.y) ||
        a.y == b.y && a.z == b.z && mergeable(a.x, b.x)
end

function mergeable(a::UnitRange, b::UnitRange)::Bool
    return a.stop + 1 == b.start || b.stop + 1 == a.start
end

function Base.merge(a::Block, b::Block)::Block
    return Block(merge(a.x, b.x), merge(a.y, b.y), merge(a.z, b.z))
end

function Base.merge(a::UnitRange, b::UnitRange)::UnitRange
    @assert a == b || mergeable(a, b)
    return min(a.start, b.start):max(a.stop, b.stop)
end

let proc1 = parse_input(test),
    proc2 = parse_input(read("day22-test.in", String))

    @assert q1(proc1) == 590784
    @assert q1(proc2) == 474140
    @assert q2(proc2) == 2758514936282235
end

procedure = parse_input(read("day22.in", String))
println("Q1: ", q1(procedure))
println("Q2: ", q2(procedure))  # approx. 1 min
