

#include "PandoraAnalyzer.h"


PandoraAnalyzer::PandoraAnalyzer() :
  fvertex_tree(new TTree("vertex_tree", "")),
  ftrack_tree(new TTree("track_tree", "")),
  fshower_tree(new TTree("shower_tree", "")) {

  fvertex_tree->Branch("dist", &fdist, "dist/D");
  fvertex_tree->Branch("distx", &fdistx, "distx/D");
  fvertex_tree->Branch("disty", &fdisty, "disty/D");
  fvertex_tree->Branch("distz", &fdistz, "distz/D");

  ftrack_tree->Branch("start_dist", &fstart_dist, "start_dist/D");
  ftrack_tree->Branch("start_distx", &fstart_distx, "start_distx/D");
  ftrack_tree->Branch("start_disty", &fstart_disty, "start_disty/D");
  ftrack_tree->Branch("start_distz", &fstart_distz, "start_distz/D");
  ftrack_tree->Branch("end_dist", &fend_dist, "end_dist/D");
  ftrack_tree->Branch("end_distx", &fend_distx, "end_distx/D");
  ftrack_tree->Branch("end_disty", &fend_disty, "end_disty/D");
  ftrack_tree->Branch("end_distz", &fend_distz, "end_distz/D");

  fshower_tree->Branch("startx", &fstartx, "startx/D");
  fshower_tree->Branch("starty", &fstarty, "starty/D");
  fshower_tree->Branch("startz", &fstartz, "startz/D");
  fshower_tree->Branch("true_pdg", &ftrue_pdg, "true_pdg/I");
  fshower_tree->Branch("true_startx", &ftrue_startx, "true_startx");
  fshower_tree->Branch("true_starty", &ftrue_starty, "true_starty");
  fshower_tree->Branch("true_startz", &ftrue_startz, "true_startz");

}


void PandoraAnalyzer::FillVertexTree() {

  std::vector<double> const & true_nuvertx = *fstorage->ftrue_nuvertx;
  std::vector<double> const & true_nuverty = *fstorage->ftrue_nuverty;
  std::vector<double> const & true_nuvertz = *fstorage->ftrue_nuvertz;
  std::vector<int> const & true_nu_vtx_tpc_contained = *fstorage->ftrue_nu_vtx_tpc_contained;

  std::vector<geoalgo::Point_t> true_nuvert_v;

  for(size_t i = 0; i < true_nuvertx.size(); ++i) {
    if(!true_nu_vtx_tpc_contained.at(i)) continue;
    true_nuvert_v.push_back({true_nuvertx.at(i), true_nuverty.at(i), true_nuvertz.at(i)});
  }

  std::vector<int> const & pfp_pdg = *fstorage->fpfp_pdg;
  std::vector<double> const & pfp_vertex_X = *fstorage->fpfp_vertex_X;
  std::vector<double> const & pfp_vertex_Y = *fstorage->fpfp_vertex_Y;
  std::vector<double> const & pfp_vertex_Z = *fstorage->fpfp_vertex_Z;

  int temp = 0;

  for(size_t i = 0; i < pfp_pdg.size(); ++i) {

    if(abs(pfp_pdg.at(i)) != 12 && abs(pfp_pdg.at(i)) != 14) continue;

    geoalgo::Point_t const reco_vert(pfp_vertex_X.at(i), pfp_vertex_Y.at(i), pfp_vertex_Z.at(i));

    fdist = DBL_MAX;

    for(size_t j = 0; j < true_nuvert_v.size(); ++j) {

      geoalgo::Point_t const & true_nuvert = true_nuvert_v.at(j);

      double dist = true_nuvert.Dist(reco_vert);

      if(dist < fdist) {
	fdist = dist;
	fdistx = true_nuvert.at(0) - reco_vert.at(0);
	fdisty = true_nuvert.at(1) - reco_vert.at(1);
	fdistz = true_nuvert.at(2) - reco_vert.at(2);
      }

    }

    fvertex_tree->Fill();

  }

}


