#!/bin/bash

#Takes two arguments
#   the first being the samweb definition
#   The second being a file_list in which the column structure is:
#   ---- Column 3   Column 4      Column 5
#   ---- Run_number subrun_number event_number

SAMDEF=$1

cat $2 | while read line
do
   RUNSUBRUN=`echo $line | awk '{print sprintf("%04d", $3)"."sprintf("%04d",$4)}'`
   EVENT=`echo $line | awk '{print $5}'`
   echo $RUNSUBRUN  and Event $EVENT
   SFILE=`samweb list-files "defname:$SAMDEF and run_number = $RUNSUBRUN"`
  # echo $SFILE
   SLOC=`samweb locate-file $SFILE`
   SLOC2=`echo $SLOC | sed -r 's/enstore://'`
   SLOC3=`echo $SLOC2 | sed -e 's/(.*)//g'`

   SLOC4=`echo $SLOC3"/"$SFILE`
   echo $SLOC4" "$RUNSUBRUN" "$EVENT

done


