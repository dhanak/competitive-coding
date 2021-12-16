test = ["8A004A801A8002F478",
        "620080001611562C8802118E34",
        "C0015000016115A2E0802F182340",
        "A0016C880162017C3686B18A3D4780"]

@enum PacketType begin
    SUM          = 0
    PROD         = 1
    MINIMUM      = 2
    MAXIMUM      = 3
    LITERAL      = 4
    GREATER_THAN = 5
    LESS_THAN    = 6
    EQUAL        = 7
end

function parse_packet(packet::AbstractString)
    (packet, rest) = all(∈(['0', '1']), packet) ?
        parse_bin_packet(packet) :
        parse_hexa_packet(packet)
    @assert all(==('0'), rest)
    return packet
end

function parse_hexa_packet(hexa::AbstractString)
    chars = split(hexa, "")
    binary = join(string.(parse.(Int, chars; base = 16); base = 2, pad = 4))
    return parse_bin_packet(binary)
end

function parse_bin_packet(bin::AbstractString)
    version = unpack(bin[1:3])
    type = PacketType(unpack(bin[4:6]))
    (payload, rest) = parse_bin_packet(Val(type), bin[7:end])
    return ((; version, type = type, payload), rest)
end

function parse_bin_packet(::Val{LITERAL}, bin)
    stop = 5 * findfirst(==('0'), bin[1:5:end])
    bits = reshape(split(bin, "")[1:stop], 5, :)
    @assert all(==("1"), bits[1, 1:end - 1]) && bits[1, end] == "0"
    return (unpack(join(bits[2:end, 1:end])), bin[stop + 1:end])
end

function parse_bin_packet(::Val, bin)
    subs = []
    if bin[1] == '0'
        len = unpack(bin[2:16])
        rest = bin[17:17 + len - 1]
        while !isempty(rest)
            (sub, rest) = parse_bin_packet(rest)
            push!(subs, sub)
        end
        return (subs, bin[17 + len:end])
    else
        rest = bin[13:end]
        for i in 1:unpack(bin[2:12])
            (sub, rest) = parse_bin_packet(rest)
            push!(subs, sub)
        end
    end
    return (subs, rest)
end

unpack(bits::AbstractString) = parse(Int, bits; base = 2)

function q1(packet)
    return packet.version +
        (packet.type != LITERAL ? sum(q1, packet.payload) : 0)
end

evaluate(packet) = evaluate(Val(packet.type), packet.payload)
evaluate(::Val{LITERAL},      payload) = payload
evaluate(::Val{SUM},          payload) = sum(evaluate, payload)
evaluate(::Val{PROD},         payload) = prod(evaluate, payload)
evaluate(::Val{MINIMUM},      payload) = payload .|> evaluate |> minimum
evaluate(::Val{MAXIMUM},      payload) = payload .|> evaluate |> maximum
evaluate(::Val{LESS_THAN},    payload) = <(evaluate.(payload)...)
evaluate(::Val{GREATER_THAN}, payload) = >(evaluate.(payload)...)
evaluate(::Val{EQUAL},        payload) = ==(evaluate.(payload)...)

q2(packet) = evaluate(packet)

@assert parse_packet("D2FE28") ==
    (version = 6, type = LITERAL, payload = 2021)

@assert parse_packet("38006F45291200") ==
    (version = 1,
     type = LESS_THAN,
     payload = [(version = 6, type = LITERAL, payload = 10),
                (version = 2, type = LITERAL, payload = 20)])

@assert parse_packet("EE00D40C823060") ==
    (version = 7,
     type = MAXIMUM,
     payload = [(version = 2, type = LITERAL, payload = 1),
                (version = 4, type = LITERAL, payload = 2),
                (version = 1, type = LITERAL, payload = 3)])

@assert sum(q1, parse_packet.(test)) == 16 + 12 + 23 + 31

packet = parse_packet(strip(read("day16.in", String)))
println("Q1: ", q1(packet))
println("Q2: ", q2(packet))
