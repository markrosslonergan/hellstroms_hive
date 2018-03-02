
#include <iomanip>


struct tree_new {

  tree_new(TTree * tree) :
    fvertex_tree(tree) {

    exiting_particle_vector = nullptr;
    reco_track_length = nullptr;
    reco_track_dirx = nullptr;
    reco_track_diry = nullptr;
    reco_track_dirz = nullptr;
    reco_track_thetayx = nullptr;
    reco_track_thetaxz = nullptr;
    reco_track_thetayz = nullptr;
    reco_track_phi = nullptr;
    reco_track_theta = nullptr;
    reco_track_calo_dEdx = nullptr;
    reco_track_calo_resrange = nullptr;

    true_track_matching_ratio = nullptr;
    true_track_index = nullptr;
    true_track_matched_to = nullptr;
    true_track_pdg = nullptr;
    true_track_parent_pdg = nullptr;
    true_track_ancestor_pdg = nullptr;
    true_track_origin = nullptr;
    true_track_startx = nullptr; 
    true_track_starty = nullptr;
    true_track_startz = nullptr;    
    true_track_endx = nullptr; 
    true_track_endy = nullptr; 
    true_track_endz = nullptr;    
    track_from_ncdeltarad_interaction = nullptr;
    track_matched_to_ncdeltarad_photon = nullptr;
    track_matched_to_ncdeltarad_proton = nullptr;
    true_track_energy = nullptr;

    reco_shower_startx = nullptr;
    reco_shower_starty = nullptr;
    reco_shower_startz = nullptr;
    reco_shower_dist = nullptr;
    reco_shower_distx = nullptr;
    reco_shower_disty = nullptr;
    reco_shower_distz = nullptr;
    reco_shower_thetayx = nullptr;
    reco_shower_thetaxz = nullptr;
    reco_shower_thetayz = nullptr;
    reco_shower_width0 = nullptr;
    reco_shower_width1 = nullptr;
    reco_shower_opening_angle = nullptr;
    reco_shower_length = nullptr;
    reco_shower_dirx = nullptr;
    reco_shower_diry = nullptr;
    reco_shower_dirz = nullptr;
    reco_shower_helper_energy = nullptr;
    reco_shower_bp_dist_to_tpc = nullptr;
    reco_shower_dedx_plane0 = nullptr;
    reco_shower_dedx_plane1 = nullptr;
    reco_shower_dedx_plane2 = nullptr;
    reco_shower_dist_to_closest_flashzcenter = nullptr;

    true_shower_matching_ratio = nullptr;
    true_shower_index = nullptr;
    true_shower_matched_to = nullptr;
    true_shower_pdg = nullptr;
    true_shower_parent_pdg = nullptr;
    true_shower_ancestor_pdg = nullptr;
    true_shower_origin = nullptr;
    true_shower_startx = nullptr; 
    true_shower_starty = nullptr; 
    true_shower_startz = nullptr;    
    true_shower_endx = nullptr; 
    true_shower_endy = nullptr; 
    true_shower_endz = nullptr;
    shower_from_ncdeltarad_interaction = nullptr;
    shower_matched_to_ncdeltarad_photon = nullptr;
    shower_matched_to_ncdeltarad_proton = nullptr;
    true_shower_energy = nullptr;
    true_shower_detprofile_energy = nullptr;

    fvertex_tree->SetBranchAddress("run_number", &run_number);
    fvertex_tree->SetBranchAddress("subrun_number", &subrun_number);
    fvertex_tree->SetBranchAddress("event_number", &event_number);
    fvertex_tree->SetBranchAddress("tracknumber", &tracknumber);
    fvertex_tree->SetBranchAddress("showernumber", &showernumber);

    fvertex_tree->SetBranchAddress("passed_swtrigger", &passed_swtrigger);
    fvertex_tree->SetBranchAddress("totalpe_sum", &totalpe_sum);
    fvertex_tree->SetBranchAddress("totalpe_ibg_sum", &totalpe_ibg_sum);
    fvertex_tree->SetBranchAddress("totalpe_bbg_sum", &totalpe_bbg_sum);

    fvertex_tree->SetBranchAddress("reco_nuvertx", &reco_nuvertx);
    fvertex_tree->SetBranchAddress("reco_nuverty", &reco_nuverty);
    fvertex_tree->SetBranchAddress("reco_nuvertz", &reco_nuvertz);

    fvertex_tree->SetBranchAddress("reco_nu_vtx_dist_to_closest_tpc_wall", &reco_nu_vtx_dist_to_closest_tpc_wall);

    fvertex_tree->SetBranchAddress("reco_nu_vtx_fid_contained", &reco_nu_vtx_fid_contained);

    fvertex_tree->SetBranchAddress("reco_asso_tracks", &reco_asso_tracks);
    fvertex_tree->SetBranchAddress("reco_asso_showers", &reco_asso_showers);

    fvertex_tree->SetBranchAddress("longest_asso_track_index", &longest_asso_track_index);
    fvertex_tree->SetBranchAddress("reco_track_displacement", &reco_track_length);
    fvertex_tree->SetBranchAddress("reco_track_dirx", &reco_track_dirx);
    fvertex_tree->SetBranchAddress("reco_track_diry", &reco_track_diry);
    fvertex_tree->SetBranchAddress("reco_track_dirz", &reco_track_dirz);
    fvertex_tree->SetBranchAddress("reco_track_thetayx", &reco_track_thetayx);
    fvertex_tree->SetBranchAddress("reco_track_thetaxz", &reco_track_thetaxz);
    fvertex_tree->SetBranchAddress("reco_track_thetayz", &reco_track_thetayz);
    fvertex_tree->SetBranchAddress("reco_track_phi", &reco_track_phi);
    fvertex_tree->SetBranchAddress("reco_track_theta", &reco_track_theta);
    fvertex_tree->SetBranchAddress("reco_track_calo_dEdx", &reco_track_calo_dEdx);
    fvertex_tree->SetBranchAddress("reco_track_calo_resrange", &reco_track_calo_resrange);

    fvertex_tree->SetBranchAddress("closest_asso_shower_dist_to_flashzcenter", &closest_asso_shower_dist_to_flashzcenter);

    fvertex_tree->SetBranchAddress("most_energetic_shower_index", &most_energetic_shower_index);
    fvertex_tree->SetBranchAddress("reco_shower_startx", &reco_shower_startx);
    fvertex_tree->SetBranchAddress("reco_shower_starty", &reco_shower_starty);
    fvertex_tree->SetBranchAddress("reco_shower_startz", &reco_shower_startz);
    fvertex_tree->SetBranchAddress("reco_shower_dist", &reco_shower_dist);
    fvertex_tree->SetBranchAddress("reco_shower_distx", &reco_shower_distx);
    fvertex_tree->SetBranchAddress("reco_shower_disty", &reco_shower_disty);
    fvertex_tree->SetBranchAddress("reco_shower_distz", &reco_shower_distz);
    fvertex_tree->SetBranchAddress("reco_shower_thetayx", &reco_shower_thetayx);
    fvertex_tree->SetBranchAddress("reco_shower_thetaxz", &reco_shower_thetaxz);
    fvertex_tree->SetBranchAddress("reco_shower_thetayz", &reco_shower_thetayz);
    //fvertex_tree->SetBranchAddress("reco_shower_width0", &reco_shower_width0);
    //fvertex_tree->SetBranchAddress("reco_shower_width1", &reco_shower_width1);
    fvertex_tree->SetBranchAddress("reco_shower_opening_angle", &reco_shower_opening_angle);
    fvertex_tree->SetBranchAddress("reco_shower_length", &reco_shower_length);
    fvertex_tree->SetBranchAddress("reco_shower_dirx", &reco_shower_dirx);
    fvertex_tree->SetBranchAddress("reco_shower_diry", &reco_shower_diry);
    fvertex_tree->SetBranchAddress("reco_shower_dirz", &reco_shower_dirz);
    fvertex_tree->SetBranchAddress("reco_shower_helper_energy", &reco_shower_helper_energy);
    fvertex_tree->SetBranchAddress("reco_shower_bp_dist_to_tpc", &reco_shower_bp_dist_to_tpc);
    fvertex_tree->SetBranchAddress("reco_shower_dedx_plane0", &reco_shower_dedx_plane0);
    fvertex_tree->SetBranchAddress("reco_shower_dedx_plane1", &reco_shower_dedx_plane1);
    fvertex_tree->SetBranchAddress("reco_shower_dedx_plane2", &reco_shower_dedx_plane2);
    fvertex_tree->SetBranchAddress("reco_shower_dist_to_closest_flashzcenter", &reco_shower_dist_to_closest_flashzcenter);

    fvertex_tree->SetBranchAddress("shortest_asso_shower_to_vert_dist", &shortest_asso_shower_to_vert_dist);
    fvertex_tree->SetBranchAddress("summed_associated_helper_shower_energy", &summed_associated_helper_shower_energy);

    fvertex_tree->SetBranchAddress("nu_pdg", &nu_pdg);
    fvertex_tree->SetBranchAddress("nu_energy", &nu_energy);
    fvertex_tree->SetBranchAddress("lep_pdg", &lep_pdg);
    fvertex_tree->SetBranchAddress("lep_energy", &lep_energy);
    fvertex_tree->SetBranchAddress("ccnc", &ccnc);
    fvertex_tree->SetBranchAddress("mode", &mode);
    fvertex_tree->SetBranchAddress("interaction_type", &interaction_type);

    fvertex_tree->SetBranchAddress("exiting_photon_number", &exiting_photon_number);
    fvertex_tree->SetBranchAddress("exiting_proton_number", &exiting_proton_number);
    fvertex_tree->SetBranchAddress("exiting_neutron_number", &exiting_neutron_number);
    fvertex_tree->SetBranchAddress("exiting_electron_number", &exiting_electron_number);
    fvertex_tree->SetBranchAddress("exiting_antielectron_number", &exiting_antielectron_number);
    fvertex_tree->SetBranchAddress("exiting_muon_number", &exiting_muon_number);
    fvertex_tree->SetBranchAddress("exiting_antimuon_number", &exiting_antimuon_number);
    fvertex_tree->SetBranchAddress("exiting_piplus_number", &exiting_piplus_number);
    fvertex_tree->SetBranchAddress("exiting_piminus_number", &exiting_piminus_number);
    fvertex_tree->SetBranchAddress("exiting_pi0_number", &exiting_pi0_number);
    fvertex_tree->SetBranchAddress("total_exiting_particles", &total_exiting_particles);
    fvertex_tree->SetBranchAddress("exiting_particle_vector", &exiting_particle_vector);

    fvertex_tree->SetBranchAddress("is_single_photon", &is_single_photon);
    fvertex_tree->SetBranchAddress("is_delta_rad", &is_delta_rad);
    fvertex_tree->SetBranchAddress("delta_true_pdg", &delta_true_pdg);
    fvertex_tree->SetBranchAddress("delta_true_energy", &delta_true_energy);
    fvertex_tree->SetBranchAddress("delta_photon_energy", &delta_photon_energy);
    fvertex_tree->SetBranchAddress("delta_proton_energy", &delta_proton_energy);
    fvertex_tree->SetBranchAddress("delta_mcshower_true_pdg", &delta_mcshower_true_pdg);
    fvertex_tree->SetBranchAddress("delta_mcshower_true_energy", &delta_mcshower_true_energy);
    fvertex_tree->SetBranchAddress("delta_mcshower_detprofile_energy", &delta_mcshower_detprofile_energy);
    fvertex_tree->SetBranchAddress("delta_mctrack_true_pdg", &delta_mctrack_true_pdg);
    fvertex_tree->SetBranchAddress("delta_mctrack_true_energy", &delta_mctrack_true_energy);
    fvertex_tree->SetBranchAddress("delta_mctrack_true_length", &delta_mctrack_true_length);

    fvertex_tree->SetBranchAddress("mctracknumber", &mctracknumber);
    fvertex_tree->SetBranchAddress("mcshowernumber", &mcshowernumber);

    fvertex_tree->SetBranchAddress("true_nuvertx", &true_nuvertx);
    fvertex_tree->SetBranchAddress("true_nuverty", &true_nuverty);
    fvertex_tree->SetBranchAddress("true_nuvertz", &true_nuvertz);

    fvertex_tree->SetBranchAddress("true_nu_E", &true_nu_E);

    fvertex_tree->SetBranchAddress("reco_true_nuvert_dist", &reco_true_nuvert_dist);
    fvertex_tree->SetBranchAddress("reco_true_nuvert_distx", &reco_true_nuvert_distx);
    fvertex_tree->SetBranchAddress("reco_true_nuvert_disty", &reco_true_nuvert_disty);
    fvertex_tree->SetBranchAddress("reco_true_nuvert_distz", &reco_true_nuvert_distz);

    fvertex_tree->SetBranchAddress("true_nu_vtx_tpc_contained", &true_nu_vtx_tpc_contained);
    fvertex_tree->SetBranchAddress("true_nu_vtx_fid_contained", &true_nu_vtx_fid_contained);

    fvertex_tree->SetBranchAddress("true_track_matching_ratio", &true_track_matching_ratio);
    fvertex_tree->SetBranchAddress("true_track_index", &true_track_index);
    fvertex_tree->SetBranchAddress("true_track_matched_to", &true_track_matched_to);
    fvertex_tree->SetBranchAddress("true_track_pdg", &true_track_pdg);
    fvertex_tree->SetBranchAddress("true_track_parent_pdg", &true_track_parent_pdg);
    fvertex_tree->SetBranchAddress("true_track_ancestor_pdg", &true_track_ancestor_pdg);
    fvertex_tree->SetBranchAddress("true_track_origin", &true_track_origin);
    fvertex_tree->SetBranchAddress("true_track_startx", &true_track_startx); 
    fvertex_tree->SetBranchAddress("true_track_starty", &true_track_starty); 
    fvertex_tree->SetBranchAddress("true_track_startz", &true_track_startz);    
    fvertex_tree->SetBranchAddress("true_track_endx", &true_track_endx); 
    fvertex_tree->SetBranchAddress("true_track_endy", &true_track_endy); 
    fvertex_tree->SetBranchAddress("true_track_endz", &true_track_endz);    
    fvertex_tree->SetBranchAddress("track_from_ncdeltarad_interaction", &track_from_ncdeltarad_interaction);
    fvertex_tree->SetBranchAddress("track_matched_to_ncdeltarad_photon", &track_matched_to_ncdeltarad_photon);
    fvertex_tree->SetBranchAddress("track_matched_to_ncdeltarad_proton", &track_matched_to_ncdeltarad_proton);
    fvertex_tree->SetBranchAddress("true_track_energy", &true_track_energy);

    fvertex_tree->SetBranchAddress("true_shower_matching_ratio", &true_shower_matching_ratio);
    fvertex_tree->SetBranchAddress("true_shower_index", &true_shower_index);
    fvertex_tree->SetBranchAddress("true_shower_matched_to", &true_shower_matched_to);
    fvertex_tree->SetBranchAddress("true_shower_pdg", &true_shower_pdg);
    fvertex_tree->SetBranchAddress("true_shower_parent_pdg", &true_shower_parent_pdg);
    fvertex_tree->SetBranchAddress("true_shower_ancestor_pdg", &true_shower_ancestor_pdg);
    fvertex_tree->SetBranchAddress("true_shower_origin", &true_shower_origin);
    fvertex_tree->SetBranchAddress("true_shower_startx", &true_shower_startx); 
    fvertex_tree->SetBranchAddress("true_shower_starty", &true_shower_starty); 
    fvertex_tree->SetBranchAddress("true_shower_startz", &true_shower_startz);    
    fvertex_tree->SetBranchAddress("true_shower_endx", &true_shower_endx); 
    fvertex_tree->SetBranchAddress("true_shower_endy", &true_shower_endy); 
    fvertex_tree->SetBranchAddress("true_shower_endz", &true_shower_endz);    
    fvertex_tree->SetBranchAddress("shower_from_ncdeltarad_interaction", &shower_from_ncdeltarad_interaction);
    fvertex_tree->SetBranchAddress("shower_matched_to_ncdeltarad_photon", &shower_matched_to_ncdeltarad_photon);
    fvertex_tree->SetBranchAddress("shower_matched_to_ncdeltarad_proton", &shower_matched_to_ncdeltarad_proton);
    fvertex_tree->SetBranchAddress("true_shower_energy", &true_shower_energy);
    fvertex_tree->SetBranchAddress("true_shower_detprofile_energy", &true_shower_detprofile_energy);    
    
  }

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
  std::vector<int> * exiting_particle_vector;

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
  std::vector<double> * reco_track_length;
  std::vector<double> * reco_track_dirx;
  std::vector<double> * reco_track_diry;
  std::vector<double> * reco_track_dirz;
  std::vector<double> * reco_track_thetayx;
  std::vector<double> * reco_track_thetaxz;
  std::vector<double> * reco_track_thetayz;
  std::vector<double> * reco_track_phi;
  std::vector<double> * reco_track_theta;
  std::vector<std::vector<double>> * reco_track_calo_dEdx;
  std::vector<std::vector<double>> * reco_track_calo_resrange;

