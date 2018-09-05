#!/bin/bash                                          


umask +w
source /cvmfs/uboone.opensciencegrid.org/products/setup_uboone.sh


log=log${PROCESS}.txt 
echo "Running on "$HOSTNAME > $log
echo "CLUSTER " ${CLUSTER} >> $log
echo "PROCESS " ${PROCESS} >> $log
echo >> $log


USER=amogan


RES=/pnfs/uboone/resilient/users/$USER
SCRATCH=/pnfs/uboone/scratch/users/$USER
VIN=vertex_quality_input
VOUT=vertex_quality_output

EXEC=RunVertexQuality
VVDICT=libVVDict_rdict.pcm
INPUT_PERM_FILE=permutations_$PROCESS.root
#INPUT_FILE=le_bnb_cosmic_200.root
#INPUT_FILE=le_nc_delta_rad_cosmic_200.root
INPUT_FILE=RunMerge2.root
#INPUT_PERM_FILE=root://fndca1.fnal.gov:1094/pnfs/fnal.gov/usr/uboone/resilient/users/$USER/vertex_quality_input/permutations/$PERM_FILE
#INPUT_FILE=root://fndca1.fnal.gov:1094/pnfs/fnal.gov/usr/uboone/resilient/users/$USER/vertex_quality_input/light_event_files/$FILE


echo setup uboonecode v06_26_01_13 -q e10:prof >> $log
setup uboonecode v06_26_01_13 -q e10:prof >> $log 2>&1
echo setup gcc v6_4_0 >> $log
setup gcc v6_4_0 >> $log 2>&1
echo setup root v6_12_04e -q e15:prof >> $log
setup root v6_12_04e -q e15:prof >> $log 2>&1
echo >> $log


PERM_DIR=permutations
EVENT_DIR=light_event_files
echo ifdh cp -D $RES/$VIN/$PERM_DIR/$INPUT_PERM_FILE $CONDOR_DIR_INPUT >> $log
ifdh cp -D $RES/$VIN/$PERM_DIR/$INPUT_PERM_FILE $CONDOR_DIR_INPUT >> $log 2>&1
echo >> $log
echo ifdh cp -D $RES/$VIN/$EVENT_DIR/$INPUT_FILE $CONDOR_DIR_INPUT >> $log
ifdh cp -D $RES/$VIN/$EVENT_DIR/$INPUT_FILE $CONDOR_DIR_INPUT >> $log 2>&1
echo >> $log


echo ifdh cp -D $RES/$VIN/$EXEC $CONDOR_DIR_INPUT >> $log
ifdh cp -D $RES/$VIN/$EXEC $CONDOR_DIR_INPUT >> $log 2>&1
echo >> $log

echo ifdh cp -D $RES/$VIN/$VVDICT $PWD >> $log
ifdh cp -D $RES/$VIN/$VVDICT $PWD >> $log 2>&1
echo >> $log

echo chmod 777 $CONDOR_DIR_INPUT/$EXEC >> $log
chmod 777 $CONDOR_DIR_INPUT/$EXEC >> $log 2>&1
echo >> $log

echo $CONDOR_DIR_INPUT/$EXEC $CONDOR_DIR_INPUT/$INPUT_PERM_FILE $CONDOR_DIR_INPUT/$INPUT_FILE >> $log
$CONDOR_DIR_INPUT/$EXEC $CONDOR_DIR_INPUT/$INPUT_PERM_FILE $CONDOR_DIR_INPUT/$INPUT_FILE >> $log 2>&1
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
    echo ifdh cp $ofile $SCRATCH/$VOUT/$PROCESS/$EXEC$PROCESS.root >> $log
    ifdh cp $ofile $SCRATCH/$VOUT/$PROCESS/$EXEC$PROCESS.root >> $log 2>&1
else 
    echo $ofile not found >> $log
fi
echo >> $log

echo ifdh cp -D $PWD/$log $SCRATCH/$VOUT/$PROCESS >> $log
ifdh cp -D $PWD/$log $SCRATCH/$VOUT/$PROCESS
