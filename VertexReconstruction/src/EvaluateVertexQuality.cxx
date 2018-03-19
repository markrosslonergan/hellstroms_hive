

#include "EvaluateVertexQuality.h"


EvaluateVertexQuality::EvaluateVertexQuality(char const * vq_name,
					     char const * perm_name,
					     char const * eval_name,
					     std::vector<char const *> const & files) {
  
  if(files.empty()) {
    std::cout << "Empty file list given to Storage\n";
    exit(1);
  }
      
  if(!CheckFile(vq_name, perm_name, eval_name, files)) exit(1);
  Initialize();

  fvq_chain = new TChain(vq_name);
  fperm_chain = new TChain(perm_name);
  feval_chain = new TChain(eval_name);

  AddFiles(files);
  FillPermutationV();
  SetupVQChain();
  SetupEvalChain();

  fcolor_map = {{"completeness", kRed}, {"cleanliness", kBlue}, {"combined", kMagenta}};

}


EvaluateVertexQuality::~EvaluateVertexQuality() {

  delete fvq_chain;
  for(std::vector<TGraph *> const & graph_v : fgraph_v) 
    for(TGraph * graph : graph_v) delete graph; 

}


bool EvaluateVertexQuality::CheckFile(char const * vq_name,
				      char const * perm_name,
				      char const * eval_name,
				      std::vector<char const *> const & files) const {

  char const * file_name = files.front();
  
  TFile * file = TFile::Open(file_name);

  if(!file) {
    std::cout << "File: " << file_name << " not found\n";
    exit(1);
  }

  TTree * vq_tree = dynamic_cast<TTree*>(file->Get(vq_name));
  TTree * perm_tree = dynamic_cast<TTree*>(file->Get(perm_name));
  TTree * eval_tree = dynamic_cast<TTree*>(file->Get(eval_name));

  bool result = true;

  if(!vq_tree) {
    std::cout << "Could not find: " << vq_name << " in " << file_name << "\n";
    result = false;
  }
  if(!perm_tree) { 
    std::cout << "Could not find: " << perm_name << " in " << file_name << "\n";
    result = false;
  }
  if(!eval_tree) {
    std::cout << "Could not find: " << eval_name << " in " << file_name << "\n";
    result = false;
  }

  file->Close();

  return result;

}


void EvaluateVertexQuality::Initialize() {

  ftrack_matching_ratio_v = nullptr;
  ftrack_true_pdg_v = nullptr;
  ftrack_true_origin_v = nullptr;

  fshower_matching_ratio_v = nullptr;
  fshower_true_pdg_v = nullptr;
  fshower_true_origin_v = nullptr;

  feval_draw_vec = nullptr;
  feval_permutation_v = nullptr;
  fdrawn_values = nullptr;

}


void EvaluateVertexQuality::AddFiles(std::vector<char const *> const & files) {

  for(char const * file : files) {
    fvq_chain->Add(file);
    fperm_chain->Add(file);
    feval_chain->Add(file);
  }

}


void EvaluateVertexQuality::FillPermutationV() {

  TObjArray * branches = fperm_chain->GetListOfBranches();
  std::vector<double> parameters(branches->GetEntries(), -10000);
  for(int i = 0; i < branches->GetEntries(); ++i) {
    fparameter_name.push_back(branches->At(i)->GetName());
    fperm_chain->SetBranchAddress(branches->At(i)->GetName(), &parameters.at(i));
  }

  for(int i = 0; i < fperm_chain->GetEntries(); ++i) {
    fperm_chain->GetEntry(i);
    fpermutation_v.push_back(parameters);
  }

  delete fperm_chain;

}


