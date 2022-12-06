tests = ["mjqjpqmgbljsphdztnvjfqwrcgsmlb"
         "bvwbjplbgvbhsrlpgdmjqwftvncz"
         "nppdvjthqldpwncqszvftbrmjlhg"
         "nznrnfrfntjfmvfwmzdfjlvtqnbhcprsg"
         "zcfzfwzzqfrljwzlrfnpqdbhtmscgvjw"]

function detect(buffer::AbstractString, blocklen::Integer)::Int
    blockend = blocklen - 1
    return findfirst(1:length(buffer) - blockend) do i
        return allunique(buffer[i:i + blockend])
    end + blockend
end

q1(buffer::AbstractString) = detect(buffer, 4)
q2(buffer::AbstractString) = detect(buffer, 14)

@assert q1.(tests) == [7, 5, 6, 10, 11]
@assert q2.(tests) == [19, 23, 23, 29, 26]

stream = read("day06.in", String)
println("Q1: ", q1(stream))
println("Q2: ", q2(stream))