  std::vector<double> * true_track_matching_ratio;
  std::vector<int> * true_track_index;
  std::vector<int> * true_track_matched_to;
  std::vector<int> * true_track_pdg;
  std::vector<int> * true_track_parent_pdg;
  std::vector<int> * true_track_ancestor_pdg;
  std::vector<int> * true_track_origin;
  std::vector<double> * true_track_startx; 
  std::vector<double> * true_track_starty;
  std::vector<double> * true_track_startz;    
  std::vector<double> * true_track_endx; 
  std::vector<double> * true_track_endy; 
  std::vector<double> * true_track_endz;    
  std::vector<int> * track_from_ncdeltarad_interaction;
  std::vector<int> * track_matched_to_ncdeltarad_photon;
  std::vector<int> * track_matched_to_ncdeltarad_proton;
  std::vector<double> * true_track_energy;
 
  double closest_asso_shower_dist_to_flashzcenter;

  int most_energetic_shower_index;
  std::vector<double> * reco_shower_startx;
  std::vector<double> * reco_shower_starty;
  std::vector<double> * reco_shower_startz;
  std::vector<double> * reco_shower_dist;
  std::vector<double> * reco_shower_distx;
  std::vector<double> * reco_shower_disty;
  std::vector<double> * reco_shower_distz;
  std::vector<double> * reco_shower_thetayx;
  std::vector<double> * reco_shower_thetaxz;
  std::vector<double> * reco_shower_thetayz;
  std::vector<double> * reco_shower_width0;
  std::vector<double> * reco_shower_width1;
  std::vector<double> * reco_shower_opening_angle;
  std::vector<double> * reco_shower_length;
  std::vector<double> * reco_shower_dirx;
  std::vector<double> * reco_shower_diry;
  std::vector<double> * reco_shower_dirz;
  std::vector<double> * reco_shower_helper_energy;
  std::vector<double> * reco_shower_bp_dist_to_tpc;
  std::vector<double> * reco_shower_dedx_plane0;
  std::vector<double> * reco_shower_dedx_plane1;
  std::vector<double> * reco_shower_dedx_plane2;
  std::vector<double> * reco_shower_dist_to_closest_flashzcenter;

