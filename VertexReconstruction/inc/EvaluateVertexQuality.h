

#ifndef EVALUATEVERTEXQUALITY_H
#define EVALUATEVERTEXQUALITY_H


#include <vector>
#include <iostream>

#include "TChain.h"
#include "TFile.h"


class EvaluateVertexQuality {

 public:

  EvaluateVertexQuality(char const * vq_name,
			char const * perm_name,
			std::vector<char const *> const & files = {});

  void Run();

 private:

  bool CheckFile(char const * vq_name,
		 std::vector<char const *> const & files) const;
  void Initialize();
  void AddFiles(std::vector<char const *> const & files);  
  void SetupChain();
  bool FillPermutationV(bool & not_consecutive);

  std::vector<std::vector<double>> fpermutation_v;

  TChain * fvq_chain;
  TChain * fperm_chain;

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

  std::vector<double> * ftrack_matching_ratio_v;
  std::vector<int> * ftrack_true_pdg_v;
  std::vector<int> * ftrack_true_origin_v;

  std::vector<double> * fshower_matching_ratio_v;
  std::vector<int> * fshower_true_pdg_v;
  std::vector<int> * fshower_true_origin_v;

};


#endif
