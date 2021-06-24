#!/bin/bash

#Some book-keeping, as well as XML locations
TAG="June24_SignalBox_Script2_v1"
COV="/uboone/app/users/markrl/SBNfit_uBooNE/July2020_SL7/whipping_star/build/bin/FinalSignalBoxUnblinging_Jun2021/Covar/April2021_Full_v1_FITE_Frozen_Copy_FluxXSDetG4.SBNcovar.root"
XML="/uboone/app/users/markrl/SBNfit_uBooNE/July2020_SL7/whipping_star/build/bin/FinalSignalBoxUnblinging_Jun2021/XML/master_mc_final_selection_1g1p_1g0p_2g1p_2g0p_June2021_signal_box_v1.xml"
#DATAXML="/uboone/app/users/markrl/SBNfit_uBooNE/July2020_SL7/whipping_star/build/bin/FinalSignalBoxUnblinging_Jun2021/XML/master_zero_data_final_selection_1g1p_1g0p_2g1p_2g0p_June2021_signal_box_v1.xml"
DATAXML="/uboone/app/users/markrl/SBNfit_uBooNE/July2020_SL7/whipping_star/build/bin/FinalSignalBoxUnblinging_Jun2021/XML/master_real_unblinded_data_final_selection_1g1p_1g0p_2g1p_2g0p_June2021_signal_box_v1.xml"

echo "Starting Script2, the Two-Hypotheis Tests. Takes one argument, if Data runs in data mode, otherwise defaults to Safe mode which only produces sensitivities/fake data."

cp /uboone/app/users/markrl/SBNfit_uBooNE/July2020_SL7/whipping_star/build/bin/sbnfit_make_spec .
cp /uboone/app/users/markrl/SBNfit_uBooNE/July2020_SL7/whipping_star/build/bin/sbnfit_scale_spec .
cp /uboone/app/users/markrl/SBNfit_uBooNE/July2020_SL7/whipping_star/build/bin/sbnfit_lee* .

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

#These two ARE ran over in SAFE mode
if (! $RUND); then

    #Starting to make necessary obejects
    echo "Starting to make MC, SM and LEE SBNspec objects"
    ./sbnfit_make_spec -x $XML -t $TAG  &> LOG.make_spec_MC._$TAG
    ./sbnfit_scale_spec -x $XML -t $TAG"_SM" -v 1.0 -s "NCDelta" -i $TAG"_CV.SBNspec.root" &> LOG.scale_spec_1_$TAG 
    ./sbnfit_scale_spec -x $XML -t $TAG"_LEE" -v 3.0 -s "NCDelta" -i $TAG"_CV.SBNspec.root" &> LOG.scale_spec_2_$TAG 

    echo "Starting script 2 two hypo test in safe mode, approx 5 mins"
    ./sbnfit_lee_frequentist_study --xml $XML -t $TAG"_MCONLY_SM_V_LEE"  -s $TAG"_LEE.SBNspec.root" -b $TAG"_SM.SBNspec.root" -e  0.00000001 -n 50000 -c $COV  -l "SM (x1)|LEE (x3)"   --reverse  &> LOG_MC.SM_V_LEE_$TAG
    ./sbnfit_lee_frequentist_study --xml $XML -t $TAG"_MCONLY_LEE_V_SM"  -b $TAG"_LEE.SBNspec.root" -s $TAG"_SM.SBNspec.root" -e  0.00000001 -n 50000 -c $COV  -l "LEE (x3)|SM (x1)"   &> LOG_MC.LEE_V_SM_$TAG

    /uboone/app/users/markrl/useful_scripts/pdfjam/pdfjam/bin/pdfjam "SBNfit_Cls_"$TAG"_MCONLY_SM_V_LEECNP_Chi.pdf" "SBNfit_Cls_"$TAG"_MCONLY_LEE_V_SMCNP_Chi.pdf" --landscape --nup 2x1 --outfile "RESULT_"$TAG_"script2_2Hypo_SENSITIVITYONLY.pdf"
fi


############## This only runs for real data!
if ($RUND); then
    echo "Starting script 2 two hypo test in Data mode, approx 3 mins"
    rm *Data*root;
    echo "Making input XML into SBNspec for data"
    ./sbnfit_make_spec -x $DATAXML -t "Data_"$TAG &> LOG_make_spec_Data.$TAG

    echo "Starting on Frequentist Tests"
    ./sbnfit_lee_frequentist_study --xml $XML -t $TAG"_DATAPLOT_SM_V_LEE"  -s $TAG"_LEE.SBNspec.root" -b $TAG"_SM.SBNspec.root" -e  0.00000001 -n 50000 -c $COV  -l "SM (x1)|LEE (x3)" --reverse -d "Data_"$TAG"_CV.SBNspec.root" &> LOG_Data.SM_V_LEE_$TAG 
    ./sbnfit_lee_frequentist_study --xml $XML -t $TAG"_DATAPLOT_LEE_V_SM"  -s $TAG"_SM.SBNspec.root" -b $TAG"_LEE.SBNspec.root" -e  0.00000001 -n 50000 -c $COV  -l "LEE (x3)|SM (x1)" -d "Data_"$TAG"_CV.SBNspec.root" &> LOG_Data.LEE_V_SM_$TAG  

    echo "Done, Merging resulting PDF's"
    /uboone/app/users/markrl/useful_scripts/pdfjam/pdfjam/bin/pdfjam "SBNfit_Cls_"$TAG"_DATAPLOT_SM_V_LEECNP_Chi.pdf" "SBNfit_Cls_"$TAG"_DATAPLOT_LEE_V_SMCNP_Chi.pdf" --landscape --nup 2x1 --outfile "RESULT_"$TAG_"script2_2Hypo_UNBLINDEDDATA.pdf"

    echo "Fully finished!"
fi
