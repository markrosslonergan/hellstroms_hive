

#include "EvaluateVertexQuality.h"


EvaluateVertexQuality::GraphAesthetics::GraphAesthetics(std::string const ilegend_name,
							int const icolor,
							int const imarker_type,
							int const ipandora_line) :
  legend_name(ilegend_name),
  color(icolor),
  marker_type(imarker_type),
  pandora_line(ipandora_line) {}
  

EvaluateVertexQuality::PlotHelper::PlotHelper(std::string const & imetric_to_study,
					      std::vector<std::string> const & imethod,
					      std::vector<std::string> const & imetrics_to_draw,
					      std::vector<std::string> const & iparameters_to_draw,
					      std::vector<std::string> const & iperformance_quantities) :
  metric_to_study(imetric_to_study, SIZE_MAX),
  method(imethod) {
    
  for(std::string const & str : imetrics_to_draw) metrics_to_draw.emplace(str, SIZE_MAX);
  for(std::string const & str : iparameters_to_draw) parameters_to_draw.emplace(str, SIZE_MAX);
  for(std::string const & str : iperformance_quantities) performance_quantities.emplace(str, SIZE_MAX);
  
}


EvaluateVertexQuality::PlotHelper::~PlotHelper() {

  for(TGraph * graph : graph_v) delete graph;
  
}


void EvaluateVertexQuality::PlotHelper::Print() {

  std::cout << "metric_to_study: " << metric_to_study.first << " " << metric_to_study.second << "\nmethods:\n";
  for(auto const & str : method) std::cout << "\t" << str << "\nmetrics_to_draw:\n";
  for(auto const & p : metrics_to_draw) std::cout << "\t" << p.first << " " << p.second << "\n";
  std::cout << "parameters_to_draw:\n";
  for(auto const & p : parameters_to_draw) std::cout << "\t" << p.first << " " << p.second << "\n";
  std::cout << "performance_quantities:\n";
  for(auto const & p : performance_quantities) std::cout << "\t" << p.first << " " << p.second << "\n";  
  std::cout << "\n";

}


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

  fmethod_map = {{"max", "Maximized"},
		 {"min", "Minimized"}};

  fxtitle_map = {{"start_prox", "start_prox"},
		 {"shower_prox", "Maximum Shower Impact Parameter [cm]"},
		 {"max_bp_dist", "Maximum Shower Backwards Projection Distance [cm]"},
		 {"cpoa_vert_prox", "cpoa_vert_prox"},
		 {"cpoa_trackend_prox", "cpoa_trackend_prox"}};

  fgraph_aesthetic_map = {{"completeness", {"Completeness", kRed, 8}},
			  {"shower_completeness", {"Shower Completeness", kRed+1, 23, 2}},
			  {"track_completeness", {"Track Completeness", kBlue+1, 33, 2}},
			  {"cleanliness", {"Cleanliness", kBlue, 8}}, 
			  {"shower_cleanliness", {"Shower Cleanliness", kGreen+2, 23, 2}}, 
			  {"track_cleanliness", {"Track Cleanliness", kMagenta+1, 33, 2}}, 
			  {"combined", {"Combined", kMagenta, 8}},
			  {"shower_combined", {"Shower Combined", kMagenta, 23}},
			  {"track_combined", {"Track Combined", kMagenta, 33}},
			  {"dist", {"Reco - True Distance [cm]", kBlack, 8, 2}}};

}


EvaluateVertexQuality::~EvaluateVertexQuality() {

  if(fpandora_file) fpandora_file->Close();
  if(foutput_file) foutput_file->Close();
  delete fvq_chain;

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
  fdraw_vec_names.push_back(param.at(3));

}


