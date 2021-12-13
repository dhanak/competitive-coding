test = """
    6,10
    0,14
    9,10
    0,3
    10,4
    4,11
    6,0
    6,12
    4,1
    0,13
    10,12
    3,4
    3,0
    8,4
    1,10
    2,14
    8,10
    9,0

    fold along y=7
    fold along x=5
    """

function parse_input(input::AbstractString)
    dots, folds = split.(split(input, "\n\n"), r"\n"; keepempty = false)
    dots = map(p -> parse.(Int, split(p, ',')), dots)
    folds = [let m = match(r"^fold along (.)=([0-9]+)$", fold)
                 (Symbol(m.captures[1]), parse(Int, m.captures[2]))
             end for fold in folds]
    card = fill(false, maximum(last.(dots)) + 1, maximum(first.(dots)) + 1)
    for (x, y) in dots
        card[y + 1, x + 1] = true
    end
    return (card, folds)
end

fold(card::AbstractMatrix, (axis, v)) = fold(Val(axis), card, v)

function fold(::Val{:x}, card::AbstractMatrix, v::Int)
    return card[:, 1:v] .| card[:, 2v + 1:-1:v + 2]
end

function  fold(::Val{:y}, card::AbstractMatrix, v::Int)
    return card[1:v, :] .| card[2v + 1:-1:v + 2, :]
end

q1(card, folds) = sum(fold(card, folds[1]))
q2(card, folds) = reduce(fold, folds; init = card) |> pretty

function pretty(card::AbstractMatrix)::String
    join([join([v ? "##" : "  " for v in row], "")
          for row in eachrow(card)], "\n")
end

let (card, folds) = parse_input(test)
    @assert q1(card, folds) == 17
end

(card, folds) = parse_input(read("day13.in", String))
println("Q1: ", q1(card, folds))
println("Q2:\n", q2(card, folds))
