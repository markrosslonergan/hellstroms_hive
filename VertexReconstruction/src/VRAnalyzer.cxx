

#include "VRAnalyzer.h"


VRAnalyzer::VRAnalyzer(std::string const & name, VertexQuality * vq) :
  fname(name),
  fverbose(false),
  fstart_prox(-10000),
  fshower_prox(-10000),
  fmax_bp_dist(-10000),
  fcpoa_vert_prox(-10000),
  fcpoa_trackend_prox(-10000),
  frun_pandora(false),
  fget_pot(false),
  frun_fill_tree_variables(false),
  fvq(vq),
  fvertices_per_event_tree(nullptr) {}


VRAnalyzer::~VRAnalyzer() {

  if(fvertices_per_event_tree) delete fvertices_per_event_tree;
  
}


void VRAnalyzer::SetVerbose(bool const verbose) {

  fverbose = verbose;

}

void VRAnalyzer::SetFlashTimings(double flash_start, double flash_end){
	fflash_start = flash_start;
	fflash_end = flash_end;
}



void VRAnalyzer::SetProducers(std::string const & track_producer,
			      std::string const & shower_producer,
			      std::string const & hit_producer,
			      std::string const & opflash_producer,
			      std::string const & trigger_product) {

  ftrack_producer = track_producer;
  fshower_producer = shower_producer;
  fhit_producer = hit_producer;
  fopflash_producer = opflash_producer;
  fswtrigger_product = trigger_product;

}


