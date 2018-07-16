

#include "Pi0Filter.h"


Pi0Filter::Pi0Filter() {
    
}


bool Pi0Filter::Run() {

  std::vector<int> const & ccnc = *fstorage->fccnc;

  if(ccnc.front() == 0) return false;

  std::vector<int> const & reco_shower_largest_mc_type = *fstorage->freco_shower_largest_mc_type;
  std::vector<int> const & reco_shower_largest_mc_index = *fstorage->freco_shower_largest_mc_index;
  auto & shower_indices = fstorage->GetShowerIndices("pandoraNu");
  
  std::vector<int> const & mcshower_PdgCode = *fstorage->fmcshower_PdgCode;
  std::vector<int> const & mcshower_MotherPdgCode = *fstorage->fmcshower_MotherPdgCode;

  bool pass = false;

  for(size_t s = shower_indices.first; s < shower_indices.second; ++s) {
    if(reco_shower_largest_mc_type.at(s) != 1) continue;
    size_t const mcs = reco_shower_largest_mc_index.at(s);
    if(mcshower_PdgCode.at(mcs) != 22 || mcshower_MotherPdgCode.at(mcs) != 111) continue;
    pass = true;
  }
  
  return pass;

}


void Pi0Filter::Finalize() {

}
