

#ifndef VERTEXQUALITY_H
#define VERTEXQUALITY_H


#include "Storage.h"
#include "ParticleAssociations.h"

#include "TTree.h"
#include "TCanvas.h"
#include "TH1.h"

#include "GeoAABox.h"


class VertexQuality {

 public:

  VertexQuality(std::string const & name = "VertexQuality", bool const track_only = false);
  ~VertexQuality();

  void FillEventTree(bool const fill_event_tree = true);
  std::vector<std::string> const & GetPerformanceQuantities();
  void SetProducers(std::string const & track_producer,
		    std::string const & shower_producer);
  void AddPermutations(std::vector<std::vector<double>> const & permutation_v);
  void AddPerformanceMetric(std::vector<std::string> const & param);
  void SetParameters(double const start_prox,
                     double const shower_prox,
                     double const max_bp_dist,
                     double const cpoa_vert_prox,
                     double const cpoa_trackend_prox);

  void SetStorage(Storage const * storage);
  void RunShowerMatch();
  void Run(ParticleAssociations const & pas);
  std::vector<double> DrawHist(TTree * tree,
			       std::string const & draw,
			       std::string const & binning,
			       std::string const & weight) const;
  void GetMinMax(int const permutation_index,
		 std::vector<double> const & results,
		 std::vector<std::pair<double, int> > & max_results,
		 std::vector<std::pair<double, int> > & min_results) const;
  std::string GetPermString(std::vector<double> const & permutation) const;
  void GetDrawnValues(TTree * tree,
		      std::vector<std::vector<double>> const & permutation_v,
		      std::vector<std::vector<std::string>> const & draw_vec,
		      std::vector<std::vector<std::vector<double>>> & drawn_values,
		      std::vector<std::vector<std::pair<double, int>>> & max_results,
		      std::vector<std::vector<std::pair<double, int>>> & min_results,
		      bool const fill_drawn_values = true) const;
  
  void Print(std::vector<std::vector<double>> const & permutation_v,
	     std::vector<std::vector<std::string>> const & draw_vec,
	     std::vector<std::vector<std::vector<double>>> const & drawn_values,
	     std::vector<std::vector<std::pair<double, int>>> const & max_results,
	     std::vector<std::vector<std::pair<double, int>>> const & min_results) const;
  void Evaluate();
  void Write() const;
  
 private:

  void RunShowerMatch(ParticleAssociations const & pas);
  void SetupVertexQualityTree(TTree * const tree);
  void GetTrueObjects(size_t const mct_index,
		      std::vector<size_t> & mctrack_v,
		      std::vector<size_t> & mcshower_v,
		      std::vector<size_t> & mcparticle_v);
  void GetTrueRecoObjects(size_t const mct_index,
			  std::vector<size_t> & track_v,
			  std::vector<size_t> & shower_v);
  void ResetEvent();
  void ResetMCTruth();
  void ResetVertex();

  void FillTree(TTree * tree,
		ParticleAssociations const & pas,
		size_t const pa_index,
		geoalgo::Point_t const & true_nu_vtx,
		std::vector<size_t> const & track_v,
		std::vector<size_t> const & shower_v);
  TTree * SetupEvalTree(std::vector<std::vector<std::vector<double>>> & drawn_values);

  std::string const fname;
  bool const ftrack_only;
  std::string ftrack_producer;
  std::string fshower_producer;
  std::vector<std::vector<std::string>> fdraw_vec;
  std::vector<std::string> fperformance_quantities;
  std::vector<std::string> fparameter_name;

  Storage const * fstorage;

  int fmc_type_shower;
  int fmc_type_track;
  int fmc_type_particle;

  geoalgo::AABox ftpc_volume;

  TTree * fevent_tree;
  TTree * fvertex_tree;

  bool frun_shower_match;

  int frun_number;
  int fsubrun_number;
  int fevent_number;

  double fstart_prox;
  double fshower_prox;
  double fmax_bp_dist;
  double fcpoa_vert_prox;
  double fcpoa_trackend_prox;

  int ftrue_nu_vtx_number;
  int freco_vtx_number;
  int ftrue_associated_shower;
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

  std::vector<std::vector<double>> fpermutation_v;

};


#endif
