#include "object_helper.h"
#include "get_pot_info.h"
#include "train.h"
#include "app.h"
#include "merge.h"
#include "significance.h"
#include "get_mva_response_hists.h"
#include "plot_mva_response_hists.h"
#include "gen_tlimits.h"
#include "plotstack.h"
#include "load_mva_param.h"
#include "data_mc.h"
#include "efficiency.h"

#include "tinyxml.h"

#include <getopt.h>

//#include "plot_bdt_variables.hpp"


std::pair<int, double> get_pot(std::string const & file_path, std::string const & tree_path) {

	object_helper<TTree> oh;
	TTree * pot_tree = oh.GetObject(file_path, tree_path);
	int events = -1;
	double pot = -1;
	get_pot_info(pot_tree, pot, events);
	auto ans =  std::pair<int, double>(events, pot);
	std::cout<<file_path<<" POT: "<<pot<<std::endl;
	return ans ;//

}






int main (int argc, char *argv[]){

	// Just some simple argument things
	//===========================================================================================

	std::string dir = "../../../samples/";
	std::string mode_option = "train"; 
	std::string xml = "default.xml";


	const struct option longopts[] = 
	{
		{"dir", 		required_argument, 	0, 'd'},
		{"option",		required_argument,	0, 'o'},
		{"xml"	,		required_argument,	0, 'x'},
		{"help",		required_argument,	0, 'h'},
		{0,			no_argument, 		0,  0},
	};

	//some optioni/argument stuff
	int iarg = 0; opterr=1; int index;
	while(iarg != -1)
	{
		iarg = getopt_long(argc,argv, "x:o:d:h?", longopts, &index);

		switch(iarg)
		{
			case 'x':
				xml = optarg;
				break;
			case 'o':
				mode_option = optarg;
				break;
			case 'd':
				dir = optarg;
				break;
			case '?':
			case 'h':
				std::cout<<"Allowed arguments:"<<std::endl;
				std::cout<<"\t-d\t--dir\t\tDirectory for file inputs"<<std::endl;
				std::cout<<"\t-o\t--option\t\tOptional mode to run, train, app..etc.."<<std::endl;
				std::cout<<"\t-x\t--xml\t\tInput .xml file for configuring what MVA/BDT & param"<<std::endl;
				std::cout<<"\t-h\t--help\t\tThis help menu"<<std::endl;
				return 0;
		}

	}


	std::string nsig = "mcc84/merged.ncdelta_v1.0.root";
	std::string nsig_cosmic = "mcc84/merged.ncdeltacosmic_v1.0.root";
	//	std::string nsig = "mcc82/runmv_sp.root";
	//	std::string nsig_cosmic = "mcc82/runmv_sp_cosmic.root";



	// Setting up necessary variables and trees for TRAINING
	//===========================================================================================
	object_helper<TTree> oh;

	std::vector<TTree *> const signal_training_trees = {
		oh.GetObject(dir + nsig, "LEEPhoton/vertex_tree")
	};
	std::vector<std::pair<int, double>> const signal_training_pots = {
		get_pot(dir + nsig, "LEEPhoton/get_pot")
	};

	std::vector<TTree *> const background_training_trees = {
		oh.GetObject(dir + "mcc84/merged.bnbcosmic_v2.0.root", "LEEPhoton/vertex_tree")
	};
	std::vector<std::pair<int, double>> const background_training_pots = {
		get_pot(dir + "mcc84/merged.bnbcosmic_v2.0.root", "LEEPhoton/get_pot")
	};

	// All the variables in the "notrack" sample
	std::vector<std::pair<std::string, std::string>> const variables_notrack = {
		{"summed_associated_helper_shower_energy", "d"},
		{"reco_nuvertx", "d"},
		{"reco_nuverty", "d"},
		{"reco_nuvertz", "d"},
		{"reco_nu_vtx_dist_to_closest_tpc_wall", "d"},
		{"most_energetic_shower_reco_startz","d"},
		{"most_energetic_shower_reco_starty","d"},
		{"most_energetic_shower_reco_startx","d"},
		//INF		{"most_energetic_shower_reco_thetaxz", "d"},
		{"totalpe_ibg_sum","d"},
		//INF		{"most_energetic_shower_reco_thetayz", "d"},
		{"most_energetic_shower_reco_thetayx", "d"},
		{"most_energetic_shower_reco_length","d"},
		{"most_energetic_shower_bp_dist_to_tpc", "d"},
		//		{"most_energetic_shower_reco_dirx","d"},
		//		{"most_energetic_shower_reco_diry","d"},
		//		{"most_energetic_shower_reco_dirz","d"},
		{"reco_shower_dedx_plane0", "d"},
		{"reco_shower_dedx_plane1", "d"},
		{"reco_shower_dedx_plane2", "d"},
		{"closest_asso_shower_dist_to_flashzcenter","d"},


	};

	std::string angle_track_shower ="(longest_asso_track_reco_dirx*most_energetic_shower_reco_dirx+longest_asso_track_reco_diry*most_energetic_shower_reco_diry+longest_asso_track_reco_dirz*most_energetic_shower_reco_dirz)/(sqrt(longest_asso_track_reco_dirx*longest_asso_track_reco_dirx+longest_asso_track_reco_diry*longest_asso_track_reco_diry+longest_asso_track_reco_dirz*longest_asso_track_reco_dirz)*sqrt(most_energetic_shower_reco_dirx*most_energetic_shower_reco_dirx+most_energetic_shower_reco_diry*most_energetic_shower_reco_diry+most_energetic_shower_reco_dirz*most_energetic_shower_reco_dirz) )";

	// For the track sample, add to the notrack variables some more track related ones
	std::vector<std::pair<std::string, std::string>> variables_trackonly = variables_notrack;
	variables_trackonly.emplace_back("shortest_asso_shower_to_vert_dist", "d");
	variables_trackonly.emplace_back("longest_asso_track_thetaxz", "d");
	//variables_trackonly.emplace_back("cos(longest_asso_track_thetaxz)", "d");
	//inf	variables_trackonly.emplace_back("longest_asso_track_thetayz", "d");
	//inf	variables_trackonly.emplace_back("longest_asso_track_thetayx", "d");
	//variables_trackonly.emplace_back("cos(longest_asso_track_thetayz)", "d");
	variables_trackonly.emplace_back("reco_asso_tracks", "i");
	variables_trackonly.emplace_back("longest_asso_track_displacement", "d");
	//inf	variables_trackonly.emplace_back("longest_asso_track_reco_dirx","d");
	//inf	variables_trackonly.emplace_back("longest_asso_track_reco_diry","d");
	variables_trackonly.emplace_back("longest_asso_track_reco_dirz","d");
	//variables_trackonly.emplace_back(angle_track_shower.c_str(), "d");
	//All the necessary methods that we want to use for MVA, will stick these into an XML sometime soon

	MVALoader xml_methods(xml);
	std::vector<method_struct> const methods  = xml_methods.GetMethods(); 



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
		std::pair<TTree *, std::string>(oh.GetObject(dir + nsig, "LEEPhoton/vertex_tree"), "ncdelta"),
		std::pair<TTree *, std::string>(oh.GetObject(dir + nsig_cosmic, "LEEPhoton/vertex_tree"), "ncdelta_cosmic"),
		std::pair<TTree *, std::string>(oh.GetObject(dir + "mcc84/merged.bnbcosmic_v2.0.root", "LEEPhoton/vertex_tree"), "bnb_cosmic_background"),
		std::pair<TTree *, std::string>(oh.GetObject(dir + "mcc84/merged.bnbcosmic_v2.0.root", "LEEPhoton/vertex_tree"), "bnb_cosmic"),
		std::pair<TTree *, std::string>(oh.GetObject(dir + "data/merged.data5e19_v6.0.root", "LEEPhotonAnalysisData/vertex_tree"), "data"),
		std::pair<TTree *, std::string>(oh.GetObject(dir + "data/merged.bnbext_v3.0.root", "LEEPhotonAnalysisData/vertex_tree"), "dataext")
	};

	std::vector<std::string> const tree_cuts = {
		all_cut + " && " + signal_definition,
		all_cut + " && " + signal_definition,
		all_cut + " && " + background_definition,
		all_cut,
		all_cut,
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

	if(mode_option == "train") {
		//training the no_track and trackonly seperately.
		train(identifier_notrack, all_cut_notrack, signal_definition, background_definition, signal_training_trees, background_training_trees, variables_notrack, methods);
		train(identifier_trackonly, all_cut_trackonly, signal_definition, background_definition, signal_training_trees, background_training_trees, variables_trackonly, methods);
	}

	else if(mode_option == "app") {
		app(identifier_notrack,"app", app_trees, tree_cuts, all_cut_notrack, variables_notrack, methods);
		app(identifier_trackonly,"app", app_trees, tree_cuts, all_cut_trackonly, variables_trackonly, methods);

		/*	}else if(mode_option == "appdata"){

			app(identifier_notrack,"data_BNB",data_trees, data_cuts, all_cut_notrack, variables_notrack, methods);
			app(identifier_trackonly,"data_BNB", data_trees, data_cuts, all_cut_trackonly, variables_trackonly, methods);

			}
			else if(mode_option == "appext"){

			app(identifier_notrack,"data_EXT",ext_trees, data_cuts, all_cut_notrack, variables_notrack, methods);
			app(identifier_trackonly,"data_EXT", ext_trees, data_cuts, all_cut_trackonly, variables_trackonly, methods);

		 */	}
	else if(mode_option == "merge") {
		merge(identifier+"_merged_app.root", identifier_notrack+"_app.root", identifier_trackonly+"_app.root", app_trees, methods, mva_branches, all_cut, cut_notrack, cut_trackonly);
	}
	/*
	   else if(mode_option == "mergedata") {
	   merge(identifier+"_merged_data_BNB.root", identifier_notrack+"_data_BNB.root", identifier_trackonly+"_data_BNB.root", data_trees, methods, mva_branches, all_cut, cut_notrack, cut_trackonly);
	   }
	   else if(mode_option == "mergeext") {
	   merge(identifier+"_merged_data_EXT.root", identifier_notrack+"_data_EXT.root", identifier_trackonly+"_data_EXT.root", ext_trees, methods, mva_branches, all_cut, cut_notrack, cut_trackonly);
	   }
	 */

	else if(mode_option == "significance_sep" || mode_option == "sig") {

		std::vector<std::pair<TTree *, std::string>> const signal_significance_trees = {
			std::pair<TTree *, std::string>(oh.GetObject(dir + nsig_cosmic, "LEEPhoton/vertex_tree"), "ncdelta_cosmic"),
		};
		std::vector<std::pair<std::string, std::string>> const signal_significance_tree_cuts = {
			{signal_definition + " && " + all_cut_notrack, signal_definition + " && " + all_cut_trackonly}
		};
		std::vector<std::pair<int, double>> const signal_significance_pots = {
			get_pot(dir + nsig_cosmic, "LEEPhoton/get_pot")
		};
		std::vector<std::pair<TTree *, std::string>> const background_significance_trees = {
			std::pair<TTree *, std::string>(oh.GetObject(dir + "mcc84/merged.bnbcosmic_v2.0.root", "LEEPhoton/vertex_tree"), "bnb_cosmic_background")
				//	,std::pair<TTree *, std::string>(oh.GetObject(dir + "data/merged.bnbext_v2.0.root", "LEEPhoton/vertex_tree"), "bnb_cosmic_background")
		};

		std::vector<std::pair<std::string, std::string>> const background_significance_tree_cuts = {
			{background_definition + " && " + all_cut_notrack, background_definition + " && " + all_cut_trackonly}
		};
		std::vector<std::pair<int, double>> const background_significance_pots = {
			get_pot(dir + "mcc84/merged.bnbcosmic_v2.0.root", "LEEPhoton/get_pot")
		};
		significance_seperate(identifier+"_merged_app.root", run_pot, signal_significance_trees, signal_significance_tree_cuts, signal_significance_pots, 
				background_significance_trees, background_significance_tree_cuts, background_significance_pots,	methods);

	}else if(mode_option == "plot"){
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
	else if(mode_option == "get_response_tlimits") {
		get_mva_response_hists(identifier+"_mva_response.root", identifier+"_merged_app.root", app_trees, methods, mva_branches, "50", tree_cuts, cut_notrack, cut_trackonly);
	}

	else if(mode_option == "plot_response_tlimits") {
		plot_mva_response_hists(identifier+"_plot_mva_response.root", identifier+"_mva_response.root", app_trees, methods, mva_branches);
	}

	else if(mode_option == "tlimits") {
		tlimits(identifier+"_mva_response.root", methods, run_pot, signal_training_pots.front().second, background_training_pots.front().second, background_training_pots.front().second);
	}  

	else if(mode_option == "tlimits_var") {
		tlimits_var(identifier+"_mva_response.root", methods, 6.6e20, 40, 6.6e20, 10e21, signal_training_pots.front().second, background_training_pots.front().second, background_training_pots.front().second);
	}  
	else if(mode_option == "test"){
		std::string MCFRIEND = "runtmva_merged_app.root";

		bdt_file *data = new bdt_file("../../../samples/data/", "merged.data5e19_v6.0.root","Data5e19","e1","LEEPhotonAnalysisData",MCFRIEND,"data",kBlue-4,true );
		bdt_file *ext = new bdt_file("../../../samples/data/", "merged.bnbext_v3.0.root","BNBext","hist","LEEPhotonAnalysisData", MCFRIEND,"dataext",kGreen-3,true);

		bdt_file *mc4 = new bdt_file("../../../samples/mcc84", "merged.bnbcosmic_v2.0.root","BNB+cosmicOverlay_8.4","hist","LEEPhoton", MCFRIEND,"bnb_cosmic",kRed-4,false);
		bdt_file *sig = new bdt_file("../../../samples/mcc84/", "merged.ncdeltacosmic_v1.0.root","NCDelta","hist","LEEPhoton", MCFRIEND,"ncdelta_cosmic",kYellow,false);
		//bdt_file *sig = new bdt_file("../../../samples/mcc82/", "runmv_sp_cosmic.root","LEE NCDelta","hist","LEEPhoton", MCFRIEND,"ncdelta_cosmic",kYellow,false);

		mc4->leg = "f";
		sig->leg = "f";
		ext->leg = "f";

		double N_off_events = 373820;//  136208;// v2.0;
		double N_on_events = 544479;

		data->setPOT(4.95e19);
		ext->setPOT(4.95e19);


		double data_scale_factor = 547616.0/N_on_events;
		double ext_scale_factor = 1.285*(382718.0/N_off_events);//*(N_on_events/547616.0);
		ext->scale(ext_scale_factor);
		//	sig->scale(2.1);

		std::vector<bdt_file*> vec_files = {data,ext,mc4,sig};
		std::vector<int> which_stack = {0,1,1,1};


		//(std::string inname, std::string inbin, bool intrack) : 
		std::vector<bdt_variable> vars;

		int level = 2;


		if(level>=0){

			vars.push_back(bdt_variable("reco_shower_dedx_plane2","(48,0,15)", "Shower dE/dx Collection Plane [MeV/cm]",false));
			vars.push_back(bdt_variable("summed_associated_helper_shower_energy","(25,0,0.5)","Reco Shower Energy [GeV]", false));
			vars.push_back(bdt_variable("longest_asso_track_displacement","(25,0,500)","Track Length [cm]", true));
			vars.push_back(bdt_variable("reco_asso_tracks","(5,0,4)","Number Reco Tracks",false));

		}if (level>=1){

			vars.push_back(bdt_variable("totalpe_ibg_sum","(25,0,2000)","Number of PE",false));
			vars.push_back(bdt_variable("reco_asso_showers","(6,0,5)","Number of Reco Showers",false));	
			vars.push_back(bdt_variable("closest_asso_shower_dist_to_flashzcenter","(25,0,1000)","Distance from shower to flashcenter [cm]",false));
			vars.push_back(bdt_variable("reco_nu_vtx_dist_to_closest_tpc_wall","(25,0,125)","Vertex-TPC Wall Distance [cm]",false));
			vars.push_back(bdt_variable("shortest_asso_shower_to_vert_dist","(25,0,10)","Photon Coversion Length [cm]" ,false));
			vars.push_back(bdt_variable("most_energetic_shower_bp_dist_to_tpc","(25,0,1000)","Back Projected Distance from shower to TPC wall [cm]",false));
			vars.push_back(bdt_variable("most_energetic_shower_reco_thetaxz","(25,-1.7,1.7)","Shower Angle xy [rad]",false));
			vars.push_back(bdt_variable("most_energetic_shower_reco_thetayz","(25,-1.7,1.7)","Shower Angle yz [rad]",false));
			vars.push_back(bdt_variable("longest_asso_track_thetayz","(25,-1.7,1.7)","Track Angle yz [rad]",true));
			vars.push_back(bdt_variable("longest_asso_track_thetaxz","(25,-1.7,1.7)","Track Angle xz [rad]",true));

		}if(level>=2){

			vars.push_back(bdt_variable("summed_associated_reco_shower_energy","(25,0,0.5)","Energy [GeV]",false));
			vars.push_back(bdt_variable("most_energetic_shower_reco_length","(25,0,125)","Distance [cm]",false));
			vars.push_back(bdt_variable("reco_nuvertx","(25,0,300)","Distance [cm]",false));
			vars.push_back(bdt_variable("reco_nuverty","(25,-300,+300)","Distance [cm]",false));
			vars.push_back(bdt_variable("reco_nuvertz","(25,0,1000)","Distance [cm]",false));
			vars.push_back(bdt_variable("reco_shower_dedx_plane0","(48,0,15)", "dE/dx [MeV/cm]",false));
			vars.push_back(bdt_variable("reco_shower_dedx_plane1","(48,0,15)", "dE/dx [MeV/cm]",false));
			vars.push_back(bdt_variable("totalpe_bbg_sum","(25,0,2000)","Num PE",false));
		}

		bdt_variable true_shower("delta_photon_energy","(25,0,1.0)","True Photon Energy [GeV]", false);


		bool run_eff = true;
		bool run_bdt_response = false;
		bool run_full_comparason = false;

		if(run_eff){
			TFile *fout = new TFile("eff_response.root","recreate");	
			eff_analysis eff(sig, true_shower);
			eff.plot(fout, 0.535, 0.4925);

			eff_analysis eff_reco(sig, vars.at(1));
			eff_reco.plot(fout, 0.535, 0.4925);

			fout->Close();
		}
		if(run_bdt_response){
			TFile *fout0 = new TFile("bdt_response.root","recreate");	
			bdt_response_analysis bdt(vec_files,which_stack);
			bdt.plot_bdt_response(fout0);
			fout0->Close();
		}
		if(run_full_comparason){
			TFile * fout = new TFile("data_mc_comparason.root","recreate");
			for(auto & v: vars){
				compare_instance c(vec_files, v, which_stack);
				c.plot(fout);

			}

			fout->Close();
		}

	}


	else {
		std::cout << "WARNING: " << mode_option << " is an invalid option\n";
	}

}
