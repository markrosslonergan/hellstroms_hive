#!/bin/bash

FXXML="/uboone/app/users/markrl/SLi7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/template_1g0p_stage_1_Jan2021.xml"
DXML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/DetSys_Neutrino2020/template_detsys_1g0p_Feb2021_v4.xml"
XML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g0p_xmls/hive4.0/1g0p_EmeraldWorldOrder_CombinedRuns13_v4_FarSideband_Plot.xml"
XML1="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g0p_xmls/hive4.0/1g0p_EmeraldWorldOrder_Run1_v4_FarSideband_Plot.xml"
XML2="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g0p_xmls/hive4.0/1g0p_EmeraldWorldOrder_Run2_v4_FarSideband_Plot.xml"
XML3="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g0p_xmls/hive4.0/1g0p_EmeraldWorldOrder_Run3_v4_FarSideband_Plot.xml"

# Full FS Cuts
sFULLFSnoC="(((simple_1g0pMar2020NCPi0_mva < 0.428825 || simple_1g0pMar2020BNB_mva < 0.892728 || simple_1g0pMar2020COSMIC_mva < 0.988209) && (simple_1g0pMar2020NCPi0_mva >=0.2 && simple_1g0pMar2020BNB_mva >= 0.3  && DeNan((reco_shower_kalman_dEdx_plane2_median[0]+(reco_shower_kalman_dEdx_allplane_median[0]*(reco_shower_kalman_dEdx_plane2_median==0))),reco_shower_dEdx_amalgamated[0]) >= 2.5)))" 
FULLFSnoC="(((1g0pMar2020NCPi0_mva < 0.428825 || 1g0pMar2020BNB_mva < 0.892728 || 1g0pMar2020COSMIC_mva < 0.988209) && (1g0pMar2020NCPi0_mva >=0.2 && 1g0pMar2020BNB_mva >= 0.3  && DeNan((reco_shower_kalman_dEdx_plane2_median[0]+(reco_shower_kalman_dEdx_allplane_median[0]*(reco_shower_kalman_dEdx_plane2_median==0))),reco_shower_dEdx_amalgamated[0]) >= 2.5)))"

# Full FS Cuts
sFULLFS="(simple_1g0pMar2020COSMIC_mva >=0.9 && ((simple_1g0pMar2020NCPi0_mva < 0.428825 || simple_1g0pMar2020BNB_mva < 0.892728 || simple_1g0pMar2020COSMIC_mva < 0.988209) && (simple_1g0pMar2020NCPi0_mva >=0.2 && simple_1g0pMar2020BNB_mva >= 0.3  && DeNan((reco_shower_kalman_dEdx_plane2_median[0]+(reco_shower_kalman_dEdx_allplane_median[0]*(reco_shower_kalman_dEdx_plane2_median==0))),reco_shower_dEdx_amalgamated[0]) >= 2.5)))" 
FULLFS="(1g0pMar2020COSMIC_mva >=0.9&& ((1g0pMar2020NCPi0_mva < 0.428825 || 1g0pMar2020BNB_mva < 0.892728 || 1g0pMar2020COSMIC_mva < 0.988209) && (1g0pMar2020NCPi0_mva >=0.2 && 1g0pMar2020BNB_mva >= 0.3  && DeNan((reco_shower_kalman_dEdx_plane2_median[0]+(reco_shower_kalman_dEdx_allplane_median[0]*(reco_shower_kalman_dEdx_plane2_median==0))),reco_shower_dEdx_amalgamated[0]) >= 2.5)))"

# NCPi0 Cuts
sNCPIFS="simple_1g0pMar2020COSMIC_mva >=0.9&& (simple_1g0pMar2020NCPi0_mva < 0.428825  &&simple_1g0pMar2020BNB_mva>= 0.892728) && ((simple_1g0pMar2020NCPi0_mva >=0.2 && simple_1g0pMar2020BNB_mva >= 0.3  && DeNan((reco_shower_kalman_dEdx_plane2_median[0]+(reco_shower_kalman_dEdx_allplane_median[0]*(reco_shower_kalman_dEdx_plane2_median==0    ))),reco_shower_dEdx_amalgamated[0]) >= 2.5))"
NCPIFS="1g0pMar2020COSMIC_mva >=0.9&& (1g0pMar2020NCPi0_mva < 0.428825  &&1g0pMar2020BNB_mva>= 0.892728) && ((1g0pMar2020NCPi0_mva >=0.2 && 1g0pMar2020BNB_mva >= 0.3  && DeNan((reco_shower_kalman_dEdx_plane2_median[0]+(reco_shower_kalman_dEdx_allplane_median[0]*(reco_shower_kalman_dEdx_plane2_median==0    ))),reco_shower_dEdx_amalgamated[0]) >= 2.5))"

