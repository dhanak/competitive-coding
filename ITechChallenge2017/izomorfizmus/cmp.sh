#!/bin/sh
args=''
for i in $*; do args="${args} epulet.$i"; done
make -f ../Makefile izomorf && (echo $#; cat $args) | ./izomorf
