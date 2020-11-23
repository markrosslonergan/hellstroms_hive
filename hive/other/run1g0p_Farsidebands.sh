#!/bin/bash

FXXML0p="/uboone/app/users/ksutton/hellstroms_hive_2.5_sl7/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/template_1g0p_stage_1_Run13combined_NoLEE.xml"
XML="/uboone/app/users/ksutton/hellstroms_hive_2.5_sl7/hellstroms_hive/hive/xml/1g0p_xmls/hive2.5/internalnote_9_23_20/1g0p_CeriseWorldOrder_FarSideband_CombinedRuns13_FullFS_v2.xml"
XML2="/uboone/app/users/ksutton/hellstroms_hive_2.5_sl7/hellstroms_hive/hive/xml/1g0p_xmls/hive2.5/internalnote_9_23_20/1g0p_CeriseWorldOrder_FarSideband_CombinedRuns13_BNBFS_v2.xml"
#XML2="1g1p_CeriseWorldOrder_FarSideband_Run2.xml"
#XML3="1g1p_CeriseWorldOrder_FarSideband_Run3.xml"

# Full FS Cuts
#sFULLFS="(simple_1g1pMar2020_v4Nue_mva >= 0.3 && (simple_1g1pMar2020_v4BNB_mva < 0.96273 || simple_1g1pMar2020_v4NCPi0_mva < 0.467404))"
#FULLFS="(1g1pMar2020_v4Nue_mva>= 0.3 && (1g1pMar2020_v4BNB_mva < 0.96273 || 1g1pMar2020_v4NCPi0_mva < 0.467404))"
sFULLFS="(1g0pMar2020COSMIC_mva >=0.9 && ((simple_1g0pMar2020NCPi0_mva < 0.428825 || simple_1g0pMar2020BNB_mva < 0.892728 || simple_1g0pMar2020COSMIC_mva < 0.988209) && (simple_1g0pMar2020NCPi0_mva >=0.2 && simple_1g0pMar2020BNB_mva >= 0.3  && DeNan((reco_shower_kalman_dEdx_plane2_median[0]+(reco_shower_kalman_dEdx_allplane_median[0]*(reco_shower_kalman_dEdx_plane2_median==0))),reco_shower_dEdx_amalgamated[0]) >= 2.5)))" 
FULLFS="(1g0pMar2020COSMIC_mva >=0.9&& ((1g0pMar2020NCPi0_mva < 0.428825 || 1g0pMar2020BNB_mva < 0.892728 || 1g0pMar2020COSMIC_mva < 0.988209) && (1g0pMar2020NCPi0_mva >=0.2 && 1g0pMar2020BNB_mva >= 0.3  && DeNan((reco_shower_kalman_dEdx_plane2_median[0]+(reco_shower_kalman_dEdx_allplane_median[0]*(reco_shower_kalman_dEdx_plane2_median==0))),reco_shower_dEdx_amalgamated[0]) >= 2.5)))"

# NCPi0 Cuts
#sNCPIFS="(simple_1g1pMar2020_v4Nue_mva >= 0.3 && (simple_1g1pMar2020_v4BNB_mva >= 0.96273 && simple_1g1pMar2020_v4NCPi0_mva < 0.467404))"
#NCPIFS="(1g1pMar2020_v4Nue_mva>= 0.3 && (1g1pMar2020_v4BNB_mva >= 0.96273 && 1g1pMar2020_v4NCPi0_mva < 0.467404))"
sNCPIFS="simple_1g0pMar2020COSMIC_mva >=0.9&& (simple_1g0pMar2020NCPi0_mva < 0.428825  &&simple_1g0pMar2020BNB_mva>= 0.892728) && ((simple_1g0pMar2020NCPi0_mva >=0.2 && 1g0pMar2020BNB_mva >= 0.3  && DeNan((reco_shower_kalman_dEdx_plane2_median[0]+(reco_shower_kalman_dEdx_allplane_median[0]*(reco_shower_kalman_dEdx_plane2_median==0    ))),reco_shower_dEdx_amalgamated[0]) >= 2.5))"
NCPIFS="1g0pMar2020COSMIC_mva >=0.9&& (1g0pMar2020NCPi0_mva < 0.428825  &&1g0pMar2020BNB_mva>= 0.892728) && ((1g0pMar2020NCPi0_mva >=0.2 && 1g0pMar2020BNB_mva >= 0.3  && DeNan((reco_shower_kalman_dEdx_plane2_median[0]+(reco_shower_kalman_dEdx_allplane_median[0]*(reco_shower_kalman_dEdx_plane2_median==0    ))),reco_shower_dEdx_amalgamated[0]) >= 2.5))"

