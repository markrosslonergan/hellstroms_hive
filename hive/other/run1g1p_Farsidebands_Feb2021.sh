#!/bin/bash

FXXML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/template_1g1p_stage_1_Jan2021.xml"
DXML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/DetSys_Neutrino2020/template_detsys_1g1p_Feb2021_v4.xml"
DXMLEXACT="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/DetSys_Neutrino2020/template_detsys_1g1p_Feb2021_EXACT_v4.xml"
XML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/1g1p_EmeraldWorldOrder_CombinedRuns13_v4_FarSideband_Plot.xml"

#XML1="1g1p_CeriseWorldOrder_FarSideband_Run1.xml"
#XML2="1g1p_CeriseWorldOrder_FarSideband_Run2.xml"
#XML3="1g1p_CeriseWorldOrder_FarSideband_Run3.xml"

# Full FS Cuts, No Cosmic
sFULLFSnoC="(simple_1g1pMar2020_v4Nue_mva>= 0.3 && (simple_1g1pMar2020_v4BNB_mva < 0.96273 || simple_1g1pMar2020_v4NCPi0_mva < 0.467404))"
FULLFSnoC="(1g1pMar2020_v4Nue_mva>= 0.3 && (1g1pMar2020_v4BNB_mva < 0.96273 || 1g1pMar2020_v4NCPi0_mva < 0.467404))"

# Full FS Cuts
sFULLFS="(simple_1g1pMar2020_v4COSMIC_mva >=0.9 &&simple_1g1pMar2020_v4Nue_mva>= 0.3 && (simple_1g1pMar2020_v4BNB_mva < 0.96273 || simple_1g1pMar2020_v4NCPi0_mva < 0.467404))"
FULLFS="(1g1pMar2020_v4COSMIC_mva >=0.9 &&1g1pMar2020_v4Nue_mva>= 0.3 && (1g1pMar2020_v4BNB_mva < 0.96273 || 1g1pMar2020_v4NCPi0_mva < 0.467404))"

# NCPi0 Cuts
sNCPIFS="(simple_1g1pMar2020_v4COSMIC_mva >=0.9 &&simple_1g1pMar2020_v4Nue_mva>= 0.3 && (simple_1g1pMar2020_v4BNB_mva >= 0.96273 && simple_1g1pMar2020_v4NCPi0_mva < 0.467404))"
NCPIFS="(1g1pMar2020_v4COSMIC_mva >=0.9 &&1g1pMar2020_v4Nue_mva>= 0.3 && (1g1pMar2020_v4BNB_mva >= 0.96273 && 1g1pMar2020_v4NCPi0_mva < 0.467404))"

# BNB Cuts
sBNBFS="(simple_1g1pMar2020_v4COSMIC_mva >=0.9 && simple_1g1pMar2020_v4Nue_mva>= 0.3 && (simple_1g1pMar2020_v4BNB_mva < 0.96273 && simple_1g1pMar2020_v4NCPi0_mva >= 0.467404))"
BNBFS="(1g1pMar2020_v4COSMIC_mva >=0.9 && 1g1pMar2020_v4Nue_mva>= 0.3 && (1g1pMar2020_v4BNB_mva < 0.96273 && 1g1pMar2020_v4NCPi0_mva >= 0.467404))"

GF=333
#Just one full farside best, without CC
#./hive -o datamc --makefluxcovar $FXXML -x $XML -g 11810 -s 1 --cuts "$sFULLFSnoC"
#./hive -o datamc  -x $XML -s 1 -g 11810  --cuts "$FULLFSnoC" --additional_tag="FullFSnoC" --systematics "flux_fracfixed"
#rm  *Far*entry*root


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

#Stopping BDT
#./hive -o datamc --makefluxcovar $FXXML -x $XML -g 6608 -s 1 --cuts "$sFULLFSnoC"
#./hive -o datamc  -x $XML -s 1 -g 6608  --cuts "$FULLFSnoC" --additional_tag="StoppingTestFullFSnoC" --systematics "flux_fracfixed"

