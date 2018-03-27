

#include "VertexQuality.h"


VertexQuality::VertexQuality(std::string const & name) :
  fname(name),
  fvertex_tree(nullptr),
  frun_shower_match(false),
  ffill_event_tree(false) {

  fperformance_quantities = {"mean", "ratio_eq_1"};
  fparameter_name = {"start_prox", "shower_prox", "max_bp_dist", "cpoa_vert_prox", "cpoa_trackend_prox"};

}


VertexQuality::~VertexQuality() {

  if(fevent_tree) delete fevent_tree;
  if(fvertex_tree) delete fvertex_tree;

}


void VertexQuality::FillEventTree(bool const fill_event_tree) {

  ffill_event_tree = fill_event_tree;

}


std::vector<std::string> const & VertexQuality::GetPerformanceQuantities() {

  return fperformance_quantities;

}


void VertexQuality::RunShowerMatch() {

  if(ffill_event_tree) {
   
    fevent_tree = new TTree("event_tree_shower_match", "");
    
    fevent_tree->Branch("run_number", &frun_number, "run_number/I");
    fevent_tree->Branch("subrun_number", &fsubrun_number, "subrun_number/I");
    fevent_tree->Branch("event_number", &fevent_number, "event_number/I");
    
    fevent_tree->Branch("start_prox", &fstart_prox, "start_prox/D");
    fevent_tree->Branch("shower_prox", &fshower_prox, "shower_prox/D");
    fevent_tree->Branch("max_bp_dist", &fmax_bp_dist, "max_bp_dist/D");
    fevent_tree->Branch("cpoa_vert_prox", &fcpoa_vert_prox, "cpoa_vert_prox/D");
    fevent_tree->Branch("cpoa_trackend_prox", &fcpoa_trackend_prox, "cpoa_trackend_prox/D");
    
    fevent_tree->Branch("true_nu_vtx_number", &ftrue_nu_vtx_number, "true_nu_vtx_number/I");
    fevent_tree->Branch("reco_vtx_number", &freco_vtx_number, "reco_vtx_number/I");
    fevent_tree->Branch("tpc_volume_contained", &ftpc_volume_contained, "tpc_volume_contained/I");
    fevent_tree->Branch("true_associated_shower", &ftrue_associated_shower, "true_associated_shower/I");

  }

  fvertex_tree = new TTree("vertex_quality_tree_shower_match", "");
  SetupVertexQualityTree(fvertex_tree);
  frun_shower_match = true;

}


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


void VertexQuality::AddPermutations(std::vector<std::vector<double>> const & permutation_v) {

  fpermutation_v = permutation_v;

}


