#!/bin/bash

# suwind key=cdp min=100 max=400 <simple-synthetic.su >simple-synthetic-cut.su 

#FILE=/home/william/Programas/referencia/data/simple-syntetic-micro_sorted.su
#FILE=instances/simple-synthetic.su
FILE=instances/simple-synthetic-cut.su
C_INI=0.000000198
C_FIN=0.00000177
C_INC=101
WIND=0.002 
APH=600

make clean
rm instances/*.out.su

make
if [ "$1" == "valgrind" ]; then
	valgrind ./bin/cmp $FILE $C_INI $C_FIN $C_INC $WIND $APH
else   
	./bin/cmp $FILE $C_INI $C_FIN $C_INC $WIND $APH
fi