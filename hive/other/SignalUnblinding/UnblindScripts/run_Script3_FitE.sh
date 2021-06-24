#!/bin/bash

function EmptyFolder() 
{
   # remove existing file in the folder
   if [ ! -z "$(ls -A $PWD)" ]; then
     echo -e "Start fresh in the dir..\n"
     rm *
   fi
}

#Some book-keeping, as well as XML locations
TAG="June24_SignalBox_Script3"
COV="/uboone/app/users/markrl/SBNfit_uBooNE/July2020_SL7/whipping_star/build/bin/FinalSignalBoxUnblinging_Jun2021/Covar/April2021_Full_v1_FITE_Frozen_Copy_FluxXSDetG4.SBNcovar.root"
XML="/uboone/app/users/markrl/SBNfit_uBooNE/July2020_SL7/whipping_star/build/bin/FinalSignalBoxUnblinging_Jun2021/XML/master_mc_final_selection_1g1p_1g0p_2g1p_2g0p_June2021_signal_box_v1.xml"

## SAFE DATA
#DATAXML="/uboone/app/users/markrl/SBNfit_uBooNE/July2020_SL7/whipping_star/build/bin/FinalSignalBoxUnblinging_Jun2021/XML/master_placeholder_data_final_selection_1g1p_1g0p_2g1p_2g0p_June2021_signal_box_v1.xml"
#DATAXML="/uboone/app/users/markrl/SBNfit_uBooNE/July2020_SL7/whipping_star/build/bin/FinalSignalBoxUnblinging_Jun2021/XML/master_zero_data_final_selection_1g1p_1g0p_2g1p_2g0p_June2021_signal_box_v1.xml"

## REAL data will be here!
DATAXML="/uboone/app/users/markrl/SBNfit_uBooNE/July2020_SL7/whipping_star/build/bin/FinalSignalBoxUnblinging_Jun2021/XML/master_real_unblinded_data_final_selection_1g1p_1g0p_2g1p_2g0p_June2021_signal_box_v1.xml"

echo -e "\nStarting Script3. Perform Fit E, takes 1 argument - if Data runs in data mode, otherwise defaults to Safe mode which only produces sensitivities/fake data.\n"


MODE=${1:-Nope}
RUND=false
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


# directory to copy executables from, and local exetuable name
copy_dir="/uboone/app/users/gge/singlephoton/whipping_star/build/bin/"
fit_exe="sbnfit_singlephoton_unblinding"
constrain_exe="sbnfit_conditional_constraint_unblinding"


## Starting to make necessary obejects
mkdir -p "Script3"
cd "Script3"
cp $copy_dir"sbnfit_singlephoton" $fit_exe
cp $copy_dir"sbnfit_conditional_constraint" $constrain_exe

## Run over in conditional constraint
if (! $RUND); then
    echo -e "Run in sensitivity mode\n"
    mkdir -p "Sensitivity"
    cd "Sensitivity"
    EmptyFolder

    echo -e "Do the sensitivity fit and plot! [take ~1 min]\n"
    ../$fit_exe -x $XML -t $TAG"_MC_Sensitivity" -m "gen" &> gen_mc.log 
    ../$fit_exe -x $XML -t $TAG"_MC_Sensitivity" -m "fit" -c $COV &> sensitivity.log
    ../$fit_exe -x $XML -t $TAG"_MC_Sensitivity" -m "plot,sensitivity" &> plot_sensitivity.log
    
    # rename the delta chi distribution plot
    cp $TAG"_MC_Sensitivity_h_delta_chi.pdf" "OUTPUT_Sensitivity_delta_chi.pdf"
    echo -e "Result saved as OUTPUT_Sensitivity_delta_chi.pdf\n"
else
    echo -e "Run fit with real data [take ~2 min in total]\n"
    mkdir -p "RealData"
    cd "RealData"
    EmptyFolder

    echo -e "First, make Data and MC spectra\n"
    ../$fit_exe -x $DATAXML -t $TAG"_Data" -m "gen" &> gen_data.log
    ../$fit_exe -x $XML -t $TAG"_MC" -m "gen" &> gen_mc.log

    echo -e "Second, fit!\n"
    ../$fit_exe -x $XML -t $TAG"_MC" -m "fit" -d $TAG"_Data_CV.SBNspec.root" -c $COV &> fit.log
    ../$fit_exe -x $XML -t $TAG"_MC" -m "plot" &> plot.log
    cp $TAG"_MC_h_delta_chi.pdf" "OUTPUT_RealData_delta_chi.pdf"

    LineNumber=$(awk '/Best/{print NR; exit}' fit.log)
    ((LineNumber+=1))   # the line where best-fit is printed out
    LineContent=$(awk "NR==$LineNumber" fit.log)
    LineWords=($(echo $LineContent | tr ": " "\n"))
    LineLength=${#LineWords[@]}
    #for i in ${LineWords[@]};do
    #	echo $i
    #done
    FittingSuchannel=${LineWords[(($LineLength-2))]}
    BFvalue=${LineWords[(($LineLength-1))]}
    #echo "Third, grab fitting subchannel and BF values: "$FittingSuchannel" "$BFvalue

    echo -e "Now, generate constrain plots at BF point\n"
    ../$constrain_exe -x $XML -t "DataOverlaid" -s $TAG"_MC_CV.SBNspec.root" -d $TAG"_Data_CV.SBNspec.root" -n 2 -c $COV -o -b $FittingSuchannel,$BFvalue &> constrain.log
    
    #pdfjam
    echo -e "Result saved as:\n\tOUTPUT_RealData_delta_chi.pdf\n\tOUTPUT_DataOverlaid_BF_combined_conditional_constraint.pdf\n"
    /uboone/app/users/markrl/useful_scripts/pdfjam/pdfjam/bin/pdfjam DataOverlaid_BF_1g1p_* DataOverlaid_BF_1g0p_* DataOverlaid_Wchi_BF_1g1p_* DataOverlaid_Wchi_BF_1g0p_* --landscape --nup 2x1 --outfile OUTPUT_DataOverlaid_BF_combined_conditional_constraint.pdf
    #/uboone/app/users/markrl/useful_scripts/pdfjam/pdfjam/bin/pdfjam DataOverlaid_BF_1g1p_* DataOverlaid_BF_1g0p_* --landscape --nup 2x1 --outfile OUTPUT_DataOverlaid_BF_combined_conditional_constraint.pdf &> /dev/null
    #/uboone/app/users/markrl/useful_scripts/pdfjam/pdfjam/bin/pdfjam DataOverlaid_Wchi_BF_1g1p_* DataOverlaid_Wchi_BF_1g0p_* --landscape --nup 2x1 --outfile OUTPUT_DataOverlaid_Wchi_BF_combined_conditional_constraint.pdf &> /dev/null

fi