void VertexQuality::AddPerformanceMetric(std::vector<std::string> const & param) {

  fdraw_vec.push_back(param);

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


void VertexQuality::SetupVertexQualityTree(TTree * tree) {

  tree->Branch("run_number", &frun_number, "run_number/I");
  tree->Branch("subrun_number", &fsubrun_number, "subrun_number/I");
  tree->Branch("event_number", &fevent_number, "event_number/I");

  tree->Branch("start_prox", &fstart_prox, "start_prox/D");
  tree->Branch("shower_prox", &fshower_prox, "shower_prox/D");
  tree->Branch("max_bp_dist", &fmax_bp_dist, "max_bp_dist/D");
  tree->Branch("cpoa_vert_prox", &fcpoa_vert_prox, "cpoa_vert_prox/D");
  tree->Branch("cpoa_trackend_prox", &fcpoa_trackend_prox, "cpoa_trackend_prox/D");

  tree->Branch("true_nu_vtx_number", &ftrue_nu_vtx_number, "true_nu_vtx_number/I");
  tree->Branch("true_associated_shower", &ftrue_associated_shower, "true_associated_shower/I");
  tree->Branch("is_nc_delta_rad", &fis_nc_delta_rad, "is_nc_delta_rad/I");
  tree->Branch("nc_delta_rad_split_shower", &fnc_delta_rad_split_shower, "nc_delta_rad_split_shower/I");
  tree->Branch("tpc_volume_contained", &ftpc_volume_contained, "tpc_volume_contained/I");

  tree->Branch("dist", &fdist, "dist/D");
  tree->Branch("distx", &fdistx, "distx/D");
  tree->Branch("disty", &fdisty, "disty/D");
  tree->Branch("distz", &fdistz, "distz/D");

  tree->Branch("true_track_total", &ftrue_track_total, "true_track_total/I");
  tree->Branch("true_shower_total", &ftrue_shower_total, "true_shower_total/I");
  tree->Branch("reco_track_total", &freco_track_total, "reco_track_total/I");
  tree->Branch("reco_shower_total", &freco_shower_total, "reco_shower_total/I");
  tree->Branch("correct_track_total", &fcorrect_track_total, "correct_track_total/I");
  tree->Branch("correct_shower_total", &fcorrect_shower_total, "correct_shower_total/I");

  tree->Branch("track_matching_ratio_v", &ftrack_matching_ratio_v);
  tree->Branch("track_true_pdg_v", &ftrack_true_pdg_v);
  tree->Branch("track_true_origin_v", &ftrack_true_origin_v);

  tree->Branch("shower_matching_ratio_v", &fshower_matching_ratio_v);
  tree->Branch("shower_true_pdg_v", &fshower_true_pdg_v);
  tree->Branch("shower_true_origin_v", &fshower_true_origin_v);

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


void VertexQuality::ResetEvent() {

  int const unset = -10000;

  frun_number = unset;
  fsubrun_number = unset;
  fevent_number = unset;

  ftrue_nu_vtx_number = unset;
  freco_vtx_number = unset;
  ftrue_associated_shower = unset;

}


void VertexQuality::ResetMCTruth() {

  int const unset = -10000;

  fis_nc_delta_rad = unset;
  fnc_delta_rad_split_shower = unset;
  ftpc_volume_contained = unset;

  ftrue_track_total = unset;
  ftrue_shower_total = unset;  

}


void VertexQuality::ResetVertex() {

  int const unset = -10000;

  fdist = unset;
  fdistx = unset;
  fdisty = unset;
  fdistz = unset;

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

  ResetVertex();

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


void VertexQuality::RunShowerMatch(ParticleAssociations const & pas,
				   bool const track_only) {

  ResetEvent();

  frun_number = fstorage->frun_number;
  fsubrun_number = fstorage->fsubrun_number;
  fevent_number = fstorage->fevent_number;

  DetectorObjects const & detos = pas.GetDetectorObjects();

  std::vector<double> const & true_nuvertx = *fstorage->ftrue_nuvertx;
  std::vector<double> const & true_nuverty = *fstorage->ftrue_nuverty;
  std::vector<double> const & true_nuvertz = *fstorage->ftrue_nuvertz;
  auto & shower_indices = fstorage->GetShowerIndices(fshower_producer);
  std::vector<int> const & reco_shower_largest_mc_index = *fstorage->freco_shower_largest_mc_index;
  std::vector<int> const & reco_shower_largest_mc_type = *fstorage->freco_shower_largest_mc_type;

  ftrue_nu_vtx_number = true_nuvertx.size();
  freco_vtx_number = pas.GetSelectedAssociations().size();
  ftrue_associated_shower = 0;
 
  bool tpc_volume_contained = false;

  for(size_t mct_index = 0; mct_index < true_nuvertx.size(); ++mct_index) {

    ResetMCTruth();

    ftpc_volume_contained = 0;

    geoalgo::Point_t const true_nu_vtx = {true_nuvertx.at(mct_index), true_nuverty.at(mct_index), true_nuvertz.at(mct_index)};
    if(ftpc_volume.Contain(true_nu_vtx)) {
      ftpc_volume_contained = 1;
      tpc_volume_contained = true;
    }

    std::vector<size_t> track_v;
    std::vector<size_t> shower_v;
    GetTrueRecoObjects(mct_index, track_v, shower_v);
    ftrue_track_total = track_v.size();
    ftrue_shower_total = shower_v.size();

    int const mc_index = fstorage->fdelta_mcshower_index->at(mct_index);
    std::vector<size_t> delta_shower_indices;
    for(size_t i = shower_indices.first; i < shower_indices.second; ++i) {
      if(reco_shower_largest_mc_index.at(i) == mc_index && reco_shower_largest_mc_type.at(i) == fstorage->fmc_type_shower) delta_shower_indices.push_back(i);
    }
    fnc_delta_rad_split_shower = delta_shower_indices.size();

    std::vector<ParticleAssociation> const & pa_v = pas.GetAssociations();
    for(size_t const i : pas.GetSelectedAssociations()) {
      ParticleAssociation const & pa = pa_v.at(i);
      bool bnb_shower = false;
      fis_nc_delta_rad = 0;
      for(size_t const n : pa.GetObjectIndices()) {
	if(detos.GetRecoType(n) != detos.fshower_reco_type) continue;
	size_t const original_index = detos.GetDetectorObject(n).foriginal_index;
	if(fis_nc_delta_rad == 0 && std::find(delta_shower_indices.begin(), delta_shower_indices.end(), original_index) != delta_shower_indices.end()) fis_nc_delta_rad = 1;
	if(!bnb_shower && std::find(shower_v.begin(), shower_v.end(), original_index) != shower_v.end()) bnb_shower = true;
      }
      if(!bnb_shower) {
	if(fis_nc_delta_rad) std::cout << "WARNING: not BNB shower tagged as NC Delta rad, entry: " << fstorage->fcurrent_entry << " event number: " << fstorage->fevent_number << "\n";
	continue;
      }
      FillTree(fvertex_tree, pas, i, true_nu_vtx, track_v, shower_v);
    }    

  }

  if(ftrue_shower_total > 0) ftrue_associated_shower = 1;
  if(tpc_volume_contained) ftpc_volume_contained = 1;
  if(fevent_tree) fevent_tree->Fill();

}



void VertexQuality::Run(ParticleAssociations const & pas,
			bool const track_only) {

  if(frun_shower_match) RunShowerMatch(pas, track_only);

}


TTree * VertexQuality::SetupEvalTree(std::vector<std::vector<std::vector<double> > > & drawn_values) {
  
  TTree * eval_tree = new TTree("eval_tree", "");
  eval_tree->Branch("draw_vec", &fdraw_vec);
  eval_tree->Branch("drawn_values", &drawn_values);
  return eval_tree;
  
};


std::string VertexQuality::GetPermString(std::vector<double> const & permutation) const {
  
  std::string result;
  for(size_t i = 0; i < permutation.size(); ++i) {
    result += fparameter_name.at(i) + " == " + std::to_string(permutation.at(i));
    if(i != permutation.size() - 1) result += " && ";
  }
  
  return result;
  
}


std::vector<double> VertexQuality::DrawHist(TTree * tree,
					    std::string const & draw,
					    std::string const & binning,
					    std::string const & weight) const {
  
  std::vector<double> results;

  TCanvas * canvas = new TCanvas("temp");
  tree->Draw((draw + ">>h" + binning).c_str(), weight.c_str());
  TH1 * h = (TH1*)gDirectory->Get("h");

  if(h->GetEntries() == 0){
    results.push_back(-DBL_MAX);
    results.push_back(-DBL_MAX);
  }
  else {
    results.push_back(h->GetMean());
    results.push_back(double(h->GetBinContent(h->GetNbinsX())) / h->GetEntries());
  }

  delete h;
  delete canvas;

  return results;

}


void VertexQuality::GetMinMax(int const permutation_index,
			      std::vector<double> const & results,
			      std::vector<std::pair<double, int> > & max_results,
			      std::vector<std::pair<double, int> > & min_results) const {

  for(size_t k = 0; k < results.size(); ++k) {

    double const result = results.at(k);
    
    if(result > max_results.at(k).first) {
      max_results.at(k).first = result;
      max_results.at(k).second = permutation_index;
    }
    
    if(result < min_results.at(k).first) {
      min_results.at(k).first = result;
      min_results.at(k).second = permutation_index;
    }
    
  }

}


void VertexQuality::GetDrawnValues(TTree * tree,
				   std::vector<std::vector<double>> const & permutation_v,
				   std::vector<std::vector<std::string>> const & draw_vec,
				   std::vector<std::vector<std::vector<double> > > & drawn_values,
				   std::vector<std::vector<std::pair<double, int> > > & max_results,
				   std::vector<std::vector<std::pair<double, int> > > & min_results,
				   bool const fill_drawn_values) const {
  
  if(fill_drawn_values) {
    drawn_values.clear();
    drawn_values.reserve(draw_vec.size());
    for(size_t i = 0; i < draw_vec.size(); ++i) {
      drawn_values.push_back({});
      drawn_values.back().reserve(permutation_v.size());
    }
  }

  max_results.clear();
  max_results.resize(draw_vec.size(), {fperformance_quantities.size(), {0, -1}});
  min_results.clear();
  min_results.resize(draw_vec.size(), {fperformance_quantities.size(), {DBL_MAX, -1}});

  for(size_t i = 0; i < permutation_v.size(); ++i) {

    std::vector<double> const & permutation = permutation_v.at(i);
    std::string const perm_weight = GetPermString(permutation);

    for(size_t j = 0; j < draw_vec.size(); ++j) {

      std::vector<std::string> const & draw = draw_vec.at(j);
      std::string const & draw_weight = draw.at(2);
      std::string modified_weight = perm_weight;
      if(!draw_weight.empty()) modified_weight += " && " + draw_weight;

      if(fill_drawn_values) drawn_values.at(j).push_back(DrawHist(tree, draw.at(0), draw.at(1), modified_weight));
      std::vector<double> const & results = drawn_values.at(j).back();

      if(results.size() != max_results.at(j).size()) {
	std::cout << "WARNING: results size != max_results size\n";
	continue;
      }

      if(results.size() != min_results.at(j).size()) {
	std::cout << "WARNING: results size != min_results size\n";
	continue;
      }

      GetMinMax(i,
		results,
		max_results.at(j),
		min_results.at(j));

    }

  }

}


void VertexQuality::Print(std::vector<std::vector<double>> const & permutation_v,
			  std::vector<std::vector<std::string>> const & draw_vec,
			  std::vector<std::vector<std::vector<double> > > const & drawn_values,
			  std::vector<std::vector<std::pair<double, int> > > const & max_results,
			  std::vector<std::vector<std::pair<double, int> > > const & min_results) const {
  
  std::cout << "\n";
  for(size_t i = 0; i < drawn_values.size(); ++i) {
    std::vector<std::string> const & draw = draw_vec.at(i);
    std::vector<std::vector<double>> const & drawn_value = drawn_values.at(i);
    for(std::string const & option : draw) std::cout << option << " | ";
    std::cout << "\n";
    /*
    for(size_t j = 0; j < permutation_v.size(); ++j) {
      std::vector<double> const & permutation = permutation_v.at(j);
      std::cout << "Drawn value: ";
      for(double const d : drawn_value.at(j)) std::cout << d << " ";
      for(size_t k = 0; k < permutation.size(); ++k) std::cout << fparameter_name.at(k) << ": " << permutation.at(k) << " ";
      std::cout << "\n";
    }
    */
    for(size_t j = 0; j < fperformance_quantities.size(); ++j) {
      std::cout << fperformance_quantities.at(j) << "\n";
      if(max_results.at(i).at(j).second != -1) {
	std::cout << "max: " << max_results.at(i).at(j).first;
	for(double const value : permutation_v.at(max_results.at(i).at(j).second)) std::cout << " " << value;
      }
      else std::cout << "No max value found";
      std::cout << "\n";
      if(min_results.at(i).at(j).second != -1) {
	std::cout << "min: " << min_results.at(i).at(j).first;
	for(double const value : permutation_v.at(min_results.at(i).at(j).second)) std::cout << " " << value;
      }
      else std::cout << "No min value found";
      std::cout << "\n";
    }
    std::cout << "\n";

  }

}


void VertexQuality::Evaluate() {

  if(fdraw_vec.empty()) {
    std::cout << "No evaluation requested\n";
    return;
  }
  if(fpermutation_v.empty()) {
    std::cout << "No permuations found\n";
    fpermutation_v.push_back({0, 0, 0, 0, 0});
  }

  std::vector<std::vector<std::vector<double> > > drawn_values;
  std::vector<std::vector<std::pair<double, int> > > max_results;
  std::vector<std::vector<std::pair<double, int> > > min_results;
  
  TTree * eval_tree = SetupEvalTree(drawn_values);
  eval_tree->Branch("performance_quantities", &fperformance_quantities);

  GetDrawnValues(fvertex_tree,
		 fpermutation_v,
		 fdraw_vec,
		 drawn_values,
		 max_results,
		 min_results);   

  Print(fpermutation_v,
	fdraw_vec,
	drawn_values,
	max_results,
	min_results);

  eval_tree->Fill();
  eval_tree->Write();

  delete eval_tree;

}


void VertexQuality::Write() const {

  if(fevent_tree) fevent_tree->Write();
  if(fvertex_tree) fvertex_tree->Write();
  else std::cout << "WARNING: call to write vertex quality tree with nullptr\n";

}
