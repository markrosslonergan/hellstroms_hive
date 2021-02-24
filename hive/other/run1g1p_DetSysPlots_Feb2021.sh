#!/bin/bash

FXXML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/template_1g1p_stage_1_Jan2021.xml"
FXXML2="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/template_1g1p_stage_6_Jan2021.xml"
DXML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/DetSys_Neutrino2020/template_detsys_1g1p_Feb2021_v4.xml"
#XML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/1g1p_EmeraldWorldOrder_Run1_v4_Plot.xml"
XML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/1g1p_EmeraldWorldOrder_CombinedRuns13_v4_Plot.xml"

#Just one full farside best, without CC
#./hive --makefluxcovar $FXXML --makedetcovar $DXML -x $XML -g 7707 -s 1  &> det.log
#./hive -o datamc  -x $XML -s 1 -g 7707  --additional_tag="wDetsys" --systematics "fluxdet"
#./hive -o datamc  -x $XML -s 1 -g 7707  --additional_tag="wJustDetsys" --systematics "merged_det"
#./hive -o datamc  -x $XML -s 1 -g 7707  --additional_tag="wOutDetsys" --systematics "flux_fracfixed"
#rm  *Far*entry*root



#Just one full farside best, without CC
#./hive --makefluxcovar $FXXML2 --makedetcovar $DXML -x $XML -g 3303 -s 6  
./hive -o stack  -x $XML -s 6 -g 3303  --additional_tag="wDetsys" --systematics "fluxdet" --pot 6.91e20

