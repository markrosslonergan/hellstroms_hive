#!/bin/bash

FXXML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/template_1g0p_stage_1_Jan2021.xml"
FXXML2="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/template_1g0p_stage_4_Jan2021.xml"
DXML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/DetSys_Neutrino2020/template_detsys_1g0p_Feb2021_v4.xml"
XML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g0p_xmls/hive4.0/1g0p_EmeraldWorldOrder_CombinedRuns13_v4_FullDatasetCombinedEpochs_Plot.xml"

XMLSTAGE4="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g0p_xmls/hive4.0/1g0p_EmeraldWorldOrder_CombinedRuns13_v4_FullDatasetCombinedEpochs_PlotTrain_Stage4.xml"
XMLSTAGE3="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g0p_xmls/hive4.0/1g0p_EmeraldWorldOrder_CombinedRuns13_v4_FullDatasetCombinedEpochs_PlotTrain_Stage3.xml"
XMLSTAGE2="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g0p_xmls/hive4.0/1g0p_EmeraldWorldOrder_CombinedRuns13_v4_FullDatasetCombinedEpochs_PlotTrain_Stage2.xml"
XMLSTAGE1="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g0p_xmls/hive4.0/1g0p_EmeraldWorldOrder_CombinedRuns13_v4_FullDatasetCombinedEpochs_PlotTrain_Stage1.xml"

FINALCUT="(1g0pMar2020COSMIC_mva >= 0.988209 && 1g0pMar2020BNB_mva >= 0.892728 && 1g0pMar2020NCPi0_mva >= 0.428825)"
sFINALCUT="(simple_1g0pMar2020COSMIC_mva >= 0.988209 && simple_1g0pMar2020BNB_mva >= 0.892728 && simple_1g0pMar2020NCPi0_mva >= 0.428825)"

#Pregenerate BNBext at correct Spills for exact final selection. Only needs to be done once.
##./hive -o sbnfit -s 4 -f 9 -x $XML --additional_tag="_FinalSelectionSpills" --cuts="$FINALCUT" 

if (false); then
    ./hive -o app -f 8 -x $XML &> final.app.Comb.log.uniq

fi
### Setting group number 407 to be the FINAL 1g1p distribtuions
#First run Flux & XS systematics only

if (false); then
    #./hive --makefluxcovar $FXXML -x $XML -g 407 -s 4 --cuts "$sFINALCUT"
    #mv *SBNcovar*root Covar_407Series_FinalSelection/
    #./hive -o stack  -x $XML -s 4 -g 407  --cuts "$FINALCUT" --additional_tag="FinalSelection407Series" --systematics "flux" --pot 6.852e+20
    #/uboone/app/users/markrl/useful_scripts/pdfjam/pdfjam/bin/pdfjam stack/*pdf --landscape --nup 2x2 --outfile stack/RESULT_1g0p_stack_407series_Jammed.pdf

#    ./hive -o datamc  -x $XML -s 4 -g 4077  --additional_tag="COMBINED4077Series"
#    ./hive -o datamc  -x $XML -s 2 -g 4077  --additional_tag="COMBINED4077Series"
#    ./hive -o datamc  -x $XML -s 3 -g 4077  --additional_tag="COMBINED4077Series" 
#    ./hive -o datamc  -x $XML -s 4 -g 4077  --additional_tag="COMBINED4077SeriesMore"  --cuts="1g0pMar2020COSMIC_mva >= 0.988209 && 1g0pMar2020BNB_mva >= 0.892728 && 1g0pMar2020NCPi0_mva >= 0.7"

    #./hive --makefluxcovar $FXXML -x $XML -g 11111 -s 1 --cuts "$sFINALCUT"
    #mv *SBNcovar*root Covar_407Series_FinalSelection/
    #./hive -o datamc  -x $XML -s 1 -g 11111  --additional_tag="COMBINED11111BDT" --systematics "flux"

    
    #./hive --makefluxcovar $FXXML -x $XML -g 4077 -s 1 --cuts "$sFINALCUT"
    #mv *SBNcovar*root Covar_407Series_FinalSelection/
    #./hive -o datamc  -x $XML -s 1 -g 4077  --additional_tag="COMBINED" --systematics "flux"

    #./hive --makefluxcovar $FXXML -x $XML -g 4078 -s 2 --cuts "$sFINALCUT"
    #mv *SBNcovar*root Covar_407Series_FinalSelection/
    #./hive -o datamc  -x $XML -s 2 -g 4078  --additional_tag="COMBINED" --systematics "flux"

    #./hive --makefluxcovar $FXXML -x $XML -g 4079 -s 3 --cuts "$sFINALCUT"
    #mv *SBNcovar*root Covar_407Series_FinalSelection/
    #./hive -o datamc  -x $XML -s 3 -g 4079  --additional_tag="COMBINED" --systematics "flux"


    #./hiveGIF -o gif -n 77 -b 2 -x $XML -s 1

    #./hive -o datamc  -x $XML -s 3 -g 4077  --additional_tag="COMBINED_deeper" --cuts="1g0pMar2020BNB_mva> 0.91"

    #/uboone/app/users/markrl/useful_scripts/pdfjam/pdfjam/bin/pdfjam datamc/*FinalSelection407Series*pdf --landscape --nup 2x2 --outfile datamc/RESULT_1g0p_datamc_407series_Jammed.pdf
    ./hive -o datamc  -x $XML -s 2 -g 11111  --additional_tag="COMBINED11111BDT" 


fi

if (true); then
    
    #Precut - 25 bins
    ./hive --makefluxcovar $FXXML -x $XMLSTAGE1 -g 44042 -s 1 
    mv *SBNcovar*root /pnfs/uboone/persistent/users/markross/Feb2021_gLEE_files/CovarianceMatricies/CombinedEpoch/1g0p/Covar_CombinedEpoch_Precut/
    ./hive -o datamc  -x $XMLSTAGE1 -s 1 -g 44042  --additional_tag="COMBINEDAllEpochsTraining" --systematics="flux"

    #Stage 2 - 25 bins
    ./hive --makefluxcovar $FXXML -x $XMLSTAGE2 -g 44042 -s 2 
    mv *SBNcovar*root /pnfs/uboone/persistent/users/markross/Feb2021_gLEE_files/CovarianceMatricies/CombinedEpoch/1g0p/Covar_CombinedEpoch_Stage2/
    ./hive -o datamc  -x $XMLSTAGE2 -s 2 -g 44042 --additional_tag="COMBINEDAllEpochsTraining" --systematics="flux"
    
    #Stage 3 - 14 bins
    #./hive --makefluxcovar $FXXML -x $XMLSTAGE3 -g 4404 -s 3 
    #mv *SBNcovar*root /pnfs/uboone/persistent/users/markross/Feb2021_gLEE_files/CovarianceMatricies/CombinedEpoch/1g0p/Covar_CombinedEpoch_Stage3/
    #./hive -o datamc  -x $XMLSTAGE3 -s 3 -g 4404 --additional_tag="COMBINEDAllEpochsTraining" --systematics="flux"


    #Stage 4 - 10 bins
    #./hive --makefluxcovar $FXXML -x $XMLSTAGE4 -g 44042 -s 4 
    #mv *SBNcovar*root /pnfs/uboone/persistent/users/markross/Feb2021_gLEE_files/CovarianceMatricies/CombinedEpoch/1g0p/Covar_CombinedEpoch_Stage4/
    #./hive -o datamc  -x $XMLSTAGE4 -s 4 -g 44042 --additional_tag="COMBINEDAllEpochsTraining" --systematics="flux"


fi
