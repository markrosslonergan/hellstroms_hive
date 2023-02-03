#!/bin/bash

    source /grid/fermiapp/products/uboone/setup_uboone.sh
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
	# HIVEDIR enviromental varaible is used to setup template xmls by bdt_covar class
        export HIVEDIR=$PWD
        echo "Done!."

    ## needed for using XrootD
	kx509
	voms-proxy-init -noregen -rfc -voms fermilab:/fermilab/uboone/Role=Analysis