  std::vector<double> * true_shower_matching_ratio;
  std::vector<int> * true_shower_index;
  std::vector<int> * true_shower_matched_to;
  std::vector<int> * true_shower_pdg;
  std::vector<int> * true_shower_parent_pdg;
  std::vector<int> * true_shower_ancestor_pdg;
  std::vector<int> * true_shower_origin;
  std::vector<double> * true_shower_startx; 
  std::vector<double> * true_shower_starty; 
  std::vector<double> * true_shower_startz;    
  std::vector<double> * true_shower_endx; 
  std::vector<double> * true_shower_endy; 
  std::vector<double> * true_shower_endz;
  std::vector<int> * shower_from_ncdeltarad_interaction;
  std::vector<int> * shower_matched_to_ncdeltarad_photon;
  std::vector<int> * shower_matched_to_ncdeltarad_proton;
  std::vector<double> * true_shower_energy;
  std::vector<double> * true_shower_detprofile_energy;

  double shortest_asso_shower_to_vert_dist;
  double summed_associated_helper_shower_energy;

};


struct tree_old {

  tree_old(TTree * tree) :
    fvertex_tree(tree) {

    exiting_particle_vector = nullptr;
    longest_asso_track_calo_dEdx = nullptr;
    longest_asso_track_calo_resrange = nullptr;
    reco_shower_energy_vector = nullptr;
    reco_shower_dedx_vector = nullptr;

    fvertex_tree->SetBranchAddress("run_number", &run_number);
    fvertex_tree->SetBranchAddress("subrun_number", &subrun_number);
    fvertex_tree->SetBranchAddress("event_number", &event_number);
    fvertex_tree->SetBranchAddress("tracknumber", &tracknumber);
    fvertex_tree->SetBranchAddress("showernumber", &showernumber);

    fvertex_tree->SetBranchAddress("passed_swtrigger", &passed_swtrigger);
    fvertex_tree->SetBranchAddress("totalpe_sum", &totalpe_sum);
    fvertex_tree->SetBranchAddress("totalpe_ibg_sum", &totalpe_ibg_sum);
    fvertex_tree->SetBranchAddress("totalpe_bbg_sum", &totalpe_bbg_sum);

    fvertex_tree->SetBranchAddress("shower_dist_to_flashzcenter", &shower_dist_to_flashzcenter);
    fvertex_tree->SetBranchAddress("shower_dist_to_flashzcenter_splitshowers", &shower_dist_to_flashzcenter_splitshowers);

    fvertex_tree->SetBranchAddress("selected", &selected);

    fvertex_tree->SetBranchAddress("reco_nuvertx", &reco_nuvertx);
    fvertex_tree->SetBranchAddress("reco_nuverty", &reco_nuverty);
    fvertex_tree->SetBranchAddress("reco_nuvertz", &reco_nuvertz);

    fvertex_tree->SetBranchAddress("reco_nu_vtx_dist_to_closest_tpc_wall", &reco_nu_vtx_dist_to_closest_tpc_wall);

    fvertex_tree->SetBranchAddress("reco_nu_vtx_fid_contained", &reco_nu_vtx_fid_contained);

    fvertex_tree->SetBranchAddress("reco_asso_tracks", &reco_asso_tracks);
    fvertex_tree->SetBranchAddress("reco_asso_showers", &reco_asso_showers);

    fvertex_tree->SetBranchAddress("longest_asso_track_displacement", &longest_asso_track_length);
    fvertex_tree->SetBranchAddress("longest_asso_track_reco_dirx", &longest_asso_track_reco_dirx);
    fvertex_tree->SetBranchAddress("longest_asso_track_reco_diry", &longest_asso_track_reco_diry);
    fvertex_tree->SetBranchAddress("longest_asso_track_reco_dirz", &longest_asso_track_reco_dirz);
    fvertex_tree->SetBranchAddress("longest_asso_track_thetayx", &longest_asso_track_thetayx);
    fvertex_tree->SetBranchAddress("longest_asso_track_thetaxz", &longest_asso_track_thetaxz);
    fvertex_tree->SetBranchAddress("longest_asso_track_thetayz", &longest_asso_track_thetayz);
    fvertex_tree->SetBranchAddress("longest_asso_track_phi", &longest_asso_track_phi);
    fvertex_tree->SetBranchAddress("longest_asso_track_theta", &longest_asso_track_theta);

    fvertex_tree->SetBranchAddress("longest_asso_track_calo_dEdx",&longest_asso_track_calo_dEdx);
    fvertex_tree->SetBranchAddress("longest_asso_track_calo_resrange",&longest_asso_track_calo_resrange);

    fvertex_tree->SetBranchAddress("closest_asso_shower_dist_to_flashzcenter", &closest_asso_shower_dist_to_flashzcenter);

    fvertex_tree->SetBranchAddress("most_energetic_shower_reco_startx", &most_energetic_shower_reco_startx);
    fvertex_tree->SetBranchAddress("most_energetic_shower_reco_starty", &most_energetic_shower_reco_starty);
    fvertex_tree->SetBranchAddress("most_energetic_shower_reco_startz", &most_energetic_shower_reco_startz);
    fvertex_tree->SetBranchAddress("most_energetic_shower_reco_dist", &most_energetic_shower_reco_dist);
    fvertex_tree->SetBranchAddress("most_energetic_shower_reco_distx", &most_energetic_shower_reco_distx);
    fvertex_tree->SetBranchAddress("most_energetic_shower_reco_disty", &most_energetic_shower_reco_disty);
    fvertex_tree->SetBranchAddress("most_energetic_shower_reco_distz", &most_energetic_shower_reco_distz);
    fvertex_tree->SetBranchAddress("most_energetic_shower_reco_thetayx", &most_energetic_shower_reco_thetayx);
    fvertex_tree->SetBranchAddress("most_energetic_shower_reco_thetaxz", &most_energetic_shower_reco_thetaxz);
    fvertex_tree->SetBranchAddress("most_energetic_shower_reco_thetayz", &most_energetic_shower_reco_thetayz);
    fvertex_tree->SetBranchAddress("most_energetic_shower_reco_width0", &most_energetic_shower_reco_width0);
    fvertex_tree->SetBranchAddress("most_energetic_shower_reco_width1", &most_energetic_shower_reco_width1);
    fvertex_tree->SetBranchAddress("most_energetic_shower_reco_opening_angle", &most_energetic_shower_reco_opening_angle);
    fvertex_tree->SetBranchAddress("most_energetic_shower_reco_length", &most_energetic_shower_reco_length);
    fvertex_tree->SetBranchAddress("most_energetic_shower_reco_dirx", &most_energetic_shower_reco_dirx);
    fvertex_tree->SetBranchAddress("most_energetic_shower_reco_diry", &most_energetic_shower_reco_diry);
    fvertex_tree->SetBranchAddress("most_energetic_shower_reco_dirz", &most_energetic_shower_reco_dirz);
    fvertex_tree->SetBranchAddress("most_energetic_shower_reco_energy", &most_energetic_shower_reco_energy);
    fvertex_tree->SetBranchAddress("most_energetic_shower_helper_energy", &most_energetic_shower_helper_energy);
    fvertex_tree->SetBranchAddress("reco_shower_energy_vector", &reco_shower_energy_vector);
    fvertex_tree->SetBranchAddress("most_energetic_shower_bp_dist_to_tpc", &most_energetic_shower_bp_dist_to_tpc);
    fvertex_tree->SetBranchAddress("reco_shower_dedx_vector",  &reco_shower_dedx_vector);
    fvertex_tree->SetBranchAddress("reco_shower_dedx_plane0", &reco_shower_dedx_plane0);
    fvertex_tree->SetBranchAddress("reco_shower_dedx_plane1", &reco_shower_dedx_plane1);
    fvertex_tree->SetBranchAddress("reco_shower_dedx_plane2", &reco_shower_dedx_plane2);
    fvertex_tree->SetBranchAddress("reco_shower_dedx_best_plane", &reco_shower_dedx_best_plane);
    fvertex_tree->SetBranchAddress("closest_shower_dedx_plane2", &closest_shower_dedx_plane2);

    fvertex_tree->SetBranchAddress("shortest_asso_shower_to_vert_dist", &shortest_asso_shower_to_vert_dist);
    fvertex_tree->SetBranchAddress("summed_associated_reco_shower_energy", &summed_associated_reco_shower_energy);
    fvertex_tree->SetBranchAddress("summed_associated_helper_shower_energy", &summed_associated_helper_shower_energy);
    fvertex_tree->SetBranchAddress("summed_associated_helper_track_energy", &summed_associated_helper_track_energy);

    fvertex_tree->SetBranchAddress("external_singlephoton_protons_neutrons_only", &external_singlephoton_protons_neutrons_only);

    fvertex_tree->SetBranchAddress("nu_pdg", &nu_pdg);
    fvertex_tree->SetBranchAddress("nu_energy", &nu_energy);
    fvertex_tree->SetBranchAddress("lep_pdg", &lep_pdg);
    fvertex_tree->SetBranchAddress("lep_energy", &lep_energy);
    fvertex_tree->SetBranchAddress("ccnc", &ccnc);
    fvertex_tree->SetBranchAddress("mode", &mode);
    fvertex_tree->SetBranchAddress("interaction_type", &interaction_type);

    fvertex_tree->SetBranchAddress("exiting_photon_number", &exiting_photon_number);
    fvertex_tree->SetBranchAddress("exiting_proton_number", &exiting_proton_number);
    fvertex_tree->SetBranchAddress("exiting_neutron_number", &exiting_neutron_number);
    fvertex_tree->SetBranchAddress("exiting_electron_number", &exiting_electron_number);
    fvertex_tree->SetBranchAddress("exiting_antielectron_number", &exiting_antielectron_number);
    fvertex_tree->SetBranchAddress("exiting_muon_number", &exiting_muon_number);
    fvertex_tree->SetBranchAddress("exiting_antimuon_number", &exiting_antimuon_number);
    fvertex_tree->SetBranchAddress("exiting_piplus_number", &exiting_piplus_number);
    fvertex_tree->SetBranchAddress("exiting_piminus_number", &exiting_piminus_number);
    fvertex_tree->SetBranchAddress("exiting_pi0_number", &exiting_pi0_number);
    fvertex_tree->SetBranchAddress("total_exiting_particles", &total_exiting_particles);
    fvertex_tree->SetBranchAddress("exiting_particle_vector", &exiting_particle_vector);

    fvertex_tree->SetBranchAddress("is_single_photon", &is_single_photon);
    fvertex_tree->SetBranchAddress("is_delta_rad", &is_delta_rad);
    fvertex_tree->SetBranchAddress("delta_true_pdg", &delta_true_pdg);
    fvertex_tree->SetBranchAddress("delta_true_energy", &delta_true_energy);
    fvertex_tree->SetBranchAddress("delta_photon_energy", &delta_photon_energy);
    fvertex_tree->SetBranchAddress("delta_proton_energy", &delta_proton_energy);
    fvertex_tree->SetBranchAddress("delta_mcshower_true_pdg", &delta_mcshower_true_pdg);
    fvertex_tree->SetBranchAddress("delta_mcshower_true_energy", &delta_mcshower_true_energy);
    fvertex_tree->SetBranchAddress("delta_mcshower_detprofile_energy", &delta_mcshower_detprofile_energy);
    fvertex_tree->SetBranchAddress("delta_mctrack_true_pdg", &delta_mctrack_true_pdg);
    fvertex_tree->SetBranchAddress("delta_mctrack_true_energy", &delta_mctrack_true_energy);
    fvertex_tree->SetBranchAddress("delta_mctrack_true_length", &delta_mctrack_true_length);

    fvertex_tree->SetBranchAddress("mctracknumber", &mctracknumber);
    fvertex_tree->SetBranchAddress("mcshowernumber", &mcshowernumber);

    fvertex_tree->SetBranchAddress("true_nuvertx", &true_nuvertx);
    fvertex_tree->SetBranchAddress("true_nuverty", &true_nuverty);
    fvertex_tree->SetBranchAddress("true_nuvertz", &true_nuvertz);

    fvertex_tree->SetBranchAddress("true_nu_E", &true_nu_E);

    fvertex_tree->SetBranchAddress("reco_true_nuvert_dist", &reco_true_nuvert_dist);
    fvertex_tree->SetBranchAddress("reco_true_nuvert_distx", &reco_true_nuvert_distx);
    fvertex_tree->SetBranchAddress("reco_true_nuvert_disty", &reco_true_nuvert_disty);
    fvertex_tree->SetBranchAddress("reco_true_nuvert_distz", &reco_true_nuvert_distz);

    fvertex_tree->SetBranchAddress("true_nu_vtx_tpc_contained", &true_nu_vtx_tpc_contained);
    fvertex_tree->SetBranchAddress("true_nu_vtx_fid_contained", &true_nu_vtx_fid_contained);

    fvertex_tree->SetBranchAddress("longest_asso_track_matching_ratio", &longest_asso_track_matching_ratio);
    fvertex_tree->SetBranchAddress("longest_asso_track_matched_to_mcshower", &longest_asso_track_matched_to_mcshower);
    fvertex_tree->SetBranchAddress("longest_asso_track_matched_to_mctrack", &longest_asso_track_matched_to_mctrack);
    fvertex_tree->SetBranchAddress("longest_asso_track_matched_to_mcparticle", &longest_asso_track_matched_to_mcparticle);
    fvertex_tree->SetBranchAddress("longest_asso_track_from_ncdeltarad_interaction", &longest_asso_track_from_ncdeltarad_interaction);
    fvertex_tree->SetBranchAddress("longest_asso_track_matched_to_ncdeltarad_photon", &longest_asso_track_matched_to_ncdeltarad_photon);
    fvertex_tree->SetBranchAddress("longest_asso_track_matched_to_ncdeltarad_proton", &longest_asso_track_matched_to_ncdeltarad_proton);
    fvertex_tree->SetBranchAddress("longest_asso_track_true_pdg", &longest_asso_track_true_pdg);
    fvertex_tree->SetBranchAddress("longest_asso_track_true_parent_pdg", &longest_asso_track_true_parent_pdg);
    fvertex_tree->SetBranchAddress("longest_asso_track_true_ancestor_pdg", &longest_asso_track_true_ancestor_pdg);
    fvertex_tree->SetBranchAddress("longest_asso_track_true_origin", &longest_asso_track_true_origin);
    fvertex_tree->SetBranchAddress("longest_asso_track_true_startx", &longest_asso_track_true_startx);
    fvertex_tree->SetBranchAddress("longest_asso_track_true_starty", &longest_asso_track_true_starty);
    fvertex_tree->SetBranchAddress("longest_asso_track_true_startz", &longest_asso_track_true_startz);
    fvertex_tree->SetBranchAddress("longest_asso_track_true_endx", &longest_asso_track_true_endx);
    fvertex_tree->SetBranchAddress("longest_asso_track_true_endy", &longest_asso_track_true_endy);
    fvertex_tree->SetBranchAddress("longest_asso_track_true_endz", &longest_asso_track_true_endz);
    fvertex_tree->SetBranchAddress("longest_asso_track_true_thetayx", &longest_asso_track_true_thetayx);
    fvertex_tree->SetBranchAddress("longest_asso_track_true_thetaxz", &longest_asso_track_true_thetaxz);
    fvertex_tree->SetBranchAddress("longest_asso_track_true_thetayz", &longest_asso_track_true_thetayz);
    fvertex_tree->SetBranchAddress("longest_asso_track_true_energy", &longest_asso_track_true_energy);

    fvertex_tree->SetBranchAddress("longest_asso_track_matched_to_mcshower", &longest_asso_track_matched_to_mcshower);
    fvertex_tree->SetBranchAddress("longest_asso_track_matched_to_mctrack", &longest_asso_track_matched_to_mctrack);
    fvertex_tree->SetBranchAddress("longest_asso_track_matched_to_mcparticle", &longest_asso_track_matched_to_mcparticle);
    fvertex_tree->SetBranchAddress("longest_asso_track_true_pdg", &longest_asso_track_true_pdg);
    fvertex_tree->SetBranchAddress("longest_asso_track_true_parent_pdg", &longest_asso_track_true_parent_pdg);
    fvertex_tree->SetBranchAddress("longest_asso_track_true_ancestor_pdg", &longest_asso_track_true_ancestor_pdg);
    fvertex_tree->SetBranchAddress("longest_asso_track_true_origin", &longest_asso_track_true_origin);

    fvertex_tree->SetBranchAddress("shower_matching_ratio", &shower_matching_ratio);
    fvertex_tree->SetBranchAddress("shower_matched_to_mcshower", &shower_matched_to_mcshower);
    fvertex_tree->SetBranchAddress("shower_matched_to_mctrack", &shower_matched_to_mctrack);
    fvertex_tree->SetBranchAddress("shower_matched_to_mcparticle", &shower_matched_to_mcparticle);
    fvertex_tree->SetBranchAddress("shower_from_ncdeltarad_interaction", &shower_from_ncdeltarad_interaction);
    fvertex_tree->SetBranchAddress("shower_matched_to_ncdeltarad_photon", &shower_matched_to_ncdeltarad_photon);
    fvertex_tree->SetBranchAddress("shower_matched_to_ncdeltarad_proton", &shower_matched_to_ncdeltarad_proton);
    fvertex_tree->SetBranchAddress("shower_true_pdg", &shower_true_pdg);
    fvertex_tree->SetBranchAddress("shower_true_parent_pdg", &shower_true_parent_pdg);
    fvertex_tree->SetBranchAddress("shower_true_ancestor_pdg", &shower_true_ancestor_pdg);
    fvertex_tree->SetBranchAddress("shower_true_origin", &shower_true_origin);
    fvertex_tree->SetBranchAddress("shower_true_startx", &shower_true_startx);
    fvertex_tree->SetBranchAddress("shower_true_starty", &shower_true_starty);
    fvertex_tree->SetBranchAddress("shower_true_startz", &shower_true_startz);
    fvertex_tree->SetBranchAddress("shower_true_endx", &shower_true_endx);
    fvertex_tree->SetBranchAddress("shower_true_endy", &shower_true_endy);
    fvertex_tree->SetBranchAddress("shower_true_endz", &shower_true_endz);
    fvertex_tree->SetBranchAddress("shower_true_dist", &shower_true_dist);
    fvertex_tree->SetBranchAddress("shower_true_distx", &shower_true_distx);
    fvertex_tree->SetBranchAddress("shower_true_disty", &shower_true_disty);
    fvertex_tree->SetBranchAddress("shower_true_distz", &shower_true_distz);
    fvertex_tree->SetBranchAddress("shower_true_thetayx", &shower_true_thetayx);
    fvertex_tree->SetBranchAddress("shower_true_thetaxz", &shower_true_thetaxz);
    fvertex_tree->SetBranchAddress("shower_true_thetayz", &shower_true_thetayz);
    fvertex_tree->SetBranchAddress("shower_true_energy", &shower_true_energy);
    fvertex_tree->SetBranchAddress("shower_detprofile_thetayx", &shower_detprofile_thetayx);
    fvertex_tree->SetBranchAddress("shower_detprofile_thetaxz", &shower_detprofile_thetaxz);
    fvertex_tree->SetBranchAddress("shower_detprofile_thetayz", &shower_detprofile_thetayz);
    fvertex_tree->SetBranchAddress("shower_detprofile_energy", &shower_detprofile_energy);

  }

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
  std::vector<int> * exiting_particle_vector;

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

