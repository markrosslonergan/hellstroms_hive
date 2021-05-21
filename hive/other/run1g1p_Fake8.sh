#!/bin/bash

FXXML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/template_1g1p_stage_1_Jan2021.xml"
FXXML2="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/template_1g1p_stage_6_Jan2021.xml"
DXML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/DetSys_Neutrino2020/template_detsys_1g1p_Feb2021_v4.xml"
#XML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/1g1p_EmeraldWorldOrder_Run1_v4_Plot.xml"
XML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/1g1p_EmeraldWorldOrder_CombinedRuns13_v4_Fake8_Plot.xml"

#./hive -o app -f 8 -x $XML

#./hive --makefluxcovar $FXXML -g 3302 -s 1 -x $XML
#./hive -o datamc  -x $XML -s 1 -g 3302  --additional_tag="Fake8" --systematics="flux_fracfixed"

#./hive -o datamc  -x $XML -s 6 -g 3303  --additional_tag="Fake8" 

#fake 8 bdt resp
#./hive -o datamc  -x $XML -s 1 -g 6  --additional_tag="Fake8" 


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

#./hive --makefluxcovar $FXXML -x $XML -s 1 -g 3304 --cuts="$sFULLFS"
#./hive -o datamc  -x $XML -s 1 -g 3304 --cuts="$FULLFS"  --additional_tag="FullFarSidebands" --systematics="flux_fracfixed"

#./hive --makefluxcovar $FXXML -x $XML -s 1 -g 3305 --cuts="$sNCPIFS"
#./hive -o datamc  -x $XML -s 1 -g 3305 --cuts="$NCPIFS"  --additional_tag="NCPi0FarSidebands" --systematics="flux_fracfixed"

#./hive --makefluxcovar $FXXML -x $XML -s 1 -g 3306 --cuts="$sBNBFS"
#./hive -o datamc  -x $XML -s 1 -g 3306 --cuts="$BNBFS"  --additional_tag="BNBOtherFarSidebands" --systematics="flux_fracfixed"

#./hive --makefluxcovar $FXXML -x $XML -s 1 -g 3307 --cuts="$sFULLFSnoC"
#./hive -o datamc  -x $XML -s 1 -g 3307 --cuts="$FULLFSnoC"  --additional_tag="FullFarSidebands" --systematics="flux_fracfixed"


# Full NS Cuts, No Cosmic
sFULLNS="(simple_1g1pMar2020_v4Nue_mva>= 0.3 && (simple_1g1pMar2020_v4BNB_mva >= 0.96273 && simple_1g1pMar2020_v4NCPi0_mva >= 0.467404) &&  simple_1g1pMar2020_v4SSVredo2wCC600parent_mva < 0.55)"
FULLNS=" (1g1pMar2020_v4Nue_mva>= 0.3 && (1g1pMar2020_v4BNB_mva >= 0.96273 && 1g1pMar2020_v4NCPi0_mva >= 0.467404) && 1g1pMar2020_v4SSVredo2wCC600parent_mva < 0.55)"
GF=333

#ENERGY
#./hive --makefluxcovar $FXXML -x $XML -s 1 -g 3306 --cuts="$sFULLNS"
#./hive -o datamc  -x $XML -s 1 -g 3306 --cuts="$FULLNS"  --additional_tag="FullNearSidebands" --systematics="flux_fracfixed"

#BDT responses
#./hive --makefluxcovar $FXXML -x $XML -s 1 -g 3309 --cuts="$sFULLNS"
#./hive -o datamc  -x $XML -s 1 -g 3309 --cuts="$FULLNS"  --additional_tag="FullNearSidebands" --systematics="flux_fracfixed"


#./hive --makefluxcovar $FXXML -g 3302 -s 1 -x $XML
#./hive -o datamc  -x $XML -s 1 -g 3302  --additional_tag="Fake8" --systematics="flux_fracfixed"

#./hive -o datamc  -x $XML -s 6 -g 3303  --additional_tag="Fake8" 

################## Signal ############3#
#for i in `seq 0 1 9`;
#do
#    ./hive -o sbnfit -s 6 -f $i -x $XML --additional_tag="_Fake8" --cuts="1"
#done

#./hive --makefluxcovar $FXXML2 -x $XML -s 6 -g 3333 
./hive -o datamc  -x $XML -s 6 -g 3333  --additional_tag="Fake8Final" --systematics="flux_fracfixed"