void EvaluateVertexQuality::AddToDraw(std::string const & metric_to_study,
				      std::vector<std::string> const & method,
				      std::vector<std::string> const & metrics_to_draw,
				      std::vector<std::string> const & parameters_to_draw,
				      std::vector<std::string> const & performance_quantities) {
  
  fplot_helper_v.push_back({metric_to_study, method, metrics_to_draw, parameters_to_draw, performance_quantities});

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
				    std::vector<std::vector<std::pair<double, int> > > & max_results,
				    std::vector<std::vector<std::pair<double, int> > > & min_results) {

  drawn_values.clear();
  max_results.clear();
  min_results.clear();
  fdraw_vec.clear();

  GetEvalInfo();

  std::vector<std::string> const & vq_pq = fvq.GetPerformanceQuantities();

  for(int i = 0; i < feval_chain->GetEntries(); ++i) {

    feval_chain->GetEntry(i);

    if(i == 0) {
      drawn_values.resize(fdrawn_values->size(), {});
      max_results.resize(fdrawn_values->size(), {vq_pq.size(), {0, -1}});
      min_results.resize(fdrawn_values->size(), {vq_pq.size(), {DBL_MAX, -1}});
    }

    for(size_t j = 0; j < fdrawn_values->size(); ++j) {
      drawn_values.at(j).insert(drawn_values.at(j).end(), fdrawn_values->at(j).begin(), fdrawn_values->at(j).end());
    }

  }

  for(size_t i = 0; i < drawn_values.size(); ++i) {

    for(size_t j = 0; j < drawn_values.at(i).size(); ++j) {
      
      std::vector<double> const & results = drawn_values.at(i).at(j);

      fvq.GetMinMax(j,
		    results,
		    max_results.at(i),
		    min_results.at(i));

    }

  }

}


void EvaluateVertexQuality::GetPandoraMetrics() {

  for(std::vector<std::string> const & draw : fdraw_vec) {

    fpandora_drawn_values.push_back(fvq.DrawHist(fpandora_tree, draw.at(0), draw.at(1), draw.at(2).c_str()));    

  }  

}


void EvaluateVertexQuality::ProcessDrawOption(std::vector<std::string> const & input,
					      std::map<std::string, size_t> & output) {

  for(size_t i = 0; i < input.size(); ++i) {
    std::string const & quantity = input.at(i);
    auto const pq_it = output.find(quantity);
    if(pq_it != output.end()) pq_it->second = i;
  }

  for(auto it = output.begin(); it != output.end();) {
 
    if(it->second != SIZE_MAX) {
      ++it;
      continue;
    }

    std::cout << "WARNING: could not find " << it->first << "\n";

    it = output.erase(it);

  }

}


void EvaluateVertexQuality::CheckPlotHelperV() {

  for(auto it = fplot_helper_v.begin(); it != fplot_helper_v.end();) {
        
    PlotHelper & ph = *it;
    
    std::map<std::string, size_t> temp;
    temp.emplace(ph.metric_to_study);
    ProcessDrawOption(fdraw_vec_names, temp);
    if(!temp.empty()) ph.metric_to_study.second = temp.begin()->second;
    
    if(ph.metric_to_study.second == SIZE_MAX) {
      it = fplot_helper_v.erase(it);
      continue;
    }
    
    ProcessDrawOption(fdraw_vec_names, ph.metrics_to_draw);
    ProcessDrawOption(fparameter_name, ph.parameters_to_draw);
    if(feval_performance_quantities) ProcessDrawOption(*feval_performance_quantities, ph.performance_quantities);
    else ProcessDrawOption(fvq.GetPerformanceQuantities(), ph.performance_quantities);
    
    ++it;
    
  }
  

}


std::string EvaluateVertexQuality::GetTitle(PlotHelper const & ph,
					    std::string const & method) const {

  std::vector<std::string> const & titles = fdraw_vec.at(ph.metric_to_study.second);

  std::string title;
  std::string title_suffix = titles.at(5);
  auto const mm_it = fmethod_map.find(method);
  if(mm_it == fmethod_map.end()) std::cout << "WARINING: no title found for " << method << "\n";
  else title_suffix += " " + mm_it->second;
  if(titles.at(4).empty()) title = title_suffix;
  else title = titles.at(4) + " " + title_suffix;

  return title;

}


TGraph * EvaluateVertexQuality::PlotGraph(PlotHelper const & ph,
					  std::string const & method,
					  std::vector<std::vector<double>> const & permutation_v,
					  std::vector<size_t> const & plot_permutations, 
					  std::vector<std::vector<std::vector<double> > > const & drawn_values, 
					  size_t const draw_vec_index, 
					  size_t const parameter_index,
					  size_t const performance_quantity_index) {

  TGraph * graph = new TGraph();
  for(size_t i = 0; i < plot_permutations.size(); ++i) {
    size_t const index = plot_permutations.at(i);
    graph->SetPoint(i, permutation_v.at(index).at(parameter_index), drawn_values.at(draw_vec_index).at(index).at(performance_quantity_index));
  }

  std::vector<std::string> const & titles = fdraw_vec.at(draw_vec_index);
  std::string const & drawn_name = fdraw_vec.at(draw_vec_index).at(3);

  graph->SetName((method + "_" + fdraw_vec.at(ph.metric_to_study.second).at(3) + "_plot_" + drawn_name + "_var_" + fparameter_name.at(parameter_index)).c_str());
  graph->SetMarkerStyle(8);
  
  graph->SetTitle((GetTitle(ph, method)).c_str());

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
  if(cm_it == fgraph_aesthetic_map.end()) std::cout << "WARNING: no GA for " << drawn_name << "\n";
  else {
    graph->SetMarkerColor(cm_it->second.color);
    graph->SetMarkerStyle(cm_it->second.marker_type);
  }

  return graph;

}


