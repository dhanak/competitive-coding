module aoc2025

export
    @pr,
    CI,
    add_interval!,
    add_interval,
    blocks,
    cross,
    diophantine,
    dot,
    grow,
    grow_poly,
    intersection,
    neighbors4,
    neighbors8,
    norm,
    norm_sq,
    normalize,
    point_on_segment,
    shrink,
    ×,
    ⋅

using MacroTools

const CI = CartesianIndex{2}

const neighbors4 = [CI(-1, 0), CI(0, 1), CI(1, 0), CI(0, -1)]
const neighbors8 = [
    CI(-1, -1), CI(-1, +0), CI(-1, +1),
    CI(+0, -1), #=========# CI(+0, +1),
    CI(+1, -1), CI(+1, +0), CI(+1, +1),
]

macro pr(expr, label = "")
    sexpr = string(MacroTools.postwalk(MacroTools.rmlines, expr))
    return quote
        let v = $(esc(expr))
            println($(esc(label)), $(sexpr), " = ", v)
            v
        end
    end
end

function Base.convert(
        ::Type{Matrix{Char}},
        lines::AbstractVector{<:AbstractString}
    )::Matrix{Char}
    return mapreduce(collect, hcat, lines) |> permutedims
end

function Base.findall(
        needle::AbstractMatrix,
        haystack::AbstractMatrix;
        ignore = isnothing
    )
    (w, h) = size(needle)
    (W, H) = size(haystack)
    Js = findall(!ignore, needle)
    return findall(CartesianIndices((W - w + 1, H - h + 1))) do I
        return all(Js) do J
            needle[J] == haystack[I + J - CartesianIndex(1, 1)]
        end
    end
end

function grow(
        M::AbstractMatrix{T},
        with = zero(T),
        by::Int = 1
    )::Matrix{T} where {T}
    (h, w) = size(M)
    C = fill(with, by, by)
    H = fill(with, by, w)
    V = fill(with, h, by)
    return [
        C H C
        V M V
        C H C
    ]
end

function shrink(
        M::AbstractMatrix{T},
        by::Int = 1
    )::Matrix{T} where {T}
    return M[(by + begin):(end - by), (by + begin):(end - by)]
end

function diophantine(a, b, c)
    (d, u, v) = gcdx(a, b)
    x0 = u * Int(c / d)
    y0 = v * Int(c / d)
    return ((b ÷ d, x0), (-a ÷ d, y0))
end

function blocks(
        lines::AbstractVector{<:T}
    )::Vector{Vector{T}} where {T <: AbstractString}
    return blocks(isempty, lines)
end

function blocks(
        sep::Function,
        lines::AbstractVector{T}
    )::Vector{Vector{T}} where {T <: AbstractString}
    lines = lines[findfirst(!sep, lines):findlast(!sep, lines)]
    @assert !isempty(lines)
    breaks = findall(sep, lines)
    ranges = range.([1; breaks .+ 1], [breaks .- 1; length(lines)])
    return getindex.(Ref(lines), ranges)
end

function add_interval(intervals::AbstractVector, (a, b))
    isempty(intervals) && return [a => b]
    ((c, d), rest...) = intervals
    b < c - 1 && return [a => b, intervals...]
    a <= d + 1 && return add_interval(rest, min(a, c) => max(b, d))
    return [c => d, add_interval(rest, a => b)...]
end

function add_interval!(intervals::AbstractVector, (a, b))
    i = 1
    while i <= length(intervals)
        (c, d) = intervals[i]
        if b < c - 1
            return insert!(intervals, i, a => b)
        elseif a <= d + 1
            (a, b) = min(a, c) => max(b, d)
            deleteat!(intervals, i)
        else
            i += 1
        end
    end
    return push!(intervals, a => b)
end

cross((x1, y1), (x2, y2)) = x1 * y2 - x2 * y1
×(a, b) = cross(a, b)

dot(a, b) = a .* b
⋅(a, b) = a .* b

function grow_poly(poly::AbstractVector; by::Number)::Vector
    return map(poly[[end; 1:(end - 1)]], poly, poly[[2:end; 1]]) do a, b, c
        v1 = b .- a
        v2 = b .- c
        d = (normalize(v1) .+ normalize(v2)) .* by
        return b .+ (d .* sign(v2 × v1))
    end
end

norm_sq(v) = sum(v .^ 2)

norm(v)::Float64 = v |> norm_sq |> sqrt

normalize(v) = v ./ norm(v)

function point_on_segment(point, (a, b))::Float64
    v1 = point .- a
    v2 = b .- a
    return sum(v1 ⋅ v2) / norm_sq(v2)
end

function intersection((a, b), (c, d))
    line(p1, p2) = (p2 .- p1, p1 × p2)
    (d1, D1) = line(a, b)
    (d2, D2) = line(c, d)
    D = d1 × d2
    D == 0 && return nothing
    Dx = (d1[1], d2[1]) × (D1, D2)
    Dy = (d1[2], d2[2]) × (D1, D2)
    return (Dx, Dy) ./ D
end

end # module aoc2025
