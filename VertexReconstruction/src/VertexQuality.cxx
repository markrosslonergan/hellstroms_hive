

#include "VertexQuality.h"


VertexQuality::VertexQuality() :
  fvertex_tree(nullptr),
  fvertex_tree_event(nullptr),
  fvertex_tree_event_signal(nullptr) {}


VertexQuality::VertexQuality(std::string const & name) :
  fname(name),
  fvertex_tree(nullptr),
  fvertex_tree_event(nullptr),
  fvertex_tree_event_signal(nullptr) {}


void VertexQuality::SetStorage(Storage const * const storage) {

  fstorage = storage;

  fmc_type_shower = fstorage->fmc_type_shower;
  fmc_type_track = fstorage->fmc_type_track;
  fmc_type_particle = fstorage->fmc_type_particle;

  ftpc_volume = geoalgo::AABox(0,
			       -fstorage->fDetHalfHeight,
			       0,
			       2*fstorage->fDetHalfWidth,
			       fstorage->fDetHalfHeight,
			       fstorage->fDetLength);
    
}


void VertexQuality::SetProducers(std::string const & track_producer,
				 std::string const & shower_producer) {

  ftrack_producer = track_producer;
  fshower_producer = shower_producer;

}


void VertexQuality::SetParameters(double const start_prox,
				  double const shower_prox,
				  double const max_bp_dist,
				  double const cpoa_vert_prox,
				  double const cpoa_trackend_prox) {
  
  fstart_prox = start_prox;
  fshower_prox = shower_prox;
  fmax_bp_dist = max_bp_dist;
  fcpoa_vert_prox = cpoa_vert_prox;
  fcpoa_trackend_prox = cpoa_trackend_prox;

}


void VertexQuality::SetupVertexQualityTreeClosest() {

  std::string name;
  if(fname != "") name = fname + "_vertex_quality_tree_closest";
  else name = "vertex_quality_tree_closest";
  
  fvertex_tree_event = new TTree(name.c_str(), "");

  fvertex_tree_event->Branch("start_prox", &fstart_prox, "start_prox/D");
  fvertex_tree_event->Branch("shower_prox", &fshower_prox, "shower_prox/D");
  fvertex_tree_event->Branch("max_bp_dist", &fmax_bp_dist, "max_bp_dist/D");
  fvertex_tree_event->Branch("cpoa_vert_prox", &fcpoa_vert_prox, "cpoa_vert_prox/D");
  fvertex_tree_event->Branch("cpoa_trackend_prox", &fcpoa_trackend_prox, "cpoa_trackend_prox/D");

  fvertex_tree_event->Branch("reco_vertex_present", &freco_vertex_present, "reco_vertex_present/I");
  fvertex_tree_event->Branch("tpc_volume_contained", &ftpc_volume_contained, "tpc_volume_contained/I");

  fvertex_tree_event->Branch("dist", &fdist, "dist/D");
  fvertex_tree_event->Branch("distx", &fdistx, "distx/D");
  fvertex_tree_event->Branch("disty", &fdisty, "disty/D");
  fvertex_tree_event->Branch("distz", &fdistz, "distz/D");

  fvertex_tree_event->Branch("true_track_total", &ftrue_track_total, "true_track_total/I");
  fvertex_tree_event->Branch("true_shower_total", &ftrue_shower_total, "true_shower_total/I");
  fvertex_tree_event->Branch("reco_track_total", &freco_track_total, "reco_track_total/I");
  fvertex_tree_event->Branch("reco_shower_total", &freco_shower_total, "reco_shower_total/I");
  fvertex_tree_event->Branch("correct_track_total", &fcorrect_track_total, "correct_track_total/I");
  fvertex_tree_event->Branch("correct_shower_total", &fcorrect_shower_total, "correct_shower_total/I");

  fvertex_tree_event->Branch("track_matching_ratio_v", &ftrack_matching_ratio_v);
  fvertex_tree_event->Branch("track_true_pdg_v", &ftrack_true_pdg_v);
  fvertex_tree_event->Branch("track_true_origin_v", &ftrack_true_origin_v);

  fvertex_tree_event->Branch("shower_matching_ratio_v", &fshower_matching_ratio_v);
  fvertex_tree_event->Branch("shower_true_pdg_v", &fshower_true_pdg_v);
  fvertex_tree_event->Branch("shower_true_origin_v", &fshower_true_origin_v);

}