TH1 * EvaluateVertexQuality::DrawHist(TTree * tree,
				      std::string const & name,
				      std::string const & draw,
				      std::string const & binning,
				      std::string const & weight,
				      std::string const & opt,
				      std::string const & title,
				      std::string const & xtitle,
				      std::string const & ytitle) const {
  
  tree->Draw((draw+">>"+name+binning).c_str(), weight.c_str(), opt.c_str());

  TH1 * h = (TH1*)gDirectory->Get(name.c_str());
  h->SetStats(0);
  h->SetTitle(title.c_str());
  h->GetXaxis()->SetTitle(xtitle.c_str());
  h->GetXaxis()->CenterTitle();
  h->GetYaxis()->SetTitle(ytitle.c_str());
  h->GetYaxis()->CenterTitle(); 

  h->Scale(1./h->GetEntries());

  h->SetLineWidth(3);
  
  return h;

}


void EvaluateVertexQuality::DrawHist(PlotHelper const & ph,
				     std::string const & method,
				     std::string const & pq,
				     std::string const & draw,
				     std::string const & binning,
				     std::string const & weight,
				     std::string const & best_permutation) const {

  TCanvas * canvas = new TCanvas(("dist_" + method + "_" + fdraw_vec.at(ph.metric_to_study.second).at(3) + "_" + pq).c_str());
  std::string const title = GetTitle(ph, method);
  std::string weight_modified = fdraw_vec.at(ph.metric_to_study.second).at(2);
  if(weight != "") weight_modified + " && " + weight;

  TH1 * h = DrawHist(fvq_chain,
		     "h",
		     draw,
		     binning,
		     weight_modified + " && " + best_permutation,
		     "hist",
		     title,
		     "True - Reco Vertex Distance [cm]",
		     "Number of Vertices");

  TH1 * hp = nullptr;
  TLegend * legend = nullptr;

  if(fpandora_tree) {

    hp = DrawHist(fpandora_tree,
		  "hp",
		  draw,
		  binning,
		  weight_modified,
		  "histsame",
		  title,
		  "True - Reco Vertex Distance [cm]",
		  "Number of Vertices");
    
    hp->SetLineColor(kRed);
    hp->SetLineStyle(2);

    legend = new TLegend(0.6, 0.9, 0.9, 0.6);
    legend->AddEntry(h, "Vertex Builder");
    legend->AddEntry(hp, "Pandora");

  }

  if(legend) legend->Draw();
  canvas->Write();
  delete canvas;
  delete h;
  if(hp) delete hp;
  
}


