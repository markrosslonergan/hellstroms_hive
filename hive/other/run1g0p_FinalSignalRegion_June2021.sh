#!/bin/bash

FXXML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/template_1g0p_stage_1_Jan2021.xml"
FXXML2="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/template_1g0p_stage_4_Jan2021.xml"
DXML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/DetSys_Neutrino2020/template_detsys_1g0p_Feb2021_v4.xml"
XML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g0p_xmls/hive4.0/1g0p_EmeraldWorldOrder_CombinedRuns13_v4_FinalSelection_Plot.xml"

FINALCUT="(1g0pMar2020COSMIC_mva >= 0.988209 && 1g0pMar2020BNB_mva >= 0.892728 && 1g0pMar2020NCPi0_mva >= 0.428825)"
sFINALCUT="(simple_1g0pMar2020COSMIC_mva >= 0.988209 && simple_1g0pMar2020BNB_mva >= 0.892728 && simple_1g0pMar2020NCPi0_mva >= 0.428825)"

#Pregenerate BNBext at correct Spills for exact final selection. Only needs to be done once.
##./hive -o sbnfit -s 4 -f 9 -x $XML --additional_tag="_FinalSelectionSpills" --cuts="$FINALCUT" 

if (false); then
    ./hive -o app -f 8 -x $XML &> final.app.log
    ./hive -o sbnfit -s 4 -f 8 -x $XML --additional_tag="_Fake3LiveUnblinding" --cuts="$FINALCUT" > sbnfit.finalsel.log

    #and cp to the location predefined by sbnfit
    cp sbnfit_1g0pMar2020_Fake3LiveUnblinding_stage_4_FinalSelection1g0p.root /uboone/data/users/markross/June2021_UnblindingMove/LiveUnblindingSBNfits/

    echo "1g0p Done! - Copied necessary files to /uboone/data/users/markross/June2021_UnblindingMove/LiveUnblindingSBNfits/ "
    echo "Can begin to run SBNfit scripts now. "

fi
### Setting group number 407 to be the FINAL 1g1p distribtuions
#First run Flux & XS systematics only

if (true); then
    #./hive --makefluxcovar $FXXML -x $XML -g 407 -s 4 --cuts "$sFINALCUT"
    #mv *SBNcovar*root Covar_407Series_FinalSelection/
    #./hive -o stack  -x $XML -s 4 -g 407  --cuts "$FINALCUT" --additional_tag="FinalSelection407Series" --systematics "flux" --pot 6.852e+20
    #/uboone/app/users/markrl/useful_scripts/pdfjam/pdfjam/bin/pdfjam stack/*pdf --landscape --nup 2x2 --outfile stack/RESULT_1g0p_stack_407series_Jammed.pdf

    rm datamc/*pdf
    ./hive -o datamc  -x $XML -s 4 -g 407  --cuts "$FINALCUT" --additional_tag="FinalSelection407Series" --systematics "flux"
    /uboone/app/users/markrl/useful_scripts/pdfjam/pdfjam/bin/pdfjam datamc/*FinalSelection407Series*pdf --landscape --nup 2x2 --outfile datamc/RESULT_1g0p_datamc_407series_Jammed.pdf



fi