void VertexQuality::SetupVertexQualityTreeSignal() {

  std::string name;
  if(fname != "") name = fname + "_vertex_quality_tree_signal";
  else name = "vertex_quality_tree_signal";

  fvertex_tree_event_signal = new TTree(name.c_str(), "");

  fvertex_tree_event_signal->Branch("start_prox", &fstart_prox, "start_prox/D");
  fvertex_tree_event_signal->Branch("shower_prox", &fshower_prox, "shower_prox/D");
  fvertex_tree_event_signal->Branch("max_bp_dist", &fmax_bp_dist, "max_bp_dist/D");
  fvertex_tree_event_signal->Branch("cpoa_vert_prox", &fcpoa_vert_prox, "cpoa_vert_prox/D");
  fvertex_tree_event_signal->Branch("cpoa_trackend_prox", &fcpoa_trackend_prox, "cpoa_trackend_prox/D");

  fvertex_tree_event_signal->Branch("reco_vertex_present", &freco_vertex_present, "reco_vertex_present/I");
  fvertex_tree_event_signal->Branch("is_nc_delta_rad", &fis_nc_delta_rad, "is_nc_delta_rad/I");
  fvertex_tree_event_signal->Branch("nc_delta_rad_split_shower", &fnc_delta_rad_split_shower, "nc_delta_rad_split_shower/I");
  fvertex_tree_event_signal->Branch("tpc_volume_contained", &ftpc_volume_contained, "tpc_volume_contained/I");

  fvertex_tree_event_signal->Branch("dist", &fdist, "dist/D");
  fvertex_tree_event_signal->Branch("distx", &fdistx, "distx/D");
  fvertex_tree_event_signal->Branch("disty", &fdisty, "disty/D");
  fvertex_tree_event_signal->Branch("distz", &fdistz, "distz/D");

  fvertex_tree_event_signal->Branch("true_track_total", &ftrue_track_total, "true_track_total/I");
  fvertex_tree_event_signal->Branch("true_shower_total", &ftrue_shower_total, "true_shower_total/I");
  fvertex_tree_event_signal->Branch("reco_track_total", &freco_track_total, "reco_track_total/I");
  fvertex_tree_event_signal->Branch("reco_shower_total", &freco_shower_total, "reco_shower_total/I");
  fvertex_tree_event_signal->Branch("correct_track_total", &fcorrect_track_total, "correct_track_total/I");
  fvertex_tree_event_signal->Branch("correct_shower_total", &fcorrect_shower_total, "correct_shower_total/I");

  fvertex_tree_event_signal->Branch("track_matching_ratio_v", &ftrack_matching_ratio_v);
  fvertex_tree_event_signal->Branch("track_true_pdg_v", &ftrack_true_pdg_v);
  fvertex_tree_event_signal->Branch("track_true_origin_v", &ftrack_true_origin_v);

  fvertex_tree_event_signal->Branch("shower_matching_ratio_v", &fshower_matching_ratio_v);
  fvertex_tree_event_signal->Branch("shower_true_pdg_v", &fshower_true_pdg_v);
  fvertex_tree_event_signal->Branch("shower_true_origin_v", &fshower_true_origin_v);

}