  int external_singlephoton_protons_neutrons_only;

  int passed_swtrigger;
  double totalpe_sum;  
  double totalpe_ibg_sum;
  double totalpe_bbg_sum;
  double shower_dist_to_flashzcenter;
  double shower_dist_to_flashzcenter_splitshowers;  

  double most_energetic_reco_shower;
  double second_most_energetic_reco_shower;

  double true_nuvertx;
  double true_nuverty;
  double true_nuvertz;

  double true_nu_E;

  int true_nu_vtx_tpc_contained;
  int true_nu_vtx_fid_contained;

  int selected;
  int number_of_selected_vertices;
  int number_of_selected_vertices_cut;

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

  double longest_asso_track_matching_ratio;
  int longest_asso_track_matched_to_mcshower;
  int longest_asso_track_matched_to_mctrack;
  int longest_asso_track_matched_to_mcparticle;
  int longest_asso_track_true_pdg;
  int longest_asso_track_true_parent_pdg;
  int longest_asso_track_true_ancestor_pdg;
  int longest_asso_track_true_origin;
  double longest_asso_track_true_startx; 
  double longest_asso_track_true_starty; 
  double longest_asso_track_true_startz;    
  double longest_asso_track_true_endx; 
  double longest_asso_track_true_endy; 
  double longest_asso_track_true_endz;    
  int longest_asso_track_from_ncdeltarad_interaction;
  int longest_asso_track_matched_to_ncdeltarad_photon;
  int longest_asso_track_matched_to_ncdeltarad_proton;
  double longest_asso_track_true_thetayx;
  double longest_asso_track_true_thetaxz;
  double longest_asso_track_true_thetayz;
  double longest_asso_track_true_energy;

