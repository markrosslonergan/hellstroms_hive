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

#run1 only xml, use for all data-MC plots
XML="/uboone/app/users/gge/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/1g1p_EmeraldWorldOrder_Run3_v4_Plot.xml"

#combined runs xml, here used only for final selection plots
XML13="/uboone/app/users/ksutton/hellstroms_hive_2.5_sl7/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/1g1p_EmeraldWorldOrder_CombinedRuns13_v4_Plot.xml"

# hive executable
EXE="/uboone/app/users/gge/hellstroms_hive/hive/build/bin/hive"

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
$EXE  --makefluxcovar $FXXML1 -x $XML -s 1 -g330
$EXE -o datamc  -x $XML -s 0 -g330 --systematics "flux_fracfixed"
$EXE -o datamc  -x $XML -s 1 -g330 --systematics "flux_fracfixed"

#stage 1
$EXE  --makefluxcovar $FXXML1 -x $XML -s 1 -g331
$EXE -o datamc  -x $XML -s 1 -g331 --systematics "flux_fracfixed"

#Single BDT Cuts
$EXE  --makefluxcovar $FXXML1 -x $XML -s 1 -g335
$EXE -o datamc  -x $XML -s 1 -g335 --systematics "flux_fracfixed"

$EXE  --makefluxcovar $FXXML1 -x $XML -s 1 -g336
$EXE -o datamc  -x $XML -s 1 -g336 --systematics "flux_fracfixed"

$EXE  --makefluxcovar $FXXML1 -x $XML -s 1 -g337
$EXE -o datamc  -x $XML -s 1 -g337 --systematics "flux_fracfixed"

$EXE  --makefluxcovar $FXXML1 -x $XML -s 1 -g338
$EXE -o datamc  -x $XML -s 1 -g338 --systematics "flux_fracfixed"

$EXE  --makefluxcovar $FXXML1 -x $XML -s 1 -g339
$EXE -o datamc  -x $XML -s 1 -g339 --systematics "flux_fracfixed"

#Efficiency plots - these are currently still broken
#$EXE -o eff -n2 -f18 -s1 -x $XML -s 1 -g335 #generate eff plots for ncdelta signal def file
#$EXE -o eff -n2 -s1 -x $XML -s 1 -g335 

#$EXE -o eff -n2 -s1 -x $XML -s 1 -g336

#$EXE -o eff -n2 -s1  -x $XML -s 1 -g337

#$EXE -o eff -n2 -s1  -x $XML -s 1 -g338

#$EXE -o eff -n2 -s1  -x $XML -s 1 -g339


#BDT Scores
$EXE  --makefluxcovar $FXXML1 -x $XML -s 1 -g6
$EXE -o datamc  -x $XML -s 1 -g6 --systematics "flux_fracfixed"

#stage 6
$EXE  --makefluxcovar $FXXML6 -x $XML -s 6 -g332
$EXE -o datamc  -x $XML -s 6 -g332 --systematics "flux_fracfixed"

#$EXE  --makefluxcovar $FXXML6 -x $XML13 -s 6 -g332 #if you're runnning something from the combined xml you always need to regenerate covar matrix
#$EXE -o stack -x $XML13 -s 6 -g332 --systematics "flux_fracfixed" --pot 6.91e20

