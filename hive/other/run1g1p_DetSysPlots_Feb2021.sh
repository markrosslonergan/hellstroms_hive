#!/bin/bash

FXXML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/template_1g1p_stage_1_Jan2021.xml"
FXXML2="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/template_1g1p_stage_6_Jan2021.xml"
DXML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/DetSys_Neutrino2020/template_detsys_1g1p_Feb2021_v4.xml"
XML1="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/1g1p_EmeraldWorldOrder_Run1_v4_Plot.xml"
XML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/1g1p_EmeraldWorldOrder_CombinedRuns13_v4_Plot.xml"



#Just one full farside best, without CC
#./hive --makefluxcovar $FXXML --makedetcovar $DXML -x $XML -g 7707 -s 1  &> det.log
#./hive -o datamc  -x $XML -s 1 -g 7707  --additional_tag="wDetsys" --systematics "fluxdet"
#./hive -o datamc  -x $XML -s 1 -g 7707  --additional_tag="wJustDetsys" --systematics "merged_det"
#./hive -o datamc  -x $XML -s 1 -g 7707  --additional_tag="wOutDetsys" --systematics "flux_fracfixed"
#rm  *Far*entry*root

#Hello
#./hive --makefluxcovar $FXXML -x $XML1 -g  3302
#./hive -o datamc  -x $XML1 -s 0 -g 3302  --additional_tag="DirtTopo" --systematics "flux_fracfixed"


#Just one full farside best, without CC
#./hive --makefluxcovar $FXXML2 --makedetcovar $DXML -x $XML -g 3303 -s 6  
#./hive -o stack  -x $XML -s 6 -g 3303  --additional_tag="wDetsys" --systematics "fluxdet" --pot 6.91e20

#./hive -o fancy -g 778 -x $XML 

#Purity Studies for Sophie
#./hive -o app  -f 18 -x $XML 
#./hive -o eff -n 4 -f 18 -x $XML -g 777


#Guiseppe plotsi
#./hive -o stack -s 6 -x $XML --additional_tag="Guiseppe" -g 779 --pot 6.91e20
#./hive -o sbnfit -s 1 -x $XML -f 4

#./hive --makefluxcovar $FXXML -x $XML -g 6 -s 1  --cut="1g1pMar2020_v4COSMIC_mva>0.25"
#./hive -o datamc  -x $XML -s 1 -g 6  --additional_tag="Guis_coscut" --systematics "flux_fracfixed" --cut="1g1pMar2020_v4COSMIC_mva>0.25"

#
#./hive2 --makefluxcovar $FXXML -x $XML1 -g 3303 -s 1 
#./hive2 -o datamc -s 1 -x $XML1 --additional_tag="GuisTest" -g 3303 --systematics="flux_fracfixed"



#./hive -o datamc  -x $XML1 -s 1 -g 3303  --additional_tag="POTcheck_After" 

#cp /uboone/app/users/markrl/SBNfit_uBooNE/July2020_SL7/whipping_star/build/bin/Feb2021_Final/AprilColl_FullRedo_afterFix/FinalMatricies/Fin_FluxXSDet_1g1p.SBNcovar.root VID267_FinApril.SBNcovar.root
#./hive -o stack  -x $XML -s 6 -g 3309  --additional_tag="Fin" --systematics "FinApril"




