#!/bin/bash                                                                    


umask +w
source /cvmfs/uboone.opensciencegrid.org/products/setup_uboone.sh


log=log${PROCESS}.txt 
echo "Running on "$HOSTNAME > $log
echo "CLUSTER " ${CLUSTER} >> $log
echo "PROCESS " ${PROCESS} >> $log
echo >> $log


USER=rmurrell
EXEC=RunVertexQuality
INPUT_PERM_FILE=root://fndca1.fnal.gov:1094/pnfs/fnal.gov/usr/uboone/scratch/users/rmurrell/vertex_quality_input/permutations_0.root
INPUT_FILE=root://fndca1.fnal.gov:1094/pnfs/fnal.gov/usr/uboone/scratch/users/rmurrell/vertex_quality_input/FillLightEvent_1.root


setup uboonecode v06_26_01_10 -q e10:prof


SCRATCH=/pnfs/uboone/scratch/users/$USER
#CONDOR_DIR_INPUT=$SCRATCH/vertex_quality_input
GRID_DIR=$SCRATCH/vertex_quality_input
#echo ifdh cp -D $GRID_DIR/$EXEC $CONDOR_DIR_INPUT >> $log
#ifdh cp -D $SCRATCH/$GRID_DIR/$EXEC $CONDOR_DIR_INPUT >> $log 2>&1
echo cp $GRID_DIR/$EXEC $PWD >> $log
cp $GRID_DIR/$EXEC $PWD >> $log 2>&1
echo >> $log


echo ./$CONDOR_DIR_INPUT/$EXEC $INPUT_FILE >> $log
#$CONDOR_DIR_INPUT/$EXEC $INPUT_PERM_FILE $INPUT_FILE
$PWD/$EXEC $INPUT_PERM_FILE $INPUT_FILE >> $log 2>&1
echo >> $log


OUT=vertex_quality_output
echo ifdh mkdir $SCRATCH/$OUT/$PROCESS >> $log
ifdh mkdir $SCRATCH/$OUT/$PROCESS >> $log 2>&1
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


echo ifdh cp -D $PWD/$log $SCRATCH/$OUT/$PROCESS >> $log
ifdh cp -D $PWD/$log $SCRATCH/$OUT/$PROCESS