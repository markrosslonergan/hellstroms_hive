#!/bin/bash

FXXML6="/uboone/app/users/ksutton/hellstroms_hive_2.5_sl7/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/template_1g1p_stage_6_Run13combined_NoLEE_Dec2020.xml"
FXXML1="/uboone/app/users/ksutton/hellstroms_hive_2.5_sl7/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/template_1g1p_stage_1_Run13combined_NoLEE_Dec2020.xml"
XML="/uboone/app/users/ksutton/hellstroms_hive_2.5_sl7/hellstroms_hive/hive/xml/1g1p_xmls/hive2.5/internalnote_1_27_21/1g1p_DandelionWorldOrder_Run1_v6_BGfix_PLOTTING.xml"


# Full FS Cuts
sFULLFS="(simple_1g1pMar2020_v4COSMIC_mva >=0.9 && simple_1g1pMar2020_v4Nue_mva >= 0.3 && (simple_1g1pMar2020_v4BNB_mva < 0.96273 || simple_1g1pMar2020_v4NCPi0_mva < 0.467404))"
FULLFS="(1g1pMar2020_v4COSMIC_mva >=0.9 &&1g1pMar2020_v4Nue_mva>= 0.3 && (1g1pMar2020_v4BNB_mva < 0.96273 || 1g1pMar2020_v4NCPi0_mva < 0.467404))"

# NCPi0 Cuts
sNCPIFS="(simple_1g1pMar2020_v4COSMIC_mva >=0.9 && simple_1g1pMar2020_v4Nue_mva >= 0.3 && (simple_1g1pMar2020_v4BNB_mva >= 0.96273 && simple_1g1pMar2020_v4NCPi0_mva < 0.467404))"
NCPIFS="(1g1pMar2020_v4COSMIC_mva >=0.9 &&1g1pMar2020_v4Nue_mva>= 0.3 && (1g1pMar2020_v4BNB_mva >= 0.96273 && 1g1pMar2020_v4NCPi0_mva < 0.467404))"

# BNB Cuts
sBNBFS="(simple_1g1pMar2020_v4COSMIC_mva >=0.9 && simple_1g1pMar2020_v4Nue_mva >= 0.3 && (simple_1g1pMar2020_v4BNB_mva < 0.96273 && simple_1g1pMar2020_v4NCPi0_mva >= 0.467404))"
BNBFS="(1g1pMar2020_v4COSMIC_mva >=0.9 && 1g1pMar2020_v4Nue_mva>= 0.3 && (1g1pMar2020_v4BNB_mva < 0.96273 && 1g1pMar2020_v4NCPi0_mva >= 0.467404))"

NUM=2212

#stage0 + 1
#./../../bin/hive -o datamc --makefluxcovar $FXXML1 -x $XML -s 1 -g330
#./../../bin/hive -o datamc  -x $XML -s 0 -g330 --systematics "flux_fracfixed"
#./../../bin/hive -o datamc  -x $XML -s 1 -g330 --systematics "flux_fracfixed"

#stage 1
#./../../bin/hive -o datamc --makefluxcovar $FXXML1 -x $XML -s 1 -g331
#./../../bin/hive -o datamc  -x $XML -s 1 -g331 --systematics "flux_fracfixed"

#Single BDT Cuts
#./../../bin/hive -o datamc --makefluxcovar $FXXML1 -x $XML -s 1 -g335
#./../../bin/hive -o datamc  -x $XML -s 1 -g335 "flux_fracfixed"
#./../../bin/hive -o datamc  -x $XML -s 1 -g335 --systematics "flux_fracfixed"
#./../../bin/hive -o eff -n2 -x $XML -s 1 -g335 --systematics "flux_fracfixed"

#BDT Scores
./../../bin/hive -o datamc --makefluxcovar $FXXML1 -x $XML -s 1 -g6
./../../bin/hive -o datamc  -x $XML -s 1 -g6 --systematics "flux_fracfixed"

#stage 6
#./../../bin/hive -o datamc --makefluxcovar $FXXML6 -x $XML -s 6 -g332
#./../../bin/hive -o datamc  -x $XML -s 6 -g332 --systematics "flux_fracfixed"
#./../../bin/hive -o stack -x $XML -s 6 -g332 --systematics "flux_fracfixed"

