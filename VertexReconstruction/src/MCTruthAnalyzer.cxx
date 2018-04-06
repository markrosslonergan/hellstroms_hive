

#include "MCTruthAnalyzer.h"


MCTruthAnalyzer::MCTruthAnalyzer() :
  ftree(new TTree("MCTruthAnalyzer_tree", "")) {

  ftree->Branch("status", &fstatus, "status/I");
  ftree->Branch("pdg", &fpdg, "pdg/I");

  ftree->Branch("nux_dist", &fnux_dist, "nux_dist/D");
  ftree->Branch("nuy_dist", &fnuy_dist, "nuy_dist/D");
  ftree->Branch("nuz_dist", &fnuz_dist, "nuz_dist/D");
  
}


void MCTruthAnalyzer::Run() {

  std::vector<double> const & true_nuvertx = *fstorage->ftrue_nuvertx;
  std::vector<double> const & true_nuverty = *fstorage->ftrue_nuverty;
  std::vector<double> const & true_nuvertz = *fstorage->ftrue_nuvertz;

  for(size_t i = 0; i < true_nuvertx.size(); ++i) {

    double const nux = true_nuvertx.at(i);
    double const nuy = true_nuverty.at(i);
    double const nuz = true_nuvertz.at(i);

    std::vector<int> const & genie_particle_StatusCode = fstorage->fgenie_particle_StatusCode->at(i);
    std::vector<int> const & genie_particle_PdgCode = fstorage->fgenie_particle_PdgCode->at(i);

    std::vector<double> const & genie_particle_X = fstorage->fgenie_particle_X->at(i);
    std::vector<double> const & genie_particle_Y = fstorage->fgenie_particle_Y->at(i);
    std::vector<double> const & genie_particle_Z = fstorage->fgenie_particle_Z->at(i);

    std::cout << "\n";

    for(size_t j = 0; j < genie_particle_StatusCode.size(); ++j) {

      fstatus = genie_particle_StatusCode.at(j);
      
      //if(fstatus != 1) continue;

      fpdg = genie_particle_PdgCode.at(j);

      fnux_dist = genie_particle_X.at(j) - nux;
      fnuy_dist = genie_particle_Y.at(j) - nuy;
      fnuz_dist = genie_particle_Z.at(j) - nuz;

      ftree->Fill();

      std::cout << fpdg << " " << genie_particle_X.at(j) << " " << genie_particle_Y.at(j) << " " << genie_particle_Z.at(j) << "\n";

    }

  }

  std::vector<std::vector<int>> const & reco_shower_mc_index = *fstorage->freco_shower_mc_index;
  std::cout << reco_shower_mc_index.size() << "\n";
  for(size_t i = 0; i < reco_shower_mc_index.size(); ++i) {
    for(auto const & v : reco_shower_mc_index.at(i)) {
      std::cout << v << " ";
    }
    std::cout << "\n";
  }
  std::cout << "\n";

}


void MCTruthAnalyzer::Finalize() {

  ftree->Write();

}
