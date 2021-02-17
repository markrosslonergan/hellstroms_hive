#!/bin/bash

FXXML="/uboone/app/users/markrl/SLi7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/template_1g1p_stage_1_Jan2021.xml"
DXML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/DetSys_Neutrino2020/template_detsys_1g1p_Feb2021_v4.xml"
XML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/1g1p_EmeraldWorldOrder_CombinedRuns13_v4_FarSideband_Plot.xml"
XML1="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/1g1p_EmeraldWorldOrder_Run1_v4_FarSideband_Plot.xml"
XML2="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/1g1p_EmeraldWorldOrder_Run2_v4_FarSideband_Plot.xml"
XML3="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/1g1p_EmeraldWorldOrder_Run3_v4_FarSideband_Plot.xml"

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


NUM=5505
#./hive -o datamc --makedetcovar $DXML -x $XML1 -s 1 -g $NUM --cuts "$sFULLFSnoC"
#./hive -o datamc  -x $XML1 -s 1 -g $NUM  --cuts "$FULLFSnoC" --additional_tag="Run2Run_FULL_Run1" --systematics "merged_det"
#./hive -o datamc  -x $XML2 -s 1 -g $NUM  --cuts "$FULLFSnoC" --additional_tag="Run2Run_FULL_Run2" --systematics "merged_det"
#./hive -o datamc  -x $XML3 -s 1 -g $NUM  --cuts "$FULLFSnoC" --additional_tag="Run2Run_FULL_Run3" --systematics "merged_det"

./hive -o datamc  -x $XML1 -s 1 -g $NUM  --cuts "$FULLFSnoC" --additional_tag="Run2Run_FULL_Run1" 
./hive -o datamc  -x $XML2 -s 1 -g $NUM  --cuts "$FULLFSnoC" --additional_tag="Run2Run_FULL_Run2" 
./hive -o datamc  -x $XML3 -s 1 -g $NUM  --cuts "$FULLFSnoC" --additional_tag="Run2Run_FULL_Run3" 


NUM=5506
#./hive -o datamc --makedetcovar $DXML -x $XML1 -s 1 -g $NUM --cuts "$sNCPIFS"
#./hive -o datamc  -x $XML1 -s 1 -g $NUM  --cuts "$NCPIFS" --additional_tag="Run2Run_NCPi0_Run1" --systematics "merged_det"
#./hive -o datamc  -x $XML2 -s 1 -g $NUM  --cuts "$NCPIFS" --additional_tag="Run2Run_NCPi0_Run2" --systematics "merged_det"
#./hive -o datamc  -x $XML3 -s 1 -g $NUM  --cuts "$NCPIFS" --additional_tag="Run2Run_NCPi0_Run3" --systematics "merged_det"


NUM=5507
#./hive -o datamc --makedetcovar $DXML -x $XML1 -s 1 -g $NUM --cuts "$sBNBFS"
#./hive -o datamc  -x $XML1 -s 1 -g $NUM  --cuts "$BNBFS" --additional_tag="Run2Run_BNBOther_Run1" --systematics "merged_det"
#./hive -o datamc  -x $XML2 -s 1 -g $NUM  --cuts "$BNBFS" --additional_tag="Run2Run_BNBOther_Run2" --systematics "merged_det"
#./hive -o datamc  -x $XML3 -s 1 -g $NUM  --cuts "$BNBFS" --additional_tag="Run2Run_BNBOther_Run3" --systematics "merged_det"