void EvaluateVertexQuality::SetupVQChain() {

  fvq_chain->SetBranchAddress("start_prox", &fstart_prox);
  fvq_chain->SetBranchAddress("shower_prox", &fshower_prox);
  fvq_chain->SetBranchAddress("max_bp_dist", &fmax_bp_dist);
  fvq_chain->SetBranchAddress("cpoa_vert_prox", &fcpoa_vert_prox);
  fvq_chain->SetBranchAddress("cpoa_trackend_prox", &fcpoa_trackend_prox);

  fvq_chain->SetBranchAddress("reco_vertex_present", &freco_vertex_present);
  fvq_chain->SetBranchAddress("tpc_volume_contained", &ftpc_volume_contained);

  fvq_chain->SetBranchAddress("dist", &fdist);
  fvq_chain->SetBranchAddress("distx", &fdistx);
  fvq_chain->SetBranchAddress("disty", &fdisty);
  fvq_chain->SetBranchAddress("distz", &fdistz);

  fvq_chain->SetBranchAddress("true_track_total", &ftrue_track_total);
  fvq_chain->SetBranchAddress("true_shower_total", &ftrue_shower_total);
  fvq_chain->SetBranchAddress("reco_track_total", &freco_track_total);
  fvq_chain->SetBranchAddress("reco_shower_total", &freco_shower_total);
  fvq_chain->SetBranchAddress("correct_track_total", &fcorrect_track_total);
  fvq_chain->SetBranchAddress("correct_shower_total", &fcorrect_shower_total);

  fvq_chain->SetBranchAddress("track_matching_ratio_v", &ftrack_matching_ratio_v);
  fvq_chain->SetBranchAddress("track_true_pdg_v", &ftrack_true_pdg_v);
  fvq_chain->SetBranchAddress("track_true_origin_v", &ftrack_true_origin_v);

  fvq_chain->SetBranchAddress("shower_matching_ratio_v", &fshower_matching_ratio_v);
  fvq_chain->SetBranchAddress("shower_true_pdg_v", &fshower_true_pdg_v);
  fvq_chain->SetBranchAddress("shower_true_origin_v", &fshower_true_origin_v);

}


void EvaluateVertexQuality::SetupEvalChain() {

  feval_chain->SetBranchAddress("draw_vec", &feval_draw_vec);
  feval_chain->SetBranchAddress("drawn_values", &fdrawn_values);

}


void EvaluateVertexQuality::SetOutputFile(char const * file_name) {

  foutput_file = TFile::Open(file_name, "recreate");

}


void EvaluateVertexQuality::AddParameterToDraw(std::vector<std::string> const & param) {

  fdraw_vec.push_back(param);
  fgraph_v.push_back({});

}


double EvaluateVertexQuality::DrawHist(std::string const & draw,
				       std::string const & binning,
				       std::string const & weight) {
  
  TCanvas * canvas = new TCanvas("temp");
  fvq_chain->Draw((draw + ">>h" + binning).c_str(), weight.c_str());
  TH1 * h = (TH1*)gDirectory->Get("h");
  double const mean = h->GetMean();
  delete h;
  delete canvas;
  return mean;

}


std::string EvaluateVertexQuality::GetPermString(std::vector<double> const & permutation) {

  std::string result;
  for(size_t i = 0; i < permutation.size(); ++i) {
    result += fparameter_name.at(i) + " == " + std::to_string(permutation.at(i));
    if(i != permutation.size() - 1) result += " && ";
  }

  return result;

}


void EvaluateVertexQuality::GetBestWorstPermutations(std::vector<std::vector<double>> & drawn_values,
						     std::vector<std::pair<double, int>> & max_results,
						     std::vector<std::pair<double, int>> & min_results) {
  
  drawn_values.clear();
  drawn_values.reserve(fdraw_vec.size());
  for(size_t i = 0; i < fdraw_vec.size(); ++i) {
    drawn_values.push_back({});
    drawn_values.back().reserve(fpermutation_v.size());
  }
  max_results.clear();
  max_results.resize(fdraw_vec.size(), {0, -1});
  min_results.clear();
  min_results.resize(fdraw_vec.size(), {DBL_MAX, -1});

  for(size_t i = 0; i < fpermutation_v.size(); ++i) {

    std::vector<double> const & permutation = fpermutation_v.at(i);
    std::string const perm_weight = GetPermString(permutation);

    for(size_t j = 0; j < fdraw_vec.size(); ++j) {

      std::vector<std::string> const & draw = fdraw_vec.at(j);
      std::string const & draw_weight = draw.at(2);
      std::string modified_weight = perm_weight;
      if(!draw_weight.empty()) modified_weight += " && " + draw_weight;
      double const result = DrawHist(draw.at(0), draw.at(1), modified_weight);
      drawn_values.at(j).push_back(result);
      if(result > max_results.at(j).first) {
	max_results.at(j).first = result;
	max_results.at(j).second = i;
      }
      if(result < min_results.at(j).first) {
	min_results.at(j).first = result;
	min_results.at(j).second = i;
      }

    }

  }

}


