#!/bin/bash

####################### 1g0p Plotting Macro ####################
#this script generates all the 1g0p plots in chapter 5 of the internal note using run1 data and MC
#the final stack plots scaled to 6.9e20POT use MC from combined runs
####################### Kathryn 2/15/21 #########################
################################################################

FXXML4="/uboone/app/users/ksutton/hellstroms_hive_2.5_sl7/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/template_1g0p_stage_1_Jan2021.xml"

FXXML1="/uboone/app/users/ksutton/hellstroms_hive_2.5_sl7/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/template_1g0p_stage_1_Jan2021.xml"

XML="/uboone/app/users/gge/hellstroms_hive/hive/xml/1g0p_xmls/hive4.0/1g0p_EmeraldWorldOrder_Run3_v4_Plot.xml"

XML13="/uboone/app/users/ksutton/hellstroms_hive_2.5_sl7/hellstroms_hive/hive/xml/1g0p_xmls/hive4.0/1g0p_EmeraldWorldOrder_CombinedRuns13_v4_Plot.xml"

EXE="/uboone/app/users/gge/hellstroms_hive/hive/build/bin/hive"

#stage0 + 1
$EXE --makefluxcovar $FXXML1 -x $XML -s 1 -g8
$EXE -o datamc  -x $XML -s 0 -g8 --systematics "flux_fracfixed"
$EXE -o datamc  -x $XML -s 1 -g8 --systematics "flux_fracfixed"

#stage 1
$EXE  --makefluxcovar $FXXML1 -x $XML -s 1 -g12
$EXE -o datamc  -x $XML -s 1 -g12 --systematics "flux_fracfixed"

#Single BDT Cuts
$EXE  --makefluxcovar $FXXML1 -x $XML -s 1 -g13
$EXE -o datamc  -x $XML -s 1 -g13 --systematics "flux_fracfixed"

#BDT Scores
$EXE  --makefluxcovar $FXXML1 -x $XML -s 1 -g6
$EXE -o datamc  -x $XML -s 1 -g6 --systematics "flux_fracfixed"

#stage 4 datamc + stack
$EXE --makefluxcovar $FXXML4 -x $XML -s 4 -g11
$EXE -o datamc  -x $XML -s 4 -g11 --systematics "flux_fracfixed"

#$EXE --makefluxcovar $FXXML4 -x $XML13 -s 4 -g11
#$EXE -o stack -x $XML13 -s 4 -g11 --systematics "flux_fracfixed" --pot 6.91e20

