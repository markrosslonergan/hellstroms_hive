#!/bin/bash

FXXML4="/uboone/app/users/ksutton/hellstroms_hive_2.5_sl7/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/template_1g0p_stage_1_Jan2021.xml"

FXXML1="/uboone/app/users/ksutton/hellstroms_hive_2.5_sl7/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/template_1g0p_stage_1_Jan2021.xml"

XML="/uboone/app/users/ksutton/hellstroms_hive_2.5_sl7/hellstroms_hive/hive/xml/1g0p_xmls/hive4.0/1g0p_EmeraldWorldOrder_Run1_v4_Plot.xml"

XML13="/uboone/app/users/ksutton/hellstroms_hive_2.5_sl7/hellstroms_hive/hive/xml/1g0p_xmls/hive4.0/1g0p_EmeraldWorldOrder_CombinedRuns13_v4_Plot.xml"


#stage0 + 1
./../../bin/hive --makefluxcovar $FXXML1 -x $XML -s 1 -g8
./../../bin/hive -o datamc  -x $XML -s 0 -g8 --systematics "flux_fracfixed"
./../../bin/hive -o datamc  -x $XML -s 1 -g8 --systematics "flux_fracfixed"

#stage 1
./../../bin/hive  --makefluxcovar $FXXML1 -x $XML -s 1 -g12
./../../bin/hive -o datamc  -x $XML -s 1 -g12 --systematics "flux_fracfixed"

#Single BDT Cuts
#./../../bin/hive  --makefluxcovar $FXXML1 -x $XML -s 1 -g13
#./../../bin/hive -o datamc  -x $XML -s 1 -g13 --systematics "flux_fracfixed"

#BDT Scores
#./../../bin/hive  --makefluxcovar $FXXML1 -x $XML -s 1 -g6
#./../../bin/hive -o datamc  -x $XML -s 1 -g6 --systematics "flux_fracfixed"

#stage 4 datamc + stack
#./../../bin/hive --makefluxcovar $FXXML4 -x $XML -s 4 -g11
#./../../bin/hive -o datamc  -x $XML -s 4 -g11 --systematics "flux_fracfixed"

#./../../bin/hive --makefluxcovar $FXXML4 -x $XML13 -s 4 -g11
#./../../bin/hive -o stack -x $XML13 -s 4 -g11 --systematics "flux_fracfixed"

