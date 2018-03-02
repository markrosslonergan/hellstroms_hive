
#ifndef DETECTOROBJECTS_CXX
#define DETECTOROBJECTS_CXX

#include "DetectorObjects.h"


DetectorObjects::DetectorObjects() :
  fobject_id(0),
  ftrack_reco_type(2),
  fshower_reco_type(1){}


void DetectorObjects::Clear() {

  for(std::pair<size_t, DetectorObject *> const & p : fobject_m) delete p.second;
  fobject_m.clear();
  ftrack_index_v.clear();
  fshower_index_v.clear();
  fobject_id = 0;

  foriginal_track_index_m.clear();
  foriginal_shower_index_m.clear();

}


void DetectorObjects::SetAssociated(size_t const i) {

  auto om_it = fobject_m.find(i);
  
  if(om_it == fobject_m.end()) {
    std::cout << __PRETTY_FUNCTION__ << "\nCould not find object: " << i << std::endl;
    exit(1);
  }
  
  om_it->second->fis_associated = true;
  
}


int DetectorObjects::GetRecoType(size_t const i) const {

  auto const om_it = fobject_m.find(i);
  
  if(om_it == fobject_m.end()) {
    std::cout << __PRETTY_FUNCTION__ << "\nCould not find object: " << i << std::endl;
    exit(1);
  }

  return om_it->second->freco_type;

}


void DetectorObjects::AddTrack(size_t const reco_index, geoalgo::Trajectory const & traj, bool const track_original_indices) {

  fobject_m.emplace(fobject_id, new Track(fobject_id, reco_index, ftrack_reco_type, traj)); 
  ftrack_index_v.push_back(fobject_id);
  if(track_original_indices) foriginal_track_index_m.emplace(reco_index, fobject_id);
  ++fobject_id;

}


void DetectorObjects::AddShower(size_t const reco_index, geoalgo::Cone_t const & cone, bool const track_original_indices) {

  fobject_m.emplace(fobject_id, new Shower(fobject_id, reco_index, fshower_reco_type, cone));
  fshower_index_v.push_back(fobject_id);
  if(track_original_indices) foriginal_shower_index_m.emplace(reco_index, fobject_id);
  ++fobject_id;

}


DetectorObject const & DetectorObjects::GetDetectorObject(size_t const i) const {

  auto const om_it = fobject_m.find(i);
  
  if(om_it == fobject_m.end()) {
    std::cout << __PRETTY_FUNCTION__ << "\nCould not find object: " << i << std::endl;
    exit(1);
  }
  
  return *om_it->second;

}


Track & DetectorObjects::GetTrack(size_t const i) {
  
  auto const om_it = fobject_m.find(i);
  
  if(om_it == fobject_m.end()) {
    std::cout << __PRETTY_FUNCTION__ << "\nCould not find object: " << i << std::endl;
    exit(1);
  }
  
  Track * t = dynamic_cast<Track *>(fobject_m.find(i)->second);
  
  if(!t) {
    std::cout << __PRETTY_FUNCTION__ << "\nCould not convert: " << i << std::endl;
    exit(1);
  }
  
  return *t;

}


Track const & DetectorObjects::GetTrack(size_t const i) const {
  
  auto const om_it = fobject_m.find(i);
  
  if(om_it == fobject_m.end()) {
    std::cout << __PRETTY_FUNCTION__ << "\nCould not find object: " << i << std::endl;
    exit(1);
  }
  
  Track * t = dynamic_cast<Track *>(fobject_m.find(i)->second);
  
  if(!t) {
    std::cout << __PRETTY_FUNCTION__ << "\nCould not convert: " << i << " to track" << std::endl;
    exit(1);
  }
  
  return *t;

}


Shower & DetectorObjects::GetShower(size_t const i) {
  
  auto const om_it = fobject_m.find(i);
  
  if(om_it == fobject_m.end()) {
    std::cout << __PRETTY_FUNCTION__ << "\nCould not find object: " << i << std::endl;
    exit(1);
  }
  
  Shower * s = dynamic_cast<Shower *>(fobject_m.find(i)->second);
  
  if(!s) {
    std::cout << __PRETTY_FUNCTION__ << "\nCould not convert: " << i << " to shower" << std::endl;
    exit(1);
  }
  
  return *s;

}


Shower const & DetectorObjects::GetShower(size_t const i) const {
  
  auto const om_it = fobject_m.find(i);
  
  if(om_it == fobject_m.end()) {
    std::cout << __PRETTY_FUNCTION__ << "\nCould not find object: " << i << std::endl;
    exit(1);
  }
  
  Shower * s = dynamic_cast<Shower *>(fobject_m.find(i)->second);
  
  if(!s) {
    std::cout << __PRETTY_FUNCTION__ << "\nCould not find convert: " << i << std::endl;
    exit(1);
  }
  
  return *s;

}


size_t DetectorObjects::GetTrackIndexFromOriginalIndex(size_t const i) const {

  auto om_it = foriginal_track_index_m.find(i);
  
  if(om_it == foriginal_track_index_m.end()) {
    std::cout << __PRETTY_FUNCTION__ << "\nCould not find object associated to original index: " << i << std::endl;
    exit(1);
  }

  return om_it->second;

}


size_t DetectorObjects::GetShowerIndexFromOriginalIndex(size_t const i) const {

  auto om_it = foriginal_shower_index_m.find(i);
  
  if(om_it == foriginal_shower_index_m.end()) {
    std::cout << __PRETTY_FUNCTION__ << "\nCould not find object associated to original index: " << i << std::endl;
    exit(1);
  }

  return om_it->second;

}


#endif
