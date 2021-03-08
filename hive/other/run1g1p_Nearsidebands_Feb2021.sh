#!/bin/bash

FXXML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/template_1g1p_stage_1_Jan2021.xml"
DXML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/DetSys_Neutrino2020/template_detsys_1g1p_Feb2021_v4.xml"
#XML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/1g1p_EmeraldWorldOrder_CombinedRuns13_v4_NearSideband_Plot.xml"
XML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/1g1p_EmeraldWorldOrder_Run1_v4_NearSideband_Plot.xml"

# Full FS Cuts, No Cosmic
sFULLNS="(simple_1g1pMar2020_v4Nue_mva>= 0.3 && (simple_1g1pMar2020_v4BNB_mva >= 0.96273 && simple_1g1pMar2020_v4NCPi0_mva >= 0.467404) &&  simple_1g1pMar2020_v4SSVredo2wCC600parent_mva < 0.55)"
FULLNS=" (1g1pMar2020_v4Nue_mva>= 0.3 && (1g1pMar2020_v4BNB_mva >= 0.96273 && 1g1pMar2020_v4NCPi0_mva >= 0.467404) && 1g1pMar2020_v4SSVredo2wCC600parent_mva < 0.55)"
GF=333

#Just one full farside best, without CC
#./hive -o datamc --makefluxcovar $FXXML -x $XML -g 11810 -s 1 --cuts "$sFULLFSnoC"
./hive -o datamc  -x $XML -s 1 -g 7707  --cuts "$FULLNS" --additional_tag="NearSideband" 
#./hive -o stack  -x $XML -s 1 -g 3334  --cuts "$FULLNS" --additional_tag="NearSideband" --pot=6.91e20


#--------- This block produces all 1g1p main chapter notes
#rm  *Far*entry*root
#./hive -o datamc --makefluxcovar $FXXML -x $XML -g 11811 -s 1 --cuts "$sFULLFS"
#./hive -o datamc  -x $XML -s 1 -g 11811  --cuts "$FULLFS" --additional_tag="FullFS" --systematics "flux_fracfixed"
#rm  *Far*entry*root

#./hive -o datamc --makefluxcovar $FXXML -x $XML -g 11911 -s 1 --cuts "$sNCPIFS"
#./hive -o datamc  -x $XML -s 1 -g 11911  --cuts "$NCPIFS" --additional_tag="NCPi0Box" --systematics "flux_fracfixed"
#rm  *Far*entry*root

#./hive -o datamc --makefluxcovar $FXXML -x $XML -g 11711 -s 1 --cuts "$sBNBFS"
#./hive -o datamc  -x $XML -s 1 -g 11711  --cuts "$BNBFS" --additional_tag="BNBOtherBox" --systematics "flux_fracfixed"
#rm  *Far*entry*root


#--- ---- 
#Bins 18 Make sure all Bins in the 4404 are 18! 
#rm  *Far*entry*root
#./hive -o datamc --makefluxcovar $FXXML -x $XML -g 4404 -s 1 --cuts "$sFULLFSnoC"
#./hive -o datamc  -x $XML -s 1 -g 4404  --cuts "$FULLFSnoC" --additional_tag="trainFullFSnoC" --systematics "flux_fracfixed"
#./hive -o datamc  -x $XML -s 1 -g 4404  --cuts "$FULLFSnoC" --additional_tag="trainFullFSnoC" 
#rm  *Far*entry*root

#--- ---- 
# - Bins 12 Make sure all Bins in the 4404 are 12!
#rm  *Far*entry*root
#./hive -o datamc --makefluxcovar $FXXML -x $XML -g 44042 -s 1 --cuts "$sNCPIFS"
#./hive -o datamc  -x $XML -s 1 -g 44042  --cuts "$NCPIFS" --additional_tag="trainNCpi0" --systematics "flux_fracfixed"
#rm  *Far*entry*root

# - Bins 8 Make sure all Bins in the 4404 are 8!
#rm  *Far*entry*root
#./hive -o datamc --makefluxcovar $FXXML -x $XML -g 44042 -s 1 --cuts "$sBNBFS"
#./hive -o datamc  -x $XML -s 1 -g 44042  --cuts "$BNBFS" --additional_tag="trainBNBOther" --systematics "flux_fracfixed"
#rm  *Far*entry*root

# BDT 
#rm  *Far*entry*root
#./hive -o datamc --makefluxcovar $FXXML -x $XML -g 6606 -s 1 --cuts "$sFULLFSnoC"
#./hive -o datamc  -x $XML -s 1 -g 6606  --cuts "$FULLFSnoC" --additional_tag="trainFullFSnoC" --systematics "flux_fracfixed"
#rm  *Far*entry*root

#Just one full farside best, without CC
#./hive --makefluxcovar $FXXML --makedetcovar $DXML -x $XML -g 11810 -s 1 --cuts "$sFULLFSnoC"
#./hive -o datamc  -x $XML -s 1 -g 11810  --cuts "$FULLFSnoC" --additional_tag="FullFSnoCwDet" --systematics "fluxdet"

#./hive --makefluxcovar $FXXML --makedetcovar $DXML -x $XML -g 117112 -s 1 --cuts "$sFULLFS"
#./hive -o datamc  -x $XML -s 1 -g 117112  --cuts "$FULLFS" --additional_tag="FullFSwDet" --systematics "fluxdet"

