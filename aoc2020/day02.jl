test = """
    1-3 a: abcde
    1-3 b: cdefg
    2-9 c: ccccccccc
    """

function policy(spec::AbstractString)::Tuple{Int, Int, Char, String}
    (a, b, chr, pwd) = split(spec, r"[- :]+")
    return (parse.(Int, (a, b))..., only(chr), pwd)
end

q1((min, max, chr, pwd)) = min <= count(==(chr), pwd) <= max
q2((a, b, chr, pwd)) = count(==(chr), pwd[[a, b]]) == 1

let policies = IOBuffer(test) |> readlines .|> policy
    @assert count(q1, policies) == 2
    @assert count(q2, policies) == 1
end

policies = readlines("day02.in") .|> policy
println("Q1: ", count(q1, policies))
println("Q2: ", count(q2, policies))
