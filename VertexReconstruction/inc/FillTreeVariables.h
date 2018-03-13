

#ifndef FILLTREEVARIABLES_H
#define FILLTREEVARIABLES_H

#include "Storage.h"
#include "ParticleAssociations.h"

#include "GeoAABox.h"
#include "GeoAlgo.h"

#include "TTree.h"


class FillTreeVariables {

 public:

  FillTreeVariables();
  ~FillTreeVariables();

  void SetVerbose(bool const verbose = true) {fverbose = verbose;}
  void SetProducers(Storage const * storage,
		    bool const mcordata,
                    std::string const & track_producer,
                    std::string const & shower_producer,
                    std::string const & hit_producer,
                    std::string const & opflash_producer,
                    std::string const & trigger_product);
  void SetupTreeBranches();
  void Fill(ParticleAssociations const & pas);
  void Write() const {fvertex_tree->Write();}

 private:

  void ResetEvent();
  void ResetVertex();
  void FillWeights();
  double DistToClosestTPCWall(geoalgo::Point_t const & pos);
  void FillTrackTruth(int const original_index);
  void FillShowerTruth(int const original_index);
  double ShowerZDistToClosestFlash(int const shower_index);
  double FindBPDist(geoalgo::Cone_t const & cone);
  void FindRecoObjectVariables(DetectorObjects const & detos,
			       ParticleAssociation const & pa);
  void FillVertexTree(ParticleAssociations const & pas,
		      size_t const pn,
		      size_t const mct_index);
  void FillDeltaInfo();
  void FillTruth(size_t const mct_index);
  bool PassedSWTrigger() const;

  Storage const * fstorage;

  bool fmcordata;
  std::string ftrack_producer;
  std::string fswtrigger_product;
  std::string fshower_producer;
  std::string fhit_producer;
  std::string fopflash_producer;

  geoalgo::AABox ftpc_volume;
  double foffset;
  geoalgo::AABox ffiducial_volume;
  geoalgo::GeoAlgo const falgo;

  int const fwire_plane;

  bool fverbose;

  TTree * fvertex_tree;

  int run_number;
  int subrun_number;
  int event_number;
  int nu_pdg;
  double nu_energy;
  int lep_pdg;
  double lep_energy;
  int ccnc;
  int mode;
  int interaction_type;

  int exiting_photon_number;
  int exiting_proton_number;
  int exiting_neutron_number;
  int exiting_electron_number;
  int exiting_antielectron_number;
  int exiting_muon_number;
  int exiting_antimuon_number;
  int exiting_piplus_number;
  int exiting_piminus_number;
  int exiting_pi0_number;
  int total_exiting_particles;
  std::vector<int> exiting_particle_vector;

  int is_single_photon;
  int is_delta_rad;
  int delta_true_pdg;
  double delta_true_energy;
  double delta_photon_energy;
  double delta_proton_energy;
  int delta_mcshower_true_pdg;
  double delta_mcshower_true_energy;
  double delta_mcshower_detprofile_energy;
  int delta_mctrack_true_pdg;
  double delta_mctrack_true_energy;
  double delta_mctrack_true_length;
  
  int mctracknumber;
  int mcshowernumber;
  int tracknumber;
  int showernumber;

  int passed_swtrigger;
  double totalpe_sum;  
  double totalpe_ibg_sum;
  double totalpe_bbg_sum;

  double true_nuvertx;
  double true_nuverty;
  double true_nuvertz;

  double true_nu_E;

  int true_nu_vtx_tpc_contained;
  int true_nu_vtx_fid_contained;

  double reco_nuvertx;
  double reco_nuverty;
  double reco_nuvertz;

  double reco_true_nuvert_dist;
  double reco_true_nuvert_distx;
  double reco_true_nuvert_disty;
  double reco_true_nuvert_distz;

  double reco_nu_vtx_dist_to_closest_tpc_wall;
  int reco_nu_vtx_fid_contained;

  int reco_asso_tracks;
  int reco_asso_showers;

  int longest_asso_track_index;
  std::vector<double> reco_track_length;
  double reco_track_length_arr[100];
  std::vector<double> reco_track_dirx;
  std::vector<double> reco_track_diry;
  std::vector<double> reco_track_dirz;
  std::vector<double> reco_track_thetayx;
  std::vector<double> reco_track_thetaxz;
  std::vector<double> reco_track_thetayz;
  std::vector<double> reco_track_phi;
  std::vector<double> reco_track_theta;
  std::vector<std::vector<double>> reco_track_calo_dEdx;
  std::vector<std::vector<double>> reco_track_calo_resrange;

  std::vector<double> true_track_matching_ratio;
  std::vector<int> true_track_index;
  std::vector<int> true_track_matched_to;
  std::vector<int> true_track_pdg;
  std::vector<int> true_track_parent_pdg;
  std::vector<int> true_track_ancestor_pdg;
  std::vector<int> true_track_origin;
  std::vector<double> true_track_startx; 
  std::vector<double> true_track_starty;
  std::vector<double> true_track_startz;    
  std::vector<double> true_track_endx; 
  std::vector<double> true_track_endy; 
  std::vector<double> true_track_endz;
  std::vector<int> track_matched_to_ncdeltarad_photon;
  std::vector<int> track_matched_to_ncdeltarad_proton;
  std::vector<double> true_track_energy;
 
  double closest_asso_shower_dist_to_flashzcenter;

  int most_energetic_shower_index;
  int second_most_energetic_shower_index;
  std::vector<double> reco_shower_startx;
  std::vector<double> reco_shower_starty;
  std::vector<double> reco_shower_startz;
  std::vector<double> reco_shower_dist;
  std::vector<double> reco_shower_distx;
  std::vector<double> reco_shower_disty;
  std::vector<double> reco_shower_distz;
  std::vector<double> reco_shower_thetayx;
  std::vector<double> reco_shower_thetaxz;
  std::vector<double> reco_shower_thetayz;
  std::vector<double> reco_shower_width0;
  std::vector<double> reco_shower_width1;
  std::vector<double> reco_shower_opening_angle;
  std::vector<double> reco_shower_length;
  std::vector<double> reco_shower_dirx;
  std::vector<double> reco_shower_diry;
  std::vector<double> reco_shower_dirz;
  std::vector<double> reco_shower_helper_energy;
  std::vector<double> reco_shower_bp_dist_to_tpc;
  std::vector<double> reco_shower_dedx_plane0;
  std::vector<double> reco_shower_dedx_plane1;
  std::vector<double> reco_shower_dedx_plane2;
  std::vector<double> reco_shower_dist_to_closest_flashzcenter;

  std::vector<double> true_shower_matching_ratio;
  std::vector<int> true_shower_index;
  std::vector<int> true_shower_matched_to;
  std::vector<int> true_shower_pdg;
  std::vector<int> true_shower_parent_pdg;
  std::vector<int> true_shower_ancestor_pdg;
  std::vector<int> true_shower_origin;
  std::vector<double> true_shower_startx; 
  std::vector<double> true_shower_starty; 
  std::vector<double> true_shower_startz;    
  std::vector<double> true_shower_endx; 
  std::vector<double> true_shower_endy; 
  std::vector<double> true_shower_endz;
  std::vector<int> shower_matched_to_ncdeltarad_photon;
  std::vector<int> shower_matched_to_ncdeltarad_proton;
  std::vector<double> true_shower_energy;
  std::vector<double> true_shower_detprofile_energy;

  double shortest_asso_shower_to_vert_dist;
  double summed_associated_helper_shower_energy;

  std::map<std::string, std::vector<double *>> weight_branch_map;

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
