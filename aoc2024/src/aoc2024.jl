module aoc2024

export CI, neighbors4, neighbors8, grow, shrink

const CI = CartesianIndex{2}

const neighbors4 = [CI(-1, 0), CI(0, 1), CI(1, 0), CI(0, -1)]
const neighbors8 = [CI(-1, -1), CI(-1, 0), CI(-1, 1),
                    CI( 0, -1),            CI( 0, 1),
                    CI( 1, -1), CI( 1, 0), CI( 1, 1)]

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

end # module aoc2024
