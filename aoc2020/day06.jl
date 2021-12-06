test = """
    abc

    a
    b
    c

    ab
    ac

    a
    a
    a
    a

    b
    """

compute(fn, io) = sum(fn, split(read(io, String), "\n\n"))

function q1(group::AbstractString)::Integer
    return split(replace(group, r"\s+" => ""), "") |> unique |> length
end

function q2(group::AbstractString)::Integer
    people = split(group, r"\n"; keepempty = false)
    return split.(people, "") |> Base.splat(intersect) |> length
end

@assert compute(q1, IOBuffer(test)) == 11
@assert compute(q2, IOBuffer(test)) == 6

println("Q1: ", compute(q1, "day06.in"))
println("Q2: ", compute(q2, "day06.in"))
