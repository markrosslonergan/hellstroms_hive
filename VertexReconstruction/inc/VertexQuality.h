

#ifndef VERTEXQUALITY_H
#define VERTEXQUALITY_H


#include "Storage.h"
#include "ParticleAssociations.h"

#include "TTree.h"

#include "GeoAABox.h"


class VertexQuality {

 public:

  VertexQuality();
  VertexQuality(std::string const & name);
  ~VertexQuality() {
    if(fvertex_tree) delete fvertex_tree;
    if(fvertex_tree_event) delete fvertex_tree_event;
    if(fvertex_tree_event_signal) delete fvertex_tree_event_signal;
  }

  void SetProducers(std::string const & track_producer,
		    std::string const & shower_producer);

  void SetParameters(double const start_prox,
                     double const shower_prox,
                     double const max_bp_dist,
                     double const cpoa_vert_prox,
                     double const cpoa_trackend_prox);

  void SetStorage(Storage const * storage);

  void SetupVertexQualityTreeClosest();
  void SetupVertexQualityTreeSignal();
  void RunClosest(ParticleAssociations const & pas,
		  bool const track_only = false);
  void RunSig(ParticleAssociations const & pas,
	      bool const track_only = false);
  void Write() const {
    if(fvertex_tree) fvertex_tree->Write();
    if(fvertex_tree_event) fvertex_tree_event->Write();
    if(fvertex_tree_event_signal) fvertex_tree_event_signal->Write();
  }

 private:

  void GetTrueObjects(size_t const mct_index,
		      std::vector<size_t> & mctrack_v,
		      std::vector<size_t> & mcshower_v,
		      std::vector<size_t> & mcparticle_v);
  void GetTrueRecoObjects(size_t const mct_index,
			  std::vector<size_t> & track_v,
			  std::vector<size_t> & shower_v);
  void Reset();
  void FillTree(TTree * tree,
		ParticleAssociations const & pas,
		size_t const pa_index,
		geoalgo::Point_t const & true_nu_vtx,
		std::vector<size_t> const & track_v,
		std::vector<size_t> const & shower_v);

  std::string fname;
  std::string ftrack_producer;
  std::string fshower_producer;

  Storage const * fstorage;

  int fmc_type_shower;
  int fmc_type_track;
  int fmc_type_particle;

  geoalgo::AABox ftpc_volume;

  TTree * fvertex_tree;
  TTree * fvertex_tree_event;
  TTree * fvertex_tree_event_signal;

  double fstart_prox;
  double fshower_prox;
  double fmax_bp_dist;
  double fcpoa_vert_prox;
  double fcpoa_trackend_prox;

  int freco_vertex_present;
  int fis_nc_delta_rad;
  int fnc_delta_rad_split_shower;
  int ftpc_volume_contained;

  double fdist;
  double fdistx;
  double fdisty;
  double fdistz;

  int ftrue_track_total;
  int ftrue_shower_total;
  int freco_track_total;
  int freco_shower_total;
  int fcorrect_track_total;
  int fcorrect_shower_total;

  std::vector<double> ftrack_matching_ratio_v;
  std::vector<int> ftrack_true_pdg_v;
  std::vector<int> ftrack_true_origin_v;

  std::vector<double> fshower_matching_ratio_v;
  std::vector<int> fshower_true_pdg_v;
  std::vector<int> fshower_true_origin_v;

};


#endif