# BNB Cuts
sBNBFS="simple_1g0pMar2020COSMIC_mva >=0.9&& (simple_1g0pMar2020NCPi0_mva > 0.428825  &&simple_1g0pMar2020BNB_mva<= 0.892728) && ((simple_1g0pMar2020NCPi0_mva >=0.2 && simple_1g0pMar2020BNB_mva >= 0.3  && DeNan((reco_shower_kalman_dEdx_plane2_median[0]+(reco_shower_kalman_dEdx_allplane_median[0]*(reco_shower_kalman_dEdx_plane2_median==0    ))),reco_shower_dEdx_amalgamated[0]) >= 2.5))"
BNBFS="1g0pMar2020COSMIC_mva >=0.9&& (1g0pMar2020NCPi0_mva > 0.428825  &&1g0pMar2020BNB_mva<= 0.892728) && ((1g0pMar2020NCPi0_mva >=0.2 && 1g0pMar2020BNB_mva >= 0.3  && DeNan((reco_shower_kalman_dEdx_plane2_median[0]+(reco_shower_kalman_dEdx_allplane_median[0]*(reco_shower_kalman_dEdx_plane2_median==0    ))),reco_shower_dEdx_amalgamated[0]) >= 2.5))"


NUM=5505
#./hive -o datamc --makedetcovar $DXML -x $XML1 -s 1 -g $NUM --cuts "$sFULLFSnoC"
#./hive -o datamc  -x $XML1 -s 1 -g $NUM  --cuts "$FULLFSnoC" --additional_tag="Run2Run_FULL_Run1" --systematics "merged_det"
#./hive -o datamc  -x $XML2 -s 1 -g $NUM  --cuts "$FULLFSnoC" --additional_tag="Run2Run_FULL_Run2" --systematics "merged_det"
#./hive -o datamc  -x $XML3 -s 1 -g $NUM  --cuts "$FULLFSnoC" --additional_tag="Run2Run_FULL_Run3" --systematics "merged_det"

#Stats only
#./hive -o datamc  -x $XML1 -s 1 -g $NUM  --cuts "$FULLFSnoC" --additional_tag="Run2Run_FULL_Run1" &> logo.1g0p.run1.stat
#./hive -o datamc  -x $XML2 -s 1 -g $NUM  --cuts "$FULLFSnoC" --additional_tag="Run2Run_FULL_Run2" &> logo.1g0p.run2.stat
#./hive -o datamc  -x $XML3 -s 1 -g $NUM  --cuts "$FULLFSnoC" --additional_tag="Run2Run_FULL_Run3" &> logo.1g0p.run3.stat


NUM=5506
#./hive -o datamc --makedetcovar $DXML -x $XML1 -s 1 -g $NUM --cuts "$sNCPIFS"
#./hive -o datamc  -x $XML1 -s 1 -g $NUM  --cuts "$NCPIFS" --additional_tag="Run2Run_NCPi0_Run1" --systematics "merged_det"
#./hive -o datamc  -x $XML2 -s 1 -g $NUM  --cuts "$NCPIFS" --additional_tag="Run2Run_NCPi0_Run2" --systematics "merged_det"
#./hive -o datamc  -x $XML3 -s 1 -g $NUM  --cuts "$NCPIFS" --additional_tag="Run2Run_NCPi0_Run3" --systematics "merged_det"


NUM=5507
#./hive -o datamc --makedetcovar $DXML -x $XML1 -s 1 -g $NUM --cuts "$sBNBFS"
#./hive -o datamc  -x $XML1 -s 1 -g $NUM  --cuts "$BNBFS" --additional_tag="Run2Run_BNBOther_Run1" --systematics "merged_det"
#./hive -o datamc  -x $XML2 -s 1 -g $NUM  --cuts "$BNBFS" --additional_tag="Run2Run_BNBOther_Run2" --systematics "merged_det"
#./hive -o datamc  -x $XML3 -s 1 -g $NUM  --cuts "$BNBFS" --additional_tag="Run2Run_BNBOther_Run3" --systematics "merged_det"


NUM=5508
#rm Far*entry*root
#./hive -o datamc --makedetcovar $DXML -x $XML1 -s 1 -g $NUM --cuts "$sFULLFSnoC"
#./hive -o datamc  -x $XML1 -s 1 -g $NUM  --cuts "$FULLFSnoC" --additional_tag="Run2Run200_FULL_Run1" --systematics "merged_det"
#./hive -o datamc  -x $XML2 -s 1 -g $NUM  --cuts "$FULLFSnoC" --additional_tag="Run2Run200_FULL_Run2" --systematics "merged_det"
#./hive -o datamc  -x $XML3 -s 1 -g $NUM  --cuts "$FULLFSnoC" --additional_tag="Run2Run200_FULL_Run3" --systematics "merged_det"

#200 bins stats only
NUM=5509 
./hive -o datamc  -x $XML1 -s 1 -g $NUM  --cuts "$FULLFSnoC" --additional_tag="Run2RunFeather_FULL_Run1" 
./hive -o datamc  -x $XML2 -s 1 -g $NUM  --cuts "$FULLFSnoC" --additional_tag="Run2RunFeather_FULL_Run2" 
./hive -o datamc  -x $XML3 -s 1 -g $NUM  --cuts "$FULLFSnoC" --additional_tag="Run2RunFeather_FULL_Run3" 