std::vector<size_t> EvaluateVertexQuality::FindPermutations(std::vector<double> const & best_permutation, size_t const parameter_index) {

  std::vector<size_t> result;

  for(size_t i = 0; i < fpermutation_v.size(); ++i) {

    std::vector<double> const & permutation = fpermutation_v.at(i);

    bool pass = true;

    for(size_t j = 0; j < permutation.size(); ++j) {
      if(j == parameter_index) continue;
      if(permutation.at(j) != best_permutation.at(j)) {
	pass = false;
	break;
      }
    }
    
    if(pass) result.push_back(i);

  }

  return result;

}


void EvaluateVertexQuality::PlotGraph(std::vector<double> const & drawn_value_v, 
				      std::vector<size_t> const & plot_permutations, 
				      size_t const draw_vec_index, 
				      size_t const parameter_index,
				      std::string const & title_suffix,
				      std::string const & name_suffix) {

  TGraph * graph = new TGraph();
  for(size_t i = 0; i < plot_permutations.size(); ++i) {
    size_t const index = plot_permutations.at(i);
    graph->SetPoint(i, fpermutation_v.at(index).at(parameter_index), drawn_value_v.at(index));
  }

  std::vector<std::string> const & titles = fdraw_vec.at(draw_vec_index);
  std::string const & drawn_name = fdraw_vec.at(draw_vec_index).at(3);

  graph->SetName((drawn_name + "_" + fparameter_name.at(parameter_index) + "_" + name_suffix).c_str());
  graph->SetMarkerStyle(8);
 
  std::string title;
  if(titles.at(4).empty()) title = title_suffix;
  else title = titles.at(4) + " " + title_suffix;
  graph->SetTitle(title.c_str());

  graph->GetXaxis()->SetTitle(fparameter_name.at(parameter_index).c_str());
  graph->GetXaxis()->CenterTitle();
  graph->GetYaxis()->SetTitle(titles.at(5).c_str());
  graph->GetYaxis()->CenterTitle();

  graph->SetFillColor(0);
  graph->SetLineColor(0);
  auto const cm_it = fcolor_map.find(drawn_name);
  if(cm_it == fcolor_map.end()) std::cout << "WARNING: no color for " << drawn_name << "\n";
  else graph->SetMarkerColor(cm_it->second);

  fgraph_v.at(draw_vec_index).push_back(graph);

}


void EvaluateVertexQuality::PlotParameters(std::vector<std::vector<double>> const & drawn_values,
					   std::vector<std::pair<double, int>> const & results,
					   std::string const & title_suffix,
					   std::string const & name_suffix) {

  for(size_t i = 0; i < fdraw_vec.size(); ++i) {

    std::string const modified_title_suffix = title_suffix + " " + fdraw_vec.at(i).at(5);
    std::string const modified_name_suffix = name_suffix + "_" + fdraw_vec.at(i).at(3);
    std::vector<double> const & best_permutation = fpermutation_v.at(results.at(i).second);

    for(size_t j = 0; j < fparameter_name.size(); ++j) {

      std::vector<size_t> const plot_permutations = FindPermutations(best_permutation, j);
      for(size_t k = 0; k < fdraw_vec.size(); ++k) PlotGraph(drawn_values.at(k), plot_permutations, k, j, modified_title_suffix, modified_name_suffix);

    }

  }

}


void EvaluateVertexQuality::DrawGraphs() {

  for(std::vector<TGraph *> const & graph_v : fgraph_v) {
    for(TGraph * graph : graph_v) {
      TCanvas * canvas = new TCanvas((std::string(graph->GetName()) + "_canvas").c_str());
      graph->Draw("ap");
      canvas->Write();
      delete canvas;
    }
  }

}


