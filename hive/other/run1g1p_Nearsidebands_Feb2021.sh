#!/bin/bash

FXXML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/template_1g1p_stage_1_Jan2021.xml"
DXML1="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/DetSys_Neutrino2020/template_detsys_1g1p_May2021_v23.2_All.xml"
DXML2="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/DetSys_Neutrino2020/template_detsys_1g1p_May2021_v23.1_SCE_Recom2.xml"
XML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/1g1p_EmeraldWorldOrder_CombinedRuns13_v4_NearSideband_Plot.xml"

XML1="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/1g1p_EmeraldWorldOrder_Run1Only_v4_NearSideband_Plot.xml"
XML2="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/1g1p_EmeraldWorldOrder_Run2Only_v4_NearSideband_Plot.xml"
XML3="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/1g1p_EmeraldWorldOrder_Run3Only_v4_NearSideband_Plot.xml"

# Full NS Cuts, No Cosmic
sFULLNS="(simple_1g1pMar2020_v4Nue_mva>= 0.3 && (simple_1g1pMar2020_v4BNB_mva >= 0.96273 && simple_1g1pMar2020_v4NCPi0_mva >= 0.467404) &&  simple_1g1pMar2020_v4SSVredo2wCC600parent_mva < 0.55)"
FULLNS="(1g1pMar2020_v4Nue_mva>= 0.3 && (1g1pMar2020_v4BNB_mva >= 0.96273 && 1g1pMar2020_v4NCPi0_mva >= 0.467404) && 1g1pMar2020_v4SSVredo2wCC600parent_mva < 0.55)"
GF=333

#./hive -o app -f 8 -x $XML &> near.app.log
#./hive -o app -f 8 -x $XML1  &> near.app.r1.log
#./hive -o app -f 8 -x $XML2 &> near.app.r2.log
#./hive -o app -f 8 -x $XML3  &> near.app.r3.log


#
#for i in `seq 0 1 9`;
#do
    #./hive -o sbnfit -s 1 -f $i -x $XML --additional_tag="_NearSideband_" --cuts="$FULLNS" >> sbnfit.near.log
#done


### Full Near Sideband - Norm variables
#./hive --makedetcovar $DXML1 -x $XML -g 700 -s 1 --cuts "$sFULLNS" 
#./hive --makefluxcovar $FXXML --makedetcovar $DXML2 -n -9 -x $XML -g 700 -s 1 --cuts "$sFULLNS"
#mv *SBNcovar*root Covar_700series_NearSidebands/
#./hive -o datamc  -x $XML -s 1 -g 700  --cuts "$FULLNS" --additional_tag="VeryRealNearSideband700Det" --systematics "fluxdet"
#./hive -o datamc  -x $XML1 -s 1 -g 700  --cuts "$FULLNS" --additional_tag="VeryRealNearSideband700DetRun1Only" --systematics "fluxdet" > Norm1
#./hive -o datamc  -x $XML2 -s 1 -g 700  --cuts "$FULLNS" --additional_tag="VeryRealNearSideband700DetRun2Only" --systematics "fluxdet" > Norm2
#./hive -o datamc  -x $XML3 -s 1 -g 700  --cuts "$FULLNS" --additional_tag="VeryRealNearSideband700DetRun3Only" --systematics "fluxdet" > Norm3

#./hive -o datamc  -x $XML1 -s 1 -g 700  --cuts "$FULLNS" --additional_tag="VeryRealNearSideband700DetRun1Only" > Norm1
#./hive -o datamc  -x $XML2 -s 1 -g 700  --cuts "$FULLNS" --additional_tag="VeryRealNearSideband700DetRun2Only" > Norm2
#./hive -o datamc  -x $XML3 -s 1 -g 700  --cuts "$FULLNS" --additional_tag="VeryRealNearSideband700DetRun3Only" > Norm3


