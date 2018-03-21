#!/bin/bash                                          


umask +w
source /cvmfs/uboone.opensciencegrid.org/products/setup_uboone.sh


log=log_${PROCESS}.txt 
echo "Running on "$HOSTNAME > $log
echo "CLUSTER " ${CLUSTER} >> $log
echo "PROCESS " ${PROCESS} >> $log
echo >> $log


RES=/pnfs/uboone/resilient/users/rmurrell
SCRATCH=/pnfs/uboone/scratch/users/rmurrell
VIN=vertex_quality_input
VOUT=vertex_quality_output


EXEC=RunVertexQuality
INPUT_FILE=le_nc_delta_rad_cosmic_200.root


echo setup uboonecode v06_26_01_10 -q e10:prof >>$log
setup uboonecode v06_26_01_10 -q e10:prof >> $log 2>&1
echo >> $log


MAKEUP_DIR=$RES/$VIN/makeup
echo ifdh cp -D $MAKEUP_DIR/$PROCESS.txt $CONDOR_DIR_INPUT >> $log
ifdh cp -D $MAKEUP_DIR/$PROCESS.txt $CONDOR_DIR_INPUT >> $log 2>&1
echo >> $log


echo $CONDOR_DIR_INPUT/$PROCESS.txt >> $log
while read p; do
  INPUT_PERM_FILE=$p
done <$CONDOR_DIR_INPUT/$PROCESS.txt
echo INPUT_PERM_FILE: $INPUT_PERM_FILE >> $log
echo >> $log


OLDPROCESS=${INPUT_PERM_FILE%.*}
OLDPROCESS=${OLDPROCESS##*_}
echo OLDPROCESS: $OLDPROCESS >> $log
#cp $PWD/$log $PWD/log_$OLDPROCESS.txt >> $log
#log=log_$OLDPROCESS.txt >> $log


echo Check dir: $SCRATCH/$VOUT/$OLDPROCESS >> $log
if [ ! -d "$SCRATCH/$VOUT/$OLDPROCESS" ]; then
    echo ifdh mkdir $SCRATCH/$VOUT/$OLDPROCESS >> $log
    ifdh mkdir $SCRATCH/$VOUT/$OLDPROCESS >> $log 2>&1
elif [ -f "$SCRATCH/$VOUT/$OLDPROCESS/${EXEC}${OLDPROCESS}.root" ]; then
    echo "File: $SCRATCH/$VOUT/$OLDPROCESS/${EXEC}${OLDPROCESS}.root already exists, exiting" >> $log
    ifdh cp -D $PWD/$log $SCRATCH/$VOUT/$OLDPROCESS
    #ifdh cp -D $PWD/$log $SCRATCH/$VOUT
    exit 1;
fi
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

echo chmod 777 $CONDOR_DIR_INPUT/$EXEC >> $log
chmod 777 $CONDOR_DIR_INPUT/$EXEC >> $log 2>&1
echo >> $log

echo $CONDOR_DIR_INPUT/$EXEC $CONDOR_DIR_INPUT/$INPUT_PERM_FILE $CONDOR_DIR_INPUT/$INPUT_FILE >> $log
$CONDOR_DIR_INPUT/$EXEC $CONDOR_DIR_INPUT/$INPUT_PERM_FILE $CONDOR_DIR_INPUT/$INPUT_FILE >> $log 2>&1
echo >> $log


ofile=$PWD/$EXEC.root
echo Output file: $ofile >> $log
echo >> $log
ls $ofile >> $log 2>&1
if [ $? -eq 0 ]; then
    chmod 777 $ofile
    echo ifdh cp $ofile $SCRATCH/$VOUT/$OLDPROCESS/$EXEC$OLDPROCESS.root >> $log
    ifdh cp $ofile $SCRATCH/$VOUT/$OLDPROCESS/$EXEC$OLDPROCESS.root >> $log 2>&1
else 
    echo $ofile not found >> $log
fi
echo >> $log


ifdh cp -D $PWD/$log $SCRATCH/$VOUT/$OLDPROCESS
#ifdh cp -D $PWD/$log $SCRATCH/$VOUT