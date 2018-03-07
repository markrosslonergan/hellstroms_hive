#!/bin/bash                                                                    


umask +w
source /cvmfs/uboone.opensciencegrid.org/products/setup_uboone.sh


log=log${PROCESS}.txt 
echo "Running on "$HOSTNAME > $log
echo "CLUSTER " ${CLUSTER} >> $log
echo "PROCESS " ${PROCESS} >> $log
echo >> $log


RES=/pnfs/uboone/resilient/users/rmurrell
SCRATCH=/pnfs/uboone/scratch/users/rmurrell
VIN=vertex_quality_input
VOUT=vertex_quality_output

EXEC=RunVertexQuality
INPUT_PERM_FILE=root://fndca1.fnal.gov:1094/pnfs/fnal.gov/usr/uboone/scratch/users/rmurrell/vertex_quality_input/permutations_$PROCESS.root
INPUT_FILE=root://fndca1.fnal.gov:1094/pnfs/fnal.gov/usr/uboone/scratch/users/rmurrell/vertex_quality_input/FillLightEvent_1.root


echo setup uboonecode v06_26_01_10 -q e10:prof >>$log
setup uboonecode v06_26_01_10 -q e10:prof >> $log 2>&1
echo >> $log

echo ifdh cp -D $RES/$VIN/$EXEC $CONDOR_DIR_INPUT >> $log
ifdh cp -D $RES/$VIN/$EXEC $CONDOR_DIR_INPUT >> $log 2>&1
echo >> $log

echo chmod 777 $CONDOR_DIR_INPUT/$EXEC >> $log
chmod 777 $CONDOR_DIR_INPUT/$EXEC >> $log 2>&1
echo >> $log

echo $CONDOR_DIR_INPUT/$EXEC $INPUT_PERM_FILE $INPUT_FILE >> $log
$CONDOR_DIR_INPUT/$EXEC $INPUT_PERM_FILE $INPUT_FILE >> $log 2>&1
echo >> $log


echo ifdh mkdir $SCRATCH/$VOUT/$PROCESS >> $log
ifdh mkdir $SCRATCH/$VOUT/$PROCESS >> $log 2>&1
echo >> $log

ofile=$PWD/$EXEC.root
echo Output file: $ofile >> $log
echo >> $log
ls $ofile >> $log 2>&1
if [ $? -eq 0 ];
then
    chmod 777 $ofile
    echo ifdh cp -D $ofile $SCRATCH/$OUT/$PROCESS >> $log
    ifdh cp -D $ofile $SCRATCH/$OUT/$PROCESS >> $log 2>&1
else 
    echo $ofile not found >> $log
fi
echo >> $log

echo ifdh cp -D $PWD/$log $SCRATCH/$VOUT >> $log
ifdh cp -D $PWD/$log $SCRATCH/$VOUT