# BNB Cuts
#sBNBFS="(simple_1g1pMar2020_v4Nue_mva >= 0.3 && (simple_1g1pMar2020_v4BNB_mva < 0.96273 && simple_1g1pMar2020_v4NCPi0_mva >= 0.467404))"
#BNBFS="(1g1pMar2020_v4Nue_mva>= 0.3 && (1g1pMar2020_v4BNB_mva < 0.96273 && 1g1pMar2020_v4NCPi0_mva >= 0.467404))"
sBNBFS="simple_1g0pMar2020COSMIC_mva >=0.9&& (simple_1g0pMar2020NCPi0_mva > 0.428825  &&simple_1g0pMar2020BNB_mva<= 0.892728) && ((simple_1g0pMar2020NCPi0_mva >=0.2 && 1g0pMar2020BNB_mva >= 0.3  && DeNan((reco_shower_kalman_dEdx_plane2_median[0]+(reco_shower_kalman_dEdx_allplane_median[0]*(reco_shower_kalman_dEdx_plane2_median==0    ))),reco_shower_dEdx_amalgamated[0]) >= 2.5))"

BNBFS="1g0pMar2020COSMIC_mva >=0.9&& (1g0pMar2020NCPi0_mva > 0.428825  &&1g0pMar2020BNB_mva<= 0.892728) && ((1g0pMar2020NCPi0_mva >=0.2 && 1g0pMar2020BNB_mva >= 0.3  && DeNan((reco_shower_kalman_dEdx_plane2_median[0]+(reco_shower_kalman_dEdx_allplane_median[0]*(reco_shower_kalman_dEdx_plane2_median==0    ))),reco_shower_dEdx_amalgamated[0]) >= 2.5))"

NUM=2212

#Full farsideband 
#./../../bin/hive -o datamc --makefluxcovar $FXXML -x $XML -s 1 -g78 --plottrainonly --cuts "$sFULLFS"
#rm *Far*entry*root
#echo $FULLFS
#./../../bin/hive -o datamc  -x $XML -s 1 -g78  --plottrainonly  --cuts "$FULLFS" --additional_tag="FullFS_CosCut" --systematics "flux_fracfixed"
rm *Far*entry*root

#NCPi0Box 
./../../bin/hive -o datamc --makefluxcovar $FXXML -x $XML2 -s 1 --plottrainonly --cuts "$sNCPIFS"
rm *Far*entry*root
./../../../bin/hive -o datamc  -x $XML2 -s 1 --plottrainonly   --cuts "$NCPIFS" --additional_tag="NCpi0FS_CosCut" --systematics "flux_fracfixed"
rm  *Far*entry*root

#BNB Box
#./../../bin/hive -o datamc --makefluxcovar $FXXML -x $XML2 -s 1 -g78 --plottrainonly --cuts "$sBNBFS"
#rm *Far*entry*root
#./../../bin/hive -o datamc  -x $XML2 -s 1 -g78 --plottrainonly  --cuts "$BNBFS" --additional_tag="BNBFS_CosCut" --systematics "flux_fracfixed"
#rm *Far*entry*root

