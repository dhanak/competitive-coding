using Test

flip(bit) = bit == '0' ? '1' : '0'
most_common(bits) = 2count(==('0'), bits) > length(bits) ? '0' : '1'
least_common(bits) = flip.(most_common(bits))
lines_to_bits(lines) = hcat(Vector{Char}.(lines)...)
parse_bits(chars::Vector{Char}) = parse(Int, join(chars, ""); base = 2)

codes = readlines("day03.in")
test_codes = ["00100",
              "11110",
              "10110",
              "10111",
              "10101",
              "01111",
              "00111",
              "11100",
              "10000",
              "11001",
              "00010",
              "01010"]

function q1(codes)
    codebits = lines_to_bits(codes)
    γ = most_common.(eachrow(codebits))
    ε = flip.(γ)
    return parse_bits(γ) * parse_bits(ε)
end

function rating(criteria, codebits)
    matching = reduce(eachrow(codebits), init = 1:size(codebits, 2)) do m, row
        return length(m) == 1 ? m : m[row[m] .== criteria(row[m])]
    end
    return vec(codebits[:, matching])
end

function q2(codes)
    codebits = lines_to_bits(codes)
    oxygen_rating = rating(most_common, codebits)
    co2_rating = rating(least_common, codebits)
    return parse_bits(oxygen_rating) * parse_bits(co2_rating)
end

@test q1(test_codes) == 198
@test q2(test_codes) == 230

println("Q1: ", q1(codes))
println("Q2: ", q2(codes))