  double longest_asso_track_length;
  double longest_asso_track_reco_dirx;
  double longest_asso_track_reco_diry;
  double longest_asso_track_reco_dirz;
  double longest_asso_track_thetayx;
  double longest_asso_track_thetaxz;
  double longest_asso_track_thetayz;
  double longest_asso_track_phi;
  double longest_asso_track_theta;

  std::vector<double> * longest_asso_track_calo_dEdx;
  std::vector<double> * longest_asso_track_calo_resrange;

  double shower_matching_ratio;
  int shower_matched_to_mcshower;
  int shower_matched_to_mctrack;
  int shower_matched_to_mcparticle;
  int shower_true_pdg;
  int shower_true_parent_pdg;
  int shower_true_ancestor_pdg;
  int shower_true_origin;
  double shower_true_startx; 
  double shower_true_starty; 
  double shower_true_startz;    
  double shower_true_endx; 
  double shower_true_endy; 
  double shower_true_endz;
  int shower_from_ncdeltarad_interaction;
  int shower_matched_to_ncdeltarad_photon;
  int shower_matched_to_ncdeltarad_proton;
  double shower_true_dist; 
  double shower_true_distx;
  double shower_true_disty;
  double shower_true_distz;
  double shower_true_thetayx;
  double shower_true_thetaxz;
  double shower_true_thetayz;
  double shower_true_energy;
  double shower_detprofile_thetayx;
  double shower_detprofile_thetaxz;
  double shower_detprofile_thetayz;
  double shower_detprofile_energy;
 