void PandoraAnalyzer::FillTrackTree() {

  std::vector<int> const & reco_track_largest_mc_type = *fstorage->freco_track_largest_mc_type;
  std::vector<int> const & reco_track_largest_mc_index = *fstorage->freco_track_largest_mc_index;
  std::vector<std::vector<double>> const & reco_track_X = *fstorage->freco_track_X;
  std::vector<std::vector<double>> const & reco_track_Y = *fstorage->freco_track_Y;
  std::vector<std::vector<double>> const & reco_track_Z = *fstorage->freco_track_Z;

  std::vector<std::vector<double>> const & mctrack_X = *fstorage->fmctrack_X;
  std::vector<std::vector<double>> const & mctrack_Y = *fstorage->fmctrack_Y;
  std::vector<std::vector<double>> const & mctrack_Z = *fstorage->fmctrack_Z;  

  for(size_t i = 0; i < reco_track_largest_mc_type.size(); ++i) {

    if(reco_track_largest_mc_type.at(i) != 2) continue;
    int const mc_index = reco_track_largest_mc_index.at(i);
    if(mctrack_X.at(mc_index).empty()) continue;

    geoalgo::Point_t const reco_track_start(reco_track_X.at(i).front(), reco_track_Y.at(i).front(), reco_track_Z.at(i).front());
    geoalgo::Point_t const reco_track_end(reco_track_X.at(i).back(), reco_track_Y.at(i).back(), reco_track_Z.at(i).back());

    geoalgo::Point_t const mctrack_start(mctrack_X.at(mc_index).front(), mctrack_Y.at(mc_index).front(), mctrack_Z.at(mc_index).front());
    geoalgo::Point_t const mctrack_end(mctrack_X.at(mc_index).back(), mctrack_Y.at(mc_index).back(), mctrack_Z.at(mc_index).back());

    double const distss = reco_track_start.Dist(mctrack_start);
    double const distes = reco_track_end.Dist(mctrack_start);

    if(distss <= distes) {

      fstart_dist = distss;
      fstart_distx = mctrack_start.at(0) - reco_track_start.at(0);
      fstart_disty = mctrack_start.at(1) - reco_track_start.at(1);
      fstart_distz = mctrack_start.at(2) - reco_track_start.at(2);

      fend_dist = reco_track_end.Dist(mctrack_end);
      fend_distx = mctrack_end.at(0) - reco_track_end.at(0);
      fend_disty = mctrack_end.at(1) - reco_track_end.at(1);
      fend_distz = mctrack_end.at(2) - reco_track_end.at(2);

    }

    else {

      fstart_dist = distes;
      fstart_distx = mctrack_start.at(0) - reco_track_end.at(0);
      fstart_disty = mctrack_start.at(1) - reco_track_end.at(1);
      fstart_distz = mctrack_start.at(2) - reco_track_end.at(2);

      fend_dist = reco_track_start.Dist(mctrack_end);
      fend_distx = mctrack_end.at(0) - reco_track_start.at(0);
      fend_disty = mctrack_end.at(1) - reco_track_start.at(1);
      fend_distz = mctrack_end.at(2) - reco_track_start.at(2);

    }

    ftrack_tree->Fill();

  }

}


void PandoraAnalyzer::FillShowerTree() {

  std::vector<double> const & true_nuvertx = *fstorage->ftrue_nuvertx;
  std::vector<double> const & true_nuverty = *fstorage->ftrue_nuverty;
  std::vector<double> const & true_nuvertz = *fstorage->ftrue_nuvertz;

  std::vector<geoalgo::Point_t> true_nuvert;

  for(size_t mct_index = 0; mct_index < true_nuvertx.size(); ++mct_index) {
    true_nuvert.push_back({true_nuvertx.at(mct_index), true_nuverty.at(mct_index), true_nuvertz.at(mct_index)});
  }

  std::vector<double> const & reco_shower_ShowerStart_X = *fstorage->freco_shower_ShowerStart_X;
  std::vector<double> const & reco_shower_ShowerStart_Y = *fstorage->freco_shower_ShowerStart_Y;
  std::vector<double> const & reco_shower_ShowerStart_Z = *fstorage->freco_shower_ShowerStart_Z;
  std::vector<int> const & reco_shower_largest_mc_type = *fstorage->freco_shower_largest_mc_type;
  std::vector<int> const & reco_shower_largest_mc_index = *fstorage->freco_shower_largest_mc_index;

  auto & shower_indices = fstorage->GetShowerIndices("pandoraNu");
  for(size_t reco_shower_index = shower_indices.first; reco_shower_index < shower_indices.second; ++reco_shower_index) {

    fstartx = reco_shower_ShowerStart_X.at(reco_shower_index);
    fstarty = reco_shower_ShowerStart_Y.at(reco_shower_index);
    fstartz = reco_shower_ShowerStart_Z.at(reco_shower_index);

    int const mc_type = reco_shower_largest_mc_type.at(reco_shower_index);

    if(mc_type == 1) {
      
    }

    fshower_tree->Fill();

  }

}


bool PandoraAnalyzer::Run() {

  FillVertexTree();
  FillTrackTree();
  FillShowerTree();

  return true;

}


void PandoraAnalyzer::Finalize() {

  if(fofile) {
    if(fvertex_tree) fvertex_tree->Write();
    if(ftrack_tree) ftrack_tree->Write();
    if(fshower_tree) fshower_tree->Write();
  }

}
