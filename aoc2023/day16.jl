using Test: @testset, @test

test = raw"""
          .|...\....
          |.-.\.....
          .....|-...
          ........|.
          ..........
          .........\
          ..../.\\..
          .-.-/..|..
          .|....-|.\
          ..//.|....
          """

CI = CartesianIndex{2}

function parse_input(lines::AbstractVector{<: AbstractString})
    return mapreduce(collect, hcat, lines) |> permutedims
end

reflection = Dict(
    (:r, '/') => :u,
    (:u, '/') => :r,
    (:l, '/') => :d,
    (:d, '/') => :l,
    (:r, '\\') => :d,
    (:d, '\\') => :r,
    (:l, '\\') => :u,
    (:u, '\\') => :l
)

move = Dict(
    :r => CI(0, 1),
    :d => CI(1, 0),
    :l => CI(0, -1),
    :u => CI(-1, 0)
)

function q1(input)
    return energize(input, CI(1,1) => :r)
end

function q2(input)
    (h, w) = size(input)
    starts = [[CI(y, 1) => :r for y in 1:h];
              [CI(y, w) => :l for y in 1:h];
              [CI(1, x) => :d for x in 1:w];
              [CI(h, x) => :u for x in 1:w]]
    return maximum(starts) do start
        return energize(input, start)
    end
end

function energize(input::Matrix{Char}, start::Pair{CI, Symbol})
    queue = [start]
    energy = [Symbol[] for _ in input]
    for (i, d) in queue
        (i ∉ keys(input) || d ∈ energy[i]) && continue
        push!(energy[i], d)
        nd = if input[i] ∈ "/\\"
            [reflection[(d, input[i])]]
        elseif input[i] == '|' && d ∈ [:r, :l]
            [:u, :d]
        elseif input[i] == '-' && d ∈ [:u, :d]
            [:l, :r]
        else
            [d]
        end
        append!(queue, [(i + move[d]) => d for d in nd])
    end
    return count(!isempty, energy)
end

if !isinteractive()
    @testset begin
        lines = filter!(!isempty, split(test, '\n'))
        input = parse_input(lines)
        @test q1(input) == 46
        @test q2(input) == 51
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
