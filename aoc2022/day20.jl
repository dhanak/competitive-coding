using Test: @testset, @test

test = "1,2,-3,3,-2,0,4"

function mix!(v)
    for i in keys(v)
        j = findfirst(==(i) ∘ first, v)
        n = v[j][2]
        k = rem(j + n - 1, length(v) - 1, RoundDown) + 1
        insert!(deleteat!(v, j), k, (i, n))
    end
    return v
end

function decode(v)
    z = only(findall(iszero, v))
    return sum([v[(i + end - 1) % end + 1] for i in z .+ [1000, 2000, 3000]])
end

feed(v) = v |> enumerate |> collect
sink(v) = v .|> last |> decode

q1(v) = v |> feed |> mix! |> sink
q2(v) = v .* 811589153 |> feed |> reduce(∘, fill(mix!, 10)) |> sink

@testset begin
    v = parse.(Int, split(test, ','))
    @test q1(v) == 3
    @test q2(v) == 1623178306
end

v = parse.(Int, readlines("day20.in"))
println("Q1: ", q1(v))
println("Q2: ", q2(v))
