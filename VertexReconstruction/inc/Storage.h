

#ifndef STORAGE_H
#define STORAGE_H


#include <iostream>
#include <unordered_map>

#include "TFile.h"
#include "TChain.h"


struct Storage {

  Storage(char const * pot_name,
	  char const * meta_name,
	  char const * event_name,
	  std::vector<char const *> const & files = std::vector<char const *>());

  ~Storage();

private:

  bool CheckFile(char const * pot_name,
		 char const * meta_name,
		 char const * event_name,
		 char const * const file) const;
  void Initialize();
  void SetupChains();
  void ProcessPotChain();
  void ProcessMetaChain();
  void ProcessEventChain();

  void GetProducerMap(std::vector<std::string> const & producers,
		      std::vector<int> const & producer_indices,
		      std::unordered_map<std::string, std::pair<int, int>> & producer_map) const;
  std::pair<int, int> const & GetIndices(std::string const & producer,
					 std::unordered_map<std::string, std::pair<int, int>> const & producer_map) const;

  TChain * fpot_chain;
  TChain * fmeta_chain;
  TChain * fevent_chain;

  std::unordered_map<std::string, std::pair<int, int>> fopflash_producer_map;
  std::unordered_map<std::string, std::pair<int, int>> fhit_producer_map;
  std::unordered_map<std::string, std::pair<int, int>> ftrack_producer_map;
  std::unordered_map<std::string, std::pair<int, int>> fshower_producer_map;

public:

  void GetEvent(int const i);
  std::pair<int, int> const & GetOpFlashIndices(std::string const & producer) const;
  std::pair<int, int> const & GetHitIndices(std::string const & producer) const;
  std::pair<int, int> const & GetTrackIndices(std::string const & producer) const;
  std::pair<int, int> const & GetShowerIndices(std::string const & producer) const;

  //pot_chain
  int fnumber_of_events;
  double fpot;

  //meta_chain
  bool fheavy;
  bool fmc;
  std::string * fpot_producer;
  std::string * fswtrigger_product;
  std::vector<std::string> * fopflash_producers;
  std::vector<std::string> * fhit_producers;
  std::vector<std::string> * ftrack_producers;
  std::vector<std::string> * fshower_producers;
  std::vector<std::string> * frmcmassociation_producers;
  double fDetHalfHeight;
  double fDetHalfWidth;
  double fDetLength;
  int fmc_type_shower;
  int fmc_type_track;
  int fmc_type_particle; 

  bool frmcm_bool;

  //event_chain
  std::vector<int> * fopflash_producer_indices;
  std::vector<int> * fhit_producer_indices;
  std::vector<int> * ftrack_producer_indices;
  std::vector<int> * fshower_producer_indices;

  //All
  int frun_number;
  int fsubrun_number;
  int fevent_number;
  int fcurrent_entry;

  //Software Trigger
  //std::vector<int> fswtrigger_producer_index;
  std::vector<std::string> * fswtrigger_getListOfAlgorithms;
  std::vector<bool> * fswtrigger_passedAlgo;
  std::vector<bool> * fswtrigger_passedPrescaleAlgo;
  std::vector<bool> * fswtrigger_vetoAlgo;
  int fswtrigger_passedAlgos;
  int fswtrigger_vetoAlgos;
  int fswtrigger_passedPrescaleAlgos;
  int fswtrigger_passed_swtrigger;

  //Reco Flash
  std::vector<double> * freco_opflash_producer_index;
  std::vector<double> * freco_opflash_Time;
  std::vector<double> * freco_opflash_TimeWidth;
  std::vector<double> * freco_opflash_AbsTime;
  std::vector<int> * freco_opflash_Frame;
  //std::vector<std::vector<double>> freco_opflash_PEs;
  std::vector<double> * freco_opflash_YCenter;
  std::vector<double> * freco_opflash_YWidth;
  std::vector<double> * freco_opflash_ZCenter;
  std::vector<double> * freco_opflash_ZWidth;
  std::vector<bool> * freco_opflash_InBeamFrame;
  std::vector<int> * freco_opflash_OnBeamFrame;
  std::vector<std::vector<double>> * freco_opflash_WireCenters;
  std::vector<std::vector<double>> * freco_opflash_WireWidths;
  std::vector<double> * freco_opflash_TotalPE;
  std::vector<double> * freco_opflash_FastToTotal;

