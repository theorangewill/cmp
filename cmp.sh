#!/bin/bash

# suwind key=cdp min=100 max=400 <simple-synthetic.su >simple-synthetic-cut.su 

#FILE=/home/william/Programas/referencia/data/simple-syntetic-micro_sorted.su
#FILE=instances/simple-synthetic.su
FILE=instances/simple-windowed.su
V_INI=2000.00
V_FIN=6000.00
V_INC=101
WIND=0.032 
APH=600
AZIMUTH=0.0

make clean
rm instances/*.out.su

make
if [ "$1" == "valgrind" ]; then
	valgrind ./bin/cmp $FILE $V_INI $V_FIN $V_INC $WIND $APH $AZIMUTH /home/william/Programas/referencia/scripts/cmp2.vel.su /home/william/Programas/referencia/scripts/cmp2.coher.su

else   
	#./bin/cmp $FILE $V_INI $V_FIN $V_INC $WIND $APH $AZIMUTH  /home/william/Programas/CMP_CRS/data/cmp_v.su /home/william/Programas/CMP_CRS/data/cmp_sembl.su
	./bin/cmp $FILE $V_INI $V_FIN $V_INC $WIND $APH $AZIMUTH /home/william/Programas/referencia/scripts/cmp2.vel.su /home/william/Programas/referencia/scripts/cmp2.coher.su
fi