  double closest_asso_shower_dist_to_flashzcenter;

  double most_energetic_shower_reco_startx;
  double most_energetic_shower_reco_starty;
  double most_energetic_shower_reco_startz;
  double most_energetic_shower_reco_dist;
  double most_energetic_shower_reco_distx;
  double most_energetic_shower_reco_disty;
  double most_energetic_shower_reco_distz;
  double most_energetic_shower_reco_thetayx;
  double most_energetic_shower_reco_thetaxz;
  double most_energetic_shower_reco_thetayz;
  double most_energetic_shower_reco_width0;
  double most_energetic_shower_reco_width1;
  double most_energetic_shower_reco_opening_angle;
  double most_energetic_shower_reco_length;
  double most_energetic_shower_reco_dirx;
  double most_energetic_shower_reco_diry;
  double most_energetic_shower_reco_dirz;
  double most_energetic_shower_reco_energy;
  double most_energetic_shower_helper_energy;
  std::vector<double> * reco_shower_energy_vector;
  double most_energetic_shower_bp_dist_to_tpc;
  std::vector<double> * reco_shower_dedx_vector;
  double reco_shower_dedx_plane0;
  double reco_shower_dedx_plane1;
  double reco_shower_dedx_plane2;
  double reco_shower_dedx_best_plane;
  double closest_shower_dedx_plane2;
  double closest_shower_dedx_best_plane;

