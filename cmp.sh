#!/bin/bash

# suwind key=cdp min=100 max=400 <simple-synthetic.su >simple-synthetic-cut.su 

#FILE=/home/william/Programas/referencia/data/simple-syntetic-micro_sorted.su
FILE=instances/simple-synthetic.su
#FILE=instances/simple-windowed.su
V_INI=2000.00
V_FIN=6000.00
V_INC=101
WIND=0.032 
APH=600
AZIMUTH=0.0

make clean
rm instances/*.out.su

mkdir results

make
CMD="./bin/cmp $FILE $V_INI $V_FIN $V_INC $WIND $APH $AZIMUTH"
if [ "$1" == "valgrind" ]; then
	valgrind $CMD
elif [ "$1" == "perf" ]; then
	perf record $CMD
else   
	$CMD
fi
