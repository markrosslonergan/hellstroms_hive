#!/bin/bash

FXXML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/template_2g1p_stage_2_Jan2021.xml"
DXML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/DetSys_Neutrino2020/template_detsys_2g1p_Feb2021_v4.xml"
XML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/2g1p_xmls/hive4.0/2g1p_EmeraldWorldOrder_CombinedRuns13_v4_Plot.xml"

#./hive -o datamc --makefluxcovar $FXXML -x $XML -g 11811 -s 1
#./hive -o datamc  -x $XML -s 1 -g 11811  --additional_tag="FluxXS" --systematics "flux_fracfixed"
#rm  *Far*entry*root
#./hive -o datamc --makefluxcovar $FXXML --makedetcovar $DXML -x $XML -g 11811 -s 1 
#./hive -o datamc  -x $XML -s 1 -g 11811  --additional_tag="DetFluxXS" --systematics "fluxdet"
#rm  *Far*entry*root


#Final Plots
#./hive --makefluxcovar $FXXML --makedetcovar $DXML -x $XML -g 3303 -s 2
#./hive -o datamc  -x $XML -s 2 -g 3303  --additional_tag="DetFluxXS" --systematics "fluxdet"

#shower dedx
./hive --makefluxcovar $FXXML --makedetcovar $DXML -x $XML -g 4404 -s 2
./hive -o datamc  -x $XML -s 2 -g 4404  --additional_tag="DetFluxXS" --systematics "fluxdet"


