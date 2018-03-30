/*#include "object_helper.h"
#include "get_pot_info.h"
#include "train.h"
#include "app.h"
#include "merge.h"
#include "significance.h"
#include "get_mva_response_hists.h"
#include "plot_mva_response_hists.h"
#include "gen_tlimits.h"
#include "plotstack.h"
*/
#include "load_mva_param.h"
#include "tinyxml.h"

#include <getopt.h>


#include "bdt_file.h"
#include "bdt_var.h"
#include "bdt_info.h"
#include "bdt_train.h"
#include "bdt_app.h"
#include "bdt_response.h"
#include "bdt_recomc.h"
#include "bdt_sig.h"
#include "bdt_spec.h"

#include "bdt_precalc.h"



int main (int argc, char *argv[]){

	// Just some simple argument things
	//===========================================================================================
	std::string dir = "/home/amogan/singlePhotonCode/hellstroms_hive/";

	//std::string dir = "/home/amogan/singlePhotonCode/hellstroms_hive/";
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


	//Define the precuts, depending on if you want to use track or notrack	
	std::string new_precuts;
    std::string num_track_cut;
	if(istrack == "track"){
	    num_track_cut = " == 1 ";
        new_precuts = "1";
        // Minimum shower energy cut in GeV
        //new_precuts = "reco_shower_helper_energy[second_most_energetic_shower_index] > 0.03 "; 

	}else if(istrack == "notrack"){
		//new_precuts = "reco_nu_vtx_dist_to_closest_tpc_wall > 10 && totalpe_ibg_sum > 50";
		new_precuts = "1";
		num_track_cut = "==0";
	}


	//Set up 2 bdt_info structs for passing information on what BDT we are running. 
	//MARK: Now with added binning here, so bdt_file->GetBDTvariable() is much simpler!
	bdt_info bnb_bdt_info("pi0bnb_"+istrack, "BNB focused BDT", "(65,0.35,0.55)");
	bdt_info cosmic_bdt_info("pi0cosmic_"+istrack, "Cosmic focused BDT", "(65,0.2,0.62)");
	

	// Our signal definition alongside any base cuts we want to make
	std::string base_cuts = "reco_asso_showers == 2 && reco_asso_tracks " + num_track_cut;
	std::string cosmic_base_cuts = "reco_asso_showers == 2 && reco_asso_tracks " + num_track_cut;
    // Signal: NC interaction, two photons from parent pi0, BNB interaction
	std::string signal_definition = "ccnc==1 &&true_shower_parent_pdg[second_most_energetic_shower_index]==111&& true_shower_parent_pdg[most_energetic_shower_index]==111&& true_shower_origin[most_energetic_shower_index]==1 && true_shower_origin[second_most_energetic_shower_index]==1";
	std::string background_definition = "!(" + signal_definition + ")";

	
	//This is a particular cut flow that a file will undergo. I.e base cuts, precuts, postcuts, and then the name of the Cosmic BDT and bnb bdt
	bdt_flow signal_flow(base_cuts +"&&"+signal_definition,	    new_precuts,     "1", 	    cosmic_bdt_info, bnb_bdt_info);
	bdt_flow cosmic_flow(cosmic_base_cuts, 			            new_precuts,     "1", 	    cosmic_bdt_info, bnb_bdt_info);
	bdt_flow bkg_flow(   base_cuts +"&&"+background_definition, new_precuts,     "1",		cosmic_bdt_info, bnb_bdt_info);
	bdt_flow data_flow(  base_cuts ,				            new_precuts,     "1",		cosmic_bdt_info, bnb_bdt_info);

	// BDT files, in the form (location, rootfilt, name, hisotgram_options, tfile_folder, tag, color, bdt_flow )		

	//bdt_file *s1 = new bdt_file(dir+"samples/vectored/","vertexed_ncdeltarad_mcc88_v3.0.root","TESTNCDeltaRad","hist","",kBlue-4, signal_flow);
	//bdt_file *s2 = new bdt_file(dir+"samples/vectored/","vertexed_bnbcosmic_mcc88_v2.0.root","TESTBNBCosmics","hist","",kBlue-4, signal_flow);
	//bdt_file *s3 = new bdt_file(dir+"samples/vectored/","vertexed_ncdeltaradcosmics_mcc88_v1.0.root","TESTNCDeltaRadCosmics","hist","",kBlue-4, signal_flow);

	// BDT files, in the form (location, rootfile, name, hisotgram_options, tfile_folder, tag, color, bdt_flow )		
	bdt_file *signal_pure    = new bdt_file(dir+"samples/mcc87", "vertexed_ncpi0cosmic_mcc88_v1.0.root", "NCpi0Pure",	 "hist", "",  kBlue-4,  signal_flow);
	bdt_file *signal_cosmics = new bdt_file(dir+"samples/mcc87", "vertexed_ncpi0cosmic_mcc88_v1.0.root", "NCpi0Cosmics", "hist", "",  kBlue-4,  signal_flow);
	bdt_file *bnb_pure    = new bdt_file(   dir+"samples/mcc87", "vertexed_bnbcosmic_mcc88_v2.0.root",	 "BNBPure",	     "hist", "",  kRed-6,   bkg_flow);
	bdt_file *bnb_cosmics = new bdt_file(   dir+"samples/mcc87", "vertexed_bnbcosmic_mcc88_v2.0.root",   "BNBCosmics",   "hist", "",  kRed-6,   bkg_flow);
	bdt_file *intime = new bdt_file(        dir+"samples/mcc87", "vertexed_intime_v2.0_mcc88.root" ,     "IntimeCosmics","hist", "",  kGreen-3, cosmic_flow);
    // Data files
	bdt_file *data5e19 = new bdt_file(dir+"samples/data",   "vertexed_data5e19_v10.root",      "Data5e19", "hist ep", "", kBlack, data_flow);
	bdt_file *bnbext = new bdt_file(  dir+"samples/bnbext", "vertexed_bnbext_mcc88_v8.0.root", "BNBext",   "hist ep", "", kBlack, data_flow);

	std::vector<bdt_file*> bdt_files = {signal_pure, signal_cosmics, bnb_pure, bnb_cosmics, intime};

    /*
	//Add Any other info in the form of FRIENDS! e.g track dEdx 
	signal_pure->addFriend("track_dEdx_tree",dir+"track_dEdx/trackdEdx_nsignal.root");			
	signal_cosmics->addFriend("track_dEdx_tree",dir+"track_dEdx/trackdEdx_nsignal_cosmics.root");			
	bnb_pure->addFriend("track_dEdx_tree",dir+"track_dEdx/trackdEdx_bnb.root");			
	bnb_cosmics->addFriend("track_dEdx_tree",dir+"track_dEdx/trackdEdx_bnb_cosmics.root");			
	intime->addFriend("track_dEdx_tree",dir+"track_dEdx/trackdEdx_intime.root");			
//	data5e19->addFriend("track_dEdx_tree",dir+"track_dEdx/trackdEdx_data5e19.root");			
//	bnbext->addFriend("track_dEdx_tree",dir+"track_dEdx/trackdEdx_bnbext.root");			
	//data5e19->addFriend("track_dEdx_tree",dir+"track_dEdx/trackdEdx_data5e19.root");			
	//bnbext->addFriend("track_dEdx_tree",dir+"track_dEdx/trackdEdx_bnbext.root");			
    */


	//Variables!
	std::string angle_track_shower1 ="(reco_track_dirx[longest_asso_track_index]*reco_shower_dirx[most_energetic_shower_index]+reco_track_diry[longest_asso_track_index]*reco_shower_diry[most_energetic_shower_index]+reco_track_dirz[longest_asso_track_index]*reco_shower_dirz[most_energetic_shower_index])";
	std::string angle_track_shower2 ="(reco_track_dirx[longest_asso_track_index]*reco_shower_dirx[second_most_energetic_shower_index]+reco_track_diry[longest_asso_track_index]*reco_shower_diry[second_most_energetic_shower_index]+reco_track_dirz[longest_asso_track_index]*reco_shower_dirz[second_most_energetic_shower_index])";
	//std::string angle_shower1_shower2 ="reco_shower_dirx[most_energetic_shower_index]*reco_shower_dirx[second_most_energetic_shower_index]+reco_shower_diry[most_energetic_shower_index]*reco_shower_diry[second_most_energetic_shower_index]+reco_shower_dirz[most_energetic_shower_index]*reco_shower_dirz[second_most_energetic_shower_index]";
	std::string angle_shower1_shower2 ="reco_shower_dirx[0]*reco_shower_dirx[1]+reco_shower_diry[0]*reco_shower_diry[1]+reco_shower_dirz[0]*reco_shower_dirz[1]";
    // Calculate invariant mass for two massless particles
    std::string E1 = "reco_shower_helper_energy[0]"; 
    std::string E2 = "reco_shower_helper_energy[1]"; 
    std::string invMass = "sqrt(2.0*"+E1+"*"+E2+"*(1.0-"+angle_shower1_shower2+"))";
    
    std::vector<bdt_variable> vars;

    vars.push_back(bdt_variable(invMass, "(20, 0., 0.5)", "Shower Invariant Mass [GeV/c]", false, "d"));
	vars.push_back(bdt_variable("reco_shower_dedx_plane2[most_energetic_shower_index]","(48,0,15)", "Shower 1 dE/dx Collection Plane [MeV/cm]",false,"d"));
	vars.push_back(bdt_variable("reco_shower_dedx_plane2[second_most_energetic_shower_index]","(48,0,15)", "Shower 2 dE/dx Collection Plane [MeV/cm]",false,"d"));
	vars.push_back(bdt_variable("summed_associated_helper_shower_energy","(25,0,0.5)","Summed Shower Energy [GeV]", false,"d"));
	vars.push_back(bdt_variable("reco_shower_length[most_energetic_shower_index]","(25,0,125)","Most Energetic Shower Length [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_shower_length[second_most_energetic_shower_index]","(25,0,125)","Least Energetic Shower Length [cm]",false,"d"));
    vars.push_back(bdt_variable(angle_shower1_shower2, "(50, -1, 1)", "Opening Angle between Showers", false, "d"));

	vars.push_back(bdt_variable("totalpe_ibg_sum","(25,0,2000)","Total in Beam-Gate PE",false,"d"));
	vars.push_back(bdt_variable("closest_asso_shower_dist_to_flashzcenter","(25,0,1000)","Distance from Shower to Flashcenter [cm]",false,"d"));

	vars.push_back(bdt_variable("reco_nu_vtx_dist_to_closest_tpc_wall","(25,0,125)","Reconstructed Vertex to TPC Wall Distance [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_shower_bp_dist_to_tpc[most_energetic_shower_index]","(25,0,1000)","Back Projected Distance from Shower 1 to TPC wall [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_shower_bp_dist_to_tpc[second_most_energetic_shower_index]","(25,0,1000)","Back Projected Distance from Shower 2 to TPC wall [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_nuvertx","(25,0,300)"," Reconstructed Vertex x-location [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_nuverty","(25,-300,+300)","Reconstructed Vertex y-location [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_nuvertz","(25,0,1000)","Reconstructed Vertex z-location [cm]",false,"d"));

	vars.push_back(bdt_variable("cos(atan2(reco_shower_diry[most_energetic_shower_index], reco_shower_dirz[most_energetic_shower_index]))","(50,-1,1)","Reconstructed Shower 1 |Cosine Theta|", true,"d"));
	vars.push_back(bdt_variable("cos(atan2(reco_shower_diry[most_energetic_shower_index],reco_shower_dirx[most_energetic_shower_index]))","(50,-1,1)","Reconstructed Shower 1 |Cosine Phi|", true,"d"));
	vars.push_back(bdt_variable("cos(atan2(reco_shower_diry[second_most_energetic_shower_index],reco_shower_dirz[second_most_energetic_shower_index]))","(50,-1,1)","Reconstructed Shower 2 |Cosine Theta|", true,"d"));
	vars.push_back(bdt_variable("cos(atan2(reco_shower_diry[second_most_energetic_shower_index],reco_shower_dirx[second_most_energetic_shower_index]))","(50,-1,1)","Reconstructed Shower 2 |Cosine Phi|", true,"d"));
    
	if(istrack=="track"){ 
		vars.push_back(bdt_variable("reco_track_displacement[longest_asso_track_index]","(25,0,500)","Reconstructed Track Length [cm]", true,"d"));
		vars.push_back(bdt_variable("shortest_asso_shower_to_vert_dist","(50,0,20)","Closest Photon Conversion Length from Reconstructed Vertex [cm]" ,false,"d"));
		vars.push_back(bdt_variable("atan2(reco_track_diry[longest_asso_track_index],reco_track_dirz[longest_asso_track_index])","(50,0,3.14)","Reconstructed Track Theta", true,"d"));
		vars.push_back(bdt_variable("atan2(reco_track_diry[longest_asso_track_index],reco_track_dirx[longest_asso_track_index])","(50,0,3.14)","Reconstructed Track Phi", true,"d"));
 
		vars.push_back(bdt_variable(angle_track_shower1,	"(50,-1,1)","|Cosine Track-Shower Angle (Most Energetic)| ",true,"d"));
		vars.push_back(bdt_variable(angle_track_shower2,	"(50,-1,1)","|Cosine Track-Shower Angle (Second Most Energetic)| ",true,"d"));
		//vars.push_back(bdt_variable("reco_asso_tracks","(5,0,4)","Number of Reconstructed Tracks",false,"i")); 
	}

	std::cout<<"--------------------------------------------------------------------------"<<std::endl;
	std::cout<<"--------------------------------------------------------------------------"<<std::endl;
	for(auto &f: bdt_files){
        std::cout << f->tvertex << std::endl;
        std::cout << f->pot << std::endl;
		std::cout<<"Loading "<<f->tag<<"\t with "<<f->numberofevents<<"\t events and "<<f->tvertex->GetEntries()<<"\t verticies."<<std::endl;
		std::cout<<"POT of file loaded is: "<<f->pot<<"\t\t which is pot/event: "<<f->numberofevents/f->pot<<std::endl;
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
		fcoscut = 0.48;
		fbnbcut = 0.46;
	}else if(istrack == "notrack"){
		fcoscut = 0.3;
		fbnbcut = 0.3;
	}

	//===========================================================================================
	//===========================================================================================
	//		Main flow of the program , using OPTIONS
	//===========================================================================================
	//===========================================================================================

	if(mode_option == "train") {
		std::cout<<"**********************Starting COSMIC BDT Training*************************"<<std::endl;
		//bdt_train(cosmic_bdt_info, signal_pure, bnb_pure, vars, TMVAmethods);
		bdt_train(cosmic_bdt_info, signal_pure, intime, vars, TMVAmethods);
		std::cout<<"**********************Starting BNB BDT Training*************************"<<std::endl;
		bdt_train(bnb_bdt_info, signal_pure, bnb_pure, vars, TMVAmethods);


	}else if(mode_option == "app"){
		//Apply! This will update cosmic_bdt_info, signal file and bkg file. As in update them PROPERLY!	
		//std::vector<bdt_file*> app_files = {data5e19,bnbext,signal_pure, bnb_pure, intime, signal_cosmics, bnb_cosmics}; 
		//std::vector<bdt_file*> app_files = {signal_pure, bnb_pure, intime, signal_cosmics, bnb_cosmics}; 
        std::cout << "Mark test " << signal_pure->tvertex->GetEntries((base_cuts+"&&"+signal_definition).c_str()) << std::endl;
		std::vector<bdt_file*> app_files = {signal_pure};
		bdt_app(bnb_bdt_info, app_files, vars, TMVAmethods);
		bdt_app(cosmic_bdt_info, app_files, vars, TMVAmethods);
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
	//	bdt_response cosmic_response(cosmic_bdt_info, signal_pure, intime);
	//	cosmic_response.plot_bdt_response(ftest);
        
		bdt_response bnb_response(bnb_bdt_info, signal_pure, bnb_pure);
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

	//First off, what MC catagories do you want to stack?
    	std::vector<std::string>recomc_names = {"BNB NC #pi^{0}", "NC BNB Background", "CC BNB Background", "Cosmic Background"};
	//How are they defined, cutwise?
	std::vector<std::string> recomc_cuts = {
        // NC pi0 signal: two photon showers whose true parents are pi0's, all resulting from NC BNB interaction
		"true_shower_pdg[most_energetic_shower_index] == 22 && true_shower_pdg[second_most_energetic_shower_index] == 22 && true_shower_parent_pdg[most_energetic_shower_index] == 111 && true_shower_parent_pdg[second_most_energetic_shower_index] == 111 && ccnc == 1 && true_shower_origin[most_energetic_shower_index]==1 && true_shower_origin[second_most_energetic_shower_index]==1",
		"(true_shower_pdg[most_energetic_shower_index] != 22 || true_shower_pdg[second_most_energetic_shower_index] != 22 || true_shower_parent_pdg[most_energetic_shower_index] != 111 || true_shower_parent_pdg[second_most_energetic_shower_index] != 111) && ccnc == 1 && true_shower_origin[most_energetic_shower_index]==1 && true_shower_origin[second_most_energetic_shower_index]==1",
		"(true_shower_pdg[most_energetic_shower_index] != 22 || true_shower_pdg[second_most_energetic_shower_index] != 22 || true_shower_parent_pdg[most_energetic_shower_index] != 111 || true_shower_parent_pdg[second_most_energetic_shower_index] != 111) && ccnc == 0 && true_shower_origin[most_energetic_shower_index]==1 && true_shower_origin[second_most_energetic_shower_index]==1",
		"true_shower_origin[most_energetic_shower_index]==2 && true_shower_origin[second_most_energetic_shower_index]==2",
        /* 
        // CC pi0's. This should be a large background. Same as signal, but with ccnc==0 (CC interaction)     
		"true_shower_pdg[most_energetic_shower_index] == 22 && true_shower_pdg[second_most_energetic_shower_index] == 22 && true_shower_parent_pdg[most_energetic_shower_index] == 111 && true_shower_parent_pdg[second_most_energetic_shower_index] == 111 && ccnc == 0",
        
        // Delta radiative is now considered a (very small) background
	    "true_shower_pdg[most_energetic_shower_index] == 22 && true_shower_parent_pdg[most_energetic_shower_index] != 111 && is_delta_rad == 1 && true_shower_origin==1",
        // Check if either shower came from electron
		"true_shower_origin[most_energetic_shower_index] ==1 && true_shower_origin[second_most_energetic_shower_index]==1 && (abs(true_shower_pdg[most_energetic_shower_index]) ==11 || abs(true_shower_pdg[second_most_energetic_shower_index])==11)",
        // Check for cases where either associated photon shower didn't come from pi0
		"(true_shower_pdg[most_energetic_shower_index] != 22 || true_shower_pdg[second_most_energetic_shower_index] != 22 || true_shower_parent_pdg[most_energetic_shower_index] != 111 || true_shower_parent_pdg[second_most_energetic_shower_index] != 111) && true_shower_origin[most_energetic_shower_index]==1 && true_shower_origin[second_most_energetic_shower_index]==1 && abs(true_shower_pdg[most_energetic_shower_index])!=11 && abs(true_shower_pdg[second_most_energetic_shower_index])!=11",
        
        // Check if either shower is cosmic in origin
		"(true_shower_origin[most_energetic_shower_index] == 2 || true_shower_origin[second_most_energetic_shower_index] == 2) && (true_shower_pdg[most_energetic_shower_index] != 22 || true_shower_pdg[second_most_energetic_shower_index] != 22) && (true_shower_parent_pdg[most_energetic_shower_index] != 111 || true_shower_parent_pdg[second_most_energetic_shower_index] != 111)",
        // Other
		"!(true_shower_pdg[most_energetic_shower_index] == 22 && true_shower_pdg[second_most_energetic_shower_index] == 22 && true_shower_parent_pdg[most_energetic_shower_index] == 111 && true_shower_parent_pdg[second_most_energetic_shower_index] == 111) && !(true_shower_origin[most_energetic_shower_index] == 2 || true_shower_origin[second_most_energetic_shower_index] == 2) && !(true_shower_pdg[most_energetic_shower_index] != 22 || true_shower_pdg[second_most_energetic_shower_index] != 22) && !(true_shower_parent_pdg[most_energetic_shower_index] != 111 || true_shower_parent_pdg[second_most_energetic_shower_index] != 111)",
        */
	};

	//and what colors
	std::vector<int> recomc_cols = {kRed-7, kRed+1, kGreen+1, kBlue+3};






        std::cout << "Done adding TreeFiends" << std::endl;
		bdt_recomc test(recomc_names, recomc_cuts, recomc_cols);
		//plot_recomc(TFile *fout, bdt_file* file, bdt_variable var, double cut_cosmic_val, double cut_bnb_val){



	//test.plot_recomc(ftest, bnb_cosmics, bnb_cosmics->getBDTVariable(bnb_bdt_info) , fcoscut,fbnbcut);
	//test.plot_recomc(ftest, signal_cosmics, signal_cosmics->getBDTVariable(cosmic_bdt_info) , fcoscut,fbnbcut);

		int h=0;
		for(auto &v:vars){
            test.plot_recomc(ftest, signal_cosmics, v, fcoscut, fbnbcut);
            test.plot_recomc(ftest, bnb_cosmics, v, fcoscut, fbnbcut);
            h++;
            if (h > 1) break;  
		}	
	//test.plot_recomc(ftest, bnb_cosmics, vars.at(1), fcoscut, fbnbcut);
					//test.plot_recomc(ftest, signal_cosmics, vars.at(1), fcoscut, fbnbcut);

		//		test.plot_recomc(ftest, signal_cosmics, signal_cosmics->getBDTVariable(cosmic_bdt_info), usecut1, usecut2);
		//		test.plot_recomc(ftest, signal_cosmics, signal_cosmics->getBDTVariable(bnb_bdt_info), usecut1, usecut2);

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

		for(auto &v:vars){
            std::cout << "In obs.plotStacks loop" << std::endl;
			obs.plotStacks(ftest,  v,fcoscut,fbnbcut);
		}
        std::cout << "Finished obs.plotStacks" << std::endl;
		obs.plotBDTStacks(ftest,bnb_bdt_info, fcoscut, fbnbcut);
        std::cout << "Done" << std::endl;

	}
	else if(mode_option == "vars"){
		
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
                std::cout<<v.name<<" "<<j<<std::endl;
				std::string cut_signal = signal_pure->getStageCuts(j,fcoscut,fbnbcut); 
				std::string cut_bnb = bnb_pure->getStageCuts(j,fcoscut,fbnbcut); 


				TH1* sig = signal_pure->getTH1(v,cut_signal.c_str(),v.safe_name+"_sig_bnb_var" ,1.0);
				TH1* bkg = bnb_pure->getTH1(v,cut_bnb.c_str(),v.safe_name+"_bkg_bnb_var" ,1.0);
	
				sig->Scale(1.0/sig->Integral());			
				bkg->Scale(1.0/bkg->Integral());			
				sig->SetLineColor(kBlue);
				bkg->SetLineColor(kRed);
	
				c_var->cd(j+1);			
				sig->Draw("hist");
				bkg->Draw("hist same");
				//sig->GetXaxis()->SetTitle(v.unit.c_str());
				sig->GetYaxis()->SetTitle("Verticies [Area Normalized]");
				sig->GetYaxis()->SetTitleOffset(1.5);

				TLegend *l = new TLegend(0.45,0.59,0.89,0.89);
				l->SetLineColor(kWhite);
				l->SetFillStyle(0);
		
				l->AddEntry(sig,"NC #pi^{0} Signal","l");	
				l->AddEntry(bkg,"BNB Background","l");	
				l->Draw();
	
				double max_height = std::max( sig->GetMaximum(), bkg->GetMaximum());
				sig->SetMaximum(max_height*1.1);
			}

			c_var->Print(("var/bnb_"+v.safe_unit+".png").c_str(),"png");
              
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
		
				l->AddEntry(sig,"NC #pi^0 Signal","l");	
				l->AddEntry(bkg,"Cosmic Background","l");	
				l->Draw("same");
	
				double max_height = std::max( sig->GetMaximum(), bkg->GetMaximum());
				sig->SetMaximum(max_height*1.1);



			}
			c_var->Print(("var/cosmic_"+v.safe_name+".png").c_str(),"png");
		}


		}



	}/*else if(mode_option == "eff"){

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
			if(bdt_files.at(i)->tag == "IntimeCosmics"){
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

			for(int j=0; j <4; j++){		
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


	}
    */
    /*
	 else if(mode_option == "precalc"){
		//bdt_precalc pre1(s1);pre1.genTrackInfo();
	bdt_precalc pre2(s2);pre2.genTrackInfo();
	//	bdt_precalc pre3(s3);pre3.genTrackInfo();

//		addPreFriends(bnb_pure,"track");

    }
    */
	
	  
    else {
		std::cout << "WARNING: " << mode_option << " is an invalid option\n";
	}

	ftest->Close();

	return 0;

	}
