#!/bin/bash

FXXML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/template_1g1p_stage_1_Jan2021.xml"
DXML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/DetSys_Neutrino2020/template_detsys_1g1p_Feb2021_v4.xml"
XML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/1g1p_EmeraldWorldOrder_CombinedRuns13_v4_NearSideband_Plot.xml"
#XML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/1g1p_EmeraldWorldOrder_Run1_v4_NearSideband_Plot.xml"

# Full NS Cuts, No Cosmic
sFULLNS="(simple_1g1pMar2020_v4Nue_mva>= 0.3 && (simple_1g1pMar2020_v4BNB_mva >= 0.96273 && simple_1g1pMar2020_v4NCPi0_mva >= 0.467404) &&  simple_1g1pMar2020_v4SSVredo2wCC600parent_mva < 0.55)"
FULLNS=" (1g1pMar2020_v4Nue_mva>= 0.3 && (1g1pMar2020_v4BNB_mva >= 0.96273 && 1g1pMar2020_v4NCPi0_mva >= 0.467404) && 1g1pMar2020_v4SSVredo2wCC600parent_mva < 0.55)"
GF=333

### Full Near Sideband - Base variables
#./hive --makefluxcovar $FXXML -x $XML -g 701 -s 1 --cuts "$sFULLNC"
#./hive -o datamc  -x $XML -s 1 -g 701  --cuts "$FULLNS" --additional_tag="RealNearSideband701" --systematics "flux_fracfixed"
#./hive -o stack  -x $XML -s 1 -g 701  --cuts "$FULLNS" --additional_tag="RealNearSideband701" --pot=6.91e20 --systematics "flux_fracfixed"

### Full Near Sideband - BDTs
#./hive --makefluxcovar $FXXML -x $XML -g 706 -s 1 --cuts "$sFULLNC"
#./hive -o datamc  -x $XML -s 1 -g 706  --cuts "$FULLNS" --additional_tag="RealNearSideband706" --systematics "flux_fracfixed"
#./hive -o stack  -x $XML -s 1 -g 706  --cuts "$FULLNS" --additional_tag="RealNearSideband706" --pot=6.91e20 --systematics "flux_fracfixed"

### Full Near Sideband - ALL SSV training
#./hive --makefluxcovar $FXXML -x $XML -g 707 -s 1 --cuts "$sFULLNC"
#./hive -o datamc  -x $XML -s 1 -g 707  --cuts "$FULLNS" --additional_tag="RealNearSideband707" --systematics "flux_fracfixed"
#./hive -o stack  -x $XML -s 1 -g 707  --cuts "$FULLNS" --additional_tag="RealNearSideband707" --pot=6.91e20 --systematics "flux_fracfixed"

### Full Near Sideband - Angular variables
#./hive --makefluxcovar $FXXML -x $XML -g 702 -s 1 --cuts "$sFULLNC"
#./hive -o datamc  -x $XML -s 1 -g 702  --cuts "$FULLNS" --additional_tag="RealNearSideband702" --systematics "flux_fracfixed"
#./hive -o stack  -x $XML -s 1 -g 702  --cuts "$FULLNS" --additional_tag="RealNearSideband702" --pot=6.91e20 

### Full Near Sideband - Misc variables
#./hive --makefluxcovar $FXXML -x $XML -g 703 -s 1 --cuts "$sFULLNC"
#./hive -o datamc  -x $XML -s 1 -g 703  --cuts "$FULLNS" --additional_tag="RealNearSideband703" --systematics "flux_fracfixed"
#./hive -o stack  -x $XML -s 1 -g 703  --cuts "$FULLNS" --additional_tag="RealNearSideband703" --pot=6.91e20 