void VertexQuality::GetTrueObjects(size_t const mct_index,
				   std::vector<size_t> & mctrack_v,
				   std::vector<size_t> & mcshower_v,
				   std::vector<size_t> & mcparticle_v) {
  
  double const threshold = 1;

  std::vector<double> const & true_nuvertx = *fstorage->ftrue_nuvertx;
  std::vector<double> const & true_nuverty = *fstorage->ftrue_nuverty;
  std::vector<double> const & true_nuvertz = *fstorage->ftrue_nuvertz;
  geoalgo::Point_t const true_nu_vtx(true_nuvertx.at(mct_index),
				     true_nuverty.at(mct_index),
				     true_nuvertz.at(mct_index));
  std::vector<int> used_trackid_v;

  std::vector<double> const & mctrack_Start_X = *fstorage->fmctrack_Start_X;
  std::vector<double> const & mctrack_Start_Y = *fstorage->fmctrack_Start_Y;
  std::vector<double> const & mctrack_Start_Z = *fstorage->fmctrack_Start_Z;
  std::vector<int> const & mctrack_TrackID = *fstorage->fmctrack_TrackID;
  for(size_t i = 0; i < mctrack_Start_X.size(); ++i) {
    if(true_nu_vtx.Dist({mctrack_Start_X.at(i), mctrack_Start_Y.at(i), mctrack_Start_Z.at(i)}) > threshold) continue;
    mctrack_v.push_back(i);
    used_trackid_v.push_back(mctrack_TrackID.at(i));
  }

  std::vector<double> const & mcshower_Start_X = *fstorage->fmcshower_Start_X;
  std::vector<double> const & mcshower_Start_Y = *fstorage->fmcshower_Start_Y;
  std::vector<double> const & mcshower_Start_Z = *fstorage->fmcshower_Start_Z;
  std::vector<int> const & mcshower_TrackID = *fstorage->fmcshower_TrackID;
  std::vector<std::vector<int>> const & mcshower_DaughterTrackID = *fstorage->fmcshower_DaughterTrackID; 
  for(size_t i = 0; i < mcshower_Start_X.size(); ++i) {
    int const trackid = mcshower_TrackID.at(i);
    if(std::find(used_trackid_v.begin(), used_trackid_v.end(), trackid) != used_trackid_v.end() ||
       true_nu_vtx.Dist({mcshower_Start_X.at(i), mcshower_Start_Y.at(i), mcshower_Start_Z.at(i)}) > threshold) continue;
    mcshower_v.push_back(i);
    used_trackid_v.push_back(trackid);
    for(unsigned int const dtrackid : mcshower_DaughterTrackID.at(i)) {
      if(dtrackid == trackid ||
	 std::find(used_trackid_v.begin(), used_trackid_v.end(), trackid) != used_trackid_v.end()) continue;
      used_trackid_v.push_back(dtrackid);
    }
  }

  /*
  for(size_t i = 0; i < ev_mcparticle->size(); ++i) {
    simb::MCParticle const & mcp = ev_mcparticle->at(i);
    if(true_nu_vtx.Dist(mcp.Position(0)) > threshold ||
       std::find(used_trackid_v.begin(), used_trackid_v.end(), mcp.TrackId()) != used_trackid_v.end())
      continue;
    mcparticle_v.push_back(i);
    used_trackid_v.push_back(mcp.TrackId());
  }
  */

}


