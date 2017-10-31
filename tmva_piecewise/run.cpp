


#include "object_helper.hpp"
#include "train.hpp"
#include "app.hpp"
#include "merge.hpp"
#include "significance.hpp"
#include "plot_bdt_variables.hpp"
#include "get_mva_response_hists.hpp"
#include "plot_mva_response_hists.hpp"
#include "tlimits.hpp"



std::pair<int, double> get_pot(std::string const & file_path, std::string const & tree_path) {

  object_helper oh;
  TTree * pot_tree = oh.GetObject<TTree>(file_path, tree_path);
  int events = -1;
  double pot = -1;
  pot_tree->SetBranchAddress("number_of_events", &events);
  pot_tree->SetBranchAddress("pot", &pot);
  pot_tree->GetEntry(0);

  return std::pair<int, double>(events, pot);

}



int main(int const argc, char const * argv[]) {

  if(argc != 3) {
    std::cout << "ERROR - Required inputs:\n->Path to sample file directory\n->Option\n";
    exit(1);
  }

  //TRAINING
  //===========================================================================================

  std::string const dir = argv[1];
  std::string const option = argv[2];

  object_helper oh;

  std::vector<TTree *> const signal_training_trees = {
    oh.GetObject<TTree>(dir + "/runmv_sp.root", "LEEPhoton/vertex_tree")
  };
  std::vector<std::pair<int, double>> const signal_training_pots = {
    get_pot(dir + "/runmv_sp.root", "LEEPhoton/get_pot")
  };

  std::vector<TTree *> const background_training_trees = {
    oh.GetObject<TTree>(dir + "/runmv_bnb_cosmic.root", "LEEPhoton/vertex_tree")
  };
  std::vector<std::pair<int, double>> const background_training_pots = {
    get_pot(dir + "/runmv_bnb_cosmic.root", "LEEPhoton/get_pot")
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

  std::string const all_cut = "passed_swtrigger == 1 && closest_asso_shower_dist_to_flashzcenter <= 40 && totalpe_ibg_sum > 140 && reco_asso_showers == 1";
  std::string const cut_notrack = "reco_asso_tracks == 0";
  std::string const all_cut_notrack = all_cut + " && " + cut_notrack;
  std::string const cut_trackonly = "reco_asso_tracks > 0";
  std::string const all_cut_trackonly = all_cut + " && " + cut_trackonly;

  std::string const signal_definition = "is_delta_rad == 1 && true_nu_vtx_fid_contained == 1";
  std::string const background_definition = "!(" + signal_definition + ")";

  //APP
  //===========================================================================================

  std::vector<std::pair<TTree *, std::string>> const app_trees = {
    std::pair<TTree *, std::string>(oh.GetObject<TTree>(dir + "/runmv_sp.root", "LEEPhoton/vertex_tree"), "ncdelta"),
    std::pair<TTree *, std::string>(oh.GetObject<TTree>(dir + "/runmv_sp_cosmic.root", "LEEPhoton/vertex_tree"), "ncdelta_cosmic"),
    std::pair<TTree *, std::string>(oh.GetObject<TTree>(dir + "/runmv_bnb_cosmic.root", "LEEPhoton/vertex_tree"), "bnb_cosmic_background"),
    std::pair<TTree *, std::string>(oh.GetObject<TTree>(dir + "/runmv_bnb_cosmic.root", "LEEPhoton/vertex_tree"), "bnb_cosmic")
  };
  
  std::vector<std::string> const tree_cuts = {
    all_cut + " && " + signal_definition,
    all_cut + " && " + signal_definition,
    all_cut + " && " + background_definition,
    all_cut
  };
  
  std::vector<std::pair<std::string, std::string>> const mva_branches = {
    {"mva", "d"}
  };

  //SIGNIFICANCE
  //===========================================================================================

  double const run_pot = 6.6e20;

  //OPTIONS
  //===========================================================================================

  if(option == "train") {
    train(identifier_notrack, all_cut_notrack, signal_definition, background_definition, signal_training_trees, background_training_trees, variables_notrack, methods);
    train(identifier_trackonly, all_cut_trackonly, signal_definition, background_definition, signal_training_trees, background_training_trees, variables_trackonly, methods);
  }
  
  else if(option == "app") {
    app(identifier_notrack, app_trees, tree_cuts, all_cut_notrack, variables_notrack, methods);
    app(identifier_trackonly, app_trees, tree_cuts, all_cut_trackonly, variables_trackonly, methods);
  }

  else if(option == "merge") {
    merge(identifier+"_app.root", identifier_notrack+"_app.root", identifier_trackonly+"_app.root", app_trees, methods, mva_branches, all_cut, cut_notrack, cut_trackonly);
  }
    
  else if(option == "significance_sep") {
    std::vector<std::pair<TTree *, std::string>> const signal_significance_trees = {
      std::pair<TTree *, std::string>(oh.GetObject<TTree>(dir + "/runmv_sp_cosmic.root", "LEEPhoton/vertex_tree"), "ncdelta_cosmic"),
    };
    std::vector<std::pair<std::string, std::string>> const signal_significance_tree_cuts = {
      {signal_definition + " && " + all_cut_notrack, signal_definition + " && " + all_cut_trackonly}
    };
    std::vector<std::pair<int, double>> const signal_significance_pots = {
      get_pot(dir + "/runmv_sp_cosmic.root", "LEEPhoton/get_pot")
    };
    std::vector<std::pair<TTree *, std::string>> const background_significance_trees = {
      std::pair<TTree *, std::string>(oh.GetObject<TTree>(dir + "/runmv_bnb_cosmic.root", "LEEPhoton/vertex_tree"), "bnb_cosmic_background"),
    };
    std::vector<std::pair<std::string, std::string>> const background_significance_tree_cuts = {
      {background_definition + " && " + all_cut_notrack, background_definition + " && " + all_cut_trackonly}
    };
    std::vector<std::pair<int, double>> const background_significance_pots = {
      get_pot(dir + "/runmv_bnb_cosmic.root", "LEEPhoton/get_pot")
    };
    significance_seperate(identifier+"_app.root", run_pot,
			  signal_significance_trees, signal_significance_tree_cuts, signal_significance_pots, 
			  background_significance_trees, background_significance_tree_cuts, background_significance_pots, 
			  methods);
  }

  else if(option == "get_response_tlimits") {
    get_mva_response_hists(identifier+"_mva_response.root", identifier+"_app.root", app_trees, methods, mva_branches, "50", tree_cuts, cut_notrack, cut_trackonly);
  }

  else if(option == "plot_response_tlimits") {
    plot_mva_response_hists(identifier+"_plot_mva_response.root", identifier+"_mva_response.root", app_trees, methods, mva_branches);
  }
  
  else if(option == "tlimits") {
    tlimits(identifier+"_mva_response.root", methods, run_pot, signal_training_pots.front().second, background_training_pots.front().second, background_training_pots.front().second);
  }  

  else if(option == "tlimits_var") {
    tlimits_var(identifier+"_mva_response.root", methods, 6.6e20, 40, 6.6e20, 10e21, signal_training_pots.front().second, background_training_pots.front().second, background_training_pots.front().second);
  }  

  else {
    std::cout << "WARNING: " << option << " is an invalid option\n";
  }

}
