#!/bin/bash

#Some book-keeping, as well as XML locations
TAG="June24_SignalBox_Script1"
COV="/uboone/app/users/markrl/SBNfit_uBooNE/July2020_SL7/whipping_star/build/bin/FinalSignalBoxUnblinging_Jun2021/Covar/April2021_Full_v1_FITE_Frozen_Copy_FluxXSDetG4.SBNcovar.root"
XML="/uboone/app/users/markrl/SBNfit_uBooNE/July2020_SL7/whipping_star/build/bin/FinalSignalBoxUnblinging_Jun2021/XML/master_mc_final_selection_1g1p_1g0p_2g1p_2g0p_June2021_signal_box_v1.xml"

## SAFE DATA
#DATAXML="/uboone/app/users/markrl/SBNfit_uBooNE/July2020_SL7/whipping_star/build/bin/FinalSignalBoxUnblinging_Jun2021/XML/master_placeholder_data_final_selection_1g1p_1g0p_2g1p_2g0p_June2021_signal_box_v1.xml"
#DATAXML="/uboone/app/users/markrl/SBNfit_uBooNE/July2020_SL7/whipping_star/build/bin/FinalSignalBoxUnblinging_Jun2021/XML/master_zero_data_final_selection_1g1p_1g0p_2g1p_2g0p_June2021_signal_box_v1.xml"

## Real data will be here
DATAXML="/uboone/app/users/markrl/SBNfit_uBooNE/July2020_SL7/whipping_star/build/bin/FinalSignalBoxUnblinging_Jun2021/XML/master_real_unblinded_data_final_selection_1g1p_1g0p_2g1p_2g0p_June2021_signal_box_v1.xml"


echo -e "\nStarting Script1, the Conditional Constraint on CV prediction. Takes two optional arguments"
echo "First argument:  optional, if Data runs in data mode, otherwise defaults to Safe mode which only produces sensitivities/fake data."
echo -e "Second argument: optional, if Generated, skips spectra generation for data and MC prediction, otherwise will generate these spectra\n" 

MODE=${1:-Nope}
MCGeneration=${2:-Nope}
RUND=false
MC_GENERATED=false
if [ $MODE == "Data" ]; then
    echo "Mode is being set to Data-Mode"
    read -p "Are you sure? (yY/nN) " -n 1 -r
    echo -e "\n"    
    if [[ $REPLY =~ ^[Yy]$ ]]
    then
        RUND=true
    else
        exit
    fi
else
    echo -e "Mode is defaulting to Safe-Mode, i.e MC only\n"
    RUND=false
fi

if [ $MCGeneration == "Generated" ]; then
    MC_GENERATED=true
fi


# executable, and corresponding directory to copy from
copy_dir="/uboone/app/users/gge/singlephoton/whipping_star/build/bin/"
fit_exe="sbnfit_singlephoton_unblinding"
constrain_exe="sbnfit_conditional_constraint_unblinding"


## Starting to make necessary obejects
mkdir -p "Script1"
cd "Script1"
cp $copy_dir"sbnfit_singlephoton" $fit_exe
cp $copy_dir"sbnfit_conditional_constraint" $constrain_exe 

if (! $MC_GENERATED); then
   echo -e "Starting to make MC spectra [take ~1 min]\n"
   ./$fit_exe -x $XML -t $TAG"_MC" -m "gen" &> gen_mc.log
fi

## Run conditional constraint
if (! $RUND); then
    echo -e "Starting to generate 2g-constrained 1g prediction in SAFE mode: No 1g-data overlaid [take a few sec]\n"
    ./$fit_exe -x $DATAXML -t $TAG"_Data_SAFE" -m "gen" &> gen_data_safe.log
    ./$constrain_exe -x $XML -t "WoutData" -s $TAG"_MC_CV.SBNspec.root" -d $TAG"_Data_SAFE_CV.SBNspec.root" -n 2 -c $COV &> constrain_safe.log
 
    #pdfjam -- needs to be run on gpvm04 only
    echo -e "Result saved as OUTPUT_WoutData_CV_combined_conditional_constraint.pdf\n"
    /uboone/app/users/markrl/useful_scripts/pdfjam/pdfjam/bin/pdfjam WoutData_CV_1g1p_* WoutData_CV_1g0p_* --landscape --nup 2x1 --outfile OUTPUT_WoutData_CV_combined_conditional_constraint.pdf &> /dev/null
    
else
    echo -e "Starting to generate 2g-constrained 1g prediction, with 1g-data overlaid [take a few sec]\n"
    ./$fit_exe -x $DATAXML -t $TAG"_Data" -m "gen" &> gen_data.log
    ./$constrain_exe -x $XML -t "DataOverlaid" -s $TAG"_MC_CV.SBNspec.root" -d $TAG"_Data_CV.SBNspec.root" -n 2 -c $COV -o &> constrain.log

    #pdfjam
    echo -e "Result saved as:\n\tOUTPUT_DataOverlaid_CV_combined_conditional_constraint.pdf\n"
    /uboone/app/users/markrl/useful_scripts/pdfjam/pdfjam/bin/pdfjam DataOverlaid_CV_1g1p_* DataOverlaid_CV_1g0p_* DataOverlaid_Wchi_CV_1g1p_* DataOverlaid_Wchi_CV_1g0p_* --landscape --nup 2x1 --outfile OUTPUT_DataOverlaid_CV_combined_conditional_constraint.pdf 
    #/uboone/app/users/markrl/useful_scripts/pdfjam/pdfjam/bin/pdfjam DataOverlaid_CV_1g1p_* DataOverlaid_CV_1g0p_* --landscape --nup 2x1 --outfile OUTPUT_DataOverlaid_CV_combined_conditional_constraint.pdf &> /dev/null
    #/uboone/app/users/markrl/useful_scripts/pdfjam/pdfjam/bin/pdfjam DataOverlaid_Wchi_CV_1g1p_* DataOverlaid_Wchi_CV_1g0p_* --landscape --nup 2x1 --outfile OUTPUT_DataOverlaid_Wchi_CV_combined_conditional_constraint.pdf &> /dev/null

fi
