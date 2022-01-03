using Base: splat
using Combinatorics: combinations, permutations
using LinearAlgebra: I, ×

function parse_input(io::IO)
    scanners = Vector{Vector{Int}}[]
    while !eof(io)
        line = readline(io)
        m = match(r"^--- scanner ([0-9]+) ---$", line)
        @assert m !== nothing
        scanner = parse(Int, m.captures[1])
        beacons = Vector{Int}[]
        line = readline(io)
        while !isempty(line)
            push!(beacons, parse.(Int, split(line, ',')))
            line = readline(io)
        end
        @assert scanner == length(scanners)
        push!(scanners, beacons)
    end
    return scanners
end

const PERMUTATIONS = [Matrix{Int}(I(3)[:, xyz]) for xyz in permutations(1:3)]

const TRANSFORMATIONS = let vs = Vector.(eachcol([I(3) -I(3)]))
    [[x y (x × y)] for x in vs for y in vs if !iszero(x × y)]
end

function distances(beacons::AbstractVector)::Vector
    return map(combinations(beacons, 2)) do (a, b)
        return abs.(a .- b)
    end
end

function overlap(scanner1::AbstractVector,
                 scanner2::AbstractVector;
                 min_overlap::Integer = 66
                )::Bool
    dists1 = distances(scanner1)
    dists2 = distances(scanner2)
    olap = map(PERMUTATIONS) do P
        intersect(dists1, [P * d for d in dists2]) |> length
    end |> maximum
    return olap >= min_overlap
end

function overlaps(scanners::AbstractVector)::Matrix
    return [overlap(s1, s2) for s1 in scanners, s2 in scanners]
end

struct Transform
    T
    d
end

(t::Transform)(v) = t.T * v + t.d

function find_transform(scanner1::AbstractVector, scanner2::AbstractVector)
    ((olap, d), T) = map(TRANSFORMATIONS) do T
        sc2 = [T * beacon for beacon in scanner2]
        return map(scanner1) do b1
            displacements = [b1 .- b2 for b2 in sc2]
            (olap, d) = map(displacements) do d
                return length(intersect(scanner1, [b2 .+ d for b2 in sc2]))
            end |> findmax
            return (olap, displacements[d])
        end |> maximum
    end |> findmax
    @assert olap >= 12
    return Transform(TRANSFORMATIONS[T], d)
end

function find_transforms(scanners::AbstractVector)
    olaps = overlaps(scanners)
    transforms = Vector{Any}(nothing, length(scanners))
    transforms[1] = identity
    unaligned = collect(2:length(scanners))
    while !isempty(unaligned)
        aligned = setdiff(keys(scanners), unaligned)
        (a, u) = findfirst(olaps[aligned, unaligned]).I
        (i1, i2) = (aligned[a], unaligned[u])
        T = find_transform(scanners[i1], scanners[i2])
        transforms[i2] = transforms[i1] ∘ T
        setdiff!(unaligned, i2)
    end
    return transforms
end

function q1(scanners::AbstractVector, transforms::AbstractVector)
    return union(map((T, sc) -> T.(sc), transforms, scanners)...) |> length
end

function q2(transforms::AbstractVector)
    scanners = map(T -> T([0, 0, 0]), transforms)
    return map(combinations(scanners, 2)) do (sc1, sc2)
        return abs.(sc1 .- sc2) |> sum
    end |> maximum
end

open("day19-test.in", "r") do io
    scanners = parse_input(io)
    transforms = find_transforms(scanners)

    @assert q1(scanners, transforms) == 79
    @assert q2(transforms) == 3621
end

open("day19.in", "r") do io
    scanners = parse_input(io)
    transforms = find_transforms(scanners)

    println("Q1: ", q1(scanners, transforms))
    println("Q2: ", q2(transforms))
end
