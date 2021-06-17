#!/bin/bash

echo "Running gLEE 1g0p Final Selection on file "
echo $1
echo "With Tag"
echo $2
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
sed "s@FULLPATH@$1@g"  /uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/other/SignalUnblinding/1g0p/1g0p_signalregion_external_template_v1.xml > "1g0p_signalregion_external_SED_"$2"_v1.xml"
sed -i "s@TAGTAG@$2@g" "1g0p_signalregion_external_SED_"$2"_v1.xml"

echo "Copying BDT model files"
cp /uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/mod/1g_mods/1g0p*mod .
echo "OK, beginning to run!"
/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/build/bin/hive_nearside_version -x /uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g0p_xmls/hive4.0/1g0p_EmeraldWorldOrder_CombinedRuns13_v4_Plot.xml -s 4 --extapp "1g0p_signalregion_external_SED_"$2"_v1.xml" --cuts "(1g0pMar2020COSMIC_mva >= 0.988209 && 1g0pMar2020BNB_mva >= 0.892728 && 1g0pMar2020NCPi0_mva >= 0.428825 ) &> "gLEE_signalregion_"$2".log"

echo "Done"