  //Reco Hit
  std::vector<int> * freco_hit_producer_index;
  std::vector<int> * freco_hit_StartTick;
  std::vector<int> * freco_hit_EndTick;
  std::vector<float> * freco_hit_PeakTime;
  std::vector<float> * freco_hit_SigmaPeakTime;
  std::vector<float> * freco_hit_RMS;
  std::vector<float> * freco_hit_PeakAmplitude;
  std::vector<float> * freco_hit_SigmaPeakAmplitude;
  std::vector<float> * freco_hit_SummedADC;
  std::vector<float> * freco_hit_Integral;
  std::vector<float> * freco_hit_SigmaIntegral;
  std::vector<int> * freco_hit_Multiplicity;
  std::vector<int> * freco_hit_LocalIndex;
  std::vector<float> * freco_hit_GoodnessOfFit;
  std::vector<int> * freco_hit_DegreesOfFreedom;
  std::vector<int> * freco_hit_View;
  std::vector<int> * freco_hit_SignalType;
  std::vector<int> * freco_hit_WireID_CryostatID;
  std::vector<int> * freco_hit_WireID_TPCID;
  std::vector<int> * freco_hit_WireID_PlaneID;
  std::vector<int> * freco_hit_WireID_WireID;
  //Reco - MC matching
  std::vector<std::vector<int>> * freco_hit_mc_type;
  std::vector<std::vector<int>> * freco_hit_mc_index;
  /*
  std::vector<std::vector<float>> * freco_hit_true_ideFraction;
  std::vector<std::vector<int>> * freco_hit_true_isMaxIDE;
  std::vector<std::vector<float>> * freco_hit_true_ideNFraction;
  std::vector<std::vector<int>> * freco_hit_true_isMaxIDEN;
  */
  std::vector<std::vector<float>> * freco_hit_true_numElectrons;
  std::vector<std::vector<float>> * freco_hit_true_energy;

  //Reco Track
  std::vector<int> * freco_track_producer_index;
  std::vector<int> * freco_track_NumberTrajectoryPoints;
  std::vector<int> * freco_track_NPoints;
  std::vector<int> * freco_track_FirstPoint;
  std::vector<int> * freco_track_LastPoint;
  std::vector<int> * freco_track_FirstValidPoint;
  std::vector<int> * freco_track_LastValidPoint;
  std::vector<int> * freco_track_CountValidPoints;
  std::vector<std::vector<double>> * freco_track_X;
  std::vector<std::vector<double>> * freco_track_Y;
  std::vector<std::vector<double>> * freco_track_Z;
  std::vector<bool> * freco_track_HasMomentum;
  std::vector<std::vector<double>> * freco_track_Length;
  std::vector<float> * freco_track_Chi2;
  std::vector<float> * freco_track_Chi2PerNdof;
  std::vector<int> * freco_track_Ndof;
  std::vector<int> * freco_track_ParticleId;
  std::vector<double> * freco_track_Theta;
  std::vector<double> * freco_track_Phi;
  std::vector<double> * freco_track_ZenithAngle;
  std::vector<double> * freco_track_AzimuthAngle;
  std::vector<double> * freco_track_VertexDirection_X;
  std::vector<double> * freco_track_VertexDirection_Y;
  std::vector<double> * freco_track_VertexDirection_Z;
  std::vector<std::vector<int>> * freco_track_to_reco_hit;
  std::vector<std::vector<double>> * freco_track_EnergyHelper_resrange;
  std::vector<std::vector<double>> * freco_track_EnergyHelper_dedx;
  std::vector<double> * freco_track_EnergyHelper_energy;
  //Reco - MC matching
  std::vector<int> * freco_track_largest_mc_type;
  std::vector<int> * freco_track_largest_mc_index;
  std::vector<double> * freco_track_largest_ratio;
  std::vector<std::vector<int>> * freco_track_mc_type;
  std::vector<std::vector<int>> * freco_track_mc_index;
  std::vector<std::vector<double>> * freco_track_charge_contribution;
  std::vector<double> * freco_track_charge_total;

