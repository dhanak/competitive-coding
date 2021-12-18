using Combinatorics: combinations

test = """
    [[[0,[5,8]],[[1,7],[9,6]]],[[4,[1,2]],[[1,4],2]]]
    [[[5,[2,8]],4],[5,[[9,9],0]]]
    [6,[[[6,2],[5,6]],[[7,6],[4,7]]]]
    [[[6,[0,7]],[0,9]],[4,[9,[9,0]]]]
    [[[7,[6,4]],[3,[1,3]]],[[[5,5],1],9]]
    [[6,[[7,3],[3,2]]],[[[3,8],[5,7]],4]]
    [[[[5,4],[7,7]],8],[[8,3],8]]
    [[9,3],[[9,9],[6,[4,9]]]]
    [[2,[[7,7],7]],[[5,8],[[9,3],[0,2]]]]
    [[[[5,2],5],[8,[3,7]]],[[5,[7,5]],[4,4]]]
    """

function parse_input(input::AbstractString)
    lines = split(input, '\n'; keepempty = false)
    return eval(Expr(:vect, Meta.parse.(lines)...))
end

struct ArrayRef
    v::Array
    i
end

Base.lastindex(ref::ArrayRef) = lastindex(ref[])
Base.getindex(ref::ArrayRef) = ref.v[ref.i]
Base.getindex(ref::ArrayRef, i) = ArrayRef(ref[], i)
Base.setindex!(ref::ArrayRef, x) = ref.v[ref.i] = x

reffirst(v) = v[] isa Vector ? reffirst(v[1]) : v
reflast(v) = v[] isa Vector ? reflast(v[end]) : v

magnitude(v::Integer) = v
magnitude(v::AbstractVector) = 3magnitude(v[1]) + 2magnitude(v[2])

explode!(v) = explode!(v, 0, Ref(0), Ref(0))
explode!(v::Integer, kwargs...) = (v, false)
function explode!(v::AbstractVector, depth, left, right)
    if depth == 4
        left[] += v[1]
        right[] += v[2]
        return (0, true)
    end
    (v1r, ok) = explode!(v[1], depth + 1, left, reffirst(ArrayRef(v, 2)))
    ok && return ([v1r, v[2]], true)
    (v2r, ok) = explode!(v[2], depth + 1, reflast(ArrayRef(v, 1)), right)
    ok && return ([v[1], v2r], true)
    return (v, false)
end

split!(i::Integer) = i >= 10 ? ([i ÷ 2, i - i ÷ 2], true) : (i, false)
function split!(v::AbstractVector)
    (v1s, ok) = split!(v[1])
    ok && return ([v1s, v[2]], true)
    (v2s, ok) = split!(v[2])
    ok && return ([v[1], v2s], true)
    return (v, false)
end

function reduce!(v::AbstractVector)
    while true
        (v, ok) = explode!(v)
        ok && continue
        (v, ok) = split!(v)
        !ok && break
    end
    return v
end

add(v1, v2) = reduce!(deepcopy([v1, v2]))

q1(v) = magnitude(foldl(add, v))

function q2(v)
    pairs = combinations(v, 2)
    return map(pairs) do (a, b)
        s1 = add(a, b)
        s2 = add(b, a)
        return maximum(magnitude.([s1, s2]))
    end |> findmax |> first
end

@assert magnitude([[9,1], [1,9]]) == 129

@assert explode!([[[[[9,8],1],2],3],4]) == ([[[[0,9],2],3],4], true)
@assert explode!([7,[6,[5,[4,[3,2]]]]]) == ([7,[6,[5,[7,0]]]], true)
@assert explode!([[6,[5,[4,[3,2]]]],1]) == ([[6,[5,[7,0]]],3], true)
@assert explode!([[3,[2,[1,[7,3]]]],[6,[5,[4,[3,2]]]]]) ==
    ([[3,[2,[8,0]]],[9,[5,[4,[3,2]]]]], true)
@assert explode!([[3,[2,[8,0]]],[9,[5,[4,[3,2]]]]]) ==
    ([[3,[2,[8,0]]],[9,[5,[7,0]]]], true)

@assert split!(11) == ([5, 6], true)

@assert add([[[[4,3],4],4],[7,[[8,4],9]]], [1,1]) ==
    [[[[0,7],4],[[7,8],[6,0]]],[8,1]]
@assert add([[[0,[4,5]],[0,0]],[[[4,5],[2,6]],[9,5]]],
            [7,[[[3,7],[4,3]],[[6,3],[8,8]]]]) ==
                [[[[4,0],[5,4]],[[7,7],[6,0]]],[[8,[7,7]],[[7,9],[5,0]]]]

@assert foldl(add, [[i,i] for i in 1:4]) == [[[[1,1],[2,2]],[3,3]],[4,4]]
@assert foldl(add, [[i,i] for i in 1:5]) == [[[[3,0],[5,3]],[4,4]],[5,5]]
@assert foldl(add, [[i,i] for i in 1:6]) == [[[[5,0],[7,4]],[5,5]],[6,6]]
@assert foldl(add, [[[[0,[4,5]],[0,0]],[[[4,5],[2,6]],[9,5]]],
                    [7,[[[3,7],[4,3]],[[6,3],[8,8]]]],
                    [[2,[[0,8],[3,4]]],[[[6,7],1],[7,[1,6]]]],
                    [[[[2,4],7],[6,[0,5]]],[[[6,8],[2,8]],[[2,1],[4,5]]]],
                    [7,[5,[[3,8],[1,4]]]],
                    [[2,[2,2]],[8,[8,1]]],
                    [2,9],
                    [1,[[[9,3],9],[[9,0],[0,7]]]],
                    [[[5,[7,4]],7],1],
                    [[[[4,2],2],6],[8,7]]]) ==
                        [[[[8,7],[7,7]],[[8,6],[7,7]]],[[[0,7],[6,6]],[8,7]]]

let v = parse_input(test)
    @assert foldl(add, v) ==
        [[[[6,6],[7,6]],[[7,7],[7,0]]],[[[7,7],[7,7]],[[7,8],[9,9]]]]
    @assert q1(v) == 4140
    @assert q2(v) == 3993
end

v = parse_input(read("day18.in", String))
println("Q1: ", q1(v))
println("Q2: ", q2(v))
