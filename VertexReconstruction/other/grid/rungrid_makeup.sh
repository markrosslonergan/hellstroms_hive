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
makeupdir=makeup

EXEC=RunVertexQuality
INPUT_FILE=le_nc_delta_rad_cosmic_1.root
#INPUT_PERM_FILE=root://fndca1.fnal.gov:1094/pnfs/fnal.gov/usr/uboone/resilient/users/rmurrell/vertex_quality_input/permutations/$PERM_FILE
#INPUT_FILE=root://fndca1.fnal.gov:1094/pnfs/fnal.gov/usr/uboone/resilient/users/rmurrell/vertex_quality_input/light_event_files/$FILE


echo setup uboonecode v06_26_01_10 -q e10:prof >>$log
setup uboonecode v06_26_01_10 -q e10:prof >> $log 2>&1
echo >> $log


#

EVENTD_DIR=light_event_files
echo ifdh cp -D $RES/$VIN/$makeupdir/$PROCESS/perm*root $CONDOR_DIR_INPUT >> $log
ifdh cp -D $RES/$VIN/$makeupdir/$PROCESS/perm*root $CONDOR_DIR_INPUT >> $log 2>&1
echo >> $log
echo ifdh cp -D $RES/$VIN/$EVENT_DIR/$INPUT_FILE $CONDOR_DIR_INPUT >> $log
ifdh cp -D $RES/$VIN/$EVENT_DIR/$INPUT_FILE $CONDOR_DIR_INPUT >> $log 2>&1
echo >> $log
#


INPUT_PERM_FILE=$(basename $CONDOR_DIR_INPUT/perm*root)
tmp=${INPUT_PERM_FILE#*_}
oldprocess=${tmp%.*}
mv $log log${oldprocess}.txt
log=log${oldprocess}.txt


echo ifdh cp -D $RES/$VIN/$EXEC $CONDOR_DIR_INPUT >> $log
ifdh cp -D $RES/$VIN/$EXEC $CONDOR_DIR_INPUT >> $log 2>&1
echo >> $log

echo chmod 777 $CONDOR_DIR_INPUT/$EXEC >> $log
chmod 777 $CONDOR_DIR_INPUT/$EXEC >> $log 2>&1
echo >> $log

echo $CONDOR_DIR_INPUT/$EXEC $CONDOR_DIR_INPUT/$INPUT_PERM_FILE $CONDOR_DIR_INPUT/$INPUT_FILE >> $log
$CONDOR_DIR_INPUT/$EXEC $CONDOR_DIR_INPUT/$INPUT_PERM_FILE $CONDOR_DIR_INPUT/$INPUT_FILE >> $log 2>&1
echo >> $log


echo ifdh mkdir $SCRATCH/$VOUT/$oldprocess >> $log
ifdh mkdir $SCRATCH/$VOUT/$oldprocess >> $log 2>&1
echo >> $log

ofile=$PWD/$EXEC.root
echo Output file: $ofile >> $log
echo >> $log
ls $ofile >> $log 2>&1
if [ $? -eq 0 ];
then
    chmod 777 $ofile
    echo ifdh cp $ofile $SCRATCH/$VOUT/$oldprocess/$EXEC$oldprocess.root >> $log
    ifdh cp $ofile $SCRATCH/$VOUT/$oldprocess/$EXEC$oldprocess.root >> $log 2>&1
else 
    echo $ofile not found >> $log
fi
echo >> $log

echo ifdh cp -D $PWD/$log $SCRATCH/$VOUT/$oldprocess >> $log
ifdh cp -D $PWD/$log $SCRATCH/$VOUT/$oldprocess