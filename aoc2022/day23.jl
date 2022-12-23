using Test: @testset, @test

test = """....#..
          ..###.#
          #...#.#
          .#...##
          #.###..
          ##.#.##
          .#..#.."""

function parse_input(lines)
    grove = lines .|> collect |> Base.splat(hcat) |> permutedims
    elves = findall(==('#'), grove)
    return elves
end

const CI = CartesianIndex
const NEIGHBORS = [[vec(CI(-1, -1):CI(-1,  1));
                    CI( 0, -1); CI(0, 1);
                    vec(CI( 1, -1):CI( 1,  1))], # all
                   CI(-1, -1):CI(-1,  1),        # N
                   CI( 1, -1):CI( 1,  1),        # S
                   CI(-1, -1):CI( 1, -1),        # W
                   CI(-1,  1):CI( 1,  1)]        # E
const MOVES = [CI(0, 0), CI(-1, 0), CI(1, 0), CI(0, -1), CI(0, 1)]

function spread(elves::Vector, rounds::Integer)
    border = CI(100, 100)
    (min, max) = extrema(elves)
    min -= CI(1, 1)
    board = fill(0, (max - min + 2border).I)
    elves = elves .+ Ref(border - min)
    board[elves] .= 1

    for r in 1:rounds
        ns = [1; ((0:3) .+ r .- 1) .% 4 .+ 2]
        proposals = map(elves) do elf
            i = findfirst(NEIGHBORS[ns]) do n
                all(iszero, board[Ref(elf) .+ n])
            end
            return elf + MOVES[ns[something(i, 1)]]
        end

        stay = findall(elves .== proposals)
        move = setdiff(keys(elves), stay)
        for i in move
            board[proposals[i]] += 1
        end
        accept = filter(i -> board[proposals[i]] == 1, move)
        isempty(accept) && return (elves, r)

        board[elves[accept]] .= 0
        elves[accept] = proposals[accept]
        reject = setdiff(move, accept)
        if !isempty(reject)
            board[proposals[reject]] .= 0
        end
    end

    return (elves, rounds)
end

function q1(elves::Vector)::Int
    (elves, _) = spread(elves, 10)
    (min, max) = extrema(elves)
    size = max - min + CI(1, 1)
    return prod(size.I) - length(elves)
end

function q2(elves::Vector)::Int
    (_, rounds) = spread(elves, typemax(Int))
    return rounds
end

@testset begin
    v = parse_input(split(test, '\n'))
    @test q1(v) == 110
    @test q2(v) == 20
end

@time begin
    v = parse_input(readlines("day23.in"))
    println("Q1: ", q1(v))
    println("Q2: ", q2(v))
end
