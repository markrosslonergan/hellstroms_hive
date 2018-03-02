

#include "VRAnalyzer.h"


VRAnalyzer::VRAnalyzer() :
  fverbose(false) {}


void VRAnalyzer::SetVerbose(bool const verbose) {

  fverbose = verbose;

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


void VRAnalyzer::RunVertexQuality(bool const run_vertex_quality) {

  frun_vertex_quality = run_vertex_quality;

}


void VRAnalyzer::Initialize() {

  fmcordata = fstorage->fmc;

  if(frun_vertex_quality) {
    fvq.SetStorage(fstorage);
    fvq.SetParameters(fstart_prox,
		      fshower_prox,
		      fmax_bp_dist,
		      fcpoa_vert_prox,
		      fcpoa_trackend_prox);
  }

  fftv.SetVerbose(fverbose);
  fftv.SetProducers(fstorage,
		    fstorage->fmc,
                    ftrack_producer,
                    fshower_producer,
                    fhit_producer,
                    fopflash_producer,
                    fswtrigger_product);
  fftv.SetupTreeBranches();

}


void VRAnalyzer::AddTracks(DetectorObjects & detos,
			   std::string const & producer,
			   bool const track_original_indices) {

  std::vector<std::vector<double>> const & reco_track_X = *fstorage->freco_track_X;
  std::vector<std::vector<double>> const & reco_track_Y = *fstorage->freco_track_Y;
  std::vector<std::vector<double>> const & reco_track_Z = *fstorage->freco_track_Z;

  auto & track_indices = fstorage->GetTrackIndices(ftrack_producer);

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

  auto & shower_indices = fstorage->GetShowerIndices(fshower_producer);
  
  for(size_t i = shower_indices.first; i < shower_indices.second; ++i) {
    detos.AddShower(i, 
		    {{reco_shower_ShowerStart_X.at(i), reco_shower_ShowerStart_Y.at(i), reco_shower_ShowerStart_Z.at(i)},
		     {reco_shower_Direction_X.at(i), reco_shower_Direction_Y.at(i), reco_shower_Direction_Z.at(i)}, 
		     reco_shower_Length.at(i),
		     0},
		    track_original_indices);
  }

}


void VRAnalyzer::Run() {

  //Runs the vertex builder
  VertexBuilder vb;
  vb.SetMaximumTrackEndProximity(fstart_prox);
  vb.SetMaximumShowerIP(fshower_prox);
  vb.SetMaximumBackwardsProjectionDist(fmax_bp_dist);
  vb.CPOAToVert(fcpoa_vert_prox);
  vb.SetMaximumTrackEndProx(fcpoa_trackend_prox);

  ParticleAssociations pas;
  AddTracks(pas.GetDetectorObjects(), "pandoraNu");
  AddShowers(pas.GetDetectorObjects(), "pandoraNu");
  vb.Run(pas);

  if(frun_vertex_quality) {
    if(fstorage->frmcm_bool) {
      fvq.RunClosest(pas);
    }
    else {
      std::cout << "Asked to run VertexQuality using file with no reco - mc matching\n";
    }
  }

  fftv.Fill(pas);
  
}


void VRAnalyzer::Finalize() {

  if(fofile) {

    fftv.GetTree()->Write();

  }

}
