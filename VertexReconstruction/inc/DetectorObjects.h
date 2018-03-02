

#ifndef DETECTOROBJECTS_H
#define DETECTOROBJECTS_H

#include <map>

#include "GeoTrajectory.h"
#include "GeoCone.h"


struct DetectorObject {

  size_t const fid;
  size_t const foriginal_index;
  int const freco_type;
  bool fis_associated;
  
  DetectorObject(size_t const id, size_t const original_index, int const reco_type) :
    fid(id),
    foriginal_index(original_index),
    freco_type(reco_type),
    fis_associated(false) {}
  
  virtual ~DetectorObject(){}
  
};


struct Track : public DetectorObject {
  
  geoalgo::Trajectory const ftrajectory;
  
 Track(size_t const id, size_t const original_index, int const reco_type, geoalgo::Trajectory const & traj) :
  DetectorObject(id, original_index, reco_type),
    ftrajectory(traj) {}
  
};


struct Shower : public DetectorObject {
  
  geoalgo::Cone const fcone;
  
 Shower(size_t const id, size_t const original_index, int const reco_type, geoalgo::Cone_t const & cone) :
  DetectorObject(id, original_index, reco_type),
    fcone(cone) {}
  
};


class DetectorObjects {

  std::map<size_t, DetectorObject *> fobject_m;
  std::vector<size_t> ftrack_index_v;
  std::vector<size_t> fshower_index_v;
  size_t fobject_id;

  std::map<size_t, size_t> foriginal_track_index_m;
  std::map<size_t, size_t> foriginal_shower_index_m;

public:

  int const ftrack_reco_type;
  int const fshower_reco_type;

  DetectorObjects();

  ~DetectorObjects() {
    for(std::pair<size_t, DetectorObject *> const & p : fobject_m) delete p.second;
  }

  void Clear();

  void AddTrack(size_t const reco_index, geoalgo::Trajectory const & traj, bool const track_original_indices = false);
  void AddShower(size_t const reco_index, geoalgo::Cone_t const & cone, bool const track_original_indices = false);

  void SetAssociated(size_t const i);
  
  int GetRecoType(size_t const i) const;
  std::vector<size_t> const & GetTrackIndices() const {return ftrack_index_v;}
  std::vector<size_t> const & GetShowerIndices() const {return fshower_index_v;}

  DetectorObject const & GetDetectorObject(size_t const i) const;

  Track & GetTrack(size_t const i);
  Track const & GetTrack(size_t const i) const;

  Shower & GetShower(size_t const i);
  Shower const & GetShower(size_t const i) const;
  
  size_t GetTrackIndexFromOriginalIndex(size_t const i) const;
  size_t GetShowerIndexFromOriginalIndex(size_t const i) const;

};


#endif
