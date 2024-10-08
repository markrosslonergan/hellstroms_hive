#!/bin/bash

source /cvmfs/uboone.opensciencegrid.org/products/setup_uboone_mcc9.sh

echo "Setting up ninja v1_8_4"
setup ninja v1_8_2
echo "Setting up uboonecode v08_00_00_64"
setup uboonecode v08_00_00_64 -qe17:prof
echo "Setting up cmake v3_14_3"
setup cmake v3_14_3
echo "Setting up gcc v7_3_0"
setup gcc v7_3_0
setup root v6_12_04e -q e15:prof
echo "Setting up GSL"
setup gsl v2_4 -q prof
echo "Setting up Eigen"
setup eigen v3_3_4a;
setup boost v1_66_0 -q e15:prof

 
# HIVEDIR enviromental varaible is used to setup template xmls by bdt_covar class
export HIVEDIR=$PWD
echo "Done!."

## needed for using XrootD
kx509
voms-proxy-init -noregen -rfc -voms fermilab:/fermilab/uboone/Role=Analysis

#In order to run xrootd: pnfsToXRootD
#kinit markross
#kx509
#voms-proxy-init -noregen -rfc -voms fermilab:/fermilab/uboone/Role=Analysis
