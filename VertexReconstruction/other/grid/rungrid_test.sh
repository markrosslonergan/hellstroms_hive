#!/bin/bash                                                                    


umask +w
source /cvmfs/uboone.opensciencegrid.org/products/setup_uboone.sh


log=log_${PROCESS}.txt 
echo "Running on "$HOSTNAME > $log
echo "CLUSTER " ${CLUSTER} >> $log
echo "PROCESS " ${PROCESS} >> $log
echo >> $log


setup ifdhc v2_0_1 -q e10:p2711:prof


RES=/pnfs/uboone/resilient/users/rmurrell
EXEC=RunVertexQuality
IN=vertex_quality_input
OUT=vertex_quality_output


#echo ifdh cp -D $RES/$IN/$EXEC $CONDIR_DIR_INPUT >> $log
#ifdh cp -D $RES/$IN/$EXEC $CONDIR_DIR_INPUT >> $log 2>&1

echo ifdh cp -D $PWD/$log $SCRATCH >> $log
ifdh cp -D $PWD/$log $SCRATCH