#!/bin/env python
# https://www.oktatas.hu/pub_bin/dload/kozoktatas/tanulmanyi_versenyek/oktv/oktv2020_2021_2ford/info2_flap2f_oktv_2021.pdf
# 2. feladat

from sys import stdin
from math import ceil


def mutant_length(dns):
    """Minden DNS szekvencia leírható olyan karaktersorozattal, amely csak az A, C,
    G és T karaktereket tartalmazhatja. Brit tudósok rájöttek, hogy egy DNS
    szekvencia mutációt okozhat, ha valamely benne szereplő karakter darabszáma
    legalább a DNS szekvencia hosszának a fele. Készíts programot, ami
    kiszámítja a vizsgált DNS szekvenciának a leghosszabb összefüggő mutáns
    részsorozatát!

    """
    dns_length = len(dns)
    acids = list(set(dns))
    balance = [[0 for _ in acids]]
    for acid in dns:
        c = [x + 1 for x in balance[-1]]
        c[acids.index(acid)] -= 2
        balance.append(c)

    maxl = 2 * max([dns.count(acid) for acid in acids])
    for l in range(maxl, 0, -1):
        i = 0
        while i + l <= dns_length:
            delta = [balance[i + l][r] - balance[i][r]
                     for r in range(len(acids))]
            short = ceil(min(delta) / 2)
            if short <= 0:
                return i + 1, l
            i += short
    assert(False)


if __name__ == "__main__":
    dns = stdin.readline().strip()
    print(mutant_length(dns))
