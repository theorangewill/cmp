#!/bin/bash

FILE=instances/ordenado.su
C_INI=0.000000198
C_FIN=0.00000177
INCR=101 #quantidade de C testados
WIND=0.002 

make clean
make

sleep 2

if [ "$1" == "valgrind" ]; then
    valgrind ./bin/cmp $FILE $C_INI $C_FIN $INCR $WIND
else   
    ./bin/cmp $FILE $C_INI $C_FIN $INCR $WIND 
fi
