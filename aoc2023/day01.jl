using Test: @testset, @test

test1 = """
        1abc2
        pqr3stu8vwx
        a1b2c3d4e5f
        treb7uchet
        """

test2 = """
        two1nine
        eightwothree
        abcone2threexyz
        xtwone3four
        4nineeightseven2
        zoneight234
        7pqrstsixteen
        """

digits = ["zero",
          "one",
          "two",
          "three",
          "four",
          "five",
          "six",
          "seven",
          "eight",
          "nine"]

function string_to_digit(s::AbstractString)::Integer
    return length(s) == 1 ? s[1] - '0' : findfirst(==(s), digits) - 1
end

function solve(lines::AbstractVector{<: AbstractString},
               digits::AbstractVector{<: AbstractString} = String[]
              )::Int
    dres = join(['0':'9'; digits], '|')
    dre = Regex(dres)
    rdre = Regex(reverse(dres))

    return map(filter(!isempty, lines)) do line
        a = string_to_digit(match(dre, line).match)
        b = string_to_digit(reverse(match(rdre, reverse(line)).match))
        return 10a + b
    end |> sum
end

@testset begin
    @test solve(split(test1, '\n')) == 142
    @test solve(split(test2, '\n'), digits) == 281
end

@time begin
    v = readlines("day01.in")
    println("Q1: ", solve(v))
    println("Q2: ", solve(v, digits))
end
