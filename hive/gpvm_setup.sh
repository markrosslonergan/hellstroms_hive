#!/bin/bash

    source /grid/fermiapp/products/uboone/setup_uboone.sh
<<<<<<< HEAD
	echo "Setting up ninja v1_8_4"
	setup ninja v1_8_2
	echo "Setting up uboonecode v06_26_01_13"
	setup uboonecode v06_26_01_13 -qe10:prof
	echo "Setting up gallery v1_03_08"
	setup gallery v1_03_08 -qe10:nu:prof
	echo "Setting up cmake v3_17_1"
	setup cmake v3_17_1
	echo "Setting up gcc v6_4_0"
	setup gcc v6_4_0
	echo "Setting up root v6_12_04e"
	setup root v6_12_04e -q e15:prof
#	setup root v6_16_00 -q e17:prof
    	echo -e "Setting up HIVEDIR environmental variable: "$PWD
=======
    echo "Setting up ninja v1_8_4"
    setup ninja v1_8_2
    echo "Setting up uboonecode v08_00_00_64"
    setup uboonecode v08_00_00_64 -qe17:prof
    echo "Setting up cmake v3_14_3"
    setup cmake v3_14_3
    echo "Setting up gcc v7_3_0"
    setup gcc v7_3_0
    echo "Setting up GSL"
    setup gsl v2_4 -q prof
    echo "Setting up Eigen"
    setup eigen v3_3_4a;

 
    echo -e "Setting up HIVEDIR environmental variable: "$PWD
>>>>>>> origin/feature/markross_v08_00_00_64_updatefix
	# HIVEDIR enviromental varaible is used to setup template xmls by bdt_covar class
    export HIVEDIR=$PWD
    echo "Done!."

    ## needed for using XrootD
<<<<<<< HEAD
	kx509
	voms-proxy-init -noregen -rfc -voms fermilab:/fermilab/uboone/Role=Analysis

=======
    kx509
    voms-proxy-init -noregen -rfc -voms fermilab:/fermilab/uboone/Role=Analysis
>>>>>>> origin/feature/markross_v08_00_00_64_updatefix
