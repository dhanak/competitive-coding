using Test: @testset, @test

test = """
       rn=1,cm-,qp=3,cm=2,qp-,pc=4,ot=9,ab=5,pc-,pc=6,ot=7
       """

function parse_input(lines::AbstractVector{<: AbstractString})
    return split(join(lines), ',')
end

function fhash(s::AbstractString)::Int
    return foldl(s; init = 0) do h, c
        h = 17 * (h + Int(c)) % 256
    end
end

q1(input) = sum(fhash, input)

function q2(input)
    boxes = [[] for _ in 1:256]
    map(input) do step
        (label, fl) = split(step, ['-', '='])
        box = boxes[fhash(label) + 1]
        i = findfirst(==(label) ∘ first, box)
        fl = tryparse(Int, fl)
        if fl === nothing
            i !== nothing && deleteat!(box, i)
        elseif i === nothing
            push!(box, label => fl)
        else
            box[i] = label => fl
        end
    end
    return sum(enumerate(boxes)) do (i, box)
        return sum(enumerate(box); init = 0) do (j, (_, fl))
            i * j * fl
        end
    end
end

if !isinteractive()
    @testset begin
        @test fhash("HASH") == 52
        lines = split(test, '\n')
        input = parse_input(lines)
        @test q1(input) == 1320
        @test q2(input) == 145
    end

    @time begin
        lines = readlines(replace(@__FILE__, ".jl" => ".in"))
        input = parse_input(lines)
        println("Q1: ", q1(input))
        println("Q2: ", q2(input))
    end
end
