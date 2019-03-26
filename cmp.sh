#!/bin/bash

FILE=instances/simple-synthetic.su
C_INI=0.000000198
C_FIN=0.00000177
INCR=101 #quantidade de C testados
WIND=0.002 

make
./bin/cmp $FILE $C_INI $C_FIN $INCR $WIND