void EvaluateVertexQuality::PlotParameters(std::vector<std::vector<double> > const & permutation_v,
					   std::vector<std::vector<std::vector<double> > > const & drawn_values,
					   std::vector<std::vector<std::pair<double, int> > > const & results,
					   std::string const & method) {

  for(PlotHelper & ph : fplot_helper_v) {

    if(std::find(ph.method.begin(), ph.method.end(), method) == ph.method.end()) continue; 

    size_t const ph_index = ph.metric_to_study.second;

    std::string title_suffix;
    auto const mm_it = fmethod_map.find(method);
    if(mm_it != fmethod_map.end()) title_suffix = mm_it->second + " ";
    else std::cout << "WARNING: no title for " << method << "\n";
    std::string const modified_title_suffix = title_suffix + fdraw_vec.at(ph_index).at(5);

    std::map<std::string, size_t> const & metrics_to_draw = ph.metrics_to_draw;
    std::map<std::string, size_t> const & parameters_to_draw = ph.parameters_to_draw;
    std::map<std::string, size_t> const & performance_quantities = ph.performance_quantities;

    for(auto const & pq : performance_quantities) {

      std::vector<double> const & best_permutation = permutation_v.at(results.at(ph_index).at(pq.second).second);
      DrawHist(ph, method, pq.first, "dist", "(20, 0, 300)", "reco_shower_total + reco_track_total > 1", fvq.GetPermString(best_permutation));
      
      for(auto const & ptd : parameters_to_draw) {
 
	int const parameter_index = ptd.second;
	std::vector<size_t> const plot_permutations = FindPermutations(permutation_v, best_permutation, parameter_index);
	
	TCanvas * canvas = new TCanvas((method + "_" + fdraw_vec.at(ph.metric_to_study.second).at(3) + "_" + pq.first + "_var_" + fparameter_name.at(parameter_index)).c_str());

	TGraph * first_graph = nullptr;

	double ymin = DBL_MAX;
	double ymax = 0;
	
	TLegend * legend = new TLegend(0.6, 0.9, 0.9, 0.6);
	std::vector<TGraph *> line_handler;

	for(auto const & mtd : metrics_to_draw) {
	  
	  TGraph * graph = PlotGraph(ph, method, permutation_v, plot_permutations, drawn_values, mtd.second, parameter_index, pq.second);	  

	  if(!first_graph) {
	    graph->Draw("ap");
	    graph->GetYaxis()->SetTitle("");
	    first_graph = graph;
	  }
	  else graph->Draw("p");
	  
	  double const xmin = TMath::MinElement(graph->GetN(), graph->GetX());
	  double const xmax = TMath::MaxElement(graph->GetN(), graph->GetX());

	  double const gymin = TMath::MinElement(graph->GetN(), graph->GetY());
	  double const gymax = TMath::MaxElement(graph->GetN(), graph->GetY());
	  
	  if(gymin < ymin) ymin = gymin;
	  if(gymax > ymax) ymax = gymax;

	  auto const cm_it = fgraph_aesthetic_map.find(mtd.first);
	  if(cm_it == fgraph_aesthetic_map.end()) std::cout << "WARNING: no GA for " << mtd.first << "\n";
	  else legend->AddEntry(graph, cm_it->second.legend_name.c_str());
	  
	  if(fpandora_tree) {
	    double arrx[2] = {xmin, xmax};
	    double const ypandora = fpandora_drawn_values.at(mtd.second).at(pq.second);
	    double arry[2] = {ypandora, ypandora};
	    TGraph * line = new TGraph(2, arrx, arry);
	    line->SetLineColor(graph->GetMarkerColor());
	    line->SetLineStyle(cm_it->second.pandora_line);
	    line->SetLineWidth(2);
	    line->SetMarkerColor(graph->GetMarkerColor());
	    line->SetFillColor(0);
	    line->Draw("l");
	    legend->AddEntry(line, (std::string("Pandora ") + cm_it->second.legend_name).c_str());
	    line_handler.push_back(line);
	    if(ypandora < ymin) ymin = ypandora;
	    else if(ypandora > ymax) ymax = ypandora;
	  }

	  ph.graph_v.push_back(graph);
	  
	}

	if(first_graph) {
	  
	  first_graph->GetYaxis()->SetRangeUser(ymin*0.9, ymax*1.1);
	  legend->Draw();
	  canvas->Write();
	  for(TGraph * line : line_handler) delete line;
	  
	}
	
	delete legend;
	delete canvas; 
	
      }
      
    } 
    
  }

}


void EvaluateVertexQuality::DrawGraphs() {

  for(PlotHelper const & ph : fplot_helper_v) {

    for(TGraph * graph : ph.graph_v) {

      TCanvas * canvas = new TCanvas((std::string(graph->GetName()) + "_canvas\n").c_str());
      graph->Draw("ap");
      canvas->Write();
      delete canvas;

    }

  }
  
}


void EvaluateVertexQuality::Run(std::vector<double> const & input_permutation) {

  if(!foutput_file) {
    std::cout << "ERROR no output file set\n";
    exit(1);
  }
     
  std::vector<std::vector<std::vector<double>>> drawn_values;
  std::vector<std::vector<std::pair<double, int>>> max_results;
  std::vector<std::vector<std::pair<double, int>>> min_results;

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

  fvq.Print(permutation_v, fdraw_vec, drawn_values, max_results, min_results);

  CheckPlotHelperV();
  PlotParameters(permutation_v, drawn_values, max_results, "max");
  PlotParameters(permutation_v, drawn_values, min_results, "min");

}
