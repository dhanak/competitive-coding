test = """
    ecl:gry pid:860033327 eyr:2020 hcl:#fffffd
    byr:1937 iyr:2017 cid:147 hgt:183cm

    iyr:2013 ecl:amb cid:350 eyr:2023 pid:028048884
    hcl:#cfa07d byr:1929

    hcl:#ae17e1 iyr:2013
    eyr:2024
    ecl:brn pid:760753108 byr:1931
    hgt:179cm

    hcl:#cfa07d eyr:2025 pid:166559648
    iyr:2011 ecl:brn hgt:59in
    """

function read_passport(ppt::AbstractString)::Dict{Symbol, <: AbstractString}
    return Dict(let (key, value) = split(field, ':')
                    Symbol(key) => value
                end
                for field in split(ppt, r"\s+"; keepempty = false))
end

function read_passports(io)::Vector
    return read_passport.(split(read(io, String), "\n\n"))
end

const REQ_FIELDS = [:byr, :iyr, :eyr, :hgt, :hcl, :ecl, :pid]

q1(ppt::AbstractDict)::Bool = REQ_FIELDS ⊆ keys(ppt)

byr(v) = 1920 <= parse(Int, v) <= 2002
iyr(v) = 2010 <= parse(Int, v) <= 2020
eyr(v) = 2020 <= parse(Int, v) <= 2030
hcl(v) = length(v) == 7 && v[1] == '#' && all(∈(['0':'9'; 'a':'f']), v[2:7])
ecl(v) = Symbol(v) ∈ [:amb, :blu, :brn, :gry, :grn, :hzl, :oth]
pid(v) = length(v) == 9 && tryparse(Int, v) !== nothing
cid(v) = true

function hgt(s)::Bool
    try
        v, u = parse(Int, s[1:end - 2]), Symbol(s[end - 1:end])
        return u == :in && 59 <= v <= 76 ||
            u == :cm && 150 <= v <= 193
    catch
        return false
    end
end

function q2(ppt::AbstractDict)::Bool
    return q1(ppt) && all(ppt) do (key, value)
        return getfield(@__MODULE__, key)(value)
    end
end

let passports = read_passports(IOBuffer(test))
    @assert count(q1, passports) == 2
    @assert count(q2, passports) == 2
end

passports = read_passports("day04.in")
println("Q1: ", count(q1, passports))
println("Q2: ", count(q2, passports))