  //Reco Shower
  std::vector<int> * freco_shower_producer_index;
  std::vector<double> * freco_shower_Direction_X;
  std::vector<double> * freco_shower_Direction_Y;
  std::vector<double> * freco_shower_Direction_Z;
  std::vector<double> * freco_shower_DirectionErr_X;
  std::vector<double> * freco_shower_DirectionErr_Y;
  std::vector<double> * freco_shower_DirectionErr_Z;
  std::vector<double> * freco_shower_ShowerStart_X;
  std::vector<double> * freco_shower_ShowerStart_Y;
  std::vector<double> * freco_shower_ShowerStart_Z;
  std::vector<double> * freco_shower_ShowerStartErr_X;
  std::vector<double> * freco_shower_ShowerStartErr_Y;
  std::vector<double> * freco_shower_ShowerStartErr_Z;
  std::vector<std::vector<double>> * freco_shower_Energy;
  std::vector<std::vector<double>> * freco_shower_EnergyErr;
  std::vector<std::vector<double>> * freco_shower_MIPEnergy;
  std::vector<std::vector<double>> * freco_shower_MIPEnergyErr;
  std::vector<int> * freco_shower_best_plane;
  std::vector<double> * freco_shower_Length;
  std::vector<double> * freco_shower_OpenAngle;
  std::vector<std::vector<double>> * freco_shower_dEdx;
  std::vector<std::vector<double>> * freco_shower_dEdxErr;
  std::vector<bool> * freco_shower_has_open_angle;
  std::vector<bool> * freco_shower_has_length;
  std::vector<std::vector<int>> * freco_shower_to_reco_hit;
  std::vector<double> * freco_shower_EnergyHelper_energy_legacy;
  std::vector<std::vector<double>> * freco_shower_EnergyHelper_energy;
  std::vector<std::vector<double>> * freco_shower_EnergyHelper_dedx;
  //Reco - MC matching
  std::vector<int> * freco_shower_largest_mc_type;
  std::vector<int> * freco_shower_largest_mc_index;
  std::vector<double> * freco_shower_largest_ratio;
  std::vector<std::vector<int>> * freco_shower_mc_type;
  std::vector<std::vector<int>> * freco_shower_mc_index;
  std::vector<std::vector<double>> * freco_shower_charge_contribution;
  std::vector<double> * freco_shower_charge_total;

  //Pandora                                    
  std::vector<int> * fpfp_pdg;
  std::vector<double> * fpfp_vertex_X;
  std::vector<double> * fpfp_vertex_Y;
  std::vector<double> * fpfp_vertex_Z;
  std::vector<int> * fpfp_original_index;
  std::vector<std::vector<int>> * fpfp_children;

  //Truth
  std::vector<int> * fnu_pdg;
  std::vector<double> * fnu_energy;
  std::vector<int> * flep_pdg;
  std::vector<double> * flep_energy;
  std::vector<int> * fccnc;
  std::vector<int> * fmode;
  std::vector<int> * finteraction_type;

  std::vector<double> * ftrue_nu_E;
  std::vector<double> * ftrue_nuvertx;
  std::vector<double> * ftrue_nuverty;
  std::vector<double> * ftrue_nuvertz;

  std::vector<int> * ftrue_nu_vtx_tpc_contained;
  std::vector<int> * ftrue_nu_vtx_fid_contained;

  //NCDelta indices
  int fdelta_mct_index;
  std::vector<int> * fis_delta_rad;
  std::vector<int> * fdelta_index;
  std::vector<int> * fdelta_photon_index;
  std::vector<int> * fdelta_mcshower_index;
  std::vector<int> * fdelta_proton_index;
  std::vector<int> * fdelta_mctrack_index;

  //GENIE MCParticle
  std::vector<std::vector<int>> * fgenie_particle_TrackId;
  std::vector<std::vector<int>> * fgenie_particle_StatusCode;
  std::vector<std::vector<int>> * fgenie_particle_PdgCode;
  std::vector<std::vector<int>> * fgenie_particle_Mother;
  std::vector<std::vector<double>> * fgenie_particle_X;
  std::vector<std::vector<double>> * fgenie_particle_Y;
  std::vector<std::vector<double>> * fgenie_particle_Z;
  std::vector<std::vector<double>> * fgenie_particle_T;
  std::vector<std::vector<double>> * fgenie_particle_Px;
  std::vector<std::vector<double>> * fgenie_particle_Py;
  std::vector<std::vector<double>> * fgenie_particle_Pz;
  std::vector<std::vector<double>> * fgenie_particle_E;

  //MCParticle
  std::vector<int> * fmcparticle_TrackId;
  std::vector<int> * fmcparticle_StatusCode;
  std::vector<int> * fmcparticle_PdgCode;
  std::vector<int> * fmcparticle_Mother;  
  /* 
  std::vector<double> * fmcparticle_X;
  std::vector<double> * fmcparticle_Y;
  std::vector<double> * fmcparticle_Z;
  std::vector<double> * fmcparticle_T;
  std::vector<double> * fmcparticle_Px;
  std::vector<double> * fmcparticle_Py;
  std::vector<double> * fmcparticle_Pz;
  std::vector<double> * fmcparticle_E;
  */

