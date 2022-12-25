using Test: @testset, @test

test = """1=-0-2
          12111
          2=0=
          21
          2=01
          111
          20012
          112
          1=-1=
          1-12
          12
          1=
          122"""

function from_snafu(n::AbstractString)::Int
    return foldr(n; init = (1, 0)) do c, (d, t)
        return (d * 5, t + (findfirst(c, "=-012") - 3)  * d)
    end |> last
end

function to_snafu(n::Int)::String
    maxes = cumsum(2 .* cumprod([1; fill(5, 26)]))
    ndigits = findfirst(>=(n), maxes)
    return foldr(0:ndigits - 1; init = (n, "")) do i, (n, s)
        max_i = i > 0 ? maxes[i] : 0
        vs = [j * 5 ^ i for j in -2:2]
        j = findfirst(vs) do v
            return n ∈ v - max_i:v + max_i
        end
        return (n - vs[j], s * "=-012"[j])
    end |> last
end

q1(v) = v .|> from_snafu |> sum |> to_snafu

@testset begin
    v = split(test, '\n')
    @test from_snafu("2=-01") == 976
    @test to_snafu(314159265) == "1121-1110-1=0"
    @test q1(v) == "2=-1=0"
end

@time begin
    v = readlines("day25.in")
    println("Q1: ", q1(v))
end
