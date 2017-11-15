#include "object_helper.h"
#include "train.h"
#include "app.h"
#include "merge.h"
#include "significance.h"
#include "get_mva_response_hists.h"
#include "plot_mva_response_hists.h"
#include "gen_tlimits.h"
#include "plotstack.h"


//#include "plot_bdt_variables.hpp"


std::pair<int, double> get_pot(std::string const & file_path, std::string const & tree_path) {

	object_helper<TTree> oh;
	TTree * pot_tree = oh.GetObject(file_path, tree_path);
	int events = -1;
	double pot = -1;
	pot_tree->SetBranchAddress("number_of_events", &events);
	pot_tree->SetBranchAddress("pot", &pot);
	pot_tree->GetEntry(0);

	return std::pair<int, double>(events, pot);

}

int main (int argc, char *argv[]){

	// Just some simple argument htings
	//===========================================================================================

	if(argc != 3) {
		std::cout << "ERROR - Required inputs:\n->Path to sample file directory\n->Option\n";
		exit(1);
	}
	std::string const dir = argv[1];
	std::string const option = argv[2];

	if(option == "help" || option == "?" || option == "-h"){
		std::cout <<"Required inputs:\n->Path to sample file directory\n->Option\n";
		std::cout <<"Options are:\n->train\n->app\n->merge\n->significance_sep\n";
		exit(1);
	}

	// Setting up necessary variables and trees for TRAINING
	//===========================================================================================
	object_helper<TTree> oh;

	std::vector<TTree *> const signal_training_trees = {
		oh.GetObject(dir + "/runmv_sp.root", "LEEPhoton/vertex_tree")
	};
	std::vector<std::pair<int, double>> const signal_training_pots = {
		get_pot(dir + "/runmv_sp.root", "LEEPhoton/get_pot")
	};

	std::vector<TTree *> const background_training_trees = {
		oh.GetObject(dir + "/runmv_bnb_cosmic.root", "LEEPhoton/vertex_tree")
	};
	std::vector<std::pair<int, double>> const background_training_pots = {
		get_pot(dir + "/runmv_bnb_cosmic.root", "LEEPhoton/get_pot")
	};

	// All the variables in the "notrack" sample
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

	// For the track sample, add to the notrack variables some more track related ones
	std::vector<std::pair<std::string, std::string>> variables_trackonly = variables_notrack;
	variables_trackonly.emplace_back("shortest_asso_shower_to_vert_dist", "d");
	variables_trackonly.emplace_back("longest_asso_track_thetaxz", "d");
	variables_trackonly.emplace_back("longest_asso_track_thetayz", "d");
	variables_trackonly.emplace_back("reco_asso_tracks", "i");
	variables_trackonly.emplace_back("longest_asso_track_displacement", "d");


	//All the necessary methods that we want to use for MVA, will stick these into an XML sometime soon
	std::vector<method_struct> const methods {
			{TMVA::Types::kBDT, "BDTG",
				"!H:!V:NTrees=2000:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=20:MaxDepth=3"}
			,{TMVA::Types::kBDT, "BDT", 
				"!H:!V:NTrees=850:MinNodeSize=2.5%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=20"}
		//	,{TMVA::Types::kBDT, "BDTB",
		//		"!H:!V:NTrees=400:BoostType=Bagging:SeparationType=GiniIndex:nCuts=20"}
		//	,{TMVA::Types::kBDT, "BDTD",
		//		"!H:!V:NTrees=400:MinNodeSize=5%:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:VarTransform=Decorrelate"}
		//	,{TMVA::Types::kBDT, "BDTF",
		//		"!H:!V:NTrees=50:MinNodeSize=2.5%:UseFisherCuts:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20"}
		//	,{TMVA::Types::kRuleFit, "RuleFit",
		//		"H:!V:RuleFitModule=RFTMVA:Model=ModRuleLinear:MinImp=0.001:RuleMinDist=0.001:NTrees=20:fEventsMin=0.01:fEventsMax=0.5:GDTau=-1.0:GDTauPrec=0.01:GDStep=0.01:GDNSteps=10000:GDErrScale=1.02"}  
	};


	//some convientant labels
	std::string const identifier = "runtmva";
	std::string const identifier_notrack = identifier + "_notrack";
	std::string const identifier_trackonly = identifier + "_trackonly";


	//And the Cuts that are definied, definitely define these in XML (and changeable by argument)
	std::string const all_cut = "passed_swtrigger == 1 && closest_asso_shower_dist_to_flashzcenter <= 40 && totalpe_ibg_sum > 140 && reco_asso_showers == 1";
	std::string const cut_notrack = "reco_asso_tracks == 0";
	std::string const all_cut_notrack = all_cut + " && " + cut_notrack;
	std::string const cut_trackonly = "reco_asso_tracks > 0";
	std::string const all_cut_trackonly = all_cut + " && " + cut_trackonly;

	std::string const signal_definition = "is_delta_rad == 1 && true_nu_vtx_fid_contained == 1";
	std::string const background_definition = "!(" + signal_definition + ")";

	// defining trees and variables to do with	APP (APPLICATION)
	//===========================================================================================

	std::vector<std::pair<TTree *, std::string>> const app_trees = {
		std::pair<TTree *, std::string>(oh.GetObject(dir + "/runmv_sp.root", "LEEPhoton/vertex_tree"), "ncdelta"),
		std::pair<TTree *, std::string>(oh.GetObject(dir + "/runmv_sp_cosmic.root", "LEEPhoton/vertex_tree"), "ncdelta_cosmic"),
		std::pair<TTree *, std::string>(oh.GetObject(dir + "/runmv_bnb_cosmic.root", "LEEPhoton/vertex_tree"), "bnb_cosmic_background"),
		std::pair<TTree *, std::string>(oh.GetObject(dir + "/runmv_bnb_cosmic.root", "LEEPhoton/vertex_tree"), "bnb_cosmic")
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



	//===========================================================================================
	//===========================================================================================
	//		Main flow of the program , using OPTIONS
	//===========================================================================================
	//===========================================================================================

	if(option == "train") {
		//training the no_track and trackonly seperately.
		train(identifier_notrack, all_cut_notrack, signal_definition, background_definition, signal_training_trees, background_training_trees, variables_notrack, methods);
		train(identifier_trackonly, all_cut_trackonly, signal_definition, background_definition, signal_training_trees, background_training_trees, variables_trackonly, methods);
	}

	else if(option == "app") {
		app(identifier_notrack, app_trees, tree_cuts, all_cut_notrack, variables_notrack, methods);
		app(identifier_trackonly, app_trees, tree_cuts, all_cut_trackonly, variables_trackonly, methods);
	}

	else if(option == "merge") {
		merge(identifier+"_merged_app.root", identifier_notrack+"_app.root", identifier_trackonly+"_app.root", app_trees, methods, mva_branches, all_cut, cut_notrack, cut_trackonly);
	}

	else if(option == "significance_sep" || option == "sig") {

		std::vector<std::pair<TTree *, std::string>> const signal_significance_trees = {
			std::pair<TTree *, std::string>(oh.GetObject(dir + "/runmv_sp_cosmic.root", "LEEPhoton/vertex_tree"), "ncdelta_cosmic"),
		};
		std::vector<std::pair<std::string, std::string>> const signal_significance_tree_cuts = {
			{signal_definition + " && " + all_cut_notrack, signal_definition + " && " + all_cut_trackonly}
		};
		std::vector<std::pair<int, double>> const signal_significance_pots = {
			get_pot(dir + "/runmv_sp_cosmic.root", "LEEPhoton/get_pot")
		};
		std::vector<std::pair<TTree *, std::string>> const background_significance_trees = {
			std::pair<TTree *, std::string>(oh.GetObject(dir + "/runmv_bnb_cosmic.root", "LEEPhoton/vertex_tree"), "bnb_cosmic_background"),
		};
		std::vector<std::pair<std::string, std::string>> const background_significance_tree_cuts = {
			{background_definition + " && " + all_cut_notrack, background_definition + " && " + all_cut_trackonly}
		};
		std::vector<std::pair<int, double>> const background_significance_pots = {
			get_pot(dir + "/runmv_bnb_cosmic.root", "LEEPhoton/get_pot")
		};
		significance_seperate(identifier+"_merged_app.root", run_pot,
				signal_significance_trees, signal_significance_tree_cuts, signal_significance_pots, 
				background_significance_trees, background_significance_tree_cuts, background_significance_pots, 
				methods);
	}else if(option == "plot"){
		//Just Some boring plotting routines for now,

		std::cout<<"Starting --plotstack-- routine."<<std::endl;
		plotstack(dir,  identifier+"_merged_app.root" );
		std::cout<<"Ending --plotstack-- routine."<<std::endl;



		TFile *fin_notrack = new TFile((identifier_notrack+"_training.root").c_str());
		TFile *fin_trackonly = new TFile((identifier_trackonly+"_training.root").c_str());

		std::vector<TH1F*> vec_sig_notrack;
		std::vector<TH1F*> vec_bkg_notrack;

		std::vector<TH1F*> vec_sig_trackonly;
		std::vector<TH1F*> vec_bkg_trackonly;

		TCanvas *c_trackonly = new TCanvas("all_variables_trackonly","all_variables_trackonly",1200,variables_trackonly.size()*1000 );		
		c_trackonly->Divide(1,variables_trackonly.size());
		int whichcan_trackonly=0;

		TCanvas *c_notrack = new TCanvas("all_variables_notrack","all_variables_notrack",1200,variables_notrack.size()*1000 );		
		c_notrack->Divide(1,variables_notrack.size());
		int whichcan_notrack=0;

		for(auto vars: variables_notrack){
			TH1F * sig = (TH1F*)fin_notrack->Get(("dataset/InputVariables_Id/"+vars.first+"__Signal_Id").c_str());	
			TH1F * bkg = (TH1F*)fin_notrack->Get(("dataset/InputVariables_Id/"+vars.first+"__Background_Id").c_str());	
			vec_sig_notrack.push_back(sig);		
			vec_bkg_notrack.push_back(bkg);		
			TPad* p = (TPad*)c_notrack->cd(++whichcan_notrack);
			p->SetTopMargin(0.075);
			p->SetBottomMargin(0.075);		

			sig->Scale(1.0/sig->Integral());
			bkg->Scale(1.0/bkg->Integral());

			sig->SetStats(0);
			sig->SetTitle(vars.first.c_str());
			sig->GetXaxis()->SetTitle(vars.first.c_str());
			sig->GetYaxis()->SetTitle("Events");

			sig->SetLineColor(kRed-6);
			bkg->SetLineColor(kBlue-7);
			sig->SetLineWidth(3);
			bkg->SetLineWidth(3);

			sig->Draw("hist");
			bkg->Draw("hist same");
			sig->SetMaximum( std::max(sig->GetMaximum(), bkg->GetMaximum())+0.01);

			TLegend * pl = new TLegend(0.6,0.6,0.89,0.89);
			pl->SetFillStyle(0);
			pl->SetLineColor(kWhite);
			pl->AddEntry(sig,"Signal","l");
			pl->AddEntry(bkg,"Background","l");
			pl->Draw();
		} 


		for(auto vars: variables_trackonly){
			TH1F * sig = (TH1F*)fin_trackonly->Get(("dataset/InputVariables_Id/"+vars.first+"__Signal_Id").c_str());	
			TH1F * bkg = (TH1F*)fin_trackonly->Get(("dataset/InputVariables_Id/"+vars.first+"__Background_Id").c_str());	
			vec_sig_notrack.push_back(sig);		
			vec_bkg_notrack.push_back(bkg);		
			TPad* p = (TPad*)c_trackonly->cd(++whichcan_trackonly);
			p->SetTopMargin(0.075);
			p->SetBottomMargin(0.075);		


			sig->Scale(1.0/sig->Integral());
			bkg->Scale(1.0/bkg->Integral());

			sig->SetStats(0);
			sig->SetTitle(vars.first.c_str());
			sig->GetXaxis()->SetTitle(vars.first.c_str());
			sig->GetYaxis()->SetTitle("Events");

			sig->SetLineColor(kRed-6);
			bkg->SetLineColor(kBlue-7);
			sig->SetLineWidth(3);
			bkg->SetLineWidth(3);

			sig->Draw("hist");
			bkg->Draw("hist same");
			sig->SetMaximum( std::max(sig->GetMaximum(), bkg->GetMaximum())+0.01);

			TLegend * pl = new TLegend(0.6,0.6,0.89,0.89);
			pl->SetFillStyle(0);
			pl->SetLineColor(kWhite);
			pl->AddEntry(sig,"Signal","l");
			pl->AddEntry(bkg,"Background","l");
			pl->Draw();
		} 



		c_notrack->SaveAs("notrack_variables.png","png");
		c_trackonly->SaveAs("trackonly_variables.png","png");
	
		fin_notrack->Close();
		fin_trackonly->Close();
	}
	else if(option == "get_response_tlimits") {
		get_mva_response_hists(identifier+"_mva_response.root", identifier+"_merged_app.root", app_trees, methods, mva_branches, "50", tree_cuts, cut_notrack, cut_trackonly);
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