  //MCTrack
  std::vector<int> * fmctrack_Origin;
  std::vector<int> * fmctrack_PdgCode;
  std::vector<int> * fmctrack_TrackID;
  std::vector<std::string> * fmctrack_Process;
  std::vector<double> * fmctrack_Start_X;
  std::vector<double> * fmctrack_Start_Y;
  std::vector<double> * fmctrack_Start_Z;
  std::vector<double> * fmctrack_Start_T;
  std::vector<double> * fmctrack_Start_Px;
  std::vector<double> * fmctrack_Start_Py;
  std::vector<double> * fmctrack_Start_Pz;
  std::vector<double> * fmctrack_Start_E;
  std::vector<double> * fmctrack_End_X;
  std::vector<double> * fmctrack_End_Y;
  std::vector<double> * fmctrack_End_Z;
  std::vector<double> * fmctrack_End_T;
  std::vector<double> * fmctrack_End_Px;
  std::vector<double> * fmctrack_End_Py;
  std::vector<double> * fmctrack_End_Pz;
  std::vector<double> * fmctrack_End_E;
  std::vector<std::vector<double>> * fmctrack_X;
  std::vector<std::vector<double>> * fmctrack_Y;
  std::vector<std::vector<double>> * fmctrack_Z;
  std::vector<std::vector<double>> * fmctrack_T;
  std::vector<std::vector<double>> * fmctrack_Px;
  std::vector<std::vector<double>> * fmctrack_Py;
  std::vector<std::vector<double>> * fmctrack_Pz;
  std::vector<std::vector<double>> * fmctrack_E;
  std::vector<std::vector<std::vector<double>>> * fmctrack_dQdx;
  std::vector<std::vector<double>> * fmctrack_dEdx;
  std::vector<int> * fmctrack_MotherPdgCode;
  std::vector<int> * fmctrack_MotherTrackID;
  std::vector<std::string> * fmctrack_MotherProcess;
  std::vector<int> * fmctrack_AncestorPdgCode;
  std::vector<int> * fmctrack_AncestorTrackID;
  std::vector<std::string> * fmctrack_AncestorProcess;
  std::vector<std::vector<double>> * fmctrack_contributed_charge;

  //MCShower
  std::vector<int> * fmcshower_Origin;
  std::vector<int> * fmcshower_PdgCode;
  std::vector<int> * fmcshower_TrackID;
  std::vector<std::string> * fmcshower_Process;
  std::vector<double> * fmcshower_Start_X;
  std::vector<double> * fmcshower_Start_Y;
  std::vector<double> * fmcshower_Start_Z;
  std::vector<double> * fmcshower_Start_T;
  std::vector<double> * fmcshower_Start_Px;
  std::vector<double> * fmcshower_Start_Py;
  std::vector<double> * fmcshower_Start_Pz;
  std::vector<double> * fmcshower_Start_E;
  std::vector<double> * fmcshower_End_X;
  std::vector<double> * fmcshower_End_Y;
  std::vector<double> * fmcshower_End_Z;
  std::vector<double> * fmcshower_End_T;
  std::vector<double> * fmcshower_End_Px;
  std::vector<double> * fmcshower_End_Py;
  std::vector<double> * fmcshower_End_Pz;
  std::vector<double> * fmcshower_End_E;
  std::vector<int> * fmcshower_MotherPdgCode;
  std::vector<int> * fmcshower_MotherTrackID;
  std::vector<std::string> * fmcshower_MotherProcess;
  std::vector<int> * fmcshower_AncestorPdgCode;
  std::vector<int> * fmcshower_AncestorTrackID;
  std::vector<std::string> * fmcshower_AncestorProcess;
  std::vector<double> * fmcshower_DetProfile_X;
  std::vector<double> * fmcshower_DetProfile_Y;
  std::vector<double> * fmcshower_DetProfile_Z;
  std::vector<double> * fmcshower_DetProfile_T;
  std::vector<double> * fmcshower_DetProfile_Px;
  std::vector<double> * fmcshower_DetProfile_Py;
  std::vector<double> * fmcshower_DetProfile_Pz;
  std::vector<double> * fmcshower_DetProfile_E;
  std::vector<std::vector<int>> * fmcshower_DaughterTrackID;
  std::vector<std::vector<double>> * fmcshower_Charge;
  std::vector<std::vector<double>> * fmcshower_dQdx;
  std::vector<double> * fmcshower_StartDir_X;
  std::vector<double> * fmcshower_StartDir_Y;
  std::vector<double> * fmcshower_StartDir_Z;
  std::vector<std::vector<double>> * fmcshower_contributed_charge;

  std::map<std::string, std::vector<double *>> fweight_branch_map;
  
