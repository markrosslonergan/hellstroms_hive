

#ifndef EVALUATEVERTEXQUALITY_H
#define EVALUATEVERTEXQUALITY_H


#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "TChain.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TMath.h"
#include "TLine.h"
#include "TTreeFormula.h"
#include "TPaveText.h"
#include "TLegendEntry.h"

#include "VertexQuality.h"


class EvaluateVertexQuality {

  struct GraphAesthetics {

    GraphAesthetics(std::string const ilegend_name,
		    int const icolor,
		    int const imarker_type,
		    int const ipandora_line = 1);

    std::string legend_name;
    int color;
    int marker_type;
    int pandora_line;

  };

  struct PlotHelper {

    PlotHelper(std::string const & imetric_to_study,
	       std::vector<std::string> const & method,	
	       std::vector<std::string> const & imetrics_to_draw,
	       std::vector<std::string> const & iparameters_to_draw,
	       std::vector<std::string> const & iperformance_quantities,
	       std::string const & iname = "",
	       std::string const & ititle = "");
    ~PlotHelper();

    void Print();

    std::pair<std::string, size_t> metric_to_study;
    std::vector<std::string> method;
    std::map<std::string, size_t> metrics_to_draw;
    std::map<std::string, size_t> parameters_to_draw;
    std::map<std::string, size_t> performance_quantities;
    std::string name;
    std::string title;

    std::vector<TGraph *> graph_v;

  };

 public:

  EvaluateVertexQuality(char const * vq_name,
			char const * perm_name,
			char const * eval_name,
			std::vector<char const *> const & files = {});
  ~EvaluateVertexQuality();

  void SetOutputFile(char const * file_name);
  void AddToDraw(std::string const & metric_to_study,
		 std::vector<std::string> const & method,
		 std::vector<std::string> const & metrics_to_draw,
		 std::vector<std::string> const & parameters_to_draw,
		 std::vector<std::string> const & performance_quantities,
		 std::string const & name = "",
		 std::string const & title = "");
  void AddPerformanceMetric(std::vector<std::string> const & params);
  void Run(std::vector<double> const & input_permutation = {});

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
  size_t GetIndex(std::vector<std::vector<double>> const & vv,
		  std::vector<double> const & v);
  void GetEvalInfo();
  void GetEval(std::vector<std::vector<std::vector<double>>> & drawn_values,
	       std::vector<std::vector<std::pair<double, int>>> & max_results,
	       std::vector<std::vector<std::pair<double, int>>> & min_results,
	       size_t const permutation_index = SIZE_MAX);
  void GetPandoraMetrics();
  void CheckPlotHelperV();
  void ProcessDrawOption(std::vector<std::string> const & input,
			 std::map<std::string, size_t> & output);
  std::vector<size_t> FindPermutations(std::vector<std::vector<double>> const & permutation_v, 
				       std::vector<double> const & best_permutation, 
				       size_t const parameter_index);
  TH1 * DrawHist(TTree * tree,
		 std::string const & name,
		 std::string const & draw,
		 std::string const & binning = "",
		 std::string const & weight = "",
		 std::string const & opt = "",
		 std::string const & title = "",
		 std::string const & xtitle = "",
		 std::string const & ytitle = "") const;
  void DrawHist(PlotHelper const & ph,
		std::string const & method,
		std::string const & pq,
		std::string const & draw,
		std::string const & binning,
		std::string const & weight,
		std::string const & best_permutation,
		std::string const & title = "",
		std::string const & xtitle = "") const;
  void PlotParameters(std::vector<std::vector<double> > const & permutation_v,
		      std::vector<std::vector<std::vector<double> > > const & drawn_values,
		      std::vector<std::vector<std::pair<double, int>>> const & results,
		      std::string const & method);
  std::string GetTitle(PlotHelper const & ph,
		       std::string const & method) const;
  TGraph * PlotGraph(PlotHelper const & ph,
		     std::string const & method,
		     std::vector<std::vector<double>> const & permutation_v,
		     std::vector<size_t> const & plot_permutations,
		     std::vector<std::vector<std::vector<double>>> const & drawn_value_v, 
		     size_t const draw_vec_index, 
		     size_t const parameter_index,
		     size_t const performance_quantity_index);
  void DrawGraphs();
  void DrawGraphsSupimp();

  std::unordered_map<std::string, std::string> fmethod_map;
  std::unordered_map<std::string, std::string> fxtitle_map;
  std::unordered_map<std::string, std::string> fytitle_map;
  std::unordered_map<std::string, GraphAesthetics> fgraph_aesthetic_map;

  std::vector<std::vector<std::string>> fdraw_vec;
  std::vector<std::string> fdraw_vec_names;
  std::vector<std::string> fparameter_name;
  std::vector<std::vector<double>> fpermutation_v;

  std::vector<PlotHelper> fplot_helper_v;

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
