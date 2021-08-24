#!/bin/bash

FXXML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/template_1g1p_stage_1_Jan2021.xml"
DXML1="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/DetSys_Neutrino2020/template_detsys_1g1p_May2021_v23.2_All.xml"
DXML2="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/DetSys_Neutrino2020/template_detsys_1g1p_May2021_v23.1_SCE_Recom2.xml"
XML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/1g1p_EmeraldWorldOrder_CombinedRuns13_v4_FullDatasetCombinedEpochs_Plot.xml"

XMLSTAGE1="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/1g1p_EmeraldWorldOrder_CombinedRuns13_v4_FullDatasetCombinedEpochs_PlotTrain_Stage1.xml"
XMLSTAGE2="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/1g1p_EmeraldWorldOrder_CombinedRuns13_v4_FullDatasetCombinedEpochs_PlotTrain_Stage2.xml"
XMLSTAGE3="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/1g1p_EmeraldWorldOrder_CombinedRuns13_v4_FullDatasetCombinedEpochs_PlotTrain_Stage3.xml"


#Do run-by-run later
#XML1="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/1g1p_EmeraldWorldOrder_Run1Only_v4_NearSideband_Plot.xml"
#XML2="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/1g1p_EmeraldWorldOrder_Run2Only_v4_NearSideband_Plot.xml"
#XML3="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/1g1p_EmeraldWorldOrder_Run3Only_v4_NearSideband_Plot.xml"

FINALCUT="(1g1pMar2020_v4COSMIC_mva >= 0.9531 && 1g1pMar2020_v4BNB_mva >= 0.9845 && 1g1pMar2020_v4NCPi0_mva >= 0.467404 && 1g1pMar2020_v4Nue_mva >= 0.7474 && 1g1pMar2020_v4SSVredo2wCC600parent_mva >=0.7088)"
sFINALCUT="(simple_1g1pMar2020_v4COSMIC_mva >= 0.9531 && simple_1g1pMar2020_v4BNB_mva >= 0.9845 && simple_1g1pMar2020_v4NCPi0_mva >= 0.467404 && simple_1g1pMar2020_v4Nue_mva >= 0.7474 && simple_1g1pMar2020_v4SSVredo2wCC600parent_mva >=0.7088)"


if (false); then
    ./hive -o app -f 8 -x $XML &> combined.app.uniq.log
#    ./hive -o sbnfit -s 6 -f 8 -x $XML --additional_tag="_RealLiveUnblinding" --cuts="$FINALCUT" > sbnfit.finalsel.log

fi

### Setting group number 407 to be the FINAL 1g1p distribtuions
#First run Flux & XS systematics Covariance matrices before the live run of course.

#All BDTs
if (false); then
    #./hive --makefluxcovar $FXXML -x $XML -g 6 -s 1
    #mv *SBNcovar*root Covar_CombinedEpoch/
#    ./hive -o datamc  -x $XML -s 1 -g 6 --additional_tag="CombinedEpochBDT" --systematics="flux"
 
    #./hive --makefluxcovar $FXXML -x $XML -g 66 -s 1 
    #./hive --makedetcovar $DXML1 -x $XML -g 66 -s 1   
    #./hive --makefluxcovar $FXXML --makedetcovar $DXML2 -n -9 -x $XML -g 66 -s 1 
    #mv *SBNcovar*root Covar_CombinedEpoch/
    #./hive_pi0mod -o datamc  -x $XML -s 1 -g 66 --additional_tag="CombinedEpochBDTDetPi0Mod" --systematics="fluxdet"
    ./hive_pi0mod -o datamc  -x $XML -s 1 -g 66 --additional_tag="CombinedEpochBDTDetPi0ModStat"
    #./hive -o datamc  -x $XML -s 1 -g 66 --additional_tag="CombinedEpochBDTDetPi0Flat" --systematics="fluxdet"
    #./hive -o datamc  -x $XML -s 1 -g 66 --additional_tag="CombinedEpochBDTDet" --systematics="fluxdet"

fi