  double fweight_genie_ncelaxial_p1sigma;
  double fweight_genie_ncelaxial_m1sigma;
  double fweight_genie_nceleta_p1sigma;
  double fweight_genie_nceleta_m1sigma;
  double fweight_genie_qema_p1sigma;
  double fweight_genie_qema_m1sigma;
  double fweight_genie_qevec_p1sigma;
  double fweight_genie_qevec_m1sigma;
  double fweight_genie_ccresaxial_p1sigma;
  double fweight_genie_ccresaxial_m1sigma;
  double fweight_genie_ccresvector_p1sigma;
  double fweight_genie_ccresvector_m1sigma;
  double fweight_genie_resganged_p1sigma;
  double fweight_genie_resganged_m1sigma;
  double fweight_genie_ncresaxial_p1sigma;
  double fweight_genie_ncresaxial_m1sigma;
  double fweight_genie_ncresvector_p1sigma;
  double fweight_genie_ncresvector_m1sigma;
  double fweight_genie_cohma_p1sigma;
  double fweight_genie_cohma_m1sigma;
  double fweight_genie_cohr0_p1sigma;
  double fweight_genie_cohr0_m1sigma;
  double fweight_genie_nonresrvp1pi_p1sigma;
  double fweight_genie_nonresrvp1pi_m1sigma;
  double fweight_genie_nonresrvbarp1pi_p1sigma;
  double fweight_genie_nonresrvbarp1pi_m1sigma;
  double fweight_genie_nonresrvp2pi_p1sigma;
  double fweight_genie_nonresrvp2pi_m1sigma;
  double fweight_genie_nonresrvbarp2pi_p1sigma;
  double fweight_genie_nonresrvbarp2pi_m1sigma;
  double fweight_genie_resdecaygamma_p1sigma;
  double fweight_genie_resdecaygamma_m1sigma;
  double fweight_genie_resdecayeta_p1sigma;
  double fweight_genie_resdecayeta_m1sigma;
  double fweight_genie_resdecaytheta_p1sigma;
  double fweight_genie_resdecaytheta_m1sigma;
  double fweight_genie_nc_p1sigma;
  double fweight_genie_nc_m1sigma;
  double fweight_genie_disath_p1sigma;
  double fweight_genie_disath_m1sigma;
  double fweight_genie_disbth_p1sigma;
  double fweight_genie_disbth_m1sigma;
  double fweight_genie_discv1u_p1sigma;
  double fweight_genie_discv1u_m1sigma;
  double fweight_genie_discv2u_p1sigma;
  double fweight_genie_discv2u_m1sigma;
  double fweight_genie_disnucl_p1sigma;
  double fweight_genie_disnucl_m1sigma;
  double fweight_genie_agkyxf_p1sigma;
  double fweight_genie_agkyxf_m1sigma;
  double fweight_genie_agkypt_p1sigma;
  double fweight_genie_agkypt_m1sigma;
  double fweight_genie_formzone_p1sigma;
  double fweight_genie_formzone_m1sigma;
  double fweight_genie_fermigasmodelkf_p1sigma;
  double fweight_genie_fermigasmodelkf_m1sigma;
  double fweight_genie_fermigasmodelsf_p1sigma;
  double fweight_genie_fermigasmodelsf_m1sigma;
  double fweight_genie_intranukenmfp_p1sigma;
  double fweight_genie_intranukenmfp_m1sigma;
  double fweight_genie_intranukencex_p1sigma;
  double fweight_genie_intranukencex_m1sigma;
  double fweight_genie_intranukenel_p1sigma;
  double fweight_genie_intranukenel_m1sigma;
  double fweight_genie_intranukeninel_p1sigma;
  double fweight_genie_intranukeninel_m1sigma;
  double fweight_genie_intranukenabs_p1sigma;
  double fweight_genie_intranukenabs_m1sigma;
  double fweight_genie_intranukenpi_p1sigma;
  double fweight_genie_intranukenpi_m1sigma;
  double fweight_genie_intranukepimfp_p1sigma;
  double fweight_genie_intranukepimfp_m1sigma;
  double fweight_genie_intranukepicex_p1sigma;
  double fweight_genie_intranukepicex_m1sigma;
  double fweight_genie_intranukepiel_p1sigma;
  double fweight_genie_intranukepiel_m1sigma;
  double fweight_genie_intranukepiinel_p1sigma;
  double fweight_genie_intranukepiinel_m1sigma;
  double fweight_genie_intranukepiabs_p1sigma;
  double fweight_genie_intranukepiabs_m1sigma;
  double fweight_genie_intranukepipi_p1sigma;
  double fweight_genie_intranukepipi_m1sigma;  

};


#endif
