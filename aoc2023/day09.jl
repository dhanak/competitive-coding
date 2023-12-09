using Test: @testset, @test

test = """
       0 3 6 9 12 15
       1 3 6 10 15 21
       10 13 16 21 30 45
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    return map(lines) do line
        return parse.(Int, split(line))
    end
end

q1(seqs) = sum(last ∘ complete, seqs)

q2(seqs) = sum(first ∘ complete, seqs)

function complete(seq)
    allequal(seq) && return seq[1] => seq[end]
    dseq = complete(diff(seq))
    return seq[1] - dseq[1] => seq[end] + dseq[end]
end

if !isinteractive()
    @testset begin
        lines = filter!(!isempty, split(test, '\n'))
        input = parse_input(lines)
        @test q1(input) == 114
        @test q2(input) == 2
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
