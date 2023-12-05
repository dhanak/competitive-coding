module aoc2023

import Base: union

export blocks

function union(v::Vector{<: T},
               a::T
              )::Vector{T} where {T <: UnitRange{<: Integer}}
    result = T[]
    for (i, b) in enumerate(v)
        if a.stop + 1 < b.start
            return append!(result, [a], v[i:end])
        elseif b.stop + 1 < a.start
            push!(result, b)
        else
            a = min(a.start, b.start):max(a.stop, b.stop)
        end
    end
    return push!(result, a)
end

function blocks(lines::AbstractVector{<: T}
               )::Vector{Vector{T}} where {T <: AbstractString}
    return blocks(isempty, lines)
end

function blocks(sep::Function,
                lines::AbstractVector{T}
               )::Vector{Vector{T}} where {T <: AbstractString}
    lines = lines[findfirst(!sep, lines):findlast(!sep, lines)]
    @assert !isempty(lines)
    breaks = findall(sep, lines)
    ranges = range.([1; breaks .+ 1], [breaks .- 1; length(lines)])
    return getindex.(Ref(lines), ranges)
end

end # module
