#!/bin/bash

FXXML="../../../../xml/SBNfit_Integration_XMLS/template_1g1p_stage_1_Run13combined_NoLEE.xml"
XML="1g1p_CeriseWorldOrder_FarSideband_CombinedRuns123.xml"
XML1="1g1p_CeriseWorldOrder_FarSideband_Run1.xml"
XML2="1g1p_CeriseWorldOrder_FarSideband_Run2.xml"
XML3="1g1p_CeriseWorldOrder_FarSideband_Run3.xml"

# Full FS Cuts
sFULLFS="(simple_1g1pMar2020_v4Nue_mva >= 0.3 && (simple_1g1pMar2020_v4BNB_mva < 0.96273 || simple_1g1pMar2020_v4NCPi0_mva < 0.467404))"
FULLFS="(1g1pMar2020_v4Nue_mva>= 0.3 && (1g1pMar2020_v4BNB_mva < 0.96273 || 1g1pMar2020_v4NCPi0_mva < 0.467404))"

# NCPi0 Cuts
sNCPIFS="(simple_1g1pMar2020_v4Nue_mva >= 0.3 && (simple_1g1pMar2020_v4BNB_mva >= 0.96273 && simple_1g1pMar2020_v4NCPi0_mva < 0.467404))"
NCPIFS="(1g1pMar2020_v4Nue_mva>= 0.3 && (1g1pMar2020_v4BNB_mva >= 0.96273 && 1g1pMar2020_v4NCPi0_mva < 0.467404))"

# BNB Cuts
sBNBFS="(simple_1g1pMar2020_v4Nue_mva >= 0.3 && (simple_1g1pMar2020_v4BNB_mva < 0.96273 && simple_1g1pMar2020_v4NCPi0_mva >= 0.467404))"
BNBFS="(1g1pMar2020_v4Nue_mva>= 0.3 && (1g1pMar2020_v4BNB_mva < 0.96273 && 1g1pMar2020_v4NCPi0_mva >= 0.467404))"

NUM=2212

#Full farsideband 
./hive -o datamc --makefluxcovar $FXXML -x $XML -s 1 -g $NUM --cuts "$sFULLFS"
rm *Far*entry*root
./hive -o datamc  -x $XML -s 1 -g $NUM  --cuts "$FULLFS" --additional_tag="FullFS_Comb" --systematics "flux_fracfixed"
rm  *Far*entry*root
./hive -o datamc  -x $XML1 -s 1 -g $NUM  --cuts "$FULLFS" --additional_tag="FullFS_Run1" --systematics "flux_fracfixed"
rm  *Far*entry*root
./hive -o datamc  -x $XML2 -s 1 -g $NUM  --cuts "$FULLFS" --additional_tag="FullFS_Run2" --systematics "flux_fracfixed"
rm  *Far*entry*root
./hive -o datamc  -x $XML3 -s 1 -g $NUM  --cuts "$FULLFS" --additional_tag="FullFS_Run3" --systematics "flux_fracfixed"

#NCPi0Box 
./hive -o datamc --makefluxcovar $FXXML -x $XML -s 1 -g $NUM --cuts "$sNCPIFS"
rm *Far*entry*root
./hive -o datamc  -x $XML -s 1 -g $NUM  --cuts "$NCPIFS" --additional_tag="NCpi0FS_Comb" --systematics "flux_fracfixed"
rm  *Far*entry*root
./hive -o datamc  -x $XML1 -s 1 -g $NUM  --cuts "$NCPIFS" --additional_tag="NCpi0FS_Run1" --systematics "flux_fracfixed"
rm  *Far*entry*root
./hive -o datamc  -x $XML2 -s 1 -g $NUM  --cuts "$NCPIFS" --additional_tag="NCpi0FS_Run2" --systematics "flux_fracfixed"
rm  *Far*entry*root
./hive -o datamc  -x $XML3 -s 1 -g $NUM  --cuts "$NCPIFS" --additional_tag="NCpi0FS_Run3" --systematics "flux_fracfixed"

#NCPi0Box 
#./hive -o datamc --makefluxcovar $FXXML -x $XML -s 1 -g $NUM --cuts "$sBNBFS"
rm *Far*entry*root
#./hive -o datamc  -x $XML -s 1 -g $NUM  --cuts "$BNBFS" --additional_tag="BNBOtherFS_Comb" --systematics "flux_fracfixed"
rm  *Far*entry*root
#./hive -o datamc  -x $XML1 -s 1 -g $NUM  --cuts "$BNBFS" --additional_tag="BNBOtherFS_Run1" --systematics "flux_fracfixed"
rm  *Far*entry*root
#./hive -o datamc  -x $XML2 -s 1 -g $NUM  --cuts "$BNBFS" --additional_tag="BNBOtherFS_Run2" --systematics "flux_fracfixed"
rm  *Far*entry*root
#./hive -o datamc  -x $XML3 -s 1 -g $NUM  --cuts "$BNBFS" --additional_tag="BNBOtherFS_Run3" --systematics "flux_fracfixed"



#NCPi Box
#./hive -o datamc --makefluxcovar $FXXML -x $XML -s 1 -g 2202 --cuts "$sNCPIFS"
#./hive -o datamc  -x $XML -s 1 -g 2202  --cuts "$NCPIFS" --additional_tag="NCpi0FS" --systematics "flux_fracfixed"
#./hive -o datamc  -x $XML -s 1 -g 2202  --cuts "$NCPIFS" --additional_tag="NCpi0FS"

#BNB Box
#./hive -o datamc --makefluxcovar $FXXML -x $XML -s 1 -g 2202 --cuts "$sBNBFS"
#./hive -o datamc  -x $XML -s 1 -g 2202  --cuts "$BNBFS" --additional_tag="BNBFS" --systematics "flux_fracfixed"

#./hive -o datamc -x 1g1p_CeriseWorldOrder_FarSideband_Run1.xml -s 1 -g 2202 --cuts "$NCPIFS" 

#./hive -o datamc --makefluxcovar $FXXML -x $XML -s 1 -g 2203 --cuts $sFULLFS 
#./hive -o datamc  -x $XML -s 1 -g 2203  --cuts $FULLFS --additional_tag="FullFS" --systematics "flux_fracfixed"
