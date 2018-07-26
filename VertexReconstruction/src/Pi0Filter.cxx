

#include "Pi0Filter.h"
#include "GeoVector.h"


Pi0Filter::Pi0Filter() {
    
}


bool Pi0Filter::Run() {

  std::vector<int> const & reco_shower_largest_mc_type = *fstorage->freco_shower_largest_mc_type;
  if(reco_shower_largest_mc_type.empty()) return false;
  std::vector<int> const & reco_shower_largest_mc_index = *fstorage->freco_shower_largest_mc_index;
  auto & shower_indices = fstorage->GetShowerIndices("pandoraNu");

  std::vector<int> const & ccnc = *fstorage->fccnc;
  std::vector<double> const & true_nuvertx = *fstorage->ftrue_nuvertx;
  std::vector<double> const & true_nuverty = *fstorage->ftrue_nuverty;
  std::vector<double> const & true_nuvertz = *fstorage->ftrue_nuvertz;

  std::vector<int> const & mcshower_Origin = *fstorage->fmcshower_Origin;
  std::vector<int> const & mcshower_PdgCode = *fstorage->fmcshower_PdgCode;
  std::vector<int> const & mcshower_MotherPdgCode = *fstorage->fmcshower_MotherPdgCode;
  std::vector<double> const & mcshower_Start_X = *fstorage->fmcshower_Start_X;
  std::vector<double> const & mcshower_Start_Y = *fstorage->fmcshower_Start_Y;
  std::vector<double> const & mcshower_Start_Z = *fstorage->fmcshower_Start_Z;

  std::vector<geoalgo::Point_t> nc_tru_nuvtx;

  for(size_t i = 0; i < ccnc.size(); ++i) {
    if(ccnc.at(i) != 1) continue;
    nc_tru_nuvtx.push_back({true_nuvertx.at(i), true_nuverty.at(i), true_nuvertz.at(i)});
  }
  if(nc_tru_nuvtx.empty()) return false;

  int counter = 0;

  for(size_t s = shower_indices.first; s < shower_indices.second; ++s) {
    if(reco_shower_largest_mc_type.at(s) != 1) continue;
    size_t const mcs = reco_shower_largest_mc_index.at(s);
    if(mcshower_Origin.at(mcs) != 1 || mcshower_PdgCode.at(mcs) != 22 || mcshower_MotherPdgCode.at(mcs) != 111) continue;
    for(auto const & nuvtx : nc_tru_nuvtx) {
      if(nuvtx.Dist({mcshower_Start_X.at(mcs), mcshower_Start_Y.at(mcs), mcshower_Start_Z.at(mcs)}) < 1e-4) ++counter;
    } 
  }

  return counter > 1;

}


void Pi0Filter::Finalize() {



}
