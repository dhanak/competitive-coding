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

abstract type LinkedTree{T} end

LinkedTree(v::AbstractVector) = LinkedNode(LinkedTree.(v)...)
LinkedTree(v) = LinkedLeaf(v)

mutable struct LinkedLeaf{T} <: LinkedTree{T}
    v::T
    prev::Ref
    next::Ref

    LinkedLeaf(v::T) where {T} = new{T}(v, Ref(nothing), Ref(nothing))
end

mutable struct LinkedNode{T} <: LinkedTree{T}
    v::T                        # grabage can
    left::LinkedTree{T}
    right::LinkedTree{T}
    first::Ref
    last::Ref

    function LinkedNode(left::LinkedTree{T}, right::LinkedTree{T}) where {T}
        t = new{T}(zero(T), left, right, first(left), last(right))
        first(left)[].prev = Ref(t)
        last(left)[].next = first(right)
        first(right)[].prev = last(left)
        last(right)[].next = Ref(t)
        return t
    end
end

Base.first(leaf::LinkedLeaf) = Ref(leaf)
Base.first(node::LinkedNode) = node.first
Base.last(leaf::LinkedLeaf) = Ref(leaf)
Base.last(node::LinkedNode) = node.last

Base.show(io::IO, leaf::LinkedLeaf) = print(io, leaf.v)
function Base.show(io::IO, node::LinkedNode)
    print(io, '[')
    print(io, node.left)
    print(io, ',')
    print(io, node.right)
    print(io, ']')
end

as_list(leaf::LinkedLeaf) = leaf.v
as_list(node::LinkedNode) = [as_list(node.left), as_list(node.right)]

magnitude(v::AbstractVector) = magnitude(LinkedTree(v))
magnitude(leaf::LinkedLeaf) = leaf.v
magnitude(node::LinkedNode) = 3magnitude(node.left) + 2magnitude(node.right)

function explode!(v::AbstractVector; ok::Bool = true)
    (t, ok_) = explode!(LinkedTree(v))
    @assert ok_ == ok
    return as_list(t)
end

explode!(tree::LinkedTree) = explode!(tree, 0)
explode!(leaf::LinkedLeaf, depth) = (leaf, false)
function explode!(node::LinkedNode, depth)
    if depth == 4
        node.left.prev[].v += node.left.v
        node.right.next[].v += node.right.v
        return (LinkedLeaf(0), true)
    end
    (left, ok) = explode!(node.left, depth + 1)
    ok && return (LinkedNode(left, node.right), true)
    (right, ok) = explode!(node.right, depth + 1)
    ok && return (LinkedNode(node.left, right), true)
    return (node, false)
end

function split!(leaf::LinkedLeaf)
    i = leaf.v
    return i >= 10 ? (LinkedTree([i ÷ 2, i - i ÷ 2]), true) : (leaf, false)
end

function split!(node::LinkedNode)
    (left, ok) = split!(node.left)
    ok && return (LinkedNode(left, node.right), true)
    (right, ok) = split!(node.right)
    ok && return (LinkedNode(node.left, right), true)
    return (node, false)
end

function reduce!(tree::LinkedTree)
    while true
        (tree, ok) = explode!(tree)
        ok && continue
        (tree, ok) = split!(tree)
        !ok && break
    end
    return tree
end

add(left, right) = as_list(add!(LinkedTree(left), LinkedTree(right)))
add!(left::LinkedTree, right::LinkedTree) = reduce!(LinkedNode(left, right))

add_all(v) = as_list(foldl(add!, LinkedTree.(v)))

q1(v) = magnitude(foldl(add!, LinkedTree.(v)))

function q2(v)
    pairs = combinations(v, 2)
    return map(pairs) do (a, b)
        m1 = add!(LinkedTree(a), LinkedTree(b)) |> magnitude
        m2 = add!(LinkedTree(b), LinkedTree(a)) |> magnitude
        return max(m1, m2)
    end |> findmax |> first
end

@assert magnitude([[9,1], [1,9]]) == 129

@assert explode!([[[[[9,8],1],2],3],4]) == [[[[0,9],2],3],4]
@assert explode!([7,[6,[5,[4,[3,2]]]]]) == [7,[6,[5,[7,0]]]]
@assert explode!([[6,[5,[4,[3,2]]]],1]) == [[6,[5,[7,0]]],3]
@assert explode!([[3,[2,[1,[7,3]]]],[6,[5,[4,[3,2]]]]]) ==
    [[3,[2,[8,0]]],[9,[5,[4,[3,2]]]]]
@assert explode!([[3,[2,[8,0]]],[9,[5,[4,[3,2]]]]]) ==
    [[3,[2,[8,0]]],[9,[5,[7,0]]]]

@assert add([[[[4,3],4],4],[7,[[8,4],9]]], [1,1]) ==
    [[[[0,7],4],[[7,8],[6,0]]],[8,1]]
@assert add([[[0,[4,5]],[0,0]],[[[4,5],[2,6]],[9,5]]],
            [7,[[[3,7],[4,3]],[[6,3],[8,8]]]]) ==
                [[[[4,0],[5,4]],[[7,7],[6,0]]],[[8,[7,7]],[[7,9],[5,0]]]]

@assert add_all([[i,i] for i in 1:4]) == [[[[1,1],[2,2]],[3,3]],[4,4]]
@assert add_all([[i,i] for i in 1:5]) == [[[[3,0],[5,3]],[4,4]],[5,5]]
@assert add_all([[i,i] for i in 1:6]) == [[[[5,0],[7,4]],[5,5]],[6,6]]
@assert add_all([[[[0,[4,5]],[0,0]],[[[4,5],[2,6]],[9,5]]],
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
    @assert add_all(v) ==
        [[[[6,6],[7,6]],[[7,7],[7,0]]],[[[7,7],[7,7]],[[7,8],[9,9]]]]
    @assert q1(v) == 4140
    @assert q2(v) == 3993
end

v = parse_input(read("day18.in", String))
println("Q1: ", q1(v))
println("Q2: ", q2(v))