void VertexQuality::GetTrueRecoObjects(size_t const mct_index,
                                       std::vector<size_t> & track_v,
                                       std::vector<size_t> & shower_v) {

  std::vector<size_t> mctrack_v;
  std::vector<size_t> mcshower_v;
  std::vector<size_t> mcparticle_v;
  GetTrueObjects(mct_index, mctrack_v, mcshower_v, mcparticle_v);

  auto & track_indices = fstorage->GetTrackIndices(ftrack_producer);
  std::vector<int> const & reco_track_mc_type_v = *fstorage->freco_track_largest_mc_type;
  std::vector<int> const & reco_track_mc_index_v = *fstorage->freco_track_largest_mc_index;
  for(size_t i = track_indices.first; i < track_indices.second; ++i) {
    int const reco_track_mc_type = reco_track_mc_type_v.at(i);
    int const reco_track_mc_index = reco_track_mc_index_v.at(i);
    if(reco_track_mc_type == fmc_type_shower &&
       std::find(mcshower_v.begin(), mcshower_v.end(), reco_track_mc_index) != mcshower_v.end()) {
      track_v.push_back(i);
    }
    else if(reco_track_mc_type == fmc_type_track &&
	    std::find(mctrack_v.begin(), mctrack_v.end(), reco_track_mc_index) != mctrack_v.end()) {
      track_v.push_back(i);
    }
    else if(reco_track_mc_type == fmc_type_particle &&
	    std::find(mcparticle_v.begin(), mcparticle_v.end(), reco_track_mc_index) != mcparticle_v.end()) {
      track_v.push_back(i);
    }
  }

  auto & shower_indices = fstorage->GetShowerIndices(fshower_producer);
  std::vector<int> const & reco_shower_mc_type_v = *fstorage->freco_shower_largest_mc_type;
  std::vector<int> const & reco_shower_mc_index_v = *fstorage->freco_shower_largest_mc_index;
  for(size_t i = shower_indices.first; i < shower_indices.second; ++i) {
    int const reco_shower_mc_type = reco_shower_mc_type_v.at(i);
    int const reco_shower_mc_index = reco_shower_mc_index_v.at(i);
    if(reco_shower_mc_type == fmc_type_shower &&
       std::find(mcshower_v.begin(), mcshower_v.end(), reco_shower_mc_index) != mcshower_v.end()) {
      shower_v.push_back(i);
    }
    else if(reco_shower_mc_type == fmc_type_track &&
	    std::find(mctrack_v.begin(), mctrack_v.end(), reco_shower_mc_index) != mctrack_v.end()) {
      shower_v.push_back(i);
    }
    else if(reco_shower_mc_type == fmc_type_particle &&
	    std::find(mcparticle_v.begin(), mcparticle_v.end(), reco_shower_mc_index) != mcparticle_v.end()) {
      shower_v.push_back(i);
    }
  }

}


void VertexQuality::Reset() {

  fdist = -10000;
  fdistx = -10000;
  fdisty = -10000;
  fdistz = -10000;

  freco_track_total = 0;
  freco_shower_total = 0;
  fcorrect_track_total = 0;
  fcorrect_shower_total = 0;

  ftrack_matching_ratio_v.clear();
  ftrack_true_pdg_v.clear();
  ftrack_true_origin_v.clear();

  fshower_matching_ratio_v.clear();
  fshower_true_pdg_v.clear();
  fshower_true_origin_v.clear();

}


