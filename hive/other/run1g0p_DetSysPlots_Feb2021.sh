#!/bin/bash


FXXML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/template_1g0p_stage_1_Jan2021.xml"
FXXML2="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/template_1g0p_stage_4_Jan2021.xml"
DXML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/DetSys_Neutrino2020/template_detsys_1g0p_Feb2021_v4.xml"

XML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g0p_xmls/hive4.0/1g0p_EmeraldWorldOrder_CombinedRuns13_v4_Plot.xml"
XML1="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g0p_xmls/hive4.0/1g0p_EmeraldWorldOrder_Run1_v4_Plot.xml"

#Just one full farside best, without CC
#./hive --makefluxcovar $FXXML2 --makedetcovar $DXML -x $XML -g 333 -s 4
#./hive -o stack  -x $XML -s 4 -g 333  --additional_tag="wDetsys" --systematics "fluxdet" --pot 6.91e20


#Just one full farside best, without CC
./hive --makefluxcovar $FXXML --makedetcovar $DXML -x $XML1 -g 1313 -s 1
./hive -o datamc  -x $XML1 -s 1 -g 1313  --additional_tag="wDetsys" --systematics "fluxdet" 

