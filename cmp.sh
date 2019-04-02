#!/bin/bash

FILE=instances/simple-synthetic.su
C_INI=0.000000198
C_FIN=0.00000177
C_INC=101
WIND=0.002 
APH=600

make clean

make
if [ "$2" == "valgrind" ]; then
	valgrind ./bin/cmp $FILE $C_INI $C_FIN $C_INC $WIND $APH
else   
	./bin/cmp $FILE $C_INI $C_FIN $C_INC $WIND $APH
fi