

#include "EvaluateVertexQuality.h"


EvaluateVertexQuality::plot_helper::plot_helper(std::string const & imetric_to_study,
						std::vector<std::string> const & imetrics_to_draw,
						std::vector<std::string> const & iparameters_to_draw,
						std::vector<std::string> const & iperformance_quantities) :
  metric_to_study(imetric_to_study),
  metrics_to_draw(imetrics_to_draw),
  parameters_to_draw(iparameters_to_draw),
  performance_quantities(iperformance_quantities) {}


EvaluateVertexQuality::EvaluateVertexQuality(char const * vq_name,
					     char const * perm_name,
					     char const * eval_name,
					     std::vector<char const *> const & files) :
  fpandora_file(nullptr),
  fpandora_tree(nullptr),
  foutput_file(nullptr),
  fvq_chain(new TChain(vq_name)),
  fperm_chain(new TChain(perm_name)),
  feval_chain(new TChain(eval_name)) {

  if(files.empty()) {
    std::cout << "Empty file list given to Storage\n";
    exit(1);
  }
      
  if(!CheckFile(vq_name, perm_name, eval_name, files)) exit(1);
  Initialize();

  AddFiles(files);
  FillPermutationV();
  SetupVQChain();
  SetupEvalChain();

  fxtitle_map = {{"start_prox", "start_prox"},
		 {"shower_prox", "Maximum Shower Impact Parameter [cm]"},
		 {"max_bp_dist", "Maximum Shower Backwards Projection Distance [cm]"},
		 {"cpoa_vert_prox", "cpoa_vert_prox"},
		 {"cpoa_trackend_prox", "cpoa_trackend_prox"}};

  fgraph_aesthetic_map = {{"completeness", kRed},
			  {"shower_completeness", kRed},
			  {"track_completeness", kRed},
			  {"cleanliness", kBlue}, 
			  {"shower_cleanliness", kBlue}, 
			  {"track_cleanliness", kBlue}, 
			  {"combined", kMagenta},
			  {"shower_combined", kMagenta},
			  {"track_combined", kMagenta},
			  {"dist", kGreen}};

}


EvaluateVertexQuality::~EvaluateVertexQuality() {

  if(fpandora_file) fpandora_file->Close();
  if(foutput_file) foutput_file->Close();
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
  feval_performance_quantities = nullptr;
  feval_permutation_v = nullptr;
  fdrawn_values = nullptr;

}


TFile * EvaluateVertexQuality::LookForPandoraFile(char const * file_name) {

  TFile * file = TFile::Open(file_name);

  if(!file) {
    std::cout << "File: " << file_name << " not found\n";
    exit(1);
  }

  if(!file->GetListOfKeys()->Contains("permutation_tree")) {
    fpandora_tree = dynamic_cast<TTree*>(file->Get(fvq_chain->GetName()));
    if(!fpandora_tree) std::cout << "WARNING: could not find pandora tree " << fvq_chain->GetName() << "\n";
    return file;
  }

  file->Close();
  return nullptr;

}


void EvaluateVertexQuality::AddFiles(std::vector<char const *> const & files) {

  fpandora_file = LookForPandoraFile(files.back());  

  size_t end;
  if(fpandora_file) end = files.size() - 1;
  else end = files.size();
  
  for(size_t i = 0; i < end; ++i) {
    char const * file = files.at(i);
    fvq_chain->Add(file);
    fperm_chain->Add(file);
    feval_chain->Add(file);
  }

}


void EvaluateVertexQuality::FillPermutationV() {

  TObjArray * branches = fperm_chain->GetListOfBranches();
  std::vector<double> parameters(branches->GetEntries(), -10000);
  for(int i = 0; i < branches->GetEntries(); ++i) {
    char const * name = branches->At(i)->GetName();
    fparameter_name.push_back(name);
    fperm_chain->SetBranchAddress(name, &parameters.at(i));
  }

  for(int i = 0; i < fperm_chain->GetEntries(); ++i) {
    fperm_chain->GetEntry(i);
    fpermutation_v.push_back(parameters);
  }

  fvq.AddPermutations(fpermutation_v);

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
  feval_chain->SetBranchAddress("performance_quantities", &feval_performance_quantities);
  feval_chain->SetBranchAddress("drawn_values", &fdrawn_values);

}


void EvaluateVertexQuality::SetOutputFile(char const * file_name) {

  foutput_file = TFile::Open(file_name, "recreate");

}


void EvaluateVertexQuality::AddPerformanceMetric(std::vector<std::string> const & param) {

  fdraw_vec.push_back(param);

}


void EvaluateVertexQuality::AddToDraw(std::string const & metric_to_study,
				      std::vector<std::string> const & metrics_to_draw,
				      std::vector<std::string> const & parameters_to_draw,
				      std::vector<std::string> const & performance_quantities) {
  
  plot_helper_v.push_back({metric_to_study, metrics_to_draw, parameters_to_draw, performance_quantities});

}


std::string EvaluateVertexQuality::GetPermString(std::vector<double> const & permutation) {

  std::string result;
  for(size_t i = 0; i < permutation.size(); ++i) {
    result += fparameter_name.at(i) + " == " + std::to_string(permutation.at(i));
    if(i != permutation.size() - 1) result += " && ";
  }

  return result;

}


std::vector<size_t> EvaluateVertexQuality::FindPermutations(std::vector<std::vector<double>> const & permutation_v,
							    std::vector<double> const & best_permutation, 
							    size_t const parameter_index) {

  std::vector<size_t> result;

  for(size_t i = 0; i < permutation_v.size(); ++i) {

    std::vector<double> const & permutation = permutation_v.at(i);

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


void EvaluateVertexQuality::GetEvalInfo() {

  if(feval_chain->GetEntries() == 0) {
    std::cout << "WARNING: eval_tree empty\n";
    return;
  }

  feval_chain->GetEntry(0);

  for(size_t i = 0; i < feval_draw_vec->size(); ++i) {
    AddPerformanceMetric(feval_draw_vec->at(i));
  }

  std::vector<std::string> const & vq_pq = fvq.GetPerformanceQuantities();
  if(vq_pq.size() != feval_performance_quantities->size()) {
    std::cout << "WARNING: performance quantities in file differ from those in vertex quality class\n";
  }
  else {
    for(size_t i = 0; i < vq_pq.size(); ++i) {
      std::string const & performance_quantity = vq_pq.at(i); 
      if(performance_quantity != feval_performance_quantities->at(i)) {
	std::cout << "WARNING: performance quantities in file differ from those in vertex quality class\n";
      }
    }
  }

}


void EvaluateVertexQuality::GetEval(std::vector<std::vector<std::vector<double> > > & drawn_values,
				    std::vector<std::pair<double, int>> & max_results,
				    std::vector<std::pair<double, int>> & min_results) {

  drawn_values.clear();
  max_results.clear();
  min_results.clear();
  fdraw_vec.clear();

  GetEvalInfo();

  for(int i = 0; i < feval_chain->GetEntries(); ++i) {

    feval_chain->GetEntry(i);

    if(i == 0) {
      drawn_values.resize(fdrawn_values->size(), {});
      max_results.resize(fdrawn_values->size(), {0, -1});
      min_results.resize(fdrawn_values->size(), {DBL_MAX, -1});
    }

    for(size_t j = 0; j < fdrawn_values->size(); ++j) {
      drawn_values.at(j).insert(drawn_values.at(j).end(), fdrawn_values->at(j).begin(), fdrawn_values->at(j).end());
    }

  }

  for(size_t i = 0; i < drawn_values.size(); ++i) {

    std::vector<std::vector<double>> const & v = drawn_values.at(i);

    for(size_t j = 0; j < v.size(); ++j) {

      double const value = v.at(j).front();

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


void EvaluateVertexQuality::GetPandoraMetrics() {

  for(std::vector<std::string> const & draw : fdraw_vec) {

    fpandora_drawn_values.push_back(fvq.DrawHist(fpandora_tree, draw.at(0), draw.at(1), draw.at(2).c_str()));    

  }  

}


void EvaluateVertexQuality::ProcessDrawOption(std::vector<std::string> const & input,
					      std::unordered_map<std::string, size_t> & output) {

  for(size_t i = 0; i < input.size(); ++i) {
    std::string const & quantity = input.at(i);
    auto const pq_it = output.find(quantity);
    if(pq_it != output.end()) pq_it->second = i;
  }
  for(auto const & p : output) {
    if(p.second != SIZE_MAX) continue;
    std::cout << "WARNING: performance quantity " << p.first << " not found\n";
  }    

}


void EvaluateVertexQuality::ProcessDrawOptions() {

  std::vector<std::string> draw_str;
  for(std::vector<std::string> const & draw_vec : fdraw_vec) draw_str.push_back(draw_vec.at(3));
  ProcessDrawOption(draw_str, finput_metric_study);
  ProcessDrawOption(draw_str, finput_metric_draw);
  ProcessDrawOption(fparameter_name, finput_parameter_names);
  if(feval_performance_quantities) ProcessDrawOption(*feval_performance_quantities, finput_performance_quantities);
  else ProcessDrawOption(fvq.GetPerformanceQuantities(), finput_performance_quantities);

}



void EvaluateVertexQuality::PlotGraph(std::vector<std::vector<double>> const & permutation_v,
				      std::vector<size_t> const & plot_permutations, 
				      std::vector<std::vector<double>> const & drawn_value_v, 
				      size_t const draw_vec_index, 
				      size_t const parameter_index,
				      size_t const performance_quantity_index,
				      std::string const & title_suffix,
				      std::string const & name_suffix) {

  TGraph * graph = new TGraph();
  for(size_t i = 0; i < plot_permutations.size(); ++i) {
    size_t const index = plot_permutations.at(i);
    graph->SetPoint(i, permutation_v.at(index).at(parameter_index), drawn_value_v.at(index).at(performance_quantity_index));
  }

  std::vector<std::string> const & titles = fdraw_vec.at(draw_vec_index);
  std::string const & drawn_name = fdraw_vec.at(draw_vec_index).at(3);

  graph->SetName(("plot_" + drawn_name + "_var_" + fparameter_name.at(parameter_index) + "_" + name_suffix).c_str());
  graph->SetMarkerStyle(8);

  std::string title;
  if(titles.at(4).empty()) title = title_suffix;
  else title = titles.at(4) + " " + title_suffix;
  graph->SetTitle(title.c_str());

  auto const xtm_it = fxtitle_map.find(fparameter_name.at(parameter_index));
  if(xtm_it == fxtitle_map.end()) {
    std::cout << "WARINING: no xtitle found for " << fparameter_name.at(parameter_index) << "\n";
    graph->GetXaxis()->SetTitle(fparameter_name.at(parameter_index).c_str());
  }
  else {
    graph->GetXaxis()->SetTitle(xtm_it->second.c_str());
  }
  graph->GetXaxis()->CenterTitle();
  
  graph->GetYaxis()->SetTitle(titles.at(5).c_str());
  graph->GetYaxis()->CenterTitle();

  graph->SetFillColor(0);
  graph->SetLineColor(0);
  auto const cm_it = fgraph_aesthetic_map.find(drawn_name);
  if(cm_it == fgraph_aesthetic_map.end()) std::cout << "WARNING: no color for " << drawn_name << "\n";
  else graph->SetMarkerColor(cm_it->second);

  fgraph_v.back().push_back(graph);

}


void EvaluateVertexQuality::PlotParameters(std::vector<std::vector<double> > const & permutation_v,
					   std::vector<std::vector<std::vector<double> > > const & drawn_values,
					   std::vector<std::pair<double, int>> const & results,
					   std::string const & title_suffix,
					   std::string const & name_suffix) {

  for(auto const & imsp : finput_metric_study) {

    if(imsp.second == SIZE_MAX) continue;

    fgraph_v.push_back({});

    std::string const modified_title_suffix = title_suffix + " " + fdraw_vec.at(imsp.second).at(5);
    std::string const modified_name_suffix = name_suffix + "_" + fdraw_vec.at(imsp.second).at(3);
    std::vector<double> const & best_permutation = permutation_v.at(results.at(imsp.second).second);

    for(auto const & ipnp : finput_parameter_names) {

      int const parameter_index = ipnp.second;
      if(parameter_index == SIZE_MAX) continue;
      std::vector<size_t> const plot_permutations = FindPermutations(permutation_v, best_permutation, parameter_index);
      
      for(auto const & imsd : finput_metric_draw) {

	if(imsd.second == SIZE_MAX) {
	  fgraph_v.back().push_back(nullptr); 
	  continue;
	}

	std::cout << imsd.first << " " << imsd.second << "\n";

	for(auto const & ipqp : finput_performance_quantities) {

	  if(ipqp.second == SIZE_MAX) continue;
	  PlotGraph(permutation_v, plot_permutations, drawn_values.at(imsd.second), imsd.second, parameter_index, ipqp.second, modified_title_suffix, modified_name_suffix + "_" + ipqp.first);

	}

      }

    }

  }

}


void EvaluateVertexQuality::PlotParametersOld(std::vector<std::vector<double> > const & permutation_v,
					      std::vector<std::vector<std::vector<double> > > const & drawn_values,
					      std::vector<std::pair<double, int>> const & results,
					      std::string const & title_suffix,
					      std::string const & name_suffix) {

  for(auto const & imsp : finput_metric_study) {

    if(imsp.second == SIZE_MAX) continue;

    fgraph_v.push_back({});

    std::string const modified_title_suffix = title_suffix + " " + fdraw_vec.at(imsp.second).at(5);
    std::string const modified_name_suffix = name_suffix + "_" + fdraw_vec.at(imsp.second).at(3);
    std::vector<double> const & best_permutation = permutation_v.at(results.at(imsp.second).second);

    for(auto const & ipnp : finput_parameter_names) {

      int const parameter_index = ipnp.second;
      if(parameter_index == SIZE_MAX) continue;
      std::vector<size_t> const plot_permutations = FindPermutations(permutation_v, best_permutation, parameter_index);
      
      for(auto const & imsd : finput_metric_draw) {

	if(imsd.second == SIZE_MAX) {
	  fgraph_v.back().push_back(nullptr); 
	  continue;
	}

	std::cout << imsd.first << " " << imsd.second << "\n";

	for(auto const & ipqp : finput_performance_quantities) {

	  if(ipqp.second == SIZE_MAX) continue;
	  PlotGraph(permutation_v, plot_permutations, drawn_values.at(imsd.second), imsd.second, parameter_index, ipqp.second, modified_title_suffix, modified_name_suffix + "_" + ipqp.first);

	}

      }

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

  for(size_t i = 0; i < fgraph_v.size(); ++i) {

    TCanvas * canvas = nullptr;
    TGraph * first_graph = nullptr;

    double ymin = DBL_MAX;
    double ymax = 0;

    TLegend * legend = new TLegend(0.6, 0.9, 0.9, 0.6);
    std::vector<TLine *> line_handler;

    for(size_t j = 0; j < fgraph_v.at(i).size(); ++j) {

      TGraph * graph = fgraph_v.at(i).at(j);
      if(!graph) continue;
      if(!canvas) {
	canvas = new TCanvas((std::string(graph->GetName()) + "_canvas").c_str());
	graph->GetYaxis()->SetTitle("");
	graph->Draw("ap");
	first_graph = graph;
      }

      else graph->Draw("p");
   
      double const gmin = TMath::MinElement(graph->GetN(), graph->GetY());
      double const gmax = TMath::MaxElement(graph->GetN(), graph->GetY());

      if(gmin < ymin) ymin = gmin;
      if(gmax > ymax) ymax = gmax;

      legend->AddEntry(graph, fdraw_vec.at(j).at(5).c_str());

      double const xmin = TMath::MinElement(graph->GetN(), graph->GetX());
      double const xmax = TMath::MaxElement(graph->GetN(), graph->GetX());

      if(fpandora_tree) {
	////////////////////////////HERE
	std::cout << graph->GetName() << "\n";
	for(std::string const & str : fdraw_vec.at(j)) {
	  std::cout << str << " ";
	}
	std::cout  << "\n" << fpandora_drawn_values.at(j).at(0) << "\n";

	TLine * line = new TLine(xmin, fpandora_drawn_values.at(j).at(0), xmax, fpandora_drawn_values.at(j).at(0));
	line->SetLineColor(graph->GetMarkerColor());
	line->Draw();
	legend->AddEntry(line, (std::string("Pandora ") + fdraw_vec.at(j).at(5)).c_str());
	line_handler.push_back(line);
      }

    }

    std::cout << "\n";

    if(canvas) {
 
      first_graph->GetYaxis()->SetRangeUser(ymin*0.9, ymax*1.1);
      legend->Draw();
      canvas->Write();
      delete canvas; 
      for(TLine * line : line_handler) delete line;

    }

    delete legend;

  }

}


void EvaluateVertexQuality::Run(std::vector<double> const & input_permutation) {

  if(!foutput_file) {
    std::cout << "ERROR no output file set\n";
    exit(1);
  }
     
  std::vector<std::vector<std::vector<double>>> drawn_values;
  std::vector<std::pair<double, int>> max_results;
  std::vector<std::pair<double, int>> min_results;

  std::vector<std::vector<double>> permutation_v;
  if(!input_permutation.empty()) {
    for(size_t const i : FindPermutations(fpermutation_v, input_permutation, 1)) 
      permutation_v.push_back(fpermutation_v.at(i));
    fvq.GetDrawnValues(fvq_chain, permutation_v, fdraw_vec, drawn_values, max_results, min_results);
  }
  else {
    permutation_v = fpermutation_v;
    GetEval(drawn_values, max_results, min_results);
  }
  for(std::vector<std::string> const & performance_metric : fdraw_vec) fvq.AddPerformanceMetric(performance_metric);

  if(fpandora_tree) GetPandoraMetrics();

  //fvq.Print(permutation_v, fdraw_vec, drawn_values, max_results, min_results);

  ProcessDrawOptions();

  PlotParameters(permutation_v, drawn_values, max_results, "Maximized", "max");
  //PlotParameters(drawn_values, min_results, "Minimized", "min");

  //DrawGraphs();
  DrawGraphsSupimp();
  //Hack();

}


void EvaluateVertexQuality::Hack() {

  TDirectory * current = gDirectory;

  TFile * pfile = TFile::Open("RunVertexQualityPandora.root");
  TTree * ptree = (TTree*)pfile->Get("vertex_quality_tree_signal");

  current->cd();

  TCanvas * canvas = new TCanvas("hack");
  std::string binning = "(40, 0, 400)";
  std::string weight = "start_prox == 4 && shower_prox == 40 && max_bp_dist == 40 && cpoa_vert_prox == 12 && cpoa_trackend_prox == 12";
  fvq_chain->Draw(("dist>>h"+binning).c_str(), ("tpc_volume_contained == 1 && is_nc_delta_rad == 1 && reco_track_total > 0 && nc_delta_rad_split_shower == 1 && " + weight).c_str());
  ptree->Draw(("dist>>h2"+binning).c_str(), "tpc_volume_contained == 1 && is_nc_delta_rad == 1 && (reco_track_total > 0 || reco_shower_total > 1) && nc_delta_rad_split_shower == 1");

  TLegend * leg = new TLegend(0.6, 0.9, 0.9, 0.6);

  TH1 * h = (TH1*)gDirectory->Get("h");
  h->SetLineColor(kBlue);

  TH1 * h2 = (TH1*)gDirectory->Get("h2");
  h2->SetLineColor(kRed);

  leg->AddEntry(h, "VB");
  leg->AddEntry(h2, "Pandora");

  h->Scale(1./h->GetEntries());
  h2->Scale(1./h2->GetEntries());

  std::cout << "vb: " << h->GetEntries() << " p: " << h2->GetEntries() << "\n";

  h->Draw("hist");
  h2->Draw("samehist");
  
  h->SetStats(0);
  h->GetXaxis()->SetTitle("Reco - True Vertex Distance [cm]");
  h->GetYaxis()->SetRangeUser(0, 0.6);
  leg->Draw();

  canvas->Write();

  delete canvas;

}
