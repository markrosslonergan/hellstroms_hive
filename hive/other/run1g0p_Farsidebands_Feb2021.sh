#!/bin/bash

FXXML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/template_1g0p_stage_1_Jan2021.xml"
XML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/1g0p_xmls/hive4.0/1g0p_EmeraldWorldOrder_CombinedRuns13_v4_FarSideband_Plot.xml"
FXXML2="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/template_1g0p_stage_4_Jan2021.xml"
DXML="/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/DetSys_Neutrino2020/template_detsys_1g0p_Feb2021_v4.xml"

#XML1="1g1p_CeriseWorldOrder_FarSideband_Run1.xml"
#XML2="1g1p_CeriseWorldOrder_FarSideband_Run2.xml"
#XML3="1g1p_CeriseWorldOrder_FarSideband_Run3.xml"


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



#Just one full farside best, without CC
#./hive -o datamc --makefluxcovar $FXXML -x $XML -g 11511 -s 1 --cuts "$sFULLFSnoC"
#./hive -o datamc  -x $XML -s 1 -g 11511  --cuts "$FULLFSnoC" --additional_tag="FullFSnoC" --systematics "flux_fracfixed"
#rm  *Far*entry*root

#--------- This block produces all 1g1p main chapter notes
#rm  *Far*entry*root
#./hive -o datamc --makefluxcovar $FXXML -x $XML -g 11611 -s 1 --cuts "$sFULLFS"
#./hive -o datamc  -x $XML -s 1 -g 11611  --cuts "$FULLFS" --additional_tag="FullFS" --systematics "flux_fracfixed"
#rm  *Far*entry*root

#./hive -o datamc --makefluxcovar $FXXML -x $XML -g 11711 -s 1 --cuts "$sNCPIFS"
#./hive -o datamc  -x $XML -s 1 -g 11711  --cuts "$NCPIFS" --additional_tag="NCPi0Box" --systematics "flux_fracfixed"
#rm  *Far*entry*root

#./hive -o datamc --makefluxcovar $FXXML -x $XML -g 11811 -s 1 --cuts "$sBNBFS"
#./hive -o datamc  -x $XML -s 1 -g 11811  --cuts "$BNBFS" --additional_tag="BNBOtherBox" --systematics "flux_fracfixed"
#rm  *Far*entry*root

# ------------ Full FarSideband BDTs~~
#rm  *Far*entry*root
#./hive -o datamc --makefluxcovar $FXXML -x $XML -g 11111 -s 1 --cuts "$sFULLFSnoC"
#./hive -o datamc  -x $XML -s 1 -g 11111  --cuts "$FULLFSnoC" --additional_tag="BDTFullFS" --systematics "flux_fracfixed"
#rm  *Far*entry*root



# ------------ Full FarSideband Training 18 bins~~
#rm  *Far*entry*root
#./hive -o datamc --makefluxcovar $FXXML -x $XML -g 4404 -s 1 --cuts "$sFULLFSnoC"
#./hive -o datamc  -x $XML -s 1 -g 4404  --cuts "$FULLFSnoC" --additional_tag="TrainFullFS" --systematics "flux_fracfixed"
#rm  *Far*entry*root

# ------------ NCPi0 FarSideband Training 12 bins
#rm  *Far*entry*root
#./hive -o datamc --makefluxcovar $FXXML -x $XML -g 4404 -s 1 --cuts "$sNCPIFS"
#./hive -o datamc  -x $XML -s 1 -g 4404  --cuts "$NCPIFS" --additional_tag="TrainNCPi0Box" --systematics "flux_fracfixed"
#rm  *Far*entry*root

# ------------ BNB FarSideband Training 12 bins
#rm  *Far*entry*root
#./hive -o datamc --makefluxcovar $FXXML -x $XML -g 44042 -s 1 --cuts "$sBNBFS"
#./hive -o datamc  -x $XML -s 1 -g 44042  --cuts "$BNBFS" --additional_tag="TrainBNBBox" --systematics "flux_fracfixed"
#rm  *Far*entry*root


#Just one full farside best, without CC but WITH detsys
#./hive  --makefluxcovar $FXXML --makedetcovar $DXML -x $XML -g 11511 -s 1 --cuts "$sFULLFSnoC"
./hive -o datamc  -x $XML -s 1 -g 11511  --cuts "$FULLFSnoC" --additional_tag="FullFSnoCwithDet" --systematics "fluxdet"
#rm  *Far*entry*root


