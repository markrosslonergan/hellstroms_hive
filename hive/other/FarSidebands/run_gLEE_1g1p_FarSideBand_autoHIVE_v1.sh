#!/bin/bash

echo "Running gLEE 1g1p far sideband on file "
echo $1

echo "#################### Setting up enviroment ##################"

source /grid/fermiapp/products/uboone/setup_uboone.sh
echo "Setting up ninja v1_8_4"
setup ninja v1_8_2
echo "Setting up uboonecode v06_26_01_13"
setup uboonecode v06_26_01_13 -qe10:prof
echo "Setting up gallery v1_03_08"
setup gallery v1_03_08 -qe10:nu:prof
echo "Setting up cmake v3_7_1"
setup cmake v3_7_1
echo "Setting up gcc v6_4_0"
setup gcc v6_4_0
echo "Setting up root v6_12_04e"
setup root v6_12_04e -q e15:prof
echo "Setting up GSL"
setup gsl v2_4 -q prof
echo "Setting up Eigen"
setup eigen v3_3_4a;
echo "Done with setup"

echo "Making XML"
sed "s@FULLPATH@$1@g"  /uboone/app/users/markrl/Hive_v3.0/hellstroms_hive/hive/other/FarSidebands/1g1p_farsideband_external_template_v1.xml > 1g1p_farsideband_external_SED_v1.xml

echo "Copying BDT model files"
cp /uboone/app/users/markrl/Hive_v3.0/hellstroms_hive/hive/build/bin/May2020_1g1p_v1/*mod .
echo "OK, beginning to run!"
/uboone/app/users/markrl/Hive_v3.0/hellstroms_hive/hive/build/bin/hive_farside_version -x /uboone/app/users/markrl/Hive_v3.0/hellstroms_hive/hive/xml/1g1p_xmls/hive2.5/1g1p_CinnabarWorldOrder_v33.3_15_04_30_Combined_Run123_v1_updatedBDTfiles.xml -s 6 --extapp 1g1p_farsideband_external_SED_v1.xml --cuts "(1g1pMar2020_v4Nue_mva >= 0.3 && (1g1pMar2020_v4BNB_mva < 0.96273 || 1g1pMar2020_v4NCPi0_mva < 0.467404))"   &> gLEE_farsideband.log

echo "Done"
