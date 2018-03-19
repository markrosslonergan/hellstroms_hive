/*#include "object_helper.h"
#include "get_pot_info.h"
#include "train.h"
#include "app.h"
#include "merge.h"
#include "significance.h"
#include "get_mva_response_hists.h"
#include "plot_mva_response_hists.h"
#include "gen_tlimits.h"
#include "plotstack.h"*/
//#include "data_mc_testing_suite.h"
//#include "efficiency.h"

#include "load_mva_param.h"
#include "tinyxml.h"

#include <getopt.h>


#include "bdt_file.h"
#include "bdt_datamc.h"
#include "bdt_var.h"
#include "bdt_precalc.h"
#include "bdt_info.h"
#include "bdt_train.h"
#include "bdt_app.h"
#include "bdt_response.h"
#include "bdt_recomc.h"
#include "bdt_sig.h"
#include "bdt_spec.h"




int main (int argc, char *argv[]){

	// Just some simple argument things
	//===========================================================================================

	std::string dir = "/home/mark/work/uBooNE/photon/tmva/";
	std::string mode_option = "train"; 
	std::string xml = "default.xml";
	std::string istrack ="track";

	const struct option longopts[] = 
	{
		{"dir", 		required_argument, 	0, 'd'},
		{"option",		required_argument,	0, 'o'},
		{"xml"	,		required_argument,	0, 'x'},
		{"track",		required_argument,	0, 't'},
		{"help",		required_argument,	0, 'h'},
		{0,			no_argument, 		0,  0},
	};

	//some optioni/argument stuff
	int iarg = 0; opterr=1; int index;
	while(iarg != -1)
	{
		iarg = getopt_long(argc,argv, "x:o:d:t:h?", longopts, &index);

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
			case 't':
				istrack = optarg;
				break;
			case '?':
			case 'h':
				std::cout<<"Allowed arguments:"<<std::endl;
				std::cout<<"\t-d\t--dir\t\tDirectory for file inputs"<<std::endl;
				std::cout<<"\t-o\t--option\t\tOptional mode to run, train, app..etc.."<<std::endl;
				std::cout<<"\t-t\t--track\t\tQuickly run between track and notrack"<<std::endl;
				std::cout<<"\t-x\t--xml\t\tInput .xml file for configuring what MVA/BDT & param"<<std::endl;
				std::cout<<"\t-h\t--help\t\tThis help menu"<<std::endl;
				return 0;
		}

	}


	TFile * ftest = new TFile(("test+"+istrack+".root").c_str(),"recreate");
	//===========================================================================================
	//===========================================================================================
	//===========================================================================================
	//===========================================================================================




	MVALoader xml_methods(xml);
	std::vector<method_struct> TMVAmethods  = xml_methods.GetMethods(); 


	std::string new_precuts;
	std::string num_track_cut = ">0";
	if(istrack == "track"){
		new_precuts =  "reco_nu_vtx_dist_to_closest_tpc_wall > 10 && shortest_asso_shower_to_vert_dist > 2 && longest_asso_track_displacement < 150 && summed_associated_helper_shower_energy > 0.05 && totalpe_ibg_sum >50";
		num_track_cut = ">0";

	}else if(istrack == "notrack"){
		new_precuts = "reco_nu_vtx_dist_to_closest_tpc_wall > 10 && summed_associated_helper_shower_energy > 0.05 && totalpe_ibg_sum > 50";
		num_track_cut = "==0";
	}



	std::string postcuts = {"reco_asso_tracks == 1 "};//
