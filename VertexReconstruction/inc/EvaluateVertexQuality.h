

#ifndef EVALUATEVERTEXQUALITY_H
#define EVALUATEVERTEXQUALITY_H


#include <vector>
#include <iostream>

#include "TChain.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TGraph.h"


class EvaluateVertexQuality {

 public:

  EvaluateVertexQuality(char const * vq_name,
			char const * perm_name,
			std::vector<char const *> const & files = {});
  ~EvaluateVertexQuality();

  void SetOutputFile(char const * file_name);
  void AddParameterToDraw(std::vector<std::string> const & param);
  void Run();

 private:

  bool CheckFile(char const * vq_name,
		 char const * perm_name,
		 std::vector<char const *> const & files) const;
  void Initialize();
  void AddFiles(std::vector<char const *> const & files);  
  void FillPermutationV();
  void SetupVQChain();
  std::string GetPermString(std::vector<double> const & permutation);
  double DrawHist(std::string const & draw,
		  std::string const & binning,
		  std::string const & weight);
  void GetBestWorstPermutations(std::vector<std::vector<double>> & drawn_values,
				std::vector<std::pair<double, int>> & max_results,
				std::vector<std::pair<double, int>> & min_results);
  std::vector<size_t> FindPermutations(std::vector<double> const & best_permutation, size_t const parameter_index);
  void PlotParameters(std::vector<std::vector<double>> const & drawn_values,
		      std::vector<std::pair<double, int>> const & max_results,
		      std::string const & title_suffix);
  void PlotGraph(std::vector<double> const & drawn_value_v, 
		 std::vector<size_t> const & plot_permutations, 
		 size_t const draw_vec_index, 
		 size_t const parameter_index,
		 std::string const & title_suffix);
  void DrawGraphs();

  std::vector<std::vector<std::string>> fdraw_vec;
  std::vector<std::string> fparameter_name;
  std::vector<std::vector<double>> fpermutation_v;
  std::vector<std::vector<TGraph *>> fgraph_v;

  TFile * foutput_file;

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