### Full Near Sideband - Base variables
#./hive --makefluxcovar $FXXML -x $XML -g 701 -s 1 --cuts "$sFULLNS"
#./hive -o datamc  -x $XML -s 1 -g 701  --cuts "$FULLNS" --additional_tag="VeryRealNearSideband701" --systematics "flux_fracfixed" 
#./hive -o stack  -x $XML -s 1 -g 701  --cuts "$FULLNS" --additional_tag="RealNearSideband701STACK" --pot=6.852e20 --systematics "fluxdet"

#./hive --makedetcovar $DXML1 -x $XML -g 701 -s 1 --cuts "$sFULLNS" 
#./hive --makefluxcovar $FXXML --makedetcovar $DXML2 -n -9 -x $XML -g 701 -s 1 --cuts "$sFULLNS"
#mv *SBNcovar*root Covar_700series_NearSidebands/
#./hive -o datamc  -x $XML -s 1 -g 701  --cuts "$FULLNS" --additional_tag="VeryRealNearSideband701Det" --systematics "fluxdet"

### Full Near Sideband - BDTs
#./hive --makefluxcovar $FXXML -x $XML -g 706 -s 1 --cuts "$sFULLNS"
#./hive --makedetcovar $DXML1 -x $XML -g 706 -s 1 --cuts "$sFULLNS" 
#./hive --makefluxcovar $FXXML --makedetcovar $DXML2 -n -9 -x $XML -g 706 -s 1 --cuts "$sFULLNS"
#mv *SBNcovar*root Covar_700series_NearSidebands/
#./hive -o datamc  -x $XML -s 1 -g 706  --cuts "$FULLNS" --additional_tag="VeryRealNearSideband706Det" --systematics "fluxdet"

./hive -o datamc  -x $XML -s 1 -g 706  --cuts "$FULLNS" --additional_tag="VeryRealNearSideband706" --systematics "flux_fracfixed"
#./hive -o stack  -x $XML -s 1 -g 706  --cuts "$FULLNS" --additional_tag="RealNearSideband706" --pot=6.91e20 --systematics "flux_fracfixed"
#./hive -o datamc  -x $XML1 -s 1 -g 706  --cuts "$FULLNS" --additional_tag="VeryRealNearSideband706DetRun1" --systematics "fluxdet"
#./hive -o datamc  -x $XML2 -s 1 -g 706  --cuts "$FULLNS" --additional_tag="VeryRealNearSideband706DetRun2" --systematics "fluxdet"
#./hive -o datamc  -x $XML3 -s 1 -g 706  --cuts "$FULLNS" --additional_tag="VeryRealNearSideband706DetRun3" --systematics "fluxdet"

### Full Near Sideband - ALL SSV training
#./hive --makefluxcovar $FXXML -x $XML -g 707 -s 1 --cuts "$sFULLNS"
#./hive --makedetcovar $DXML1 -x $XML -g 707 -s 1 --cuts "$sFULLNS"  
#./hive --makefluxcovar $FXXML --makedetcovar $DXML2 -n -9 -x $XML -g 707 -s 1 --cuts "$sFULLNS"
#mv *SBNcovar*root Covar_700series_NearSidebands/
#./hive -o datamc  -x $XML -s 1 -g 707  --cuts "$FULLNS" --additional_tag="VeryRealNearSideband707Det" --systematics "fluxdet"
#./hive -o datamc  -x $XML -s 1 -g 707  --cuts "$FULLNS" --additional_tag="VeryRealNearSideband707" --systematics "flux_fracfixed"
#./hive -o stack  -x $XML -s 1 -g 707  --cuts "$FULLNS" --additional_tag="RealNearSideband707" --pot=6.91e20 --systematics "flux_fracfixed"

