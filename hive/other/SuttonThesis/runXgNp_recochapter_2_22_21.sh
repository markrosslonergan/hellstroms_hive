#!/bin/bash


####################### XgNp Plotting Macro ####################
#
#this script generates all the XgNp plots in the reco chapter of my thesis
#
#for all showers there is a 30MeV energy cut
#
#for some variables we require at least one track associated to the interaction vertex
#
####################### Kathryn 2/22/21 #########################
################################################################


#flux and xsec xml for final stage 6
FXXML6="/uboone/app/users/ksutton/hellstroms_hive_2.5_sl7/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/template_1g1p_stage_6_Run13combined_NoLEE_Dec2020.xml"

#flux and xsec xml for precuts stage 1 (use for all stages except final selection)
FXXML1="/uboone/app/users/ksutton/hellstroms_hive_2.5_sl7/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/template_1g1p_stage_1_Run13combined_NoLEE_Dec2020.xml"

#combined runs xml for atleast 1 shower 1 track
XMLNN="/uboone/app/users/ksutton/hellstroms_hive_2.5_sl7/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/NgNp_EmeraldWorldOrder_CombinedRuns13_v4_Plot.xml"

#combined runs xml for atleast 1 shower, any track
XMLNX="/uboone/app/users/ksutton/hellstroms_hive_2.5_sl7/hellstroms_hive/hive/xml/1g1p_xmls/hive4.0/NgXp_EmeraldWorldOrder_CombinedRuns13_v4_Plot.xml"


## for each stage you generate the covariance matrix for each plot (--makefluxcovar), which needs to be done every time you rebin
## if the covariance matrix exists you add --systematics "flux_fracfixed" to put the errors on the plots
## in order to run a specific group of plots you can comment sections in and out

#stage1 shower plots reco chapter with at least one track
#rm *entrylist*
#./../../bin/hive -o datamc  -x $XMLNN -s 1 -g67 

#stage1 shower plots reco chapter with no track requirement
#rm *entrylist*
./../../bin/hive -o datamc  -x $XMLNX -s 1 -g68


