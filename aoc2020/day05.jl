function decode_pass(pass::AbstractString)::Tuple
    row = parse(Int, map(c -> c =='B' ? '1' : '0', pass[1:7]); base = 2)
    seat = parse(Int, map(c -> c =='R' ? '1' : '0', pass[8:10]); base = 2)
    return (row, seat)
end

seat_id((row, seat)) = 8row + seat

@assert decode_pass("FBFBBFFRLR") == (44, 5)
@assert seat_id((44, 5)) == 357

seats = readlines("day05.in") .|> decode_pass .|> seat_id |> sort!
println("Q1: ", last(seats))
println("Q2: ", seats[findfirst(==(2), diff(seats))] + 1)
