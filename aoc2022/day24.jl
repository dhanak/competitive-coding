using Test: @testset, @test

test = """#.######
          #>>.<^<#
          #.<..<<#
          #>v.><>#
          #<^v^^>#
          ######.#"""

const CI = CartesianIndex
const BLIZZARDS = ">v<^"
const MOVES = [CI(0, 1), CI(1, 0), CI(0, -1), CI(-1, 0)]

Base.rem(a::CI, b::CI) = CI(a.I .% b.I)

function parse_input(lines::AbstractVector{<: AbstractString})
    l = length(lines[1])
    @assert all(==(l) ∘ length, lines)
    @assert all(==('#'), lines[1][[1; 3:end]])
    @assert lines[1][2] == '.'
    @assert all(==('#'), lines[end][[1:end-3; end]])
    @assert lines[end][end - 1] == '.'
    @assert all(==('#') ∘ first, lines)
    @assert all(==('#') ∘ last, lines)
    M = permutedims(hcat(collect.(lines)...))
    blizzards = findall(∈(BLIZZARDS), M)
    return (size(M)...,
            Set((b, findfirst(==(M[b]), BLIZZARDS)) for b in blizzards))
end

function move(h::Int, w::Int, blizzards::Set)::Set
    bounds = CI(2, 2):CI(h - 1, w - 1)
    return Set(let b1 = b + MOVES[m]
                   (b1 ∈ bounds ? b1 :
                       (b1 + CI(h - 4, w - 4)) % CI(h - 2, w - 2) + CI(2, 2),
                    m)
               end for (b, m) in blizzards)
end

function print_board(h::Int, w::Int, blizzards::Set)
    M = fill('.', h, w)
    M[1, [1; 3:end]] .= '#'
    M[end, [1:end - 2; end]] .= '#'
    M[:, 1] .= '#'
    M[:, end] .= '#'
    for (b, m) in blizzards
        M[b] = M[b] == '.' ? BLIZZARDS[m] : M[b] ∈ BLIZZARDS ? '2' : M[b] + 1
    end
    println(join(join.(eachrow(M), ""), "\n"))
end

function solve(h::Int, w::Int, blizzards::Set; start, stop)
    bounds = CI(2, 2):CI(h - 1, w - 1)
    minutes = [blizzards]
    visit = [(start, 1)]
    for (i, m) in visit
        m1 = m + 1
        if length(minutes) < m1
            push!(minutes, move(h, w, minutes[end]))
        end
        s = minutes[m1]
        for j in Ref(i) .+ [CI(0, 0); MOVES]
            if j == stop
                return (m, s)
            elseif i == j == start
                push!(visit, (j, m1))
            elseif j ∈ bounds && all(m -> (j, m) ∉ s, 1:4) && (j, m1) ∉ visit
                push!(visit, (j, m1))
            end
        end
    end
    @assert false
    return (0, minutes[1])
end

q1(t::Tuple) = q1(t...)
function q1(h::Int, w::Int, bs::Set)::Int
    return first(solve(h, w, bs; start = CI(1, 2), stop = CI(h, w - 1)))
end

q2(t::Tuple) = q2(t...)
function q2(h::Int, w::Int, bs::Set)::Int
    a = CI(1, 2)
    b = CI(h, w - 1)
    (n1, bs1) = solve(h, w, bs;  start = a, stop = b)
    (n2, bs2) = solve(h, w, bs1; start = b, stop = a)
    (n3, _)   = solve(h, w, bs2; start = a, stop = b)
    return n1 + n2 + n3
end

@testset begin
    v = parse_input(split(test, '\n'))
    @test q1(v) == 18
    @test q2(v) == 54
end

# slow, takes about 1min to finish
@time begin
    v = parse_input(readlines("day24.in"))
    println("Q1: ", q1(v))
    println("Q2: ", q2(v))
end