### Full Near Sideband - Angular variables
#./hive --makefluxcovar $FXXML -x $XML -g 702 -s 1 --cuts "$sFULLNC"
#./hive --makedetcovar $DXML1 -x $XML -g 702 -s 1 --cuts "$sFULLNS"  
#./hive --makefluxcovar $FXXML --makedetcovar $DXML2 -n -9 -x $XML -g 702 -s 1 --cuts "$sFULLNS"
#mv *SBNcovar*root Covar_700series_NearSidebands/
#./hive -o datamc  -x $XML -s 1 -g 702  --cuts "$FULLNS" --additional_tag="VeryRealNearSideband702Det" --systematics "fluxdet"

#./hive -o datamc  -x $XML -s 1 -g 702  --cuts "$FULLNS" --additional_tag="VeryRealNearSideband702" --systematics "flux_fracfixed"
#./hive -o stack  -x $XML -s 1 -g 702  --cuts "$FULLNS" --additional_tag="RealNearSideband702" --pot=6.91e20 

### Full Near Sideband - Misc variables
#./hive --makefluxcovar $FXXML -x $XML -g 703 -s 1 --cuts "$sFULLNC"
#./hive --makedetcovar $DXML1 -x $XML -g 703 -s 1 --cuts "$sFULLNS"  
#./hive --makefluxcovar $FXXML --makedetcovar $DXML2 -n -9 -x $XML -g 703 -s 1 --cuts "$sFULLNS"
#mv *SBNcovar*root Covar_700series_NearSidebands/
#./hive -o datamc  -x $XML -s 1 -g 703  --cuts "$FULLNS" --additional_tag="VeryRealNearSideband703Det" --systematics "fluxdet"

#./hive -o datamc  -x $XML -s 1 -g 703  --cuts "$FULLNS" --additional_tag="VeryRealNearSideband703" --systematics "flux_fracfixed"
#./hive -o stack  -x $XML -s 1 -g 703  --cuts "$FULLNS" --additional_tag="RealNearSideband703" --pot=6.91e20 

#### Two Straggleers *****
#./hive --makedetcovar $DXML1 -x $XML -g 711 -s 1 --cuts "$sFULLNS"  
#./hive --makefluxcovar $FXXML --makedetcovar $DXML2 -n -9 -x $XML -g 711 -s 1 --cuts "$sFULLNS"
#mv *SBNcovar*root Covar_700series_NearSidebands/
#./hive -o datamc  -x $XML -s 1 -g 711  --cuts "$FULLNS" --additional_tag="VeryRealNearSideband711Det" --systematics "fluxdet"

#JUST 1 BDT###
#./hive --makedetcovar $DXML1 -x $XML -g 7077 -s 1 --cuts "$sFULLNS" &>LOG1
#./hive --makefluxcovar $FXXML --makedetcovar $DXML2 -n -9 -x $XML -g 7077 -s 1 --cuts "$sFULLNS" &> LOG2
#mv *SBNcovar*root Covar_700series_NearSidebands/
#./hive -o datamc  -x $XML -s 1 -g 7066  --cuts "$FULLNS" --additional_tag="VeryRealNearSideband7066Det" --systematics "fluxdet"

#JUST 1 BDT###
#./hive --makedetcovar $DXML1 -x $XML -g 710 -s 1 --cuts "$sFULLNS" 
#./hive --makefluxcovar $FXXML --makedetcovar $DXML2 -n -9 -x $XML -g 710 -s 1 --cuts "$sFULLNS" 
#mv *SBNcovar*root Covar_700series_NearSidebands/
#./hive -o datamc  -x $XML -s 1 -g 710  --cuts "$FULLNS" --additional_tag="VeryRealNearSideband710Det" --systematics "fluxdet"


#RUn2RUN 
#./hive -o datamc  -x $XML1 -s 1 -g 711  --cuts "$FULLNS" --additional_tag="VeryRealNearSideband701DetRun1" 
#./hive -o datamc  -x $XML2 -s 1 -g 711  --cuts "$FULLNS" --additional_tag="VeryRealNearSideband701DetRun2" 
#./hive -o datamc  -x $XML3 -s 1 -g 711  --cuts "$FULLNS" --additional_tag="VeryRealNearSideband701DetRun3" 

