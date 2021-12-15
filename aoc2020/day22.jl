test = """
    Player 1:
    9
    2
    6
    3
    1

    Player 2:
    5
    8
    4
    7
    10
    """

function parse_input(input::AbstractString)::Vector{Vector}
    decks = split.(split(input, "\n\n"), '\n'; keepempty = false)
    @assert all(startswith.(first.(decks), "Player"))
    return [parse.(Int, deck[2:end]) for deck in decks]
end

function game(combat::Function, decks)::Int
    decks = deepcopy(decks)
    winner = combat(decks)
    winning = decks[winner]
    return sum(winning .* reverse(keys(winning)))
end

function combat!(decks)::Int
    while !any(isempty.(decks))
        simple_step!(decks)
    end
    return findfirst(!isempty, decks)
end

function simple_step!(decks)
    play = popfirst!.(decks)
    winner = argmax(play)
    append!(decks[winner], play[[winner, 3 - winner]])
    return decks
end

function recursive_combat!(decks)::Int
    deal_hashes = Set()
    while !any(isempty.(decks))
        h = hash(decks)
        h ∈ deal_hashes && return 1
        push!(deal_hashes, h)
        recursive_step!(decks)
    end
    return findfirst(!isempty, decks)
end

function recursive_step!(decks)
    play = popfirst!.(decks)
    if all(@. play <= length(decks))
        subdecks = getindex.(decks, range.(1, play; step = 1))
        winner = recursive_combat!(subdecks)
    else
        winner = argmax(play)
    end
    append!(decks[winner], play[[winner, 3 - winner]])
    return decks
end

q1(decks) = game(combat!, decks)
q2(decks) = game(recursive_combat!, decks)

let decks = parse_input(test)
    @assert q1(decks) == 306
    @assert q2(decks) == 291
end

decks = parse_input(read("day22.in", String))
println("Q1: ", q1(decks))
println("Q2: ", q2(decks))
