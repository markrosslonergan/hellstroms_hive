#!/bin/bash

XML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g0p_xmls/hive5.0/1g0p_FuchsiaWorldOrder_Coherent_CombinedRuns13_v2_Jan2022.xml"

TAG="NC_Coherent_v2"

#./hive -o sbnfit -s 0 -n 0 -x $XML
#./hive -o datamc -s 1 -x $XML 


# vars takes input a stage, (usually 1) and a BDT number (-b 0 is first BDT, and so forth)   
#./hive -o vars -b 0 -s 1 -x $XML
./hive -o vars -b 3 -s 1 -x $XML

if (false); then
    ./hive -o train -b 0 -x $XML &> train.log.cosmic
    ./hive -o train -b 1 -x $XML &> train.log.bnb
    ./hive -o train -b 2 -x $XML &> train.log.ncpi0

    ./hive -o app -b 0 -x $XML &> app.log.cosmic
    ./hive -o app -b 1 -x $XML &> app.log.bnb
    ./hive -o app -b 2 -x $XML &> app.log.ncpi0
fi

