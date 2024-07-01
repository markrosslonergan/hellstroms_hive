#!/bin/bash

#    source /grid/fermiapp/products/uboone/setup_uboone.sh

	export PS1="\[\033[36m\]\u\[\033[m\]@\[\033[32m\]\h[ub]:\[\033[33;1m\]\w\[\033[m\]\$ "
	source /cvmfs/uboone.opensciencegrid.org/products/setup_uboone_mcc9.sh
	setup uboonecode v09_87_00 -q e26:prof

    echo "Setting up ninja v1_8_4"
    setup ninja v1_8_2
#    echo "Setting up uboonecode v08_00_00_64"
#    setup uboonecode v08_00_00_64 -qe17:prof
    echo "Setting up cmake v3_14_3"
    setup cmake v3_14_3
#    echo "Setting up gcc v7_3_0"
#    setup gcc v7_3_0
    echo "Setting up GSL"
    setup gsl v2_4 -q prof
    echo "Setting up Eigen"
    setup eigen v3_3_4a;

 
#    echo -e "Setting up HIVEDIR environmental variable: "$PWD
	# HIVEDIR enviromental varaible is used to setup template xmls by bdt_covar class
#   export HIVEDIR=$PWD
#    echo "Done!."

    ## needed for using XrootD
    kx509
    voms-proxy-init -noregen -rfc -voms fermilab:/fermilab/uboone/Role=Analysis