  double shortest_asso_shower_to_vert_dist;
  double summed_associated_reco_shower_energy;
  double summed_associated_helper_shower_energy;
  double summed_associated_helper_track_energy;  

};


void checkvv(std::vector<std::vector<double>> const * v1,
	     std::vector<std::vector<double>> const * v2) {
  
  if(v1->size() != v2->size()) cout << "checkvv\n";

  for(size_t i = 0; i < v1->size(); ++i) {

    auto const & vv1 = v1->at(i);
    auto const & vv2 = v2->at(i);

    if(vv1.size() != vv2.size()) cout << "checkvv2\n";

    for(size_t j = 0; j < vv1.size(); ++j) {
      
      if(vv1.at(j) != vv2.at(j)) {
	cout << "checkvv3: " << vv1.at(j) << " " << vv2.at(j) << "\n";
      }

    }

  }

}


void checkv(std::vector<double> const * v1,
	    std::vector<double> const * v2) {

  if(v1->size() != v2->size()) cout << "checkv\n";
  
  for(size_t j = 0; j < v1->size(); ++j) {
    
    if(v1->at(j) != v2->at(j)) {
      cout << "checkv2: " << v1->at(j) << " " << v2->at(j) << "\n";
    }
    
  }

}


void test_variables(char const * name1, char const * name2) {

  TFile * file1 = TFile::Open(name1);
  TFile * file2 = TFile::Open(name2);

  TTree * tree1 = (TTree*)file1->Get("vertex_tree");
  TTree * tree2 = (TTree*)file2->Get("LEEPhoton/vertex_tree");

  if(tree1->GetEntries() != tree2->GetEntries()) {
    std::cout << "TTrees have different number of entries\n";
    return;
  }

  double const prec = 1e-14;
  cout << setprecision(14);

  tree_new tn(tree1);
  tree_old to(tree2);

  for(int i = 0; i < tree1->GetEntries(); ++i) {
    
    tree1->GetEntry(i);
    tree2->GetEntry(i);

    int const ti = tn.longest_asso_track_index;
    int const si = tn.most_energetic_shower_index;

    if(i == 0) {

      cout << tn.tracknumber << " " << to.tracknumber << "\n";
      cout << tn.showernumber << " " << to.showernumber << "\n";
      cout << tn.passed_swtrigger << " " << to.passed_swtrigger << "\n";
      cout << tn.totalpe_sum << " " << to.totalpe_sum << "\n";
      cout << tn.totalpe_ibg_sum << " " << to.totalpe_ibg_sum << "\n";
      cout << tn.totalpe_bbg_sum << " " << to.totalpe_bbg_sum << "\n";
      cout << tn.reco_nuvertx << " " << to.reco_nuvertx << "\n";
      cout << tn.reco_nuverty << " " << to.reco_nuverty << "\n";
      cout << tn.reco_nuvertz << " " << to.reco_nuvertz << "\n";
      cout << tn.reco_nu_vtx_dist_to_closest_tpc_wall << " " << to.reco_nu_vtx_dist_to_closest_tpc_wall << "\n";
      cout << tn.reco_nu_vtx_fid_contained << " " << to.reco_nu_vtx_fid_contained << "\n";
      cout << tn.reco_asso_tracks << " " << to.reco_asso_tracks << "\n";
      cout << tn.reco_asso_showers << " " << to.reco_asso_showers << "\n";
      if(ti != -1) {
	cout << tn.reco_track_length->at(ti) << " " << to.longest_asso_track_length << "\n";
	cout << tn.reco_track_dirx->at(ti) << " " << to.longest_asso_track_reco_dirx << "\n";
	cout << tn.reco_track_diry->at(ti) << " " << to.longest_asso_track_reco_diry << "\n";
	cout << tn.reco_track_dirz->at(ti) << " " << to.longest_asso_track_reco_dirz << "\n";
	cout << tn.reco_track_thetayx->at(ti) << " " << to.longest_asso_track_thetayx << "\n";
	cout << tn.reco_track_thetaxz->at(ti) << " " << to.longest_asso_track_thetaxz << "\n";
	cout << tn.reco_track_thetayz->at(ti) << " " << to.longest_asso_track_thetayz << "\n";
	cout << tn.reco_track_phi->at(ti) << " " << to.longest_asso_track_phi << "\n";
	cout << tn.reco_track_theta->at(ti) << " " << to.longest_asso_track_theta << "\n";
	cout << tn.reco_track_calo_dEdx->at(ti).size() << " " << to.longest_asso_track_calo_dEdx->size() << "\n";
	cout << tn.reco_track_calo_resrange->at(ti).size() << " " << to.longest_asso_track_calo_resrange->size() << "\n";
      }
      cout << tn.closest_asso_shower_dist_to_flashzcenter << " " << to.closest_asso_shower_dist_to_flashzcenter << "\n";
      if(tn.reco_asso_showers == 1) {
	cout << tn.reco_shower_startx->at(si) << " " << to.most_energetic_shower_reco_startx << "\n";
	cout << tn.reco_shower_starty->at(si) << " " << to.most_energetic_shower_reco_starty << "\n";
	cout << tn.reco_shower_startz->at(si) << " " << to.most_energetic_shower_reco_startz << "\n";
	cout << tn.reco_shower_dist->at(si) << " " << to.most_energetic_shower_reco_dist << "\n";
	cout << tn.reco_shower_distx->at(si) << " " << to.most_energetic_shower_reco_distx << "\n";
	cout << tn.reco_shower_disty->at(si) << " " << to.most_energetic_shower_reco_disty << "\n";
	cout << tn.reco_shower_distz->at(si) << " " << to.most_energetic_shower_reco_distz << "\n";
      }

    }

    if(tn.run_number != to.run_number) cout << "run_number\n";
    if(tn.subrun_number != to.subrun_number) cout << "subrun_number\n";
    if(tn.event_number != to.event_number) cout << "event_number\n";

    if(tn.tracknumber != to.tracknumber) cout << "tracknumber\n";
    if(tn.showernumber != to.showernumber) cout << "showernumber\n";
    if(tn.passed_swtrigger != to.passed_swtrigger) cout << "passed_swtrigger\n";
    if(tn.totalpe_sum != to.totalpe_sum) cout << "totalpe_sum\n";
    if(tn.totalpe_ibg_sum != to.totalpe_ibg_sum) cout << "totalpe_ibg_sum\n";
    if(tn.totalpe_bbg_sum != to.totalpe_bbg_sum) cout << "totalpe_bbg_sum\n";
    if(tn.reco_nuvertx != to.reco_nuvertx) cout << "reco_nuvertx\n";
    if(tn.reco_nuverty != to.reco_nuverty) cout << "reco_nuverty\n";
    if(tn.reco_nuvertz != to.reco_nuvertz) cout << "reco_nuvertz\n";
    if(tn.reco_nu_vtx_dist_to_closest_tpc_wall != to.reco_nu_vtx_dist_to_closest_tpc_wall) cout << "reco_nu_vtx_dist_to_closest_tpc_wall\n";
    if(tn.reco_nu_vtx_fid_contained != to.reco_nu_vtx_fid_contained) cout << "reco_nu_vtx_fid_contained\n";
    if(tn.reco_asso_tracks != to.reco_asso_tracks) cout << "reco_asso_tracks\n";
    if(tn.reco_asso_showers != to.reco_asso_showers) cout << "reco_asso_showers\n";
    if(ti != -1) {
      if(tn.reco_track_length->at(ti) != to.longest_asso_track_length) cout << "longest_asso_track_length\n";
      if(tn.reco_track_dirx->at(ti) != to.longest_asso_track_reco_dirx) cout << "longest_asso_track_reco_dirx\n";
      if(tn.reco_track_diry->at(ti) != to.longest_asso_track_reco_diry) cout << "longest_asso_track_reco_diry\n";
      if(tn.reco_track_dirz->at(ti) != to.longest_asso_track_reco_dirz) cout << "longest_asso_track_reco_dirz\n";
      if(fabs(tn.reco_track_thetayx->at(ti) - to.longest_asso_track_thetayx) > prec) {
	cout << "longest_asso_track_thetayx\n";
	cout << tn.reco_track_thetayx->at(ti) << " " << to.longest_asso_track_thetayx << "\n";
      }
      if(fabs(tn.reco_track_thetaxz->at(ti) - to.longest_asso_track_thetaxz) > prec) cout << "longest_asso_track_thetaxz\n";
      if(fabs(tn.reco_track_thetayz->at(ti) - to.longest_asso_track_thetayz) > prec) cout << "longest_asso_track_thetayz\n";
      if(tn.reco_track_phi->at(ti) != to.longest_asso_track_phi) cout << "longest_asso_track_phi\n";
      if(tn.reco_track_theta->at(ti) != to.longest_asso_track_theta) cout << "longest_asso_track_theta\n";
      checkv(&tn.reco_track_calo_dEdx->at(ti), to.longest_asso_track_calo_dEdx);
      checkv(&tn.reco_track_calo_resrange->at(ti), to.longest_asso_track_calo_resrange);
    }
    if(tn.closest_asso_shower_dist_to_flashzcenter != to.closest_asso_shower_dist_to_flashzcenter) cout << "closest_asso_shower_dist_to_flashzcenter\n";
    if(tn.reco_asso_showers == 1) {
      if(tn.reco_shower_startx->at(si) != to.most_energetic_shower_reco_startx) {
	cout << "most_energetic_shower_reco_startx event number: " << tn.event_number << "\n";
	cout << tn.reco_shower_startx->at(si) << " " << to.most_energetic_shower_reco_startx << "\n";
      }
      if(tn.reco_shower_starty->at(si) != to.most_energetic_shower_reco_starty) {
	cout << "most_energetic_shower_reco_starty\n";
	cout << tn.reco_shower_starty->at(si) << " " << to.most_energetic_shower_reco_starty << "\n";
      }
      if(tn.reco_shower_startz->at(si) != to.most_energetic_shower_reco_startz) {
	cout << "most_energetic_shower_reco_startz\n";
	cout << tn.reco_shower_startz->at(si) << " " << to.most_energetic_shower_reco_startz << "\n";
      }
      /*
      if(tn.reco_shower_dist->at(si) != to.most_energetic_shower_reco_dist) {
	cout << "most_energetic_shower_reco_dist\n";
	cout << tn.reco_shower_dist->at(si) << " " << to.most_energetic_shower_reco_dist << "\n";
      }	
      if(tn.reco_shower_distx->at(si) != to.most_energetic_shower_reco_distx) cout << "most_energetic_shower_reco_distx\n";
      if(tn.reco_shower_disty->at(si) != to.most_energetic_shower_reco_disty) cout << "most_energetic_shower_reco_disty\n";
      if(tn.reco_shower_distz->at(si) != to.most_energetic_shower_reco_distz) cout << "most_energetic_shower_reco_distz\n";
      */
      if(fabs(tn.reco_shower_thetayx->at(si) - to.most_energetic_shower_reco_thetayx) > prec) {
	cout << "most_energetic_shower_reco_thetayx\n";
	cout << tn.reco_shower_thetayx->at(si) << " " << to.most_energetic_shower_reco_thetayx << "\n";
      }
      if(fabs(tn.reco_shower_thetaxz->at(si) - to.most_energetic_shower_reco_thetaxz) > prec) cout << "most_energetic_shower_reco_thetaxz\n";
      if(fabs(tn.reco_shower_thetayz->at(si) - to.most_energetic_shower_reco_thetayz) > prec) cout << "most_energetic_shower_reco_thetayz\n";
      /*
      if(tn.reco_shower_dirx->at(si) != to.most_energetic_shower_reco_dirx) {
	cout << "most_energetic_shower_reco_dirx\n";
	cout << tn.reco_shower_dirx->at(si) << " " <<  to.most_energetic_shower_reco_dirx << "\n";
      }
      if(tn.reco_shower_diry->at(si) != to.most_energetic_shower_reco_diry) cout << "most_energetic_shower_reco_diry\n";
      if(tn.reco_shower_dirz->at(si) != to.most_energetic_shower_reco_dirz) cout << "most_energetic_shower_reco_dirz\n";
      */

    }
    
    
  }



}
