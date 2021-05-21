#!/bin/bash

KeyPress(){
  echo "Press any key to continue"
  while [ true ] ; do
      read -t 3 -n 1
      if [ $? = 0 ] ; then
          break ;
      fi
  done
}

source /grid/fermiapp/products/uboone/setup_uboone.sh
echo "Setting up root v6_12_04e"
setup root v6_12_04e -q e15:prof


FILE=$1
echo "Running gLEE near-sideband crosscheck script v1 on file : "$FILE
KeyPress

echo "Checking first to see if file and TTrees exist and are correctly filled. Then check to see how many events directly fail the nearsideband definition. These should be all Zero!"
root -q -l -b "/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/other/NearSidebands/gleeDirectNearCheck.c(\"$FILE\")" 
KeyPress