#./hive -o datamc --makefluxcovar $FXXML -x $XML -g 6608 -s 1 --cuts "$sNCPIFS"
#./hive -o datamc  -x $XML -s 1 -g 6608  --cuts "$NCPIFS" --additional_tag="StoppingTestNCpi0" --systematics "flux_fracfixed"

#Bonnie Question
#./hive --makefluxcovar $FXXML --makedetcovar $DXMLEXACT -x $XML -g 5050 -s 1 --cuts "$sFULLFS"
#./hive -o datamc  -x $XML -s 1 -g 5050  --cuts "$FULLFS" --additional_tag="FullFSwDetBonnie" --systematics "fluxdet"

#Andy M, score with 0.7 scaling
#./hive --makefluxcovar $FXXML --makedetcovar $DXMLEXACT -x $XML -g 117112 -s 1 --cuts "$sFULLFS"
#./hive -o datamc  -x $XML -s 1 -g 117112  --cuts "$FULLFS" --additional_tag="FullFSwDetAndyMEx" --systematics "fluxdet"

######## WES This section studing Further-Near Sidebands #################
FULLNS=" (1g1pMar2020_v4Nue_mva>= 0.3 && (1g1pMar2020_v4BNB_mva >= 0.96273 && 1g1pMar2020_v4NCPi0_mva >= 0.467404) && 1g1pMar2020_v4SSVredo2wCC600parent_mva < 0.55)"
REDUCEDNS="((1g1pMar2020_v4BNB_mva >= 0.5 && 1g1pMar2020_v4NCPi0_mva >= 0.2))" #&& 1g1pMar2020_v4SSVredo2wCC600parent_mva < 0.7088)"

sFULLNS=" (simple_1g1pMar2020_v4Nue_mva>= 0.3 && (simple_1g1pMar2020_v4BNB_mva >= 0.96273 && simple_1g1pMar2020_v4NCPi0_mva >= 0.467404) && simple_1g1pMar2020_v4SSVredo2wCC600parent_mva < 0.55)"
sREDUCEDNS="((simple_1g1pMar2020_v4BNB_mva >= 0.5 && simple_1g1pMar2020_v4NCPi0_mva >= 0.2))" #&& simple_1g1pMar2020_v4SSVredo2wCC600parent_mva < 0.7088)"

#BDT Responses
#./hive --makefluxcovar $FXXML -x $XML -s 1 -g 3536 --cuts="$sREDUCEDNS&&$sFULLFSnoC && simple_1g1pMar2020_v4COSMIC_mva >=0.5"
#./hive -o datamc  -x $XML -s 1 -g 3536 --cuts="$REDUCEDNS&&$FULLFSnoC && 1g1pMar2020_v4COSMIC_mva >=0.5"  --additional_tag="FurtherNearSidebands" --systematics="flux_fracfixed"

#ALL SSV
#./hive --makefluxcovar $FXXML -x $XML -s 1 -g 3530 --cuts="$sREDUCEDNS&&$sFULLFSnoC && simple_1g1pMar2020_v4COSMIC_mva >=0.5"
#./hive -o datamc  -x $XML -s 1 -g 3530 --cuts="$REDUCEDNS&&$FULLFSnoC && 1g1pMar2020_v4COSMIC_mva >=0.5"  --additional_tag="FurtherNearSidebands" --systematics="flux_fracfixed"

./hive --makefluxcovar $FXXML --makedetcovar $DXMLEXACT -x $XML -s 1 -g 35300 --cuts="$sREDUCEDNS&&$sFULLFSnoC && simple_1g1pMar2020_v4COSMIC_mva >=0.5"
./hive -o datamc  -x $XML -s 1 -g 35300 --cuts="$REDUCEDNS&&$FULLFSnoC && 1g1pMar2020_v4COSMIC_mva >=0.5"  --additional_tag="FurtherNearSidebands" --systematics="fluxdet"

