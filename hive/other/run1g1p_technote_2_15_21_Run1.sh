#!/bin/bash


####################### 1g1p Plotting Macro ####################
#this script generates all the 1g1p plots in chapter 5 of the internal note using run1 data and MC
#the final stack plots scaled to 6.9e20POT use MC from combined runs
####################### Kathryn 2/15/21 #########################
################################################################


#flux and xsec xml for final stage 6
FXXML6="/uboone/app/users/ksutton/hellstroms_hive_2.5_sl7/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/template_1g1p_stage_1_Jan2021.xml"

#flux and xsec xml for precuts stage 1 (use for all stages except final selection)
FXXML1="/uboone/app/users/ksutton/hellstroms_hive_2.5_sl7/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/template_1g1p_stage_1_Jan2021.xml"

#run1 only xml, used for all data-MC plots in Chapter 5
XML="/uboone/app/users/ksutton/hellstroms_hive_2.5_sl7/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/1g1p_EmeraldWorldOrder_Run1_v4_Plot.xml"

#run1 only xml, used to make all the training vars plots for the appendix
XMLTRAINVARS="/uboone/app/users/ksutton/hellstroms_hive_2.5_sl7/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/1g1p_EmeraldWorldOrder_Run1_v4_PlotTrainVars.xml"

#has the signal training definition, only for making the eff plots 
XMLEFF="/uboone/app/users/ksutton/hellstroms_hive_2.5_sl7/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/1g1p_EmeraldWorldOrder_Run1_v4_EffPlots.xml"

#combined runs xml, here used only for final selection plots
XML13="/uboone/app/users/ksutton/hellstroms_hive_2.5_sl7/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/1g1p_EmeraldWorldOrder_CombinedRuns13_v4_Plot.xml"

##this cut defines the ncdelta signal
SIGNAL="(TMath::Min(reco_track_end_dist_to_SCB,reco_track_start_dist_to_SCB)>2 && mctruth_is_delta_radiative==1 && mctruth_nu_vertex_x > 2.0 && mctruth_nu_vertex_x < 253.35 && mctruth_nu_vertex_y > -113.5 && mctruth_nu_vertex_y < 113.5 && mctruth_nu_vertex_z > 2.0 && mctruth_nu_vertex_z < 1033.8 && mctruth_cc_or_nc == 1  && mctruth_num_exiting_pi0 == 0  && mctruth_exiting_photon_energy > 0.02  && Sum\$(mctruth_exiting_proton_energy-0.93827 > 0.02 )==1 )"


##these cuts are for far sidebands#
# Full FS Cuts
sFULLFS="(simple_1g1pMar2020_v4COSMIC_mva >=0.9 && simple_1g1pMar2020_v4Nue_mva >= 0.3 && (simple_1g1pMar2020_v4BNB_mva < 0.96273 || simple_1g1pMar2020_v4NCPi0_mva < 0.467404))"
FULLFS="(1g1pMar2020_v4COSMIC_mva >=0.9 &&1g1pMar2020_v4Nue_mva>= 0.3 && (1g1pMar2020_v4BNB_mva < 0.96273 || 1g1pMar2020_v4NCPi0_mva < 0.467404))"

# NCPi0 Cuts
sNCPIFS="(simple_1g1pMar2020_v4COSMIC_mva >=0.9 && simple_1g1pMar2020_v4Nue_mva >= 0.3 && (simple_1g1pMar2020_v4BNB_mva >= 0.96273 && simple_1g1pMar2020_v4NCPi0_mva < 0.467404))"
NCPIFS="(1g1pMar2020_v4COSMIC_mva >=0.9 &&1g1pMar2020_v4Nue_mva>= 0.3 && (1g1pMar2020_v4BNB_mva >= 0.96273 && 1g1pMar2020_v4NCPi0_mva < 0.467404))"