void VertexQuality::FillTree(TTree * tree,
                             ParticleAssociations const & pas,
                             size_t const pa_index,
                             geoalgo::Point_t const & true_nu_vtx,
                             std::vector<size_t> const & track_v,
                             std::vector<size_t> const & shower_v) {

  Reset();

  if(pa_index == SIZE_MAX) {
    tree->Fill();
    return;
  }  

  DetectorObjects const & deto_v = pas.GetDetectorObjects();
  ParticleAssociation const & pa = pas.GetAssociations().at(pa_index);
  geoalgo::Point_t const & reco_vertex = pa.GetRecoVertex();

  fdist = true_nu_vtx.Dist(reco_vertex);
  fdistx = true_nu_vtx.at(0) - reco_vertex.at(0);
  fdisty = true_nu_vtx.at(1) - reco_vertex.at(1);
  fdistz = true_nu_vtx.at(2) - reco_vertex.at(2);

  std::vector<double> const & reco_track_largest_ratio = *fstorage->freco_track_largest_ratio;
  std::vector<int> const & reco_track_largest_mc_type = *fstorage->freco_track_largest_mc_type;
  std::vector<int> const & reco_track_largest_mc_index = *fstorage->freco_track_largest_mc_index;

  std::vector<int> const & mctrack_PdgCode = *fstorage->fmctrack_PdgCode;
  std::vector<int> const & mctrack_Origin = *fstorage->fmctrack_Origin;

  std::vector<double> const & reco_shower_largest_ratio = *fstorage->freco_shower_largest_ratio;
  std::vector<int> const & reco_shower_largest_mc_type = *fstorage->freco_shower_largest_mc_type;
  std::vector<int> const & reco_shower_largest_mc_index = *fstorage->freco_shower_largest_mc_index;

  std::vector<int> const & mcshower_PdgCode = *fstorage->fmcshower_PdgCode;
  std::vector<int> const & mcshower_Origin = *fstorage->fmcshower_Origin;

  for(size_t const object_index : pa.GetObjectIndices()) {

    DetectorObject const & deto = deto_v.GetDetectorObject(object_index);
    size_t const original_index = deto.foriginal_index;

    if(deto.freco_type == deto_v.ftrack_reco_type) {
      ftrack_matching_ratio_v.push_back(reco_track_largest_ratio.at(original_index));
      int const mc_type = reco_track_largest_mc_type.at(original_index);
      int const mc_index = reco_track_largest_mc_index.at(original_index);
      if(mc_type == fmc_type_track) {
	ftrack_true_pdg_v.push_back(mctrack_PdgCode.at(mc_index));
	ftrack_true_origin_v.push_back(mctrack_Origin.at(mc_index));
      }
      else if(mc_type == fmc_type_shower) {
	ftrack_true_pdg_v.push_back(mcshower_PdgCode.at(mc_index));
	ftrack_true_origin_v.push_back(mcshower_Origin.at(mc_index));
      }
      /*
      else if(mc_type == frmcm->fmc_type_particle) {
	ftrack_true_pdg_v.push_back(mcparticle_PdgCode.at(mc_index));
	ftrack_true_origin_v.push_back(mcparticle_Origin.at(mc_index));
      }
      */
      else {
	ftrack_true_pdg_v.push_back(0);
	ftrack_true_origin_v.push_back(-1);	
      }
      ++freco_track_total;
      if(std::find(track_v.begin(), track_v.end(), original_index) != track_v.end()) {
	++fcorrect_track_total;
      }
    }

    else if(deto.freco_type == deto_v.fshower_reco_type) {
      fshower_matching_ratio_v.push_back(reco_shower_largest_ratio.at(original_index));
      int const mc_type = reco_shower_largest_mc_type.at(original_index);
      int const mc_index = reco_shower_largest_mc_index.at(original_index);
      if(mc_type == fmc_type_track) {
	fshower_true_pdg_v.push_back(mctrack_PdgCode.at(mc_index));
	fshower_true_origin_v.push_back(mctrack_Origin.at(mc_index));
      }
      else if(mc_type == fmc_type_shower) {
	fshower_true_pdg_v.push_back(mcshower_PdgCode.at(mc_index));
	fshower_true_origin_v.push_back(mcshower_Origin.at(mc_index));
      }
      /*
      else if(mc_type == frmcm->fmc_type_particle) {
	fshower_true_pdg_v.push_back(mcparticle_PdgCode.at(mc_index));
	fshower_true_origin_v.push_back(mcparticle_Origin.at(mc_index));
      }
      */
      else {
	fshower_true_pdg_v.push_back(0);
	fshower_true_origin_v.push_back(-1);	
      }
      ++freco_shower_total;
      if(std::find(shower_v.begin(), shower_v.end(), original_index) != shower_v.end()) {
	++fcorrect_shower_total;
      }
    }

  }

  tree->Fill();
  
}


void VertexQuality::RunClosest(ParticleAssociations const & pas,
			       bool const track_only) {

  if(!fvertex_tree_event) SetupVertexQualityTreeClosest();
  
  std::vector<ParticleAssociation> const & pa_v = pas.GetAssociations();
  std::vector<size_t> const * asso_v;
  if(track_only) asso_v = &pas.GetAssociationIndices();
  else asso_v = &pas.GetSelectedAssociations();    

  if(!asso_v->empty()) freco_vertex_present = 1;
  else {
    freco_vertex_present = 0;
    return;
  }

  std::vector<double> const & true_nuvertx = *fstorage->ftrue_nuvertx;
  std::vector<double> const & true_nuverty = *fstorage->ftrue_nuverty;
  std::vector<double> const & true_nuvertz = *fstorage->ftrue_nuvertz;

  std::vector<std::vector<size_t>> track_vv(true_nuvertx.size());
  std::vector<std::vector<size_t>> shower_vv(true_nuvertx.size());
  std::vector<geoalgo::Point_t> true_nu_vertices;
  for(size_t i = 0; i < true_nuvertx.size(); ++i) {
    GetTrueRecoObjects(i, track_vv.at(i), shower_vv.at(i));
    true_nu_vertices.push_back({true_nuvertx.at(i), true_nuverty.at(i), true_nuvertz.at(i)});
  }

  std::vector<size_t> closest_true_indices;
  for(size_t i = 0; i < asso_v->size(); ++i) {

    size_t const pa_index = asso_v->at(i);
    double smallest_dist = DBL_MAX;
    size_t closest_true_index = SIZE_MAX;
    for(size_t j = 0; j < true_nuvertx.size(); ++j) {
      
      double const dist = true_nu_vertices.at(j).Dist(pa_v.at(pa_index).GetRecoVertex());
      if(dist < smallest_dist) {
	smallest_dist = dist;
	closest_true_index = j;
      }
      
    }
    
    closest_true_indices.push_back(closest_true_index);

  }

  for(size_t const i : closest_true_indices) {

    ftpc_volume_contained = 0;
    if(ftpc_volume.Contain(true_nu_vertices.at(i))) ftpc_volume_contained = 1;

    ftrue_track_total = track_vv.at(i).size();
    ftrue_shower_total = shower_vv.at(i).size();

    FillTree(fvertex_tree_event, pas, i, true_nu_vertices.at(i), track_vv.at(i), shower_vv.at(i));

  }

}