void VRAnalyzer::SetVBVariables(double const start_prox,
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


void VRAnalyzer::RunPandora(bool const run_pandora) {

  frun_pandora = run_pandora;

}


void VRAnalyzer::GetPOT(bool const get_pot) {

  fget_pot = get_pot;

}


void VRAnalyzer::RunFillTreeVariables(bool const run_fill_tree_variables) {

  frun_fill_tree_variables = run_fill_tree_variables;

}


void VRAnalyzer::RunVerticesPerEvent() {

  if(fvertices_per_event_tree) {
    std::cout << "vertices_per_event_tree already exists\n";
    return;
  }

  fvertices_per_event_tree = new TTree("vertices_per_event", "");
  fvertices_per_event_tree->Branch("run_number", &frun_number, "run_number/I");
  fvertices_per_event_tree->Branch("subrun_number", &fsubrun_number, "subrun_number/I");
  fvertices_per_event_tree->Branch("event_number", &fevent_number, "event_number/I");
  fvertices_per_event_tree->Branch("vertex_number", &fvertex_number, "vertex_number/I");

}


void VRAnalyzer::FillPOTTree() {

  TTree * pot_tree = new TTree("pot_tree", "");

  int number_of_events = fstorage->fnumber_of_events;
  double pot = fstorage->fpot;

  pot_tree->Branch("number_of_events", &number_of_events, "number_of_events/I");
  pot_tree->Branch("pot", &pot, "pot/D");

  pot_tree->Fill();
  pot_tree->Write();
  delete pot_tree;

}


void VRAnalyzer::Initialize() {

  fmcordata = fstorage->fmc;

  if(fget_pot) {
    FillPOTTree();
  }

  if(fvq) {
    fvq->SetStorage(fstorage);
  }

  if(frun_fill_tree_variables) {
    fftv.SetVerbose(fverbose);
    fftv.SetFlashTimings(fflash_start, fflash_end);
    fftv.SetProducers(fstorage,
		      ftrack_producer,
		      fshower_producer,
		      fhit_producer,
		      fopflash_producer,
		      fswtrigger_product);
    fftv.SetupTreeBranches();
  }

}


void VRAnalyzer::AddTracks(DetectorObjects & detos,
			   std::string const & producer,
			   bool const track_original_indices) {

  std::vector<std::vector<double>> const & reco_track_X = *fstorage->freco_track_X;
  std::vector<std::vector<double>> const & reco_track_Y = *fstorage->freco_track_Y;
  std::vector<std::vector<double>> const & reco_track_Z = *fstorage->freco_track_Z;

  auto & track_indices = fstorage->GetTrackIndices(producer);

  for(size_t i = track_indices.first; i < track_indices.second; ++i) {
    
    std::vector<double> const & reco_traj_X = reco_track_X.at(i);
    std::vector<double> const & reco_traj_Y = reco_track_Y.at(i);
    std::vector<double> const & reco_traj_Z = reco_track_Z.at(i);

    geoalgo::Trajectory traj;
    traj.reserve(reco_traj_X.size());
    for(size_t j = 0; j < reco_traj_X.size(); ++j) {
      traj.push_back({reco_traj_X.at(j), reco_traj_Y.at(j), reco_traj_Z.at(j)});
    }    

    detos.AddTrack(i, traj, track_original_indices);
    
  }

}


void VRAnalyzer::AddShowers(DetectorObjects & detos,
			    std::string const & producer,
			    bool const track_original_indices) {

  std::vector<double> const & reco_shower_Direction_X = *fstorage->freco_shower_Direction_X;
  std::vector<double> const & reco_shower_Direction_Y = *fstorage->freco_shower_Direction_Y;
  std::vector<double> const & reco_shower_Direction_Z = *fstorage->freco_shower_Direction_Z;
  std::vector<double> const & reco_shower_ShowerStart_X = *fstorage->freco_shower_ShowerStart_X;
  std::vector<double> const & reco_shower_ShowerStart_Y = *fstorage->freco_shower_ShowerStart_Y;
  std::vector<double> const & reco_shower_ShowerStart_Z = *fstorage->freco_shower_ShowerStart_Z;
  std::vector<double> const & reco_shower_Length = *fstorage->freco_shower_Length;

  auto & shower_indices = fstorage->GetShowerIndices(producer);
  
  for(size_t i = shower_indices.first; i < shower_indices.second; ++i) {
    detos.AddShower(i, 
		    {{reco_shower_ShowerStart_X.at(i), reco_shower_ShowerStart_Y.at(i), reco_shower_ShowerStart_Z.at(i)},
		     {reco_shower_Direction_X.at(i), reco_shower_Direction_Y.at(i), reco_shower_Direction_Z.at(i)}, 
		     reco_shower_Length.at(i),
		     0},
		    track_original_indices);
  }

}


void VRAnalyzer::PrintPandoraRec(size_t const pfp_index, std::string indent) {

  std::vector<int> const & pfp_pdg = *fstorage->fpfp_pdg;
  int const original_index = fstorage->fpfp_original_index->at(pfp_index);
  std::vector<std::vector<int>> const & pfp_children = *fstorage->fpfp_children;

  indent += "\t";
  std::cout << indent << original_index << ": " << pfp_pdg.at(pfp_index) << "\n";
  for(int const dpfp_index : pfp_children.at(pfp_index)) {
    PrintPandoraRec(dpfp_index, indent);
  }

}


void VRAnalyzer::PrintPandoraRec() {

  std::vector<int> const & pfp_pdg = *fstorage->fpfp_pdg;
  std::cout << "Number of pfparticles: " << pfp_pdg.size() << "\n";
  for(size_t i = 0; i < pfp_pdg.size(); ++i) {
    int const pdg = pfp_pdg.at(i);
    if(abs(pdg) != 12 && abs(pdg) != 14) continue;
    PrintPandoraRec(i);
  }
}


geoalgo::Point_t VRAnalyzer::GetSecondaryTrackPandoraVertex(ParticleAssociations const & pas,
							    size_t const track_index,
							    geoalgo::Point_t const & vertex) {

  geoalgo::Point_t const unset_result(-10000, -10000, -10000);
  geoalgo::Point_t result = unset_result;
  
  DetectorObjects const & detos = pas.GetDetectorObjects();

  geoalgo::Trajectory const & traj = detos.GetTrack(track_index).ftrajectory;
  result = traj.front();
  double dist_shortest = result.Dist(vertex);
  double dist_largest = traj.back().Dist(vertex);
  if(dist_shortest < dist_largest) result = traj.back();

  if(result == unset_result) std::cout << "WARNiNG: " << __PRETTY_FUNCTION__ << "\nunset vertex\n";

  return result;
  
}


geoalgo::Point_t VRAnalyzer::GetSecondaryTrackPandoraVertex(ParticleAssociations const & pas,
							    std::vector<size_t> const & track_indices,
							    geoalgo::Point_t const & vertex) {
  geoalgo::Point_t const unset_result(-10000, -10000, -10000);
  geoalgo::Point_t result = unset_result;

  if(track_indices.size() < 2) {
    std::cout << "WARINING: " << __PRETTY_FUNCTION__ << "\n< 2 track indices\n";
    return result;
  } 

  DetectorObjects const & detos = pas.GetDetectorObjects();

  double dist = DBL_MAX;
  geoalgo::Point_t const * verta = nullptr;
  geoalgo::Point_t const * vertb = nullptr;

  for(size_t const i : track_indices) {

    geoalgo::Trajectory const & traja = detos.GetTrack(i).ftrajectory;
    geoalgo::Point_t const & starta = traja.front();
    geoalgo::Point_t const & enda = traja.back();

    for(size_t const j : track_indices) {

      if(j == i) continue;

      geoalgo::Trajectory const & trajb = detos.GetTrack(j).ftrajectory;
      geoalgo::Point_t const & startb = trajb.front();
      geoalgo::Point_t const & endb = trajb.back();	

      if(startb.Dist(starta) < dist) {
	dist = startb.Dist(starta);
	verta = &starta;
	vertb = &startb;
      }

      if(startb.Dist(enda) < dist) {
	dist = startb.Dist(enda);
	verta = &enda;
	vertb = &startb;
      }

      if(endb.Dist(starta) < dist) {
	dist = endb.Dist(starta);
	verta = &starta;
	vertb = &endb;
      }

      if(endb.Dist(enda) < dist) {
	dist = endb.Dist(enda);
	verta = &enda;
	vertb = &endb;
      }

    }

  }

  if(!verta || !vertb) {
    std::cout << "WARINING: " << __PRETTY_FUNCTION__ << "\nFound no closest vertices\n";
  }

  else {
    result = (*verta + *vertb) / 2;
  }

  if(result == unset_result) std::cout << "WARNiNG: " << __PRETTY_FUNCTION__ << "\nunset vertex\n";

  return result;

}


void VRAnalyzer::FillWPandoraTrackRec(ParticleAssociations & pas,
				      size_t const pfp_index,
				      geoalgo::Point_t const & vertex) {
  
  DetectorObjects const & detos = pas.GetDetectorObjects();
  std::vector<size_t> associated_indices;
  std::vector<size_t> associated_track_indices;

  std::vector<int> const & pfp_pdg = *fstorage->fpfp_pdg;
  std::vector<int> const & pfp_original_index = *fstorage->fpfp_original_index;
  std::vector<std::vector<int>> const & pfp_children = *fstorage->fpfp_children;

  int const original_index = detos.GetTrackIndexFromOriginalIndex(pfp_original_index.at(pfp_index));
  associated_indices.push_back(original_index);
  associated_track_indices.push_back(original_index);

  for(int const s : pfp_children.at(pfp_index)) {

    int const coriginal_index = pfp_original_index.at(s);
    if(coriginal_index == -1) {
      std::cout << "WARNING: original_index of pfp: " << s << " == -1\n";
      continue;
    }

    int const dpdg = pfp_pdg.at(s);

    if(abs(dpdg) == 13) {
      size_t const track_index = detos.GetTrackIndexFromOriginalIndex(coriginal_index);
      associated_indices.push_back(track_index);
      associated_track_indices.push_back(track_index);
    }
    else if(abs(dpdg) == 11) associated_indices.push_back(detos.GetShowerIndexFromOriginalIndex(coriginal_index));	
    else std::cout << __LINE__ << " " << __PRETTY_FUNCTION__ << "\nWarning: pfp pdg: " << dpdg << "\n";
    
  }

  if(associated_indices.size() == 1) return;

  geoalgo::Point_t secondary_vertex;
  if(associated_track_indices.size() == 1) secondary_vertex = GetSecondaryTrackPandoraVertex(pas, associated_track_indices.front(), vertex);
  else if(associated_track_indices.size() > 1) secondary_vertex = GetSecondaryTrackPandoraVertex(pas, associated_track_indices, vertex);

  pas.AddAssociation(associated_indices, {}, secondary_vertex);
  
  for(int const s : pfp_children.at(pfp_index)) {
    if(abs(pfp_pdg.at(s)) == 13 && pfp_original_index.at(s) != -1) FillWPandoraTrackRec(pas, s, secondary_vertex);
  }

}


void VRAnalyzer::FillLoneShowers(ParticleAssociations & pas,
				 std::vector<size_t> const & associated_showers) {

  DetectorObjects const & detos = pas.GetDetectorObjects();
  auto & shower_indices = fstorage->GetShowerIndices(fshower_producer);

  std::vector<double> const & reco_shower_ShowerStart_X = *fstorage->freco_shower_ShowerStart_X;
  std::vector<double> const & reco_shower_ShowerStart_Y = *fstorage->freco_shower_ShowerStart_Y;
  std::vector<double> const & reco_shower_ShowerStart_Z = *fstorage->freco_shower_ShowerStart_Z;

  for(size_t i = shower_indices.first; i < shower_indices.second; ++i) {
    if(std::find(associated_showers.begin(), associated_showers.end(), i) != associated_showers.end()) continue;
    pas.AddAssociation({detos.GetShowerIndexFromOriginalIndex(i)}, {}, {reco_shower_ShowerStart_X.at(i), reco_shower_ShowerStart_Y.at(i), reco_shower_ShowerStart_Z.at(i)});
  }

}


void VRAnalyzer::FillWPandora(ParticleAssociations & pas) {

  DetectorObjects const & detos = pas.GetDetectorObjects();

  std::vector<int> const & pfp_pdg = *fstorage->fpfp_pdg;
  std::vector<double> const & pfp_vertex_X = *fstorage->fpfp_vertex_X;
  std::vector<double> const & pfp_vertex_Y = *fstorage->fpfp_vertex_Y;
  std::vector<double> const & pfp_vertex_Z = *fstorage->fpfp_vertex_Z;
  std::vector<int> const & pfp_original_index = *fstorage->fpfp_original_index;
  std::vector<std::vector<int>> const & pfp_children = *fstorage->fpfp_children;

  std::vector<size_t> associated_showers;

  for(size_t i = 0; i < pfp_pdg.size(); ++i) {

    int const pdg = pfp_pdg.at(i);

    if(abs(pdg) != 12 && abs(pdg) != 14) continue;
    if(fverbose) std::cout << "PFP: " << i << " Parent pdg: " << pdg << "\n";

    geoalgo::Point_t const vertex(pfp_vertex_X.at(i), pfp_vertex_Y.at(i), pfp_vertex_Z.at(i));
    if(fverbose) std::cout << "Vertex:"  << vertex << "\n";

    std::vector<size_t> associated_indices;

    for(int const s : pfp_children.at(i)) {

      int const dpdg = pfp_pdg.at(s);
      int const original_index = pfp_original_index.at(s);
      if(original_index == -1) continue;

      if(abs(dpdg) == 13) {
	associated_indices.push_back(detos.GetTrackIndexFromOriginalIndex(original_index));
	FillWPandoraTrackRec(pas, s, vertex);
      }
      else if(abs(dpdg) == 11) {
	associated_indices.push_back(detos.GetShowerIndexFromOriginalIndex(original_index));
	associated_showers.push_back(original_index);
      }	
      else std::cout << __LINE__ << " " << __PRETTY_FUNCTION__ << "\nWarning: pfp pdg: " << dpdg << "\n";

    }

    if(!associated_indices.empty()) pas.AddAssociation(associated_indices, {}, vertex, 0);

  }

  FillLoneShowers(pas,
		  associated_showers);

  pas.GetShowerAssociations();

}


void VRAnalyzer::VerticesPerEvent(ParticleAssociations const & pas) {

  frun_number = fstorage->frun_number;
  fsubrun_number = fstorage->fsubrun_number;
  fevent_number = fstorage->fevent_number;
  fvertex_number = pas.GetSelectedAssociations().size();
  fvertices_per_event_tree->Fill();

}


bool VRAnalyzer::Run() {

  //Runs the vertex builder
  VertexBuilder vb;
  vb.SetMaximumTrackEndProximity(fstart_prox);
  vb.SetMaximumShowerIP(fshower_prox);
  vb.SetMaximumBackwardsProjectionDist(fmax_bp_dist);
  vb.CPOAToVert(fcpoa_vert_prox);
  vb.SetMaximumTrackEndProx(fcpoa_trackend_prox);

  ParticleAssociations pas;
  if(frun_pandora) {
    AddTracks(pas.GetDetectorObjects(), "pandoraNu", true);
    AddShowers(pas.GetDetectorObjects(), "pandoraNu", true);
    FillWPandora(pas);
  } 
  else {
    AddTracks(pas.GetDetectorObjects(), "pandoraNu");
    AddShowers(pas.GetDetectorObjects(), "pandoraNu");
    vb.Run(pas);
  }
  
  if(fvq) {
    if(fstorage->frmcm_bool) {
      fvq->SetParameters(fstart_prox,
			 fshower_prox,
			 fmax_bp_dist,
			 fcpoa_vert_prox,
			 fcpoa_trackend_prox);      
      fvq->Run(pas);
    }
    else {
      std::cout << "Asked to run VertexQuality using file with no reco - mc matching\n";
    }
  }

  //Testing tuhis bit
  AddTracks(pas.GetDetectorObjects(), "pandoraCosmic");
  AddShowers(pas.GetDetectorObjects(), "pandoraCosmic");

  if(frun_fill_tree_variables){
    fftv.Fill(pas);
  }    

  if(fvertices_per_event_tree) {
    VerticesPerEvent(pas);
  }

  return true;

}


void VRAnalyzer::Finalize() {

  if(fofile) {

    if(frun_fill_tree_variables) fftv.Write();
    if(fvertices_per_event_tree) fvertices_per_event_tree->Write();

  }

}
