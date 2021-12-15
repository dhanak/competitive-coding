test = """
    mxmxvkd kfcds sqjhc nhms (contains dairy, fish)
    trh fvjkl sbzzf mxmxvkd (contains dairy)
    sqjhc fvjkl (contains soy)
    sqjhc mxmxvkd sbzzf (contains fish)
    """

function parse_input(io)
    foods = []
    while !eof(io)
        line = readline(io)
        (ingredients, allergens) = split(line, " (contains ")
        ingredients = Symbol.(split(ingredients, r"\s+"; keepempty = false))
        allergens = isempty(allergens) ? String[] :
            Symbol.(split(rstrip(==(')'), allergens), ", "))
        push!(foods, (ingredients, allergens))
    end
    return foods
end

function q1(foods)::Int
    assmt = assignment(foods)
    allergic = values(assmt)
    ingredients = vcat(first.(foods)...)
    return deleteat!(ingredients, findall(∈(allergic), ingredients)) |> length
end

function q2(foods)::String
    assmt = sort(collect(assignment(foods)))
    return join(last.(assmt), ',')
end

function assignment(foods::AbstractVector)::Dict
    assmt = Dict{Symbol, Vector{Symbol}}()
    for (ingredients, allergens) in foods
        for allergen in allergens
            if !haskey(assmt, allergen)
                assmt[allergen] = copy(ingredients)
            else
                intersect!(assmt[allergen], ingredients)
            end
        end
    end

    n_unambig = 0
    unambig = findall(==(1) ∘ length, assmt)
    while n_unambig != length(unambig)
        covered = [only(v) for (k, v) in assmt if k ∈ unambig]
        for k in setdiff(keys(assmt), unambig)
            setdiff!(assmt[k], covered)
        end
        n_unambig = length(unambig)
        unambig = findall(==(1) ∘ length, assmt)
    end

    return Dict(k => only(v) for (k, v) in assmt)
end

let foods = parse_input(IOBuffer(test))
    @assert q1(foods) == 5
    @assert q2(foods) == "mxmxvkd,sqjhc,fvjkl"
end

open("day21.in", "r") do io
    foods = parse_input(io)
    println("Q1: ", q1(foods))
    println("Q2: ", q2(foods))
end
