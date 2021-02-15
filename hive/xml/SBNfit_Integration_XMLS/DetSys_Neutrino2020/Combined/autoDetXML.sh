#!/bin/bash


DIRDIRDIR1g1p="/uboone/data/users/markross/Mar2020/SBNfit_files/1g1p_April_collab/DetSys/Run3/"
DIRDIRDIR1g0p="/uboone/data/users/markross/Mar2020/SBNfit_files/1g0p_PostApril_collab_v2/DetSys/Run3/"
DIRDIRDIR2g1p="/uboone/data/users/markross/Mar2020/SBNfit_files/2g1p_April_collab/DetSys/Run3/"
DIRDIRDIR2g0p="/uboone/data/users/markross/Mar2020/SBNfit_files/2g0p_April_collab/DetSys/Run3/"

VARVARVAR2g1p="(sqrt(((1.21989*reco_shower_energy_max[i_shr[0]] + 8.50486)*reco_shower_implied_dirx[i_shr[0]] + (1.21989*reco_shower_energy_max[i_shr[1]] + 8.50486)*reco_shower_implied_dirx[(i_shr[1])])*((1.21989*reco_shower_energy_max[i_shr[0]] + 8.50486)*reco_shower_implied_dirx[i_shr[0]] + (1.21989*reco_shower_energy_max[i_shr[1]]      + 8.50486)*reco_shower_implied_dirx[(i_shr[1])]) + ((1.21989*reco_shower_energy_max[i_shr[0]] + 8.50486)*reco_shower_implied_diry[i_shr[0]] + (1.21989*reco_shower_energy_max[i_shr[1]] + 8.50486)*reco_shower_implied_diry[(i_shr[1])])*((1.21989*reco_shower_energy_max[i_shr[0]] + 8.50486)*reco_shower_implied_diry[i_shr[0]] + (1.21989*reco_shower_energy_max[i_shr[1]] + 8.50486)*reco_shower_implied_diry[(i_shr[1])]) + ((1.21989*reco_shower_energy_max[i_shr[0]] + 8.50486)*reco_shower_implied_dirz[i_shr[0]] + (1.21989*reco_shower_energy_max[i_shr[1]] + 8.50486)*reco_shower_implied_dirz[(i_shr[1])])*(     (1.21989*reco_shower_energy_max[i_shr[0]] + 8.50486)*reco_shower_implied_dirz[i_shr[0]] + (1.21989*reco_shower_energy_max[i_shr[1]] + 8.50486)*reco_shower_implied_dirz[(i_shr[1])]))/1000)"
VARVARVAR2g0p="(sqrt(((1.21989*reco_shower_energy_max[i_shr[0]] + 8.50486)*reco_shower_dirx[i_shr[0]] + (1.21989*reco_shower_energy_max[i_shr[1]] + 8.50486)*reco_shower_dirx[(i_shr[1])])*((1.21989*reco_shower_energy_max[i_shr[0]] + 8.50486)*reco_shower_dirx[i_shr[0]] + (1.21989*reco_shower_energy_max[i_shr[1]]      + 8.50486)*reco_shower_dirx[(i_shr[1])]) + ((1.21989*reco_shower_energy_max[i_shr[0]] + 8.50486)*reco_shower_diry[i_shr[0]] + (1.21989*reco_shower_energy_max[i_shr[1]] + 8.50486)*reco_shower_diry[(i_shr[1])])*((1.21989*reco_shower_energy_max[i_shr[0]] + 8.50486)*reco_shower_diry[i_shr[0]] + (1.21989*reco_shower_energy_max[i_shr[1]] + 8.50486)*reco_shower_diry[(i_shr[1])]) + ((1.21989*reco_shower_energy_max[i_shr[0]] + 8.50486)*reco_shower_dirz[i_shr[0]] + (1.21989*reco_shower_energy_max[i_shr[1]] + 8.50486)*reco_shower_dirz[(i_shr[1])])*(     (1.21989*reco_shower_energy_max[i_shr[0]] + 8.50486)*reco_shower_dirz[i_shr[0]] + (1.21989*reco_shower_energy_max[i_shr[1]] + 8.50486)*reco_shower_dirz[(i_shr[1])]))/1000)"
VARVARVAR1g1p="((reco_shower_energy_plane2[0]+reco_shower_energy_max[0]*(reco_shower_energy_plane2[0]==0))*1.21989 +8.50486)*0.001"
VARVARVAR1g0p="((reco_shower_energy_plane2[0]+reco_shower_energy_max[0]*(reco_shower_energy_plane2[0]==0))*1.21989 +8.50486)*0.001"

STAGE1g1p="stage_6"
STAGE1g0p="stage_4"
STAGE2g1p="stage_2"
STAGE2g0p="stage_2"

XML="master_mom_detsys_2g1p_2g1p_Sept2020_v16"

strings=(
"recom2"
"AngleXZ"
"AngleYZ"
"WireYZ"
"WireX"
"SCE"
"LY"
"LYAtt"
"LYRay"
)


for i in "${strings[@]}"; do
    echo "$i"
    SYSVAR="$i"
    cp $XML".xml" $XML"_"$SYSVAR.xml
    sed -i "s@VARVARVAR2g1p@$VARVARVAR2g1p@g" "$XML"_"$SYSVAR.xml"
    sed -i "s@VARVARVAR2g0p@$VARVARVAR2g0p@g" "$XML"_"$SYSVAR.xml"
#    sed -i "s@VARVARVAR1g1p@$VARVARVAR1g1p@g" "$XML"_"$SYSVAR.xml"
#    sed -i "s@VARVARVAR1g0p@$VARVARVAR1g0p@g" "$XML"_"$SYSVAR.xml"

#    sed -i "s@DIRDIRDIR1g1p@$DIRDIRDIR1g1p@g" "$XML"_"$SYSVAR.xml"
#    sed -i "s@DIRDIRDIR1g0p@$DIRDIRDIR1g0p@g" "$XML"_"$SYSVAR.xml"
#    sed -i "s@DIRDIRDIR2g1p@$DIRDIRDIR2g1p@g" "$XML"_"$SYSVAR.xml"
#    sed -i "s@DIRDIRDIR2g0p@$DIRDIRDIR2g0p@g" "$XML"_"$SYSVAR.xml"
    
    sed -i "s@STAGESTAGESTAGE@$STAGE2g1p@g" "$XML"_"$SYSVAR.xml"
#    sed -i "s@STAGE2g0p@$STAGE2g0p@g" "$XML"_"$SYSVAR.xml"
 #   sed -i "s@STAGE1g1p@$STAGE1g1p@g" "$XML"_"$SYSVAR.xml"
  #  sed -i "s@STAGE1g0p@$STAGE1g0p@g" "$XML"_"$SYSVAR.xml"

    sed -i "s@SYSVAR@$SYSVAR@g" "$XML"_"$SYSVAR.xml"
    
    OUTVAR=$XML"_"$SYSVAR.xml
    ./uboone/app/users/markrl/SBNfit_uBooNE/July2020_SL7/whipping_star/build/bin/sbnfit_make_covariance_hive_integration -x $OUTVAR -t $SYSVAR -d >  log_$SYSVAR
done

