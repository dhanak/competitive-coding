#!/bin/bash
declare -i rtotal
declare -i ototal
for f in "$@"; do
    echo "$f: $(( $(< $f.out) - $(< $f.ref) ))"
    rtotal=$rtotal+$(< $f.ref)
    ototal=$ototal+$(< $f.out)
done
echo "-------------------"
echo "Total ref: $rtotal"
echo "Total out: $ototal"
echo "Total delta: $(( $ototal - $rtotal )) ($(( 100*($ototal - $rtotal)/$rtotal ))%)"