void VertexQuality::RunSig(ParticleAssociations const & pas,
                           bool const track_only) {

  if(!fvertex_tree_event_signal) SetupVertexQualityTreeSignal();

  DetectorObjects const & detos = pas.GetDetectorObjects();

  std::vector<double> const & true_nuvertx = *fstorage->ftrue_nuvertx;
  std::vector<double> const & true_nuverty = *fstorage->ftrue_nuverty;
  std::vector<double> const & true_nuvertz = *fstorage->ftrue_nuvertz;
  auto & shower_indices = fstorage->GetShowerIndices(fshower_producer);
  std::vector<int> const & reco_shower_largest_mc_index = *fstorage->freco_shower_largest_mc_index;

  for(size_t mct_index = 0; mct_index < true_nuvertx.size(); ++mct_index) {

    size_t exiting_photon_index = SIZE_MAX;
    ftpc_volume_contained = 0;

    if(!fstorage->fis_delta_rad->at(mct_index)) continue;
    int const mc_index = fstorage->fdelta_mcshower_index->at(mct_index);
    if(mct_index == -1) continue;

    std::vector<size_t> delta_shower_indices;
    for(size_t i = shower_indices.first; i < shower_indices.second; ++i) {
      if(reco_shower_largest_mc_index.at(i) != mc_index) continue;
      delta_shower_indices.push_back(i);
    }
    fnc_delta_rad_split_shower = 0;
    if(delta_shower_indices.empty()) continue;
    fnc_delta_rad_split_shower = delta_shower_indices.size();

    geoalgo::Point_t const true_nu_vtx = {true_nuvertx.at(mct_index), true_nuverty.at(mct_index), true_nuvertz.at(mct_index)};
    if(ftpc_volume.Contain(true_nu_vtx)) ftpc_volume_contained = 1;

    std::vector<size_t> track_v;
    std::vector<size_t> shower_v;
    GetTrueRecoObjects(mct_index, track_v, shower_v);
    ftrue_track_total = track_v.size();
    ftrue_shower_total = shower_v.size();

    std::vector<ParticleAssociation> const & pa_v = pas.GetAssociations();
    for(size_t const i : pas.GetSelectedAssociations()) {
      ParticleAssociation const & pa = pa_v.at(i);
      bool signal_vertex = false;
      for(size_t const n : pa.GetObjectIndices()) {
	if(detos.GetRecoType(n) != detos.fshower_reco_type) continue;
	size_t const original_index = detos.GetDetectorObject(n).foriginal_index;
	auto const si_it = std::find(delta_shower_indices.begin(), delta_shower_indices.end(), original_index);
	if(si_it == delta_shower_indices.end()) continue;
	signal_vertex = true;
	break;
      }
      fis_nc_delta_rad = 0;
      if(!signal_vertex) continue;
      fis_nc_delta_rad = 1;
      FillTree(fvertex_tree_event_signal, pas, i, true_nu_vtx, track_v, shower_v);
    }

  }

}