if (false); then
    #./hive --makefluxcovar $FXXML -x $XML -g 6 -s 1
    #mv *SBNcovar*root Covar_CombinedEpoch/
    #./hive -o datamc  -x $XML -s 1 -g 407 --additional_tag="CombinedEpochSTEP" 
    #./hive -o datamc  -x $XML -s 2 -g 407 --additional_tag="CombinedEpochSTEP" 
    #./hive -o datamc  -x $XML -s 3 -g 407 --additional_tag="CombinedEpochSTEP" 
    #./hive -o datamc  -x $XML -s 4 -g 407 --additional_tag="CombinedEpochSTEP" 
    #./hive -o datamc  -x $XML -s 5 -g 407 --additional_tag="CombinedEpochSTEP" 
    #./hive -o datamc  -x $XML -s 5 -g 4066 --additional_tag="CombinedEpochSTEPBDT" 
    
    #./hive -o datamc  -x $XML -s 5 -g 407 --additional_tag="CombinedEpochSTEP5z" --cuts="1g1pMar2020_v4SSVredo2wCC600parent_mva>0.4"
    #./hive -o datamc  -x $XML -s 5 -g 407 --additional_tag="CombinedEpochSTEP5a" --cuts="1g1pMar2020_v4SSVredo2wCC600parent_mva>0.5"
    #./hive -o datamc  -x $XML -s 5 -g 407 --additional_tag="CombinedEpochSTEP5b" --cuts="1g1pMar2020_v4SSVredo2wCC600parent_mva>0.6"
    #./hive -o datamc  -x $XML -s 5 -g 407 --additional_tag="CombinedEpochSTEP5c" --cuts="1g1pMar2020_v4SSVredo2wCC600parent_mva>0.7"

    #Reco Vertex X
    ./hive -o datamc  -x $XML -s 5 -g 4077 --additional_tag="CombinedEpochVERT5z" --cuts="1g1pMar2020_v4SSVredo2wCC600parent_mva>0.4"
    ./hive -o datamc  -x $XML -s 5 -g 4077 --additional_tag="CombinedEpochVERT5a" --cuts="1g1pMar2020_v4SSVredo2wCC600parent_mva>0.5"
    ./hive -o datamc  -x $XML -s 5 -g 4077 --additional_tag="CombinedEpochVERT5b" --cuts="1g1pMar2020_v4SSVredo2wCC600parent_mva>0.6"
    ./hive -o datamc  -x $XML -s 5 -g 4077 --additional_tag="CombinedEpochVERT5c" --cuts="1g1pMar2020_v4SSVredo2wCC600parent_mva>0.7"

fi

#Full Training run of 4404
if (true); then
    
    #Run these once!
    #25 bins for all below.
    #./hive --makefluxcovar $FXXML -x $XMLSTAGE1 -g 4404 -s 1
    #mv *SBNcovar*root /pnfs/uboone/persistent/users/markross/Feb2021_gLEE_files/CovarianceMatricies/CombinedEpoch/1g1p/Covar_CombinedEpoch_Precut/
    #./hive -o datamc  -x $XMLSTAGE1 -s 1 -g 4404 --additional_tag="CombinedEpochTrainALL" --systematics="flux"

    #./hive --makefluxcovar $FXXML -x $XMLSTAGE2 -g 44042 -s 2
    #mv *SBNcovar*root /pnfs/uboone/persistent/users/markross/Feb2021_gLEE_files/CovarianceMatricies/CombinedEpoch/1g1p/Covar_CombinedEpoch_Stage2/
    #./hive -o datamc  -x $XMLSTAGE2 -s 2 -g 44042 --additional_tag="CombinedEpochTrainALL" --systematics="flux"

    #25 bins above, 14 below!
    ./hive --makefluxcovar $FXXML -x $XMLSTAGE3 -g 44042 -s 3
    mv *SBNcovar*root /pnfs/uboone/persistent/users/markross/Feb2021_gLEE_files/CovarianceMatricies/CombinedEpoch/1g1p/Covar_CombinedEpoch_Stage3/
    ./hive -o datamc  -x $XMLSTAGE3 -s 3 -g 44042 --additional_tag="CombinedEpochTrainALL" --systematics="flux"


fi

