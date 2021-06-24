#!/bin/bash

FXXML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/template_1g1p_stage_1_Jan2021.xml"
DXML1="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/DetSys_Neutrino2020/template_detsys_1g1p_May2021_v23.2_All.xml"
DXML2="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/DetSys_Neutrino2020/template_detsys_1g1p_May2021_v23.1_SCE_Recom2.xml"
XML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/1g1p_EmeraldWorldOrder_CombinedRuns13_v4_FinalSelection_Plot.xml"

#Do run-by-run later
#XML1="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/1g1p_EmeraldWorldOrder_Run1Only_v4_NearSideband_Plot.xml"
#XML2="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/1g1p_EmeraldWorldOrder_Run2Only_v4_NearSideband_Plot.xml"
#XML3="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/1g1p_EmeraldWorldOrder_Run3Only_v4_NearSideband_Plot.xml"

FINALCUT="(1g1pMar2020_v4COSMIC_mva >= 0.9531 && 1g1pMar2020_v4BNB_mva >= 0.9845 && 1g1pMar2020_v4NCPi0_mva >= 0.467404 && 1g1pMar2020_v4Nue_mva >= 0.7474 && 1g1pMar2020_v4SSVredo2wCC600parent_mva >=0.7088)"
sFINALCUT="(simple_1g1pMar2020_v4COSMIC_mva >= 0.9531 && simple_1g1pMar2020_v4BNB_mva >= 0.9845 && simple_1g1pMar2020_v4NCPi0_mva >= 0.467404 && simple_1g1pMar2020_v4Nue_mva >= 0.7474 && simple_1g1pMar2020_v4SSVredo2wCC600parent_mva >=0.7088)"

#Pregenerate BNBext at correct Spills for exact final selection
#./hive -o sbnfit -s 6 -f 9 -x $XML --additional_tag="_FinalSelectionSpills" --cuts="$FINALCUT" 

if (false); then
    ./hive -o app -f 8 -x $XML &> final.app.log
    ./hive -o sbnfit -s 6 -f 8 -x $XML --additional_tag="_Fake3LiveUnblinding" --cuts="$FINALCUT" > sbnfit.finalsel.log

    #And copy to the predefined location that the SBNfit xml points to
    cp sbnfit_1g1pMar2020_v4_Fake3LiveUnblinding_stage_6_FinalSelection1g1p.root /uboone/data/users/markross/June2021_UnblindingMove/LiveUnblindingSBNfits/

    echo "1g1p Done! - Copied necessary files to /uboone/data/users/markross/June2021_UnblindingMove/LiveUnblindingSBNfits/ "
    echo "Can begin to run SBNfit scripts now. "

fi

### Setting group number 407 to be the FINAL 1g1p distribtuions
#First run Flux & XS systematics Covariance matrices before the live run of course.

if (true); then
    #./hive --makefluxcovar $FXXML -x $XML -g 407 -s 6 --cuts "$sFINALCUT"
    #mv *SBNcovar*root Covar_407Series_FinalSelection/
    #./hive -o stack  -x $XML -s 6 -g 407  --cuts "$FINALCUT" --additional_tag="FinalSelection407Series" --systematics "flux" --pot=6.852e+20
    #/uboone/app/users/markrl/useful_scripts/pdfjam/pdfjam/bin/pdfjam stack/*pdf --landscape --nup 2x2 --outfile stack/RESULT_1g1p_stack_407series_Jammed.pdf

    rm datamc/*pdf 
    ./hive -o datamc  -x $XML -s 6 -g 407  --cuts "$FINALCUT" --additional_tag="FinalSelection407Series" --systematics "flux"
    /uboone/app/users/markrl/useful_scripts/pdfjam/pdfjam/bin/pdfjam datamc/*FinalSelection407Series*pdf --landscape --nup 2x2 --outfile datamc/RESULT_1g1p_datamc_407series_Jammed.pdf
fi

