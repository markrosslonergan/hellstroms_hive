#!/bin/bash

source ~/setup_job/hell.sh

USER=rmurrell
APP=/uboone/app/users/$USER
DATA=/uboone/data/users/$USER

EXEC=$APP/hellstroms_hive/deltaRad/build/src/deltaRad
DIR=$DATA/hellstroms_hive_files
declare -a OPTION=("response" "recomc" "sig" "stack" "datamc" "vars" "eff" "effdata")
declare -a TRACK=("track" "notrack")
XML=$RM/hellstroms_hive/deltaRad/xml/default.xml

LOGDIR=$PWD/run_hh_log
if [ ! -d "$LOGDIR" ]; then
    mkdir $LOGDIR
fi

for opt in "${OPTION[@]}"; do
    if [ ! -d "$opt" ]; then
	mkdir $opt
    fi
    for track in "${TRACK[@]}"; do
	log=$PWD/run_hh_log/log_${opt}_${track}.txt
	echo $EXEC --dir $DIR --option "$opt" --track "$track" --xml $XML > $log
	echo >> $log
	$EXEC --dir $DIR --option "$opt" --track "$track" --xml $XML >> $log 2>&1
    done
    if [ -z "$(ls -A $opt)" ]; then
	rm -r $opt
    fi
done