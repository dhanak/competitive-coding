module aoc2024

export CI, neighbors4, neighbors8, grow, shrink, diophantine

const CI = CartesianIndex{2}

const neighbors4 = [CI(-1, 0), CI(0, 1), CI(1, 0), CI(0, -1)]
const neighbors8 = [CI(-1, -1), CI(-1, 0), CI(-1, 1),
                    CI( 0, -1),            CI( 0, 1),
                    CI( 1, -1), CI( 1, 0), CI( 1, 1)]

function Base.convert(::Type{Matrix{Char}},
                      lines::AbstractVector{<: AbstractString}
                     )::Matrix{Char}
    return mapreduce(collect, hcat, lines) |> permutedims
end

function Base.findall(needle::Matrix, haystack::Matrix; ignore = isnothing)
    (w, h) = size(needle)
    (W, H) = size(haystack)
    Js = findall(!ignore, needle)
    return findall(CartesianIndices((W - w + 1, H - h + 1))) do I
        return all(Js) do J
            needle[J] == haystack[I + J - CartesianIndex(1, 1)]
        end
    end
end

function grow(M::AbstractMatrix{T},
              with = zero(T),
              by::Int = 1
             )::Matrix{T} where {T}
    (h, w) = size(M)
    C = fill(with, by, by)
    H = fill(with, by, w)
    V = fill(with, h, by)
    return [C H C
            V M V
            C H C]
end

function shrink(M::AbstractMatrix{T},
                by::Int = 1
               )::Matrix{T} where {T}
    return M[by + begin:end - by, by + begin:end - by]
end

function diophantine(a, b, c)
    (d, u, v) = gcdx(a, b)
    x0 = u * Int(c / d)
    y0 = v * Int(c / d)
    return ((b ÷ d, x0), (-a ÷ d, y0))
end

end # module aoc2024