void EvaluateVertexQuality::DrawGraphsSupimp() {

  if(fgraph_v.empty()) return;
  size_t const drawn_graphs = fgraph_v.front().size();

  for(size_t i = 0; i < drawn_graphs; ++i) {
    TCanvas * canvas = new TCanvas((std::string(fgraph_v.front().at(i)->GetName()) + "_canvas").c_str());
    TLegend * legend = new TLegend(0.6, 0.9, 0.9, 0.6);
    TGraph * first_graph = nullptr;
    double ymin = DBL_MAX;
    double ymax = 0;
    for(size_t j = 0; j < fgraph_v.size(); ++j) {
      TGraph * graph = fgraph_v.at(j).at(i);
      if(j == 0) {
	first_graph = graph;
	graph->Draw("ap");
      }
      else graph->Draw("p");
      double const gmin = TMath::MinElement(graph->GetN(), graph->GetY());
      double const gmax = TMath::MaxElement(graph->GetN(), graph->GetY());
      if(gmin < ymin) ymin = gmin;
      if(gmax > ymax) ymax = gmax;
      legend->AddEntry(graph, fdraw_vec.at(j).at(5).c_str());
    }
    if(first_graph) first_graph->GetYaxis()->SetRangeUser(ymin*0.9, ymax*1.1);
    legend->Draw();
    canvas->Write();
    delete canvas; 
    delete legend;
  }

}


void EvaluateVertexQuality::GetEval(std::vector<std::vector<double>> & drawn_values,
				    std::vector<std::pair<double, int>> & max_results,
				    std::vector<std::pair<double, int>> & min_results) {

  fdraw_vec.clear();
  
  for(int i = 0; i < feval_chain->GetEntries(); ++i) {

    feval_chain->GetEntry(i);

    if(i == 0) {
      for(size_t j = 0; j < feval_draw_vec->size(); ++j) AddParameterToDraw(feval_draw_vec->at(j));
      drawn_values.resize(fdrawn_values->size(), {});
      max_results.resize(fdrawn_values->size(), {0, -1});
      min_results.resize(fdrawn_values->size(), {DBL_MAX, -1});
    }

    for(size_t j = 0; j < fdrawn_values->size(); ++j) {

      drawn_values.at(j).insert(drawn_values.at(j).end(), fdrawn_values->at(j).begin(), fdrawn_values->at(j).end());

    }

  }

  for(size_t i = 0; i < drawn_values.size(); ++i) {

    std::vector<double> const & v = drawn_values.at(i);

    for(size_t j = 0; j < v.size(); ++j) {

      double const value = v.at(j);

      if(value > max_results.at(i).first) {
	max_results.at(i).first = value;
	max_results.at(i).second = j;
      }
      if(value < min_results.at(i).first) {
	min_results.at(i).first = value;
	min_results.at(i).second = j;
      }

    }

  }

}


void EvaluateVertexQuality::Print(std::vector<std::vector<double>> const & drawn_values,
				  std::vector<std::pair<double, int>> const & max_results,
				  std::vector<std::pair<double, int>> const & min_results) const {
  
  std::cout << "\n";
  for(size_t i = 0; i < drawn_values.size(); ++i) {
    std::vector<std::string> const & draw = fdraw_vec.at(i);
    std::vector<double> const & drawn_value = drawn_values.at(i);
    for(std::string const & option : draw) std::cout << option << " | ";
    std::cout << draw.size() << "\n";
    std::cout << "\n";
    /*
    for(size_t j = 0; j < fpermutation_v.size(); ++j) {
      std::vector<double> const & permutation = fpermutation_v.at(j);
      std::cout << "Drawn value: " << drawn_value.at(j);
      for(size_t k = 0; k < permutation.size(); ++k) std::cout << " " << fparameter_name.at(k) << ": " << permutation.at(k);
      std::cout << "\n";
    }
    */
    std::cout << "\nmax: " << max_results.at(i).first << " " << max_results.at(i).second << "\n"
	      << "min: " << min_results.at(i).first << " " << min_results.at(i).second << "\n\n";
  }

}


void EvaluateVertexQuality::Run() {

  if(!foutput_file) {
    std::cout << "ERROR no output file set\n";
    exit(1);
  }
     
  std::vector<std::vector<double>> drawn_values;
  std::vector<std::pair<double, int>> max_results;
  std::vector<std::pair<double, int>> min_results;

  if(feval_chain) {
    GetEval(drawn_values,
	    max_results,
	    min_results);
  }
  else {
    GetBestWorstPermutations(drawn_values,
			     max_results,
			     min_results); 
  } 

  Print(drawn_values,
	max_results,
	min_results);

  PlotParameters(drawn_values,
		 max_results,
		 "Maximized",
		 "max");
  PlotParameters(drawn_values,
		 min_results,
		 "Minimized",
		 "min");

  //DrawGraphs();
  DrawGraphsSupimp();

}
