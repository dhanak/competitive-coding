test = """
    vJrwpWtwJgWrhcsFMMfFFhFp
    jqHRNqRjqzjGDLGLrsFMfFZSrLrFZsSL
    PmmdzqPrVvPwwTWBwg
    wMqvLMZHhHMvwLHjbvcjnnSBnvTQFn
    ttgJtRGJQctTZtZT
    CrZsJsPPZsGzwwsLwLmpwMDw
    """

function parse_input(input::AbstractString)
    lines = split(input, r"\s+"; keepempty = false)
    return lines
end

function q1(rucksacks)
    return sum(rucksacks) do sack
        return priority(only(sack[1:end ÷ 2] ∩ sack[end ÷ 2 + 1:end]))
    end
end

function q2(rucksacks)
    return sum(eachcol(reshape(rucksacks, 3, :))) do elves
        return priority(only(intersect(elves...)))
    end
end

priority(item) = islowercase(item) ? item - 'a' + 1 : item - 'A' + 27

let v = parse_input(test)
    @assert q1(v) == 157
    @assert q2(v) == 70
end

v = parse_input(read("day03.in", String))
println("Q1: ", q1(v))
println("Q2: ", q2(v))
