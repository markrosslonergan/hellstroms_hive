

#include "PandoraAnalyzer.h"


PandoraAnalyzer::PandoraAnalyzer() :
  ftree(new TTree("PandoraAnalyzer_tree", "")) {

  ftree->Branch("dist", &dist, "dist/D");
  ftree->Branch("distx", &distx, "distx/D");
  ftree->Branch("disty", &disty, "disty/D");
  ftree->Branch("distz", &distz, "distz/D");
  
}


void PandoraAnalyzer::Run() {

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

    double closest_dist = DBL_MAX;

    for(size_t j = 0; j < true_nuvert_v.size(); ++j) {

      geoalgo::Point_t const & true_nuvert = true_nuvert_v.at(j);

      dist = true_nuvert.Dist(reco_vert);

      if(dist < closest_dist) {
	closest_dist = dist;
	distx = true_nuvert.at(0) - reco_vert.at(0);
	disty = true_nuvert.at(1) - reco_vert.at(1);
	distz = true_nuvert.at(2) - reco_vert.at(2);
      }

    }

    ftree->Fill();

  }

}


void PandoraAnalyzer::Finalize() {

  if(fofile) {
    if(ftree) ftree->Write();
  }

}