//&& fabs(cos(atan(longest_asso_track_reco_diry/longest_asso_track_reco_dirz))) > 0.4  && fabs(cos(atan(most_energetic_shower_reco_diry/most_energetic_shower_reco_dirz))) > 0.4  "};//"most_energetic_shower_bp_dist_to_tpc > 30 &&most_energetic_shower_bp_dist_to_tpc < 400 && reco_asso_tracks ==1 &&reco_shower_dedx_plane2 < 5 && track_dEdx_tree.longest_asso_track_bragg_start_parD > -0.5 &&track_dEdx_tree.longest_asso_track_bragg_start_parD < 0.25 "};
//"closest_asso_shower_dist_to_flashzcenter < 75 &&cos(atan(longest_asso_track_reco_diry/longest_asso_track_reco_dirz)) > 0.5  &&cos(atan(most_energetic_shower_reco_diry/most_energetic_shower_reco_dirz)) > 0.5  && 

	//Set up some info about the BDTs to pass along
	bdt_info bnb_bdt_info("bnb_"+istrack, "BNB focused BDT");
	bdt_info cosmic_bdt_info("cosmic_"+istrack, "Cosmic focused BDT");


	std::string base_cuts = "totalpe_ibg_sum > 0 && reco_asso_showers == 1 && reco_asso_tracks "+num_track_cut;
	std::string intime_base_cuts = "totalpe_ibg_sum > 0 && reco_asso_showers == 1 && reco_asso_tracks "+num_track_cut;

	std::string signal_definition = "is_delta_rad == 1 && true_nu_vtx_fid_contained == 1";
	std::string background_definition = "is_delta_rad == 0";//"!(" + bnb_bdt_info.signal_definition + ")";

	// takes 5 arguments ( 
	bdt_flow signal_flow(base_cuts +"&&"+signal_definition,	new_precuts+"&& passed_swtrigger ==1",	postcuts,cosmic_bdt_info.identifier,	bnb_bdt_info.identifier);
	bdt_flow cosmic_flow(intime_base_cuts,			new_precuts , postcuts,	cosmic_bdt_info.identifier,bnb_bdt_info.identifier);
	bdt_flow bkg_flow(base_cuts +"&&"+background_definition,new_precuts+ "&& passed_swtrigger ==1",postcuts,	cosmic_bdt_info.identifier,	bnb_bdt_info.identifier);
	bdt_flow data_flow(base_cuts ,				new_precuts+"&& passed_swtrigger ==1",postcuts,	cosmic_bdt_info.identifier, 	bnb_bdt_info.identifier);


	// BDT files, in the form (location, rootfile, name, hisotgram_options, tfile_folder, tag, color, BDT_CUT )		
	bdt_file *signal_pure    = new bdt_file(dir+"samples/mcc86/", "merged.ncsignal_v2.0.root",	"NCDeltaSignalPure",	   "hist","LEEPhoton/",  kRed-7, signal_flow);
	bdt_file *signal_cosmics = new bdt_file(dir+"samples/mcc86/", "merged.ncsignal_cosmics_v2.0.root", "NCDeltaSignalCosmics", "hist","LEEPhoton/",  kRed-7, signal_flow);
	bdt_file *bnb_pure    = new bdt_file(dir+"samples/mcc86/", "merged.bnb_v1.0.root",	"BNBPure",	   "hist","LEEPhoton/",  kBlue-4, bkg_flow);
	bdt_file *bnb_cosmics = new bdt_file(dir+"samples/mcc86/", "merged.bnbcosmic_v3.0_mcc86_withcalo.root", "BNBCosmics", "hist","LEEPhoton/",  kBlue-4, bkg_flow);
	bdt_file *intime = new bdt_file(dir+"samples/mcc86/", "merged.intime_v1.0.root" ,"IntimeCosmics","hist","LEEPhoton/", kGreen-3, cosmic_flow);

	bdt_file *data5e19 = new bdt_file(dir+"samples/mcc87/", "merged.data5e19_v10.root" ,"Data5e19","E1","LEEPhotonAnalysisData", kBlack, data_flow);
	bdt_file *bnbext = new bdt_file(dir+"samples/mcc87/", "merged.bnbext_v7.0.root" ,"BNBext","E1","LEEPhotonAnalysisData", kBlack, data_flow);


	std::vector<bdt_file*> bdt_files = {signal_pure, signal_cosmics, bnb_pure, bnb_cosmics, intime, data5e19,bnbext};

	signal_pure->scale_data = 3.1;
	signal_cosmics->scale_data = 3.1;

	//Add Any other info in the form of FRIENDS! e.g track dEdx 
	signal_pure->addFriend("track_dEdx_tree",dir+"track_dEdx/trackdEdx_nsignal.root");			
	signal_cosmics->addFriend("track_dEdx_tree",dir+"track_dEdx/trackdEdx_nsignal_cosmics.root");			
	bnb_pure->addFriend("track_dEdx_tree",dir+"track_dEdx/trackdEdx_bnb.root");			
	bnb_cosmics->addFriend("track_dEdx_tree",dir+"track_dEdx/trackdEdx_bnb_cosmics.root");			
	intime->addFriend("track_dEdx_tree",dir+"track_dEdx/trackdEdx_intime.root");			
	data5e19->addFriend("track_dEdx_tree",dir+"track_dEdx/trackdEdx_data5e19.root");			
	bnbext->addFriend("track_dEdx_tree",dir+"track_dEdx/trackdEdx_bnbext.root");			

	//Variables!
	std::string angle_track_shower ="longest_asso_track_reco_dirx*most_energetic_shower_reco_dirx+longest_asso_track_reco_diry*most_energetic_shower_reco_diry+longest_asso_track_reco_dirz*most_energetic_shower_reco_dirz";

	std::vector<bdt_variable> vars;

	vars.push_back(bdt_variable("reco_shower_dedx_plane2","(40,0,9)", "Shower dE/dx Collection Plane [MeV/cm]",false,"d"));
	vars.push_back(bdt_variable("summed_associated_helper_shower_energy","(25,0,0.6)","Reconstructed Shower Energy [GeV]", false,"d"));
	vars.push_back(bdt_variable("most_energetic_shower_reco_length","(25,0,125)","Shower Length [cm]",false,"d"));

	vars.push_back(bdt_variable("totalpe_ibg_sum","(25,0,2000)","Total in Beam-Gate PE",false,"d"));
	vars.push_back(bdt_variable("closest_asso_shower_dist_to_flashzcenter","(25,0,1000)","Distance from Shower to Flashcenter [cm]",false,"d"));

	vars.push_back(bdt_variable("reco_nu_vtx_dist_to_closest_tpc_wall","(25,0,125)","Reconstructed Vertex to TPC Wall Distance [cm]",false,"d"));
	vars.push_back(bdt_variable("most_energetic_shower_bp_dist_to_tpc","(25,0,1000)","Back Projected Distance from Shower to TPC wall [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_nuvertx","(25,0,300)"," Reconstructed Vertex x-location [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_nuverty","(25,-300,+300)","Reconstructed Vertex y-location [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_nuvertz","(25,0,1000)","Reconstructed Vertex z-location [cm]",false,"d"));

	vars.push_back(bdt_variable("cos(atan2(most_energetic_shower_reco_diry,most_energetic_shower_reco_dirz))","(25,-1,1)","Reconstructed Shower - Cosine Theta", true,"d"));
	vars.push_back(bdt_variable("cos(atan2(most_energetic_shower_reco_diry,most_energetic_shower_reco_dirx))","(25,-1,1)","Reconstructed Shower - Cosine Phi", true,"d"));



	if(istrack=="track"){

		vars.push_back(bdt_variable("track_dEdx_tree.longest_asso_track_range","(25,0,500)","Reconstructed Track Range? [cm]", true,"d"));
		vars.push_back(bdt_variable("longest_asso_track_displacement","(25,0,500)","Reconstructed Track Length [cm]", true,"d"));

		vars.push_back(bdt_variable("track_dEdx_tree.longest_asso_track_bragg_start_parD", "(25,-2,2)","Bragg Parameter D", true,"d"));
		vars.push_back(bdt_variable("shortest_asso_shower_to_vert_dist","(25,0,10)","Photon Coversion Length from Reconstructed Vertex [cm]" ,false,"d"));
		vars.push_back(bdt_variable("track_dEdx_tree.longest_asso_track_bragg_start_parA", "(30,0,30)","Proton Bragg Fit Parameter", true,"d"));


		vars.push_back(bdt_variable("cos(atan2(longest_asso_track_reco_diry,longest_asso_track_reco_dirz))","(25,-1,1)","Reconstructed Track - Cosine Theta", true,"d"));
		vars.push_back(bdt_variable("cos(atan2(longest_asso_track_reco_diry,longest_asso_track_reco_dirx))","(25,-1,1)","Reconstructed Track - Cosine Phi", true,"d"));

		//		vars.push_back(bdt_variable("cos(longest_asso_track_theta)","(25,-3.2,3.2)","Theta [Rad]", true,"d"));
		//		vars.push_back(bdt_variable("cos(longest_asso_track_phi)","(25,0,0,3.2)","Phi [Degrees]", true,"d"));

		vars.push_back(bdt_variable(angle_track_shower,	"(25,-1,1)","Cosine Track-Shower Angle ",true,"d"));
		vars.push_back(bdt_variable("reco_asso_tracks","(5,0,4)","Number of Reconstructed Tracks",false,"i"));

	}


	//This batch appears to be nan or inf a lot
	//vars.push_back(bdt_variable("most_energetic_shower_reco_thetaxz","(25,-1.7,1.7)","Shower Angle xy [rad]",false));
	//vars.push_back(bdt_variable("most_energetic_shower_reco_thetayz","(25,-1.7,1.7)","Shower Angle yz [rad]",false));
	//vars.push_back(bdt_variable("longest_asso_track_thetaxz","(25,-1.7,1.7)","Track Angle xz [rad]",true));
	//vars.push_back(bdt_variable("longest_asso_track_thetayz","(25,-1.7,1.7)","Track Angle yz [rad]",true));

	//This batch appears to be boring
	//vars.push_back(bdt_variable("reco_shower_dedx_plane0","(48,0,15)", "dE/dx [MeV/cm]",false));
	//vars.push_back(bdt_variable("reco_shower_dedx_plane1","(48,0,15)", "dE/dx [MeV/cm]",false));
	//vars.push_back(bdt_variable("totalpe_bbg_sum","(25,0,2000)","Num PE",false));

	//This is fixed to be 1!
	//vars.push_back(bdt_variable("reco_asso_showers","(6,0,5)","Number of Reco Showers",false));	

	std::cout<<"--------------------------------------------------------------------------"<<std::endl;
	std::cout<<"--------------------------------------------------------------------------"<<std::endl;
	for(auto &f: bdt_files){
		std::cout<<"Loading "<<f->tag<<"\t with "<<f->tevent->GetEntries()<<"\t events and "<<f->tvertex->GetEntries()<<"\t verticies."<<std::endl;
		std::cout<<"POT of file loaded is: "<<f->pot<<"\t\t which is pot/event: "<<f->tevent->GetEntries()/f->pot<<std::endl;
		std::cout<<"Scale factor is then: "<<f->scale_data<<std::endl;
	}
	std::cout<<"--------------------------------------------------------------------------"<<std::endl;
	std::cout<<"--------------------------------------------------------------------------"<<std::endl;





	/*
	   Best is at track_cosmicut: 0.536655 track_bnbcut: 0.500287
	   Best is at notrack.g_cosmicut: 0.525239 notrack_bnbcut: 0.508714
	   track sig: 22.1861 track bkg 511.006 0.98145
	   notrack sig: 16.8038 notrack bkg 368.859 0.874938
Combined: 1.31445 with sig 38.9899 879.865 s/sqrtb 1.31445
	 */

	//0.53842 0.505593 1.21062
	double fcoscut;
	double fbnbcut;
	if(istrack == "track"){
		//fcoscut = 0.53842;
		//fbnbcut = 0.505593;
		fcoscut = 0.53665;
		fbnbcut = 0.500287;

	}else if(istrack == "notrack"){
		fcoscut = 0.525239;
		fbnbcut = 0.508714;
	}






	//===========================================================================================
	//===========================================================================================
	//		Main flow of the program , using OPTIONS
	//===========================================================================================
	//===========================================================================================

	if(mode_option == "train") {
		std::cout<<"**********************Starting COSMIC BDT Training*************************"<<std::endl;
		bdt_train(cosmic_bdt_info, signal_pure, intime, vars, TMVAmethods);
		std::cout<<"**********************Starting BNB BDT Training*************************"<<std::endl;
		bdt_train(bnb_bdt_info, signal_pure, bnb_pure, vars, TMVAmethods);


	}else if(mode_option == "app"){
		//Apply! This will update cosmic_bdt_info, signal file and bkg file. As in update them PROPERLY!	
		std::vector<bdt_file*> app_files = {data5e19,bnbext,signal_pure, bnb_pure, intime, signal_cosmics, bnb_cosmics}; 
		bdt_app(cosmic_bdt_info, app_files, vars, TMVAmethods);
		bdt_app(bnb_bdt_info, app_files, vars, TMVAmethods);
	}
	else if(mode_option == "response"){

		for(auto &method: TMVAmethods){
			for(int i=0; i< bdt_files.size(); i++){
				std::cout<<"Now adding TreeFriend: "<<cosmic_bdt_info.identifier<<"_app.root"<<" "<<bdt_files.at(i)->tag<<std::endl;
				bdt_files.at(i)->addFriend(bdt_files.at(i)->tag +"_"+cosmic_bdt_info.identifier,  cosmic_bdt_info.identifier+"_app"+".root");

				std::cout<<"Now adding TreeFriend: "<<bnb_bdt_info.identifier<<"_app.root"<<" "<<bdt_files.at(i)->tag<<std::endl;
				bdt_files.at(i)->addFriend(bdt_files.at(i)->tag +"_"+bnb_bdt_info.identifier,  bnb_bdt_info.identifier+"_app"+".root");
			}
		}

		//Ok print out Cosmic BDT
		bdt_response cosmic_response(cosmic_bdt_info, signal_pure, intime);
		cosmic_response.plot_bdt_response(ftest);

		bdt_response bnb_response(bnb_bdt_info,signal_pure, bnb_pure);
		bnb_response.plot_bdt_response(ftest);

	}	
	else if(mode_option == "recomc"){

		for(auto &method: TMVAmethods){
			for(int i=0; i< bdt_files.size(); i++){
				std::cout<<"Now adding TreeFriend: "<<cosmic_bdt_info.identifier<<"_app.root"<<" "<<bdt_files.at(i)->tag<<std::endl;
				bdt_files.at(i)->addFriend(bdt_files.at(i)->tag +"_"+cosmic_bdt_info.identifier,  cosmic_bdt_info.identifier+"_app"+".root");

				std::cout<<"Now adding TreeFriend: "<<bnb_bdt_info.identifier<<"_app.root"<<" "<<bdt_files.at(i)->tag<<std::endl;
				bdt_files.at(i)->addFriend(bdt_files.at(i)->tag +"_"+bnb_bdt_info.identifier,  bnb_bdt_info.identifier+"_app"+".root");
			}
		}

		bdt_recomc test;
		//plot_recomc(TFile *fout, bdt_file* file, bdt_variable var, double cut_cosmic_val, double cut_bnb_val){




		test.plot_recomc(ftest, bnb_cosmics, bnb_cosmics->getBDTVariable("bnb_track") , fcoscut,fbnbcut);
		test.plot_recomc(ftest, bnb_cosmics, bnb_cosmics->getBDTVariable("cosmic_track") , fcoscut,fbnbcut);
		test.plot_recomc(ftest, signal_cosmics, signal_cosmics->getBDTVariable("bnb_track") , fcoscut,fbnbcut);
		test.plot_recomc(ftest, signal_cosmics, signal_cosmics->getBDTVariable("cosmic_track") , fcoscut,fbnbcut);

		return 0;
		int h=0;
		for(auto &v:vars){
			test.plot_recomc(ftest, signal_cosmics, v, fcoscut, fbnbcut);
			test.plot_recomc(ftest, bnb_cosmics, v, fcoscut, fbnbcut);
			h++;
			if (h > 1) break;  
		}	
		//test.plot_recomc(ftest, bnb_cosmics, vars.at(1), fcoscut, fbnbcut);
		//test.plot_recomc(ftest, signal_cosmics, vars.at(1), fcoscut, fbnbcut);

		//		test.plot_recomc(ftest, signal_cosmics, signal_cosmics->getBDTVariable("cosmic_track"), usecut1, usecut2);
		//		test.plot_recomc(ftest, signal_cosmics, signal_cosmics->getBDTVariable("bnb_track"), usecut1, usecut2);

		//		for(int i=1; i<=4; i++){
		//			test.plot_recomc(ftest, bnb_cosmics, vars.at(i), usecut1, usecut2);
		//			test.plot_recomc(ftest, signal_cosmics, vars.at(i), usecut1, usecut2);
		//		}

	}
	else if(mode_option == "sig"){

		for(auto &method: TMVAmethods){
			for(int i=0; i< bdt_files.size(); i++){
				std::cout<<"Now adding TreeFriend: "<<cosmic_bdt_info.identifier<<"_app.root"<<" "<<bdt_files.at(i)->tag<<std::endl;
				bdt_files.at(i)->addFriend(bdt_files.at(i)->tag +"_"+cosmic_bdt_info.identifier,  cosmic_bdt_info.identifier+"_app"+".root");

				std::cout<<"Now adding TreeFriend: "<<bnb_bdt_info.identifier<<"_app.root"<<" "<<bdt_files.at(i)->tag<<std::endl;
				bdt_files.at(i)->addFriend(bdt_files.at(i)->tag +"_"+bnb_bdt_info.identifier,  bnb_bdt_info.identifier+"_app"+".root");
			}
		}


		TFile *fsig = new TFile(("significance_"+istrack+".root").c_str(),"recreate");
		std::vector<double> ans = scan_significance(fsig, {signal_cosmics} , {bnb_cosmics, intime}, cosmic_bdt_info, bnb_bdt_info);
		std::cout<<"Best Fit Significance: "<<ans.at(0)<<" "<<ans.at(1)<<" "<<ans.at(2)<<std::endl;
		fsig->Close();


	}else if(mode_option == "stack"){

		for(auto &method: TMVAmethods){
			for(int i=0; i< bdt_files.size(); i++){
				std::cout<<"Now adding TreeFriend: "<<cosmic_bdt_info.identifier<<"_app.root"<<" "<<bdt_files.at(i)->tag<<std::endl;
				bdt_files.at(i)->addFriend(bdt_files.at(i)->tag +"_"+cosmic_bdt_info.identifier,  cosmic_bdt_info.identifier+"_app"+".root");

				std::cout<<"Now adding TreeFriend: "<<bnb_bdt_info.identifier<<"_app.root"<<" "<<bdt_files.at(i)->tag<<std::endl;
				bdt_files.at(i)->addFriend(bdt_files.at(i)->tag +"_"+bnb_bdt_info.identifier,  bnb_bdt_info.identifier+"_app"+".root");
			}
		}

		bdt_stack obs("obs");
		obs.addToStack(signal_cosmics);
		obs.addToStack(bnb_cosmics);
		obs.addToStack(intime);

		int ip=0;
		for(auto &v:vars){
			ip++;
			obs.plotStacks(ftest,  v,fcoscut,fbnbcut);
			if (ip >1) return 0;
		}
		//obs.plotBDTStacks(ftest,bnb_bdt_info.identifier, fcoscut, fbnbcut);


	}else if(mode_option == "datamc"){

		for(auto &method: TMVAmethods){
			for(int i=0; i< bdt_files.size(); i++){
				std::cout<<"Now adding TreeFriend: "<<cosmic_bdt_info.identifier<<"_app.root"<<" "<<bdt_files.at(i)->tag<<std::endl;
				bdt_files.at(i)->addFriend(bdt_files.at(i)->tag +"_"+cosmic_bdt_info.identifier,  cosmic_bdt_info.identifier+"_app"+".root");

				std::cout<<"Now adding TreeFriend: "<<bnb_bdt_info.identifier<<"_app.root"<<" "<<bdt_files.at(i)->tag<<std::endl;
				bdt_files.at(i)->addFriend(bdt_files.at(i)->tag +"_"+bnb_bdt_info.identifier,  bnb_bdt_info.identifier+"_app"+".root");
			}
		}

		bdt_stack *obs = new bdt_stack("obs");
		obs->plot_pot = 5e19;
		obs->addToStack(signal_cosmics);
		obs->addToStack(bnb_cosmics);
		obs->addToStack(intime);

		bdt_stack *obs2 = new bdt_stack("obs2");
		obs2->plot_pot = 5e19;
		obs2->addToStack(intime);


		bdt_datamc datamc(data5e19, obs);	
		bdt_datamc datamc2(bnbext, obs2);	


		int ip=0;
		for(auto &v:vars){
			ip++;
			break;
			bdt_datamc tdatamc(data5e19, obs);	
			bdt_datamc tdatamc2(bnbext, obs2);	


			tdatamc.plotStacks(ftest,  v,fcoscut,fbnbcut);
			tdatamc2.plotStacks(ftest,  v,fcoscut,fbnbcut);

		}
	
		datamc.plotBDTStacks(ftest, bnb_bdt_info.identifier ,fcoscut,fbnbcut);
		datamc.plotBDTStacks(ftest, cosmic_bdt_info.identifier ,fcoscut,fbnbcut);
	
		datamc2.plotBDTStacks(ftest, bnb_bdt_info.identifier ,fcoscut,fbnbcut);
		datamc2.plotBDTStacks(ftest, cosmic_bdt_info.identifier ,fcoscut,fbnbcut);


	}else if(mode_option == "vars"){

		for(auto &method: TMVAmethods){
			for(int i=0; i< bdt_files.size(); i++){
				std::cout<<"Now adding TreeFriend: "<<cosmic_bdt_info.identifier<<"_app.root"<<" "<<bdt_files.at(i)->tag<<std::endl;
				bdt_files.at(i)->addFriend(bdt_files.at(i)->tag +"_"+cosmic_bdt_info.identifier,  cosmic_bdt_info.identifier+"_app"+".root");

				std::cout<<"Now adding TreeFriend: "<<bnb_bdt_info.identifier<<"_app.root"<<" "<<bdt_files.at(i)->tag<<std::endl;
				bdt_files.at(i)->addFriend(bdt_files.at(i)->tag +"_"+bnb_bdt_info.identifier,  bnb_bdt_info.identifier+"_app"+".root");
			}
		}


		for(auto &v:vars){
			TCanvas *c_var = new TCanvas(("cvar_"+v.name+"_bnb").c_str(), ("cvar_"+v.name+"_bnb").c_str(),2000,1600);
			c_var->Divide(2,2);


			for(int j=0; j<4;j++){	

				std::string cut_signal = signal_pure->getStageCuts(j,fcoscut,fbnbcut); 
				std::string cut_bnb = bnb_pure->getStageCuts(j,fcoscut,fbnbcut); 


				TH1* sig = signal_pure->getTH1(v,cut_signal.c_str(),v.safe_name+"_sig_bnb_var" ,1.0);
				TH1* bkg = bnb_pure->getTH1(v,cut_bnb.c_str(),v.safe_name+"_bkg_bnb_var" ,1.0);

				sig->Scale(1.0/sig->Integral());			
				bkg->Scale(1.0/bkg->Integral());			
				//		sig->SetLineColor(kRed-7);
				//		bkg->SetLineColor(kGreen-3);

				c_var->cd(j+1);			
				sig->Draw("hist");
				bkg->Draw("hist same");
				//sig->GetXaxis()->SetTitle(v.unit.c_str());
				sig->GetYaxis()->SetTitle("Verticies [Area Normalized]");
				sig->GetYaxis()->SetTitleOffset(1.5);

				TLegend *l = new TLegend(0.45,0.59,0.89,0.89);
				l->SetLineColor(kWhite);
				l->SetFillStyle(0);

				l->AddEntry(sig,"NC #Delta #gamma Signal","l");	
				l->AddEntry(bkg,"BNB Background","l");	
				l->Draw();

				double max_height = std::max( sig->GetMaximum(), bkg->GetMaximum());
				sig->SetMaximum(max_height*1.1);



			}
			c_var->Print(("var/bnb_"+v.safe_name+".pdf").c_str(),"pdf");
		}


		if(false){



			for(auto &v:vars){
				TCanvas *c_var = new TCanvas(("cvar_"+v.name+"_cosmo").c_str(), ("cvar_"+v.name+"_cosmo").c_str(),2000,1600);
				c_var->Divide(2,2);


				for(int j=0; j<4;j++){	

					std::string cut_signal = signal_pure->getStageCuts(j,fcoscut,fbnbcut); 
					std::string cut_intime = intime->getStageCuts(j,fcoscut,fbnbcut); 


					TH1* sig = signal_pure->getTH1(v,cut_signal.c_str(),v.safe_name+"_sig_cosmo_var" ,1.0);
					TH1* bkg = intime->getTH1(v,cut_intime.c_str(),v.safe_name+"_bkg_cosmo_var" ,1.0);

					sig->Scale(1.0/sig->Integral());			
					bkg->Scale(1.0/bkg->Integral());			
					sig->SetLineColor(kRed-7);
					bkg->SetLineColor(kGreen-3);

					c_var->cd(j+1);			
					sig->Draw("hist");
					bkg->Draw("hist same");
					//sig->GetXaxis()->SetTitle(v.unit.c_str());
					sig->GetYaxis()->SetTitle("Verticies [Area Normalized]");
					sig->GetYaxis()->SetTitleOffset(1.5);

					TLegend *l = new TLegend(0.45,0.59,0.89,0.89);
					l->SetLineColor(kWhite);
					l->SetFillStyle(0);

					l->AddEntry(sig,"NC #Delta #gamma Signal","l");	
					l->AddEntry(bkg,"Intime Cosmic Background","l");	
					l->Draw();

					double max_height = std::max( sig->GetMaximum(), bkg->GetMaximum());
					sig->SetMaximum(max_height*1.1);



				}
				c_var->Print(("var/cosmic_"+v.safe_name+".pdf").c_str(),"pdf");
			}


		}



	}else if(mode_option == "eff"){

		for(auto &method: TMVAmethods){
			for(int i=0; i< bdt_files.size(); i++){
				std::cout<<"Now adding TreeFriend: "<<cosmic_bdt_info.identifier<<"_app.root"<<" "<<bdt_files.at(i)->tag<<std::endl;
				bdt_files.at(i)->addFriend(bdt_files.at(i)->tag +"_"+cosmic_bdt_info.identifier,  cosmic_bdt_info.identifier+"_app"+".root");

				std::cout<<"Now adding TreeFriend: "<<bnb_bdt_info.identifier<<"_app.root"<<" "<<bdt_files.at(i)->tag<<std::endl;
				bdt_files.at(i)->addFriend(bdt_files.at(i)->tag +"_"+bnb_bdt_info.identifier,  bnb_bdt_info.identifier+"_app"+".root");
			}
		}


		double plot_pot = 6.6e20;

		std::cout<<"Starting efficiency study: coscut @ "<<fcoscut<<" bnbcut@: "<<fbnbcut<<std::endl;

		std::cout<<"########################################################################"<<std::endl;
		std::cout<<"###############################Efficiency###############################"<<std::endl;
		for(int i=0; i< bdt_files.size(); i++){
			std::cout<<bdt_files.at(i)->tag<<std::endl;
			std::vector<double> prev;
			double start;
			double sel;		

			double pot_scale = (plot_pot/bdt_files.at(i)->pot )*bdt_files.at(i)->scale_data;
			std::string gencut = "true_nu_vtx_fid_contained == 1";
			if(bdt_files.at(i)->tag == "IntimeCosmics" || bdt_files.at(i)->tag == "Data5e19" || bdt_files.at(i)->tag == "BNBext"){
				gencut = "1";
			}	

			double nevents = bdt_files.at(i)->tevent->GetEntries(gencut.c_str())*pot_scale;	
			double nv = bdt_files.at(i)->tvertex->GetEntries((gencut+"&&1").c_str())*pot_scale;	
			double ns = bdt_files.at(i)->tvertex->GetEntries((gencut+"&&  reco_asso_showers==1 && reco_asso_tracks "+num_track_cut).c_str())*pot_scale;	

			start = nevents;
			sel = ns;
			std::cout<<"Stage G\t\t"<<nevents<<"\t\t"<<nevents/start*100<<std::endl;
			std::cout<<"Stage V\t\t"<<nv<<"\t\t"<<nv/start*100<<std::endl;
			std::cout<<"Stage S\t\t"<<ns<<"\t\t"<<ns/start*100<<"\t\t"<<100<<std::endl;

			for(int j=0; j <5; j++){		
				std::string thiscut = bdt_files.at(i)->getStageCuts(j,fcoscut,fbnbcut); 
				double nvert =bdt_files.at(i)->tvertex->GetEntries(thiscut.c_str())*pot_scale;
				double eff =0;
				//	if(j==0){
				//		start = nvert;
				//		
				//	}

				eff = nvert/start*100;

				std::cout<<"Stage "<<std::to_string(j)<<"\t\t"<<nvert<<"\t\t"<<eff<<"\t\t"<<nvert/sel*100<<std::endl;	

			}
		}


	}
	else if(mode_option == "effdata"){
		std::vector<bdt_file*> data_files = {data5e19, bnbext};

		for(auto &method: TMVAmethods){
			for(int i=0; i< data_files.size(); i++){
				std::cout<<"Now adding TreeFriend: "<<cosmic_bdt_info.identifier<<"_app.root"<<" "<<data_files.at(i)->tag<<std::endl;
				data_files.at(i)->addFriend(data_files.at(i)->tag +"_"+cosmic_bdt_info.identifier,  cosmic_bdt_info.identifier+"_app"+".root");
				std::cout<<"Now adding TreeFriend: "<<bnb_bdt_info.identifier<<"_app.root"<<" "<<data_files.at(i)->tag<<std::endl;
				data_files.at(i)->addFriend(data_files.at(i)->tag +"_"+bnb_bdt_info.identifier,  bnb_bdt_info.identifier+"_app"+".root");
			}
		}



		std::cout<<"Starting efficiency study: coscut @ "<<fcoscut<<" bnbcut@: "<<fbnbcut<<std::endl;

		std::cout<<"########################################################################"<<std::endl;
		std::cout<<"###############################Efficiency###############################"<<std::endl;
		for(int i=0; i< data_files.size(); i++){
			std::cout<<data_files.at(i)->tag<<std::endl;
			std::vector<double> prev;
			double start;
			double sel;		

			double pot_scale = 1.0;// (plot_pot/data_files.at(i)->pot )*data_files.at(i)->scale_data;
			std::string gencut = "1";

			double nevents = data_files.at(i)->tevent->GetEntries(gencut.c_str())*pot_scale;	
			double nv = data_files.at(i)->tvertex->GetEntries((gencut+"&&1").c_str())*pot_scale;	
			double ns = data_files.at(i)->tvertex->GetEntries((gencut+"&&  reco_asso_showers==1 && reco_asso_tracks "+num_track_cut).c_str())*pot_scale;	

			start = nevents;
			sel = ns;
			std::cout<<"Stage G\t\t"<<nevents<<"\t\t"<<nevents/start*100<<std::endl;
			std::cout<<"Stage V\t\t"<<nv<<"\t\t"<<nv/start*100<<std::endl;
			std::cout<<"Stage S\t\t"<<ns<<"\t\t"<<ns/start*100<<"\t\t"<<100<<std::endl;

			for(int j=0; j <4; j++){		
				std::string thiscut = data_files.at(i)->getStageCuts(j,fcoscut,fbnbcut); 
				double nvert =data_files.at(i)->tvertex->GetEntries(thiscut.c_str())*pot_scale;
				double eff =0;
				eff = nvert/start*100;
				std::cout<<"Stage "<<std::to_string(j)<<"\t\t"<<nvert<<"\t\t"<<eff<<"\t\t"<<nvert/sel*100<<std::endl;	
				if(j==3){data_files.at(i)->tvertex->Scan( "run_number:subrun_number:event_number:reco_shower_dedx_plane2",thiscut.c_str());};
			}
		}


	} else if(mode_option == "precalc"){
		bdt_precalc pre(signal_pure);
		pre.addTrackInfo();

	}

	else {
		std::cout << "WARNING: " << mode_option << " is an invalid option\n";
	}

	ftest->Close();

	return 0;

	}
