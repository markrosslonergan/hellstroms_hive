

#include "VertexQuality.h"


VertexQuality::VertexQuality() :
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

  fvertex_tree_event = new TTree("vertex_quality_tree_closest", "");

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


void VertexQuality::GetTrueRecoObjects(size_t const mct_index,
                                       std::vector<size_t> & track_v,
                                       std::vector<size_t> & shower_v) {

  std::vector<size_t> mctrack_v;
  std::vector<size_t> mcshower_v;
  std::vector<size_t> mcparticle_v;
  //GetTrueObjects(e, mct_index, mctrack_v, mcshower_v, mcparticle_v);

  std::vector<int> const & reco_track_mc_type_v = *fstorage->freco_track_largest_mc_type;
  std::vector<int> const & reco_track_mc_index_v = *fstorage->freco_track_largest_mc_index;
  for(size_t i = 0; i <  reco_track_mc_type_v.size(); ++i) {
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

  std::vector<int> const & reco_shower_mc_type_v = *fstorage->freco_shower_largest_mc_type;
  std::vector<int> const & reco_shower_mc_index_v = *fstorage->freco_shower_largest_mc_index;
  for(size_t i = 0; i < reco_shower_mc_type_v.size(); ++i) {
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


void VertexQuality::RunClosest(ParticleAssociations const & pas,
			       bool const track_only) {

  if(!fvertex_tree_event) SetupVertexQualityTreeClosest();

  /*

  geoalgo::Point_t const true_nu_vtx(fstorage->ftrue_nuvertx,
				     fstorage->ftrue_nuverty,
				     fstorage->ftrue_nuvertz);

  ftpc_volume_contained = 0;
  if(ftpc_volume.Contain(true_nu_vtx)) ftpc_volume_contained = 1;

  std::vector<size_t> track_v;
  std::vector<size_t> shower_v;
  GetTrueRecoObjects(0, track_v, shower_v);
  ftrue_track_total = track_v.size();
  ftrue_shower_total = shower_v.size();

  std::vector<ParticleAssociation> const & pa_v = pas.GetAssociations();

  double smallest_dist = DBL_MAX;
  size_t closest_index = SIZE_MAX;
  std::vector<size_t> const * asso_v;
  if(track_only) asso_v = &pas.GetAssociationIndices();
  else asso_v = &pas.GetSelectedAssociations();

  for(size_t const pa_index : *asso_v) {
    //FillTree(e, fvertex_tree, pas, pa_index, true_nu_vtx, track_v, shower_v);                                                                                                      
    double const dist = true_nu_vtx.Dist(pa_v.at(pa_index).GetRecoVertex());
    if(dist < smallest_dist) {
      smallest_dist = dist;
      closest_index = pa_index;
    }
  }

  if(closest_index != SIZE_MAX) freco_vertex_present = 1;
  else freco_vertex_present = 0;
  FillTree(e, fvertex_tree_event, pas, closest_index, true_nu_vtx, track_v, shower_v);
  */

}
