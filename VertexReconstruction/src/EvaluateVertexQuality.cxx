

#include "EvaluateVertexQuality.h"
#include <iomanip>

EvaluateVertexQuality::EvaluateVertexQuality(char const * vq_name,
					     char const * perm_name,
					     std::vector<char const *> const & files) {
  
  if(files.empty()) {
    std::cout << "Empty file list given to Storage\n";
    exit(1);
  }
      
  if(!CheckFile(vq_name, perm_name, files)) exit(1);
  Initialize();

  fvq_chain = new TChain(vq_name);
  fperm_chain = new TChain(perm_name);

  AddFiles(files);
  FillPermutationV();
  SetupVQChain();

}


EvaluateVertexQuality::~EvaluateVertexQuality() {

  delete fvq_chain;
  for(TGraph * graph : fgraph_v) delete graph; 

}


bool EvaluateVertexQuality::CheckFile(char const * vq_name,
				      char const * perm_name,
				      std::vector<char const *> const & files) const {

  TFile * file = TFile::Open(files.front());

  if(!file) {
    std::cout << "File: " << files.front() << " not found\n";
    exit(1);
  }

  TTree * vq_tree = dynamic_cast<TTree*>(file->Get(vq_name));
  TTree * perm_tree = dynamic_cast<TTree*>(file->Get(perm_name));

  if(!vq_tree || !perm_tree) {
    std::cout << __PRETTY_FUNCTION__ << "\nCould not find a tree\n";
    return false;
  }

  file->Close();

  return true;

}


void EvaluateVertexQuality::Initialize() {

  ftrack_matching_ratio_v = nullptr;
  ftrack_true_pdg_v = nullptr;
  ftrack_true_origin_v = nullptr;

  fshower_matching_ratio_v = nullptr;
  fshower_true_pdg_v = nullptr;
  fshower_true_origin_v = nullptr;

}


void EvaluateVertexQuality::AddFiles(std::vector<char const *> const & files) {

  for(char const * file : files) {
    fvq_chain->Add(file);
    fperm_chain->Add(file);
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


void EvaluateVertexQuality::SetOutputFile(char const * file_name) {

  foutput_file = TFile::Open(file_name, "recreate");

}


double EvaluateVertexQuality::DrawHist(std::string const & draw,
				       std::string const & binning,
				       std::string const & weight) {
  
  TCanvas * canvas = new TCanvas("temp");
  fvq_chain->Draw((draw + ">>h" + binning).c_str(), weight.c_str());
  TH1 * h = (TH1*)gDirectory->Get("h");
  double const mean = h->GetMean();
  std::cout << std::setprecision(14);
  std::cout << h->GetEntries() << "\n";
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


void EvaluateVertexQuality::GetBestPermutations(std::vector<std::vector<double>> & drawn_values,
						std::vector<std::pair<double, int>> & max_results) {
  
  drawn_values.clear();
  drawn_values.reserve(fdraw_vec.size());
  for(size_t i = 0; i < fdraw_vec.size(); ++i) {
    drawn_values.push_back({});
    drawn_values.back().reserve(fpermutation_v.size());
  }
  max_results.clear();
  max_results.resize(fdraw_vec.size(), {0, 0});

  for(size_t i = 0; i < fpermutation_v.size(); ++i) {

    std::vector<double> const & permutation = fpermutation_v.at(i);
    std::string const perm_weight = GetPermString(permutation);

    for(size_t j = 0; j < fdraw_vec.size(); ++j) {
      std::vector<std::string> const & draw = fdraw_vec.at(j);
      std::string const & draw_weight = draw.at(2);
      std::string modified_weight = perm_weight;
      if(!draw_weight.empty()) modified_weight += " && " + draw_weight;
      double const result = DrawHist(draw.at(0), draw.at(1), modified_weight);
      std::cout << i << " " << permutation.at(0) << " " << permutation.at(1) << " " << permutation.at(2) << " "<< permutation.at(3) << " "<< permutation.at(4) << " "<< result << "\n";
      drawn_values.at(j).push_back(result);
      if(result > max_results.at(j).first) {
	max_results.at(j).first = result;
	max_results.at(j).second = i;
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
				      size_t const parameter_index) {

  std::cout << fparameter_name.at(parameter_index) << "\n";
  TGraph * graph = new TGraph();
  for(size_t i = 0; i < plot_permutations.size(); ++i) {
    size_t const index = plot_permutations.at(i);
    graph->SetPoint(i, fpermutation_v.at(index).at(parameter_index), drawn_value_v.at(index));
    std::cout << i << " " << fpermutation_v.at(index).at(parameter_index) << " " << drawn_value_v.at(index) << "\n";
  }
  std::cout << "\n";

  std::vector<std::string> const & titles = fdraw_vec.at(draw_vec_index);
  
  graph->SetName((fdraw_vec.at(draw_vec_index).at(3) + "_" + fparameter_name.at(parameter_index)).c_str());
  graph->SetMarkerStyle(8);
  graph->SetTitle(titles.at(4).c_str());
  graph->GetXaxis()->SetTitle(fparameter_name.at(parameter_index).c_str());
  graph->GetXaxis()->CenterTitle();
  graph->GetYaxis()->SetTitle(titles.at(5).c_str());
  graph->GetYaxis()->CenterTitle();

  fgraph_v.push_back(graph);

}


void EvaluateVertexQuality::PlotParameters(std::vector<std::vector<double>> const & drawn_values,
					   std::vector<std::pair<double, int>> const & max_results) {

  for(size_t i = 0; i < fdraw_vec.size(); ++i) {

    std::vector<double> const & best_permutation = fpermutation_v.at(max_results.at(i).second);

    for(size_t j = 0; j < fparameter_name.size(); ++j) {

      std::vector<size_t> const plot_permutations = FindPermutations(best_permutation, j);
      PlotGraph(drawn_values.at(i), plot_permutations, i, j);

    }

  }

}


void EvaluateVertexQuality::DrawGraphs() {

  for(TGraph * graph : fgraph_v) {
    TCanvas * canvas = new TCanvas((std::string(graph->GetName()) + "_canvas").c_str());
    graph->Draw("ap");
    canvas->Write();
    delete canvas;
  }

}


void EvaluateVertexQuality::Run() {

  if(!foutput_file) {
    std::cout << "ERROR no output file set\n";
    exit(1);
  }
   
  /*
  fdraw_vec.push_back({"correct_shower_total / reco_shower_total", "", "tpc_volume_contained == 1",
	"cleanliness", "", "Cleanliness"});
  */
  fdraw_vec.push_back({"correct_shower_total / true_shower_total", "", "tpc_volume_contained == 1",
	"completeness", "", "Completeness"});
  
  std::vector<std::vector<double>> drawn_values;
  std::vector<std::pair<double, int>> max_results;

  GetBestPermutations(drawn_values,
		      max_results); 

  PlotParameters(drawn_values,
		 max_results);

  //DrawGraphs();

}
