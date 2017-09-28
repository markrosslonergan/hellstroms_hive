


#include "object_helper.hpp"
#include "train.hpp"
#include "app.hpp"
#include "merge.hpp"
#include "significance.hpp"



double get_pot(std::string const & file_path, std::string const & tree_path) {

  object_helper oh;
  TTree * pot_tree = oh.GetObject<TTree>(file_path, tree_path);
  double pot = -1;
  pot_tree->SetBranchAddress("pot", &pot);
  pot_tree->GetEntry(0);
  
  return pot;

}



int main(int const argc, char const * argv[]) {

  if(argc != 3) {
    std::cout << "ERROR - Required inputs:\n->Path to sample file directory\n->Option\n";
    exit(1);
  }
  
  std::string const dir = argv[1];
  std::string const option = argv[2];
  
  double const run_pot = 6.6e20;

  object_helper oh;

  std::vector<TTree *> const signal_trees = {
    oh.GetObject<TTree>(dir + "/runmv_sp.root", "LEEPhoton/vertex_tree")
  };
  std::vector<double> signal_weights = {
    run_pot / get_pot(dir + "/runmv_sp.root", "LEEPhoton/get_pot")
  };

  std::vector<TTree *> const background_trees = {
    oh.GetObject<TTree>(dir + "/runmv_bnb_cosmic.root", "LEEPhoton/vertex_tree")
  };
  std::vector<double> background_weights = {
    run_pot / get_pot(dir + "/runmv_bnb_cosmic.root", "LEEPhoton/get_pot")
  };

  std::vector<std::pair<std::string, std::string>> const variables_notrack = {
    {"summed_associated_helper_shower_energy", "d"},
    {"reco_nu_vtx_dist_to_closest_tpc_wall", "d"},
    {"most_energetic_shower_reco_thetaxz", "d"},
    {"most_energetic_shower_reco_thetayz", "d"},
    {"most_energetic_shower_bp_dist_to_tpc", "d"},
    {"reco_shower_dedx_plane0", "d"},
    {"reco_shower_dedx_plane1", "d"},
    {"reco_shower_dedx_plane2", "d"}
  };

  std::vector<std::pair<std::string, std::string>> variables_trackonly = variables_notrack;
  variables_trackonly.emplace_back("shortest_asso_shower_to_vert_dist", "d");
  variables_trackonly.emplace_back("longest_asso_track_thetaxz", "d");
  variables_trackonly.emplace_back("longest_asso_track_thetayz", "d");
  variables_trackonly.emplace_back("reco_asso_tracks", "i");
  variables_trackonly.emplace_back("longest_asso_track_displacement", "d");

  std::vector<method_struct> const methods {
    {TMVA::Types::kBDT, "BDTG", "!H:!V:NTrees=1000:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=20:MaxDepth=2"},
    {TMVA::Types::kBDT, "BDT",
	"!H:!V:NTrees=850:MinNodeSize=2.5%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=20"},
    {TMVA::Types::kBDT, "BDTB",
	"!H:!V:NTrees=400:BoostType=Bagging:SeparationType=GiniIndex:nCuts=20"},
    {TMVA::Types::kBDT, "BDTD",
	"!H:!V:NTrees=400:MinNodeSize=5%:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:VarTransform=Decorrelate"},
    {TMVA::Types::kBDT, "BDTF",
	"!H:!V:NTrees=50:MinNodeSize=2.5%:UseFisherCuts:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20"},
    {TMVA::Types::kRuleFit, "RuleFit",
	"H:!V:RuleFitModule=RFTMVA:Model=ModRuleLinear:MinImp=0.001:RuleMinDist=0.001:NTrees=20:fEventsMin=0.01:fEventsMax=0.5:GDTau=-1.0:GDTauPrec=0.01:GDStep=0.01:GDNSteps=10000:GDErrScale=1.02"}  
  };

  std::string const identifier = "runtmva";
  std::string const identifier_notrack = identifier + "_notrack";
  std::string const identifier_trackonly = identifier + "_trackonly";

  std::string const all_cut = "closest_asso_shower_dist_to_flashzcenter <= 40 && totalpe_ibg_sum > 140 && reco_asso_showers == 1";
  std::string const all_cut_notrack = all_cut + " && reco_asso_tracks == 0";
  std::string const all_cut_trackonly = all_cut + " && reco_asso_tracks > 0";

  std::string const signal_definition = "is_delta_rad == 1 && true_nu_vtx_fid_contained == 1";
  std::string const background_definition = "!(" + signal_definition + ")";

  std::vector<std::pair<TTree *, std::string>> trees = {
    std::pair<TTree *, std::string>(oh.GetObject<TTree>(dir + "/runmv_sp.root", "LEEPhoton/vertex_tree"), "signal"),
    std::pair<TTree *, std::string>(oh.GetObject<TTree>(dir + "/runmv_bnb_cosmic.root", "LEEPhoton/vertex_tree"), "background"),
    std::pair<TTree *, std::string>(oh.GetObject<TTree>(dir + "/runmv_bnb_cosmic.root", "LEEPhoton/vertex_tree"), "data")
  };

  std::vector<std::string> tree_cuts = {
    all_cut + " && " + signal_definition,
    all_cut + " && " + background_definition,
    all_cut
  };

  std::vector<std::pair<std::string, std::string>> branches = {
    {"mva", "d"}
  };

  if(option == "train") {
    train(identifier_notrack, all_cut_notrack, signal_definition, background_definition, signal_trees, background_trees, variables_notrack, methods);
    train(identifier_trackonly, all_cut_trackonly, signal_definition, background_definition, signal_trees, background_trees, variables_trackonly, methods);
  }
  
  else if(option == "app") {
    app(identifier_notrack, trees, tree_cuts, all_cut_notrack, variables_notrack, methods);
    app(identifier_trackonly, trees, tree_cuts, all_cut_trackonly, variables_trackonly, methods);
    merge(identifier+"_app.root", identifier_notrack+"_app.root", identifier_trackonly+"_app.root", trees, methods, branches, all_cut, all_cut_notrack, all_cut_trackonly);
  }

  else if(option == "merge") {
    merge(identifier+"_app.root", identifier_notrack+"_app.root", identifier_trackonly+"_app.root", trees, methods, branches, all_cut, all_cut_notrack, all_cut_trackonly);
  }

  else if(option == "sig") {
    significance(identifier+"_app.root", signal_trees, signal_weights, background_trees, background_weights);
  }

}
