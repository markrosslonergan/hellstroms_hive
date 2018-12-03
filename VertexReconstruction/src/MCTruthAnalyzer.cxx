

#include "MCTruthAnalyzer.h"


MCTruthAnalyzer::MCTruthAnalyzer() :
  ftree(new TTree("MCTruthAnalyzer_tree", "")) {

  
  
}


void MCTruthAnalyzer::Run() {

  for(size_t i = 0; i < fstorage->fgenie_particle_PdgCode->size(); ++i) {
    std::unordered_map<int, std::pair<int, std::vector<int> > > particle_map;
    std::vector<int> const & genie_particle_TrackId = fstorage->fgenie_particle_TrackId->at(i);
    std::vector<int> const & genie_particle_PdgCode = fstorage->fgenie_particle_PdgCode->at(i);
    std::vector<int> const & genie_particle_StatusCode = fstorage->fgenie_particle_StatusCode->at(i);
    std::vector<int> const & genie_particle_Mother = fstorage->fgenie_particle_Mother->at(i);
    std::vector<double> const & genie_particle_X = fstorage->fgenie_particle_X->at(i);
    std::vector<double> const & genie_particle_Y = fstorage->fgenie_particle_Y->at(i);
    std::vector<double> const & genie_particle_Z = fstorage->fgenie_particle_Z->at(i);
    std::vector<double> const & genie_particle_E = fstorage->fgenie_particle_E->at(i);

    for(size_t j = 0; j < genie_particle_PdgCode.size(); ++j) {
      std::cout << genie_particle_TrackId[j] << ' ' << genie_particle_PdgCode[j] << ' ' << genie_particle_StatusCode[j] << ' ' 
		<< genie_particle_E[j] << '\n';
    }
    std::cout << '\n';

  }

  /*
  std::vector<int> const & mcparticle_TrackId = *fstorage->fmcparticle_TrackId;
  std::vector<int> const & mcparticle_StatusCode = *fstorage->fmcparticle_StatusCode;
  std::vector<int> const & mcparticle_PdgCode = *fstorage->fmcparticle_PdgCode;
  std::vector<int> const & mcparticle_Mother = *fstorage->fmcparticle_Mother;

  std::unordered_map<int, std::pair<int, std::vector<int> > > particle_map;
    
  for(int i = 0; i < mcparticle_TrackId.size(); ++i) {
    if(mcparticle_PdgCode.at(i) == 111) particle_map[mcparticle_TrackId.at(i)] = std::make_pair(i, std::vector<int>());
  }

  for(int i = 0; i < mcparticle_TrackId.size(); ++i) {

    auto const pm_it = particle_map.find(mcparticle_Mother.at(i));
    if(pm_it == particle_map.end()) continue;

    pm_it->second.second.push_back(i);
      
  }  

  for(auto & p : particle_map) {
    
    int photon_counter = 0;
    
    for(int const i : p.second.second) {
      
    }
      
  }
  */

}


void MCTruthAnalyzer::Finalize() {

  if(fofile) {

    if(ftree) ftree->Write();

  }

}
