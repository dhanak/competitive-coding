import Base: getindex, keys, read

struct Board
    values::Matrix{Int}
    marked::Set{CartesianIndex{2}}

    Board(v) = new(v, Set())
end

getindex(board::Board, idx) = getindex(board.values, idx)
keys(board::Board) = keys(board.values)

function read_input(f::AbstractString)
    return open(f, "r") do io
        numbers = parse.(Int, split(readline(io), ','))
        boards = []
        while !eof(io)
            # skip empty line
            @assert readline(io) == ""
            push!(boards, read(io, Board))
        end
        return ([boards...], numbers)
    end
end

function read(io::IO, ::Type{Board}; dims = (5, 5))
    board = vcat([let line = split(readline(io), r"\s+"; keepempty = false)
                      parse.(Int, line)'
                  end for _ in 1:dims[1]]...)
    @assert size(board) == dims
    return Board(board)
end

function mark!(board::Board, number::Int)::Board
    union!(board.marked, findall(==(number), board.values))
    return board
end

const ROWS_AND_COLUMNS = vcat([[[CartesianIndex(i, j) for j in 1:5],
                                [CartesianIndex(j, i) for j in 1:5]]
                               for i in 1:5]...)

function winning_score(board::Board)::Union{Nothing, Int}
    return any(fs -> fs ⊆ board.marked, ROWS_AND_COLUMNS) ?
        sum(board[setdiff(keys(board), board.marked)]) :
        nothing
end

function find_first_winner!(boards::AbstractVector{<: Board},
                            numbers::AbstractVector{<: Integer}
                           )::Union{Nothing, Int}
    for n in numbers, b in boards
        mark!(b, n)
        score = winning_score(b)
        if score !== nothing
            return score * n
        end
    end
    return nothing
end

function find_last_winner!(boards::AbstractVector{<: Board},
                           numbers::AbstractVector{<: Integer}
                          )::Union{Nothing, Int}
    winners = findall(b -> winning_score(b) !== nothing, boards)
    playing = setdiff(keys(boards), winners)
    for n in numbers, p in copy(playing)
        b = boards[p]
        mark!(b, n)
        score = winning_score(b)
        if score !== nothing
            deleteat!(playing, findfirst(==(p), playing))
            if isempty(playing)
                return score * n
            end
        end
    end
    return nothing
end

let (bs, ns) = read_input("day04-test.in")
    @assert find_first_winner!(bs, ns) == 4512
    @assert find_last_winner!(bs, ns) == 1924
end

(bs, ns) = read_input("day04.in")
println("Q1: ", find_first_winner!(bs, ns))
println("Q2: ", find_last_winner!(bs, ns))
