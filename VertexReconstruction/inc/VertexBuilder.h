

#ifndef VERTEXBUILDER_H
#define VERTEXBUILDER_H

#include "ParticleAssociations.h"

#include "GeoAlgo.h"


class VertexBuilder {

  geoalgo::GeoAlgo const falgo;

  double fstart_prox;
  double fshower_prox;
  double fmax_bp_dist;
  double fcpoa_vert_prox; 
  double fcpoa_trackend_prox;

  DetectorObjects const * fdetos;

  bool fshower_score;
  bool fverbose;

  bool CheckSetVariables();

  void Erase(std::multimap<size_t, geoalgo::Point_t const *> & pn,
	     std::multimap<size_t, geoalgo::Point_t const *>::iterator const best_it,
	     geoalgo::Point_t const & sv);

  double FindClosestApproach(geoalgo::HalfLine_t const & shr1,
			     geoalgo::HalfLine_t const & shr2,
			     geoalgo::Point_t & PtShr1,
			     geoalgo::Point_t & PtShr2) const;
  double FindClosestApproach(const geoalgo::HalfLine_t & shr1,
			     const geoalgo::HalfLine_t & shr2,
			     geoalgo::Point_t & vtx) const;
  double FindClosestApproach(geoalgo::Trajectory const & traj,
			     geoalgo::HalfLine_t const & shr,
			     geoalgo::Point_t & vtx) const;
  double FindClosestApproach(geoalgo::Point_t const & pav,
			     geoalgo::HalfLine_t const & shr,
			     geoalgo::Point_t & vtx) const;
  double GetShowerAssociationScore(geoalgo::HalfLine_t const & shr1,
				   geoalgo::HalfLine_t const & shr2,
				   geoalgo::Point_t & vtx) const;
  bool ConeCheck(geoalgo::Cone_t const & cone,
		 geoalgo::Point_t const & x) const;
  void AssociateShowers(ParticleAssociations & pas);
  void AddLoneTracks(ParticleAssociations & pas);
  void AddLoneShowers(ParticleAssociations & pas);

 public:

  VertexBuilder();

  void AssociateTracks(ParticleAssociations & pas);

  void SetDetectorObjects(DetectorObjects const * detos) {
    fdetos = detos;
  }
  
  void SetShowerScore(bool const shower_score = true) {
    fshower_score = shower_score;
  }
  void SetVerbose(bool const verbose = true) {
    fverbose = verbose;
  }

  void SetMaximumTrackEndProximity(double const start_prox) {
    fstart_prox = start_prox;
  }

  void SetMaximumShowerIP(double const shower_prox) {
    fshower_prox = shower_prox;
  }

  void SetMaximumBackwardsProjectionDist(double const max_bp_dist) {
    fmax_bp_dist = max_bp_dist;
  }

  void CPOAToVert(double const cpoa_vert_prox) {
    fcpoa_vert_prox = cpoa_vert_prox;
  }

  void SetMaximumTrackEndProx(double const cpoa_trackend_prox) {
    fcpoa_trackend_prox = cpoa_trackend_prox;
  }

  void Run(ParticleAssociations & pas);

};


#endif
