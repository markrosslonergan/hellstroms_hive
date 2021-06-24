#!/bin/bash

#Some book-keeping, as well as XML locations
TAG="June24_SignalBox_Script2_Prep_v1"
COV="/uboone/app/users/markrl/SBNfit_uBooNE/July2020_SL7/whipping_star/build/bin/FinalSignalBoxUnblinging_Jun2021/Covar/April2021_Full_v1_FITE_Frozen_Copy_FluxXSDetG4.SBNcovar.root"
XML="/uboone/app/users/markrl/SBNfit_uBooNE/July2020_SL7/whipping_star/build/bin/FinalSignalBoxUnblinging_Jun2021/XML/master_mc_final_selection_1g1p_1g0p_2g1p_2g0p_June2021_signal_box_v1.xml"
DATAXML="/uboone/app/users/markrl/SBNfit_uBooNE/July2020_SL7/whipping_star/build/bin/FinalSignalBoxUnblinging_Jun2021/XML/master_placeholder_data_final_selection_1g1p_1g0p_2g1p_2g0p_June2021_signal_box_v1.xml"

echo "Starting Script4, the Feldman-Cousins Approach. Takes one argument, if Data runs in data mode, otherwise defaults to Safe mode which only produces sensitivities/fake data."

MODE="$1"
RUND=false
if [ -z $1 ]; then
    echo "Mode is defaulting to Safe-Mode, i.e MC only"
    RUND=false
elif [ $MODE == "Data" ]; then
    echo "Mode is being set to Data-Mode"
    read -p "Are you sure? (yY/nN) " -n 1 -r
    echo    
    if [[ $REPLY =~ ^[Yy]$ ]]
    then
        RUND=true
    else
        exit
    fi
else
    echo "Mode is defaulting to Safe-Mode, i.e MC only"
    RUND=false
fi

#Starting to make necessary obejects
echo "Starting to make MC, SM and LEE SBNspec objects"
./sbnfit_make_spec -x $XML -t $TAG  &> LOG.make_spec_MC._$TAG &> LOG.make_spec.mc

cp $COV $TAG".SBNcovar.root"

#These two ARE ran over in SAFE mode
if (! $RUND); then
    echo "Starting script Feldman-Cousins Longrun and belt construction. Can take a while, don't run on the fly!"

    #./sbnfit_uboone_scaling_fc -t $TAG -x $XML -i NCDelta -g '1e-8 8.0 160' -n 5000  --mode feldman --cnp &>LOG.feldman.long
    #./sbnfit_uboone_scaling_fc -t $TAG -x $XML -i NCDelta -g '1e-8 8.0 160' -n 5000  --mode belt --cnp &> LOG.belt.long

    echo "Done safe Mode"
fi

if ($RUND); then
    echo "Starting script Feldman-Cousins Data run. Should be quick enough!"
    ./sbnfit_make_spec -x $DATAXML -t "Data_"$TAG &> LOG.make_spec.data
    ./sbnfit_uboone_scaling_fc -t $TAG -x $XML -i NCDelta -g '1e-8 8.0 160' -n 5000  --mode data --cnp -d "Data_"$TAG"_CV.SBNspec.root" &> LOG.FC.Data.run
fi





