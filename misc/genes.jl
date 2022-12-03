#!/bin/env julia
# https://www.oktatas.hu/pub_bin/dload/kozoktatas/tanulmanyi_versenyek/oktv/oktv2020_2021_2ford/info2_flap2f_oktv_2021.pdf
# 2. feladat

"""
Minden DNS szekvencia leírható olyan karaktersorozattal, amely csak az A, C, G
és T karaktereket tartalmazhatja. Brit tudósok rájöttek, hogy egy DNS szekvencia
mutációt okozhat, ha valamely benne szereplő karakter darabszáma legalább a DNS
szekvencia hosszának a fele. Készíts programot, ami kiszámítja a vizsgált DNS
szekvenciának a leghosszabb összefüggő mutáns részsorozatát!
"""
function mutant_length(dns)
    dns_length = length(dns)
    acids = unique(dns)
    balance = fill(0, length(acids), dns_length + 1)
    for (c, r) in enumerate(indexin(dns, acids))
        balance[:, c + 1] = balance[:, c] .+ 1
        balance[r, c + 1] -= 2
    end

    maxl = 2maximum(count(==(acid), dns) for acid in acids)
    for l in maxl:-1:1
        i = 1
        while i + l - 1 <= dns_length
            short = ceil(Int, minimum(balance[:, i + l] - balance[:, i]) / 2)
            short <= 0 && return (i, l)
            i += short
        end
    end
    @assert false
end

if abspath(PROGRAM_FILE) == @__FILE__
    println(mutant_length(strip(readline())))
end