# BNB Cuts
sBNBFS="(simple_1g1pMar2020_v4COSMIC_mva >=0.9 && simple_1g1pMar2020_v4Nue_mva >= 0.3 && (simple_1g1pMar2020_v4BNB_mva < 0.96273 && simple_1g1pMar2020_v4NCPi0_mva >= 0.467404))"
BNBFS="(1g1pMar2020_v4COSMIC_mva >=0.9 && 1g1pMar2020_v4Nue_mva>= 0.3 && (1g1pMar2020_v4BNB_mva < 0.96273 && 1g1pMar2020_v4NCPi0_mva >= 0.467404))"

NUM=2212

## for each stage you generate the covariance matrix for each plot (--makefluxcovar), which needs to be done every time you rebin
## if the covariance matrix exists you add --systematics "flux_fracfixed" to put the errors on the plots
## in order to run a specific group of plots you can comment sections in and out

#stage0 + 1
#./../../bin/hive  --makefluxcovar $FXXML1 -x $XML -s 1 -g330
#./../../bin/hive -o datamc  -x $XML -s 0 -g330 --systematics "flux_fracfixed"
#./../../bin/hive -o datamc  -x $XML -s 1 -g330 --systematics "flux_fracfixed"

#stage 1
#./../../bin/hive  --makefluxcovar $FXXML1 -x $XML -s 1 -g331
#./../../bin/hive -o datamc  -x $XML -s 1 -g331 --systematics "flux_fracfixed"

#Single BDT Cuts
#./../../bin/hive  --makefluxcovar $FXXML1 -x $XML -s 1 -g335
#./../../bin/hive -o datamc  -x $XML -s 1 -g335 --systematics "flux_fracfixed"

#./../../bin/hive  --makefluxcovar $FXXML1 -x $XML -s 1 -g336
#./../../bin/hive -o datamc  -x $XML -s 1 -g336 --systematics "flux_fracfixed"

#./../../bin/hive  --makefluxcovar $FXXML1 -x $XML -s 1 -g337
#./../../bin/hive -o datamc  -x $XML -s 1 -g337 --systematics "flux_fracfixed"

#./../../bin/hive  --makefluxcovar $FXXML1 -x $XML -s 1 -g338
#./../../bin/hive -o datamc  -x $XML -s 1 -g338 --systematics "flux_fracfixed"

#./../../bin/hive  --makefluxcovar $FXXML1 -x $XML -s 1 -g339
#./../../bin/hive -o datamc  -x $XML -s 1 -g339 --systematics "flux_fracfixed"

#Efficiency plots - these are currently still broken
#./../../bin/hive -o eff -n2 -s1 -x $XMLEFF  -g335 
#./../../bin/hive -o eff -n2 -f18 -s1 -x $XML -g335  


#./../../bin/hive -o eff -n2 -s1 -x $XMLEFF  -g336

#./../../bin/hive -o eff -n2 -s1  -x $XMLEFF  -g337

#./../../bin/hive -o eff -n2 -s1  -x $XMLEFF  -g338

#./../../bin/hive -o eff -n2 -s1  -x $XMLEFF  -g339


#BDT Scores
#./../../bin/hive  --makefluxcovar $FXXML1 -x $XML -s 1 -g6
#./../../bin/hive -o datamc  -x $XML -s 1 -g6 --systematics "flux_fracfixed"

#stage 6
#./../../bin/hive  --makefluxcovar $FXXML6 -x $XML -s 6 -g332
#./../../bin/hive -o datamc  -x $XML -s 6 -g332 --systematics "flux_fracfixed"

#./../../bin/hive  --makefluxcovar $FXXML6 -x $XML13 -s 6 -g332 #if you're runnning something from the combined xml you always need to regenerate covar matrix
#./../../bin/hive -o stack -x $XML13 -s 6 -g332 --systematics "flux_fracfixed" --pot 6.91e20


#all training variables for appendix
./../../bin/hive --makefluxcovar $FXXML1 -x $XMLTRAINVARS -s 1 -g32 
./../../bin/hive -o datamc  -x $XMLTRAINVARS -s 1 -g32 --systematics "flux_fracfixed"
