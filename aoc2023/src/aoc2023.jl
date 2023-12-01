module aoc2023

import Base: union

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

end # module
