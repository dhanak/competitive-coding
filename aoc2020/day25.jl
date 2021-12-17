test = (5764801, 17807724)
puzzle = (8987316, 14681524)
modulus = 20201227
seed = 7

mods = let mods = Vector(undef, modulus - 1)
    mods[1] = seed
    for i in 2:modulus - 1
        mods[i] = (7 * mods[i - 1]) % modulus
    end
    mods
end
@assert mods[end] == 1

function q1(card, door)::Int
    loops = (findfirst(==(card), mods), findfirst(==(door), mods))
    return mods[(*(loops...) - 1) % (modulus - 1) + 1]
end

@assert q1(test...) == 14897079

println("Q1: ", q1(puzzle...))
