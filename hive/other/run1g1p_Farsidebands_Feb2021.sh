#!/bin/bash

FXXML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/template_1g1p_stage_1_Jan2021.xml"
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
rm  *Far*entry*root
./hive -o datamc --makefluxcovar $FXXML -x $XML -g 6606 -s 1 --cuts "$sFULLFSnoC"
./hive -o datamc  -x $XML -s 1 -g 6606  --cuts "$FULLFSnoC" --additional_tag="trainFullFSnoC" --systematics "flux_fracfixed"
rm  *Far*entry*root










#Full farsideband 
#./hive -o datamc --makefluxcovar $FXXML -x $XML -s 1 --plottrainonly --cuts "$sFULLFS"
#rm *Far*entry*root
#./hive -o datamc  -x $XML -s 1 --plottrainonly  --cuts "$FULLFS" --additional_tag="FullFS_CosCut" --systematics "flux_fracfixed"
#rm  *Far*entry*root

#./hive -o datamc  -x $XML1 -s 1 -g $NUM  --cuts "$FULLFS" --additional_tag="FullFS_Run1" --systematics "flux_fracfixed"
#rm  *Far*entry*root
#./hive -o datamc  -x $XML2 -s 1 -g $NUM  --cuts "$FULLFS" --additional_tag="FullFS_Run2" --systematics "flux_fracfixed"
#rm  *Far*entry*root
#./hive -o datamc  -x $XML3 -s 1 -g $NUM  --cuts "$FULLFS" --additional_tag="FullFS_Run3" --systematics "flux_fracfixed"

#NCPi0Box 
#./../../bin/hive -o datamc --makefluxcovar $FXXML -x $XML3 -s 1 --plottrainonly --cuts "$sNCPIFS"
#rm *Far*entry*root
#./../../bin/hive -o datamc  -x $XML3 -s 1 --plottrainonly  --cuts "$NCPIFS" --additional_tag="NCpi0FS_CosCut" --systematics "flux_fracfixed"
#rm  *Far*entry*root
#./hive -o datamc  -x $XML1 -s 1 -g $NUM  --cuts "$NCPIFS" --additional_tag="NCpi0FS_Run1" --systematics "flux_fracfixed"
#rm  *Far*entry*root
#./hive -o datamc  -x $XML2 -s 1 -g $NUM  --cuts "$NCPIFS" --additional_tag="NCpi0FS_Run2" --systematics "flux_fracfixed"
#rm  *Far*entry*root
#./hive -o datamc  -x $XML3 -s 1 -g $NUM  --cuts "$NCPIFS" --additional_tag="NCpi0FS_Run3" --systematics "flux_fracfixed"

#NCPi0Box 
#./hive -o datamc --makefluxcovar $FXXML -x $XML -s 1 -g $NUM --cuts "$sBNBFS"
#rm *Far*entry*root
#./hive -o datamc  -x $XML -s 1 -g $NUM  --cuts "$BNBFS" --additional_tag="BNBFS_Coscut" --systematics "flux_fracfixed"
#rm  *Far*entry*root
#./hive -o datamc  -x $XML1 -s 1 -g $NUM  --cuts "$BNBFS" --additional_tag="BNBOtherFS_Run1" --systematics "flux_fracfixed"
#rm  *Far*entry*root
#./hive -o datamc  -x $XML2 -s 1 -g $NUM  --cuts "$BNBFS" --additional_tag="BNBOtherFS_Run2" --systematics "flux_fracfixed"
#rm  *Far*entry*root
#./hive -o datamc  -x $XML3 -s 1 -g $NUM  --cuts "$BNBFS" --additional_tag="BNBOtherFS_Run3" --systematics "flux_fracfixed"

#NCPi Box
#./hive -o datamc --makefluxcovar $FXXML -x $XML -s 1 -g 2202 --cuts "$sNCPIFS"
#./hive -o datamc  -x $XML -s 1 -g 2202  --cuts "$NCPIFS" --additional_tag="NCpi0FS" --systematics "flux_fracfixed"
#./hive -o datamc  -x $XML -s 1 -g 2202  --cuts "$NCPIFS" --additional_tag="NCpi0FS"

#BNB Box
#./../../bin/hive -o datamc --makefluxcovar $FXXML -x $XML2 -s 1 --plottrainonly --cuts "$sBNBFS"
#rm  *Far*entry*root
#./../../bin/hive -o datamc  -x $XML2 -s 1 --plottrainonly  --cuts "$BNBFS" --additional_tag="BNBFS_CosCut" --systematics "flux_fracfixed"
# rm  *Far*entry*root

#./hive -o datamc -x 1g1p_CeriseWorldOrder_FarSideband_Run1.xml -s 1 -g 2202 --cuts "$NCPIFS" 
#./hive -o datamc --makefluxcovar $FXXML -x $XML -s 1 -g 2203 --cuts $sFULLFS 
#./hive -o datamc  -x $XML -s 1 -g 2203  --cuts $FULLFS --additional_tag="FullFS" --systematics "flux_fracfixed"
