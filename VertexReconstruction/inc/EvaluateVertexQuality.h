

#ifndef EVALUATEVERTEXQUALITY_H
#define EVALUATEVERTEXQUALITY_H


#include <vector>
#include <iostream>

#include "TChain.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TMath.h"
#include "TLine.h"

#include "VertexQuality.h"


class EvaluateVertexQuality {

  struct plot_helper {

    plot_helper(std::string const & imetric_to_study,
		std::vector<std::string> const & imetrics_to_draw,
		std::vector<std::string> const & iparameters_to_draw,
		std::vector<std::string> const & iperformance_quantities);

    std::string const metric_to_study;
    std::vector<std::string> const metrics_to_draw;
    std::vector<std::string> const parameters_to_draw;
    std::vector<std::string> const performance_quantities;

  };

 public:

  EvaluateVertexQuality(char const * vq_name,
			char const * perm_name,
			char const * eval_name,
			std::vector<char const *> const & files = {});
  ~EvaluateVertexQuality();

  void SetOutputFile(char const * file_name);
  void AddToDraw(std::string const & metric_to_study,
		 std::vector<std::string> const & metrics_to_draw,
		 std::vector<std::string> const & parameters_to_draw,
		 std::vector<std::string> const & performance_quantities);
  void AddPerformanceMetric(std::vector<std::string> const & params);
  void Run(std::vector<double> const & input_permutation = {});
  void Hack();

 private:

  bool CheckFile(char const * vq_name,
		 char const * perm_name,
		 char const * eval_name,
		 std::vector<char const *> const & files) const;
  void Initialize();
  TFile *  LookForPandoraFile(char const * file_name);
  void AddFiles(std::vector<char const *> const & files);  
  void FillPermutationV();
  void SetupVQChain();
  void SetupEvalChain();
  std::string GetPermString(std::vector<double> const & permutation);
  void GetEvalInfo();
  void GetEval(std::vector<std::vector<std::vector<double>>> & drawn_values,
	       std::vector<std::pair<double, int>> & max_results,
	       std::vector<std::pair<double, int>> & min_results);
  void GetPandoraMetrics();
  void ProcessDrawOption(std::vector<std::string> const & input,
			 std::unordered_map<std::string, size_t> & output);
  void ProcessDrawOptions();
  std::vector<size_t> FindPermutations(std::vector<std::vector<double>> const & permutation_v, 
				       std::vector<double> const & best_permutation, 
				       size_t const parameter_index);
  void PlotParameters(std::vector<std::vector<double>> const & permutation_v,
		      std::vector<std::vector<std::vector<double>>> const & drawn_values,
		      std::vector<std::pair<double, int>> const & max_results,
		      std::string const & title_suffix,
		      std::string const & name_suffix);
  void PlotParametersOld(std::vector<std::vector<double> > const & permutation_v,
			 std::vector<std::vector<std::vector<double> > > const & drawn_values,
			 std::vector<std::pair<double, int>> const & results,
			 std::string const & title_suffix,
			 std::string const & name_suffix);
  void PlotGraph(std::vector<std::vector<double>> const & permutation_v,
		 std::vector<size_t> const & plot_permutations,
		 std::vector<std::vector<double>> const & drawn_value_v, 
		 size_t const draw_vec_index, 
		 size_t const parameter_index,
		 size_t const performance_quantity_index,
		 std::string const & title_suffix,
		 std::string const & name_suffix);
  void DrawGraphs();
  void DrawGraphsSupimp();

  std::unordered_map<std::string, std::string> fxtitle_map;
  std::unordered_map<std::string, int> fgraph_aesthetic_map;

  std::vector<std::vector<std::string>> fdraw_vec;
  std::unordered_map<std::string, size_t> finput_metric_study;
  std::unordered_map<std::string, size_t> finput_metric_draw;
  std::unordered_map<std::string, size_t> finput_performance_quantities;
  std::unordered_map<std::string, size_t> finput_parameter_names;
  std::vector<std::string> fparameter_name;
  std::vector<std::vector<double>> fpermutation_v;
  std::vector<std::vector<TGraph *>> fgraph_v;

  std::vector<plot_helper> plot_helper_v;

  VertexQuality fvq;

  TFile * fpandora_file;
  TTree * fpandora_tree;
  std::vector<std::vector<double>> fpandora_drawn_values;

  TFile * foutput_file;

  TChain * fvq_chain;
  TChain * fperm_chain;
  TChain * feval_chain;

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

  std::vector<std::vector<std::string>> * feval_draw_vec;
  std::vector<std::string> * feval_performance_quantities;
  std::vector<std::vector<double>> * feval_permutation_v;
  std::vector<std::vector<std::vector<double>>> * fdrawn_values;

};


#endif
