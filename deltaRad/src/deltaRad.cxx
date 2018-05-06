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

	std::string dir2 = "/uboone/app/users/markrl/single_photon/hellstroms_hive/hellstroms_hive/";
	std::string dir = "/uboone/data/users/markross/vertexed_v2";//"/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_v2/";
	//std::string dir2 = "/home/mark/work/uBooNE/photon/tmva/";
	//std::string dir = "/home/mark/work/uBooNE/photon/tmva/samples/fresh_NCDR_bf/";

	std::string mode_option = "fake"; 
	std::string xml = "default.xml";
	std::string istrack ="track";

	bool run_cosmic = true;
	bool run_bnb = true;
	int number = 0;

	const struct option longopts[] = 
	{
		{"dir", 		required_argument, 	0, 'd'},
		{"option",		required_argument,	0, 'o'},
		{"xml"	,		required_argument,	0, 'x'},
		{"track",		required_argument,	0, 't'},
		{"help",		required_argument,	0, 'h'},
		{"number",		required_argument,	0, 'n'},
		{"cosmic",		no_argument,		0, 'c'},
		{"bnb",			no_argument,		0, 'b'},
		{0,			no_argument, 		0,  0},
	};

	//some optioni/argument stuff
	int iarg = 0; opterr=1; int index;
	while(iarg != -1)
	{
		iarg = getopt_long(argc,argv, "cbx:o:d:t:n:h?", longopts, &index);

		switch(iarg)
		{
			case 'n':
				number = strtof(optarg,NULL);
				run_bnb = false;
				break;

			case 'c':
				run_cosmic = true;
				run_bnb = false;
				break;
			case 'b':
				run_bnb = true;
				run_cosmic = false;	
				break;
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

				std::cout<<"\t-c\t--cosmic\t\t Run only cosmic training/app"<<std::endl;
				std::cout<<"\t-b\t--bnb\t\t Run only BNB training/app"<<std::endl;
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
	// X 0 to 256 Y -117 to 117 Z 0 to 1036
	// 10 cm from all sides, and 20cm from the top.
	std::string fiducial_vertex = "reco_nuvertx > 10 && reco_nuvertx < 246 && reco_nuverty > -107 && reco_nuverty < 97 && reco_nuvertz > 10 && reco_nuvertz < 1026 ";
	std::string fiducial_shower_end = "reco_shower_endx > 10 && reco_shower_endx < 246 && reco_shower_endy > -107 && reco_shower_endy < 97 && reco_shower_endz > 10 && reco_shower_endz < 1026 ";
	std::string fiducial_track_end = "reco_track_endx > 10 && reco_track_endx < 246 && reco_track_endy > -107 && reco_track_endy < 97 && reco_track_endz > 10 && reco_track_endz < 1026 ";

	std::string fiducial_cut = fiducial_vertex;//"&&"+fiducial_shower_end;

	std::string new_precuts;
	std::string num_track_cut = "==1";
	if(istrack == "track"){
		fiducial_cut =  fiducial_cut;// +"&&"+ fiducial_track_end;
		new_precuts = "shortest_asso_shower_to_vert_dist > 1 && reco_track_displacement[0] < 100 &&  reco_shower_helper_energy[0] > 0.03 && track_info.reco_track_good_calo[0]>0 && track_info.reco_track_mean_dEdx[0] > 1 && track_info.reco_track_start_mean_dEdx[0]/track_info.reco_track_end_mean_dEdx[0] >= 0.75 && "+ fiducial_cut;
		num_track_cut = "== 1";

	}else if(istrack == "notrack"){
		new_precuts = "reco_shower_helper_energy[0] > 0.03  && "+ fiducial_cut;
		num_track_cut = "== 0";
	}


	std::string postcuts = "1";//

	//Set up some info about the BDTs to pass along
	bdt_info bnb_bdt_info("bnb_"+istrack, "BNB focused BDT","(50,0.34,0.57)");
	bdt_info cosmic_bdt_info("cosmic_"+istrack, "Cosmic focused BDT","(50,0.3,0.69)");
	//bdt_info ncpi0_bdt_info("ncpi0_"+istrack, "NCPi0 focused BDT","(50,0.2,0.50)");

	// apply with PE
	//std::string base_cuts = "totalpe_ibg_sum > 20 && reco_asso_showers == 1 && reco_asso_tracks "+num_track_cut;
	// and train without PE
	std::string base_cuts = "reco_asso_showers == 1 && reco_asso_tracks "+num_track_cut;
	std::string signal_definition = "is_delta_rad == 1";
	std::string background_definition = "is_delta_rad == 0";

	//Train on "good" signals, defined as ones matched to the ncdelta and have little "clutter" around.	
	std::string true_signal = "shower_matched_to_ncdeltarad_photon[0]==1";

	std::string true_bkg    = "true_shower_origin[0]==1";
	if(istrack == "track"){
		true_signal = true_signal+ "&& track_matched_to_ncdeltarad_proton[0]==1";
		true_bkg = true_bkg +"&& true_track_origin[0]==1";
	}

	if(mode_option == "datamc" || mode_option == "eff"){
		new_precuts = new_precuts + "&& totalpe_ibg_sum > 20";
	}

	// takes 5 arguments ( 
	//bdt_flow(std::string intop, std::string indef, std::string inpre, std::string inpost, bdt_info incos, bdt_info inbnb) 


	bdt_flow signal_pure_flow(base_cuts, signal_definition +"&&"+ true_signal, new_precuts,	postcuts,cosmic_bdt_info,	bnb_bdt_info);
	bdt_flow signal_flow(base_cuts, signal_definition , new_precuts,	postcuts,cosmic_bdt_info,	bnb_bdt_info);
	bdt_flow cosmic_flow(base_cuts,"1", new_precuts , postcuts,	cosmic_bdt_info,bnb_bdt_info);
	bdt_flow bkg_flow(base_cuts,background_definition, new_precuts,postcuts,	cosmic_bdt_info,	bnb_bdt_info);
	bdt_flow bkg_pure_flow(base_cuts,background_definition+"&&"+ true_bkg ,new_precuts,postcuts,	cosmic_bdt_info,	bnb_bdt_info);

	bdt_flow data_flow(base_cuts,"1",	new_precuts, postcuts,	cosmic_bdt_info, 	bnb_bdt_info);
	//bdt_flow ncpi0_flow(base_cuts + " &&" + true_bkg , new_precuts,postcuts, cosmic_bdt_info, ncpi0_bdt_info); 


	// BDt files , bdt_file::bdt_file(std::string indir,std::string inname, std::string intag, std::string inops, std::string inrootdir, int incol, bdt_flow inflow) :
	bdt_file *signal_pure    = new bdt_file(dir, "vertexed_ncdeltaradcosmics_mcc8.9_fresh_v3.0.root",	"NCDeltaRad",	   "hist","",  kRed-7, signal_pure_flow);
	bdt_file *signal_cosmics = new bdt_file(dir, "vertexed_ncdeltaradcosmics_mcc8.9_fresh_v3.0.root", "NCDeltaRadCosmics", "hist","",  kRed-7, signal_flow);

	bdt_file *bnb_pure    = new bdt_file(dir, "vertexed_bnbcosmics_mcc8.9_fresh_v3.0.root", "BNBPure",	  "hist","",  kBlue-4, bkg_pure_flow);
	bdt_file *bnb_cosmics = new bdt_file(dir, "vertexed_bnbcosmics_mcc8.9_fresh_v3.0.root", "BNBCosmics", "hist","",  kBlue-4, bkg_flow);

	bdt_file *intime = new bdt_file(dir, "vertexed_intime_mcc8.9_fresh_v2.0.root" ,"IntimeCosmics","hist","", kGreen-3, cosmic_flow);
	//bdt_file *ncpi0 = new bdt_file(dir+"samples/vectored/", "vertexed_ncpi0cosmic_mcc88_v1.0.root" ,"NCpi0","hist","", kGreen-3, ncpi0_flow);

	//Data files
	//bdt_file *overlay = new bdt_file(dir2+"samples/vectored/", "vertexed_overlay_mcc88_v1.0.root",	"BNBOverlay",	   "hist","",  kMagenta-3, bkg_flow);
	bdt_file *data5e19    = new bdt_file(dir, "vertexed_data5e19_mcc8.9_fresh_v3.0.root",	"Data5e19",	   "E1p","",  kBlack, data_flow);
	bdt_file *bnbext    = new bdt_file(dir, "vertexed_bnbext_mcc8.9_fresh_v2.0.root",	"BNBext",	"E1p","",  kBlack, data_flow);

	//std::vector<bdt_file*> bdt_files = {signal_pure, signal_cosmics, bnb_pure, bnb_cosmics, intime, data5e19,bnbext};
	std::vector<bdt_file*> bdt_files = {signal_cosmics, signal_pure, bnb_pure, bnb_cosmics, intime, data5e19, bnbext};

	signal_pure->scale_data = 3.1;
	signal_cosmics->scale_data = 3.1;

	std::cout<<"--------------------------------------------------------------------------"<<std::endl;
	std::cout<<"--------------------------------------------------------------------------"<<std::endl;
	for(auto &f: bdt_files){
		std::cout<<"Loading "<<f->tag<<"\t with "<<f->tvertex->GetEntries()<<"\t verticies."<<std::endl;
		std::cout<<"POT of file loaded is: "<<f->pot<<"\t\t "<<std::endl;
		std::cout<<"Scale factor is then: "<<f->scale_data<<std::endl;
	}
	std::cout<<"--------------------------------------------------------------------------"<<std::endl;
	std::cout<<"--------------------------------------------------------------------------"<<std::endl;




	//you get access to these with track_info.XXX
	std::cout<<"--------------------------------------------------------------------------"<<std::endl;
	std::cout<<"Going to add any precomputed tree friends, or any trained BDT responses   "<<std::endl;
	std::cout<<" If you see warnings, but havenet yet ran app stage, thats ok!            "<<std::endl;
	std::cout<<"--------------------------------------------------------------------------"<<std::endl;
	if(mode_option != "precalc"){
		for(auto &f: bdt_files){
			addPreFriends(f,"track");
			addPreFriends(f,"pi0");
			f->addBDTResponses(cosmic_bdt_info, bnb_bdt_info, TMVAmethods);
		}
	}
	std::cout<<"--------------------------------------------------------------------------"<<std::endl;
	std::cout<<"--------------------------------------------------------------------------"<<std::endl;


	//Adding plot names
	signal_pure->addPlotName("NC Delta Radiative");
	signal_cosmics->addPlotName("NC Delta Rad w/ Corsika");
	bnb_pure->addPlotName("BNB Backgrounds");
	bnb_cosmics->addPlotName("BNB w/ Corsika");
	intime->addPlotName("Intime Corsika cosmics");


	std::cout<<"--------------------------------------------------------------------------"<<std::endl;
	std::cout<<"--------------------------------------------------------------------------"<<std::endl;
	//Variables!
	std::string angle_track_shower ="(reco_track_dirx[0]*reco_shower_dirx[0]+reco_track_diry[0]*reco_shower_diry[0]+reco_track_dirz[0]*reco_shower_dirz[0])";
	std::string shower_mom = "reco_shower_helper_energy[0]"; 
	std::string reco_track_energy = "reco_track_energy_new_legacy[0]+0.938272";
	std::string track_mom  = "sqrt("+reco_track_energy+"*"+reco_track_energy+"-0.938272)";

	std::string invariant_mass = "0.938272*0.938272+2.0*("+reco_track_energy+"*reco_shower_helper_energy[0]-"+track_mom+"*"+shower_mom+"*"+angle_track_shower+")";

	std::vector<bdt_variable> vars;

	vars.push_back(bdt_variable("reco_shower_dedx_plane2[0]","(50,0,6)", "Shower dE/dx Collection Plane [MeV/cm]",false,"d"));
	vars.push_back(bdt_variable("reco_shower_helper_energy[0]","(52,0,0.6)","Reconstructed Shower Energy [GeV]", false,"d"));
	vars.push_back(bdt_variable("reco_shower_length[0]","(50,0,100)","Shower Length [cm]",false,"d"));

	vars.push_back(bdt_variable("reco_shower_opening_angle[0]","(50,0,0.8)","Shower Opening Angle",false,"d"));
	vars.push_back(bdt_variable("reco_shower_dist_to_closest_flashzcenter[0]","(50,0,400)","Distance from Shower to Flashcenter [cm]",false,"d"));

	//THIS IS NEEDED TO BE EXACTLY HERE FOR NOTRACK
	//vars.push_back(bdt_variable("reco_shower_opening_angle[0]*reco_shower_length[0]","(50,0,15)","Shower Area",false,"d"));
	vars.push_back(bdt_variable("reco_nu_vtx_dist_to_closest_tpc_wall","(50,0,120)","Reconstructed Vertex to TPC Wall Distance [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_shower_bp_dist_to_tpc[0]","(50,0,550)","Back Projected Distance from Shower to TPC wall [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_nuvertx","(50,0,250)"," Reconstructed Vertex X pos [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_nuverty","(50,-110,+110)","Reconstructed Vertex Y pos [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_nuvertz","(50,0,1050)","Reconstructed Vertex Z pos [cm]",false,"d"));

	vars.push_back(bdt_variable("cos(atan2(reco_shower_diry[0],reco_shower_dirz[0]))","(50,-1,1)","Reconstructed Shower - Cosine Theta", true,"d"));
	vars.push_back(bdt_variable("cos(atan2(reco_shower_diry[0], reco_shower_dirx[0]))","(50,-1,1)","Reconstructed Shower - Cosine Phi", true,"d"));

	vars.push_back(bdt_variable("reco_shower_endx[0]","(50,0,250)"," Reconstructed Shower End X pos [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_shower_endy[0]","(50,-110,+110)","Reconstructed Shower End Y pos [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_shower_endz[0]","(50,0,1050)","Reconstructed Shower End Z pos [cm]",false,"d"));

	//New Variables for Showers!
	//vars.push_back(bdt_variable("reco_shower_helpernew_energy[0]","(50,0,0.4)","Reconstructed Shower Energy (NEW) [GeV]", false,"d"));
	//vars.push_back(bdt_variable("reco_shower_dedxnew_plane2[0]","(50,0,6)", "Shower dE/dx Collection Plane (NEW) [MeV/cm]",false,"d"));


	vars.push_back(bdt_variable("pi0_info.num_reco_showers_within_10cm_vertex","(10,0,10)","Num Showers within 10cm",false,"i"));

	if(istrack=="track"){

		vars.push_back(bdt_variable("reco_track_displacement[0]","(50,0,150)","Reconstructed Track Displacement [cm]", true,"d"));

		vars.push_back(bdt_variable("track_info.reco_track_mean_dEdx[0]", "(50,0,12)","Mean Track dE/dx", true,"d"));
		//vars.push_back(bdt_variable("track_info.reco_track_end_mean_dEdx[0]", "(50,0,12)","Mean End Track dE/dx", true,"d"));
		//vars.push_back(bdt_variable("track_info.reco_track_start_mean_dEdx[0]", "(50,0,12)","Mean Start Track dE/dx", true,"d"));
		vars.push_back(bdt_variable("track_info.reco_track_start_mean_dEdx[0]/track_info.reco_track_end_mean_dEdx[0]", "(50,0,3.5)","Ratio of Mean Start/End Track dE/dx", true,"d"));
		vars.push_back(bdt_variable("track_info.reco_track_PIDA[0]","(50,0,20)","Track PIDA",true,"d"));
		vars.push_back(bdt_variable("track_info.reco_track_braggA[0]","(50,-1,32)","Track Bragg A",true,"d"));

		vars.push_back(bdt_variable("shortest_asso_shower_to_vert_dist","(50,0,18)","Photon Conversion Length from Reconstructed Vertex [cm]" ,false,"d"));

		vars.push_back(bdt_variable(invariant_mass,"(50,1,2.5)","Invariant Mass",true,"d"));	

		vars.push_back(bdt_variable("cos(atan2(reco_track_diry[0],reco_track_dirz[0]))","(50,-1,1)","Reconstructed Track - Cosine Theta", true,"d"));
		vars.push_back(bdt_variable("cos(atan2(reco_track_diry[0],reco_track_dirx[0]))","(50,-1,1)","Reconstructed Track - Cosine Phi", true,"d"));

		vars.push_back(bdt_variable(angle_track_shower,	"(50,-1,1)","Cosine Track-Shower Angle ",true,"d"));

		vars.push_back(bdt_variable("reco_track_endx[0]","(50,0,250)"," Reconstructed Track End X pos [cm]",false,"d"));
		vars.push_back(bdt_variable("reco_track_endy[0]","(50,-110,+110)","Reconstructed Track End Y pos [cm]",false,"d"));
		vars.push_back(bdt_variable("reco_track_endz[0]","(50,0,1050)","Reconstructed Track End Z pos [cm]",false,"d"));

		//NEW variables for tracks
		vars.push_back(bdt_variable("reco_track_energy_new_legacy[0]", "(50,0,2)","Track Energy (New -Best Plane)",true, "d"));
	}

	if(mode_option != "train" && mode_option != "app"){
		//We don't want to train using these variables, but we would like to plot them just to see how things are

		if(false && mode_option != "train"){
			//We don't want to train using these variables, but we would like to plot them just to see how things are
			//Hmm. This is needed in track..	
			vars.push_back(bdt_variable("track_info.reco_track_range[0]","(50,0,150)","Reconstructed Track Range [cm]", true,"d"));
			vars.push_back(bdt_variable("track_info.reco_track_kinetic[0]","(50,0,1)","Reconstructed Track Kinetic Energy [cm]", true,"d"));
			vars.push_back(bdt_variable("reco_track_energy_new[0][0]", "(50,0,2)","Track Energy (New -Plane 0)",true, "d"));
			vars.push_back(bdt_variable("reco_track_energy_new[0][1]", "(50,0,2)","Track Energy (New -Plane 1)",true, "d"));
			vars.push_back(bdt_variable("reco_track_energy_new[0][2]", "(50,0,2)","Track Energy (New -Plane 2)",true, "d"));

			vars.push_back(bdt_variable("reco_track_energy_from_dEdx[0][0]", "(50,0,2)","Track Energy (dEdx - Plane 0)",true, "d"));
			vars.push_back(bdt_variable("reco_track_energy_from_dEdx[0][1]", "(50,0,2)","Track Energy (dEdx - Plane 1)",true, "d"));
			vars.push_back(bdt_variable("reco_track_energy_from_dEdx[0][2]", "(50,0,2)","Track Energy (dEdx - Plane 2)",true, "d"));

			vars.push_back(bdt_variable("reco_track_energy[0]", "(50,0,2)","Track Energy (0ld)",true, "d"));
			vars.push_back(bdt_variable("totalpe_ibg_sum","(50,0,15000)","Total in Beam-Gate PE",false,"d"));
			vars.push_back(bdt_variable("totalpe_bbg_sum","(50,0,15000)","Total Before Beam-Gate PE",false,"d"));
			vars.push_back(bdt_variable("pi0_info.num_reco_showers_within_20cm_vertex","(10,0,10)","Num Showers within 20cm",false,"i"));
			vars.push_back(bdt_variable("pi0_info.num_reco_showers_within_30cm_vertex","(10,0,10)","Num Showers within 30cm",false,"i"));

			vars.push_back(bdt_variable("pi0_info.num_reco_tracks_within_10cm_vertex","(10,0,10)","Num tracks within 10cm",false,"i"));
			vars.push_back(bdt_variable("pi0_info.num_reco_tracks_within_20cm_vertex","(10,0,10)","Num tracks within 20cm",false,"i"));
			vars.push_back(bdt_variable("pi0_info.num_reco_tracks_within_30cm_vertex","(10,0,10)","Num tracks within 30cm",false,"i"));

			//	vars.push_back(bdt_variable("reco_asso_tracks","(5,0,4)","Number of Reconstructed Tracks",false,"i"));
			//	vars.push_back(bdt_variable("pi0_info.pi0_class_number","(52,-1,50)","Pi0 Class Number",false,"i"));
		}
	}
	std::vector<bdt_variable> vars2;
	if(mode_option == "vars" || mode_option == "stack"){
		vars2.push_back(bdt_variable("reco_shower_startx[0]","(50,0,250)"," Shower Start X pos [cm]",false,"d"));
		vars2.push_back(bdt_variable("reco_shower_starty[0]","(50,-110,+110)","Shower Start Y pos [cm]",false,"d"));
		vars2.push_back(bdt_variable("reco_shower_startz[0]","(50,0,1050)","Shower Start Z pos [cm]",false,"d"));

		vars2.push_back(bdt_variable("reco_shower_startx[0] + reco_shower_dirx[0]*reco_shower_length[0]","(50,0,250)"," Shower End X pos [cm]",false,"d"));
		vars2.push_back(bdt_variable("reco_shower_starty[0] + reco_shower_diry[0]*reco_shower_length[0]","(50,-110,+110)","Shower End Y pos [cm]",false,"d"));
		vars2.push_back(bdt_variable("reco_shower_startz[0] + reco_shower_dirz[0]*reco_shower_length[0]","(50,0,1050)","Shower End Z pos [cm]",false,"d"));

	}


	if(mode_option == "recomc"){

		vars2.push_back(bdt_variable("reco_true_nuvert_dist","(50,0,450)","Distance from Reco to Truth Vertex [cm]",false,"d"));


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



	/*

	//0.53842 0.505593 1.21062
	=======
	 *************************** TrackOnly *********************
	 Best is at track_cosmicut: 0.552 track_bnbcut: 0.528
	 track sig: 11.6057 track bkg 72.2287 1.36558
	 *************************** NoTrackOnly *********************
	 Best is at notrack_cosmicut: 0.557621 notrack_bnbcut: 0.525971
	 notrack sig: 21.0077 notrack bkg 421.783 1.0229

	 *************************** Combined *********************
	 Best is at track_cosmicut: 0.552 track_bnbcut: 0.521
	 Best is at notrack.g_cosmicut: 0.557621 notrack_bnbcut: 0.539692
	 */
	double fcoscut;
	double fbnbcut;
	if(istrack == "track"){
		fcoscut = 0.552;
		fbnbcut = 0.528;

		//		fcoscut = 0.552;
		//		fbnbcut = 0.521;

	}else if(istrack == "notrack"){
		//: 0.573333 bcut: 0.544  #signal: 22.6913 #bkg: 594.396 ||  bnb: 569.984 cos: 24.4124 || 0.930727
		fcoscut = 0.57333;
		fbnbcut = 0.544;

	}



	//===========================================================================================
	//===========================================================================================
	//		Main flow of the program , using OPTIONS
	//===========================================================================================
	//===========================================================================================

	if(mode_option == "train") {
		std::cout<<"**********************Starting COSMIC BDT Training*************************"<<std::endl;
		if(run_cosmic) bdt_train(cosmic_bdt_info, signal_pure, intime, vars, TMVAmethods);
		std::cout<<"**********************Starting BNB BDT Training*************************"<<std::endl;
		if(run_bnb) bdt_train(bnb_bdt_info, signal_pure, bnb_pure, vars, TMVAmethods);
		std::cout<<"**********************Starting NCpi0 BDT Training*************************"<<std::endl;
		//		bdt_train(ncpi0_bdt_info, signal_pure, ncpi0, vars,TMVAmethods);



		return 0;



	}else if(mode_option == "app"){

		//Apply! This will update cosmic_bdt_info, signal file and bkg file. As in update them PROPERLY!	
		std::vector<bdt_file*> app_files = {bnbext}; 
		if(run_cosmic) bdt_app(cosmic_bdt_info, bdt_files, vars, TMVAmethods);
		if(run_bnb)    bdt_app(bnb_bdt_info, bdt_files, vars, TMVAmethods);

		return 0;

	}
	else if(mode_option == "response"){

		//Ok print out Cosmic BDT
		if(run_cosmic){
			bdt_response cosmic_response(cosmic_bdt_info, signal_pure, intime);
			cosmic_response.plot_bdt_response(ftest);
		}

		if(run_bnb){
			bdt_response bnb_response(bnb_bdt_info, signal_pure, bnb_pure);
			bnb_response.plot_bdt_response(ftest);
		}
	}	
	else if(mode_option == "recomc"){

		//std::vector<int> recomc_cols = {kRed-7, kRed+1, kYellow-7, kOrange-3, kBlue+3, kBlue,  kGreen+1, kBlue-7, kMagenta, kCyan, kOrange};
		std::vector<int> recomc_cols = {kRed-7, kBlue+3, kBlue, kBlue-7, kMagenta-3, kYellow-7, kOrange-3, kGreen+1 ,kGray};
		std::vector<std::string> recomc_names = {"NC #Delta Radiative #gamma", "#pi^{0} #rightarrow #gamma, Outside", "#pi^{0} #rightarrow #gamma, Low E_{#gamma} ","#pi^{0} #rightarrow #gamma, Other","Non #pi^{0} #gamma","BNB electron","BNB other","Cosmic"};
		std::vector<std::string> recomc_cuts = {
			"true_shower_origin[0]==1 && true_shower_pdg[0] == 22 && true_shower_parent_pdg[0] !=111 && is_delta_rad ==1 ",
			"true_shower_pdg[0] == 22 && true_shower_parent_pdg[0] == 111 && true_shower_origin[0]==1 && (pi0_info.pi0_class_number == 10 || pi0_info.pi0_class_number ==11)",
			"true_shower_pdg[0] == 22 && true_shower_parent_pdg[0] == 111 && true_shower_origin[0]==1 && pi0_info.pi0_class_number == 41",
			"true_shower_pdg[0] == 22 && true_shower_parent_pdg[0] == 111 && true_shower_origin[0]==1 && !(pi0_info.pi0_class_number ==10 || pi0_info.pi0_class_number == 11) && pi0_info.pi0_class_number != 41",
			"true_shower_pdg[0] == 22 && true_shower_parent_pdg[0] != 111 && is_delta_rad!=1 && true_shower_origin[0]==1",
			"true_shower_origin[0] ==1 && abs(true_shower_pdg[0]) ==11",
			"true_shower_origin[0] ==1 && true_shower_pdg[0] !=22 && abs(true_shower_pdg[0]) !=11",
			"true_shower_origin[0] ==2"
		};

		bdt_recomc test(recomc_names, recomc_cuts, recomc_cols,istrack);
		//plot_recomc(TFile *fout, bdt_file* file, bdt_variable var, double cut_cosmic_val, double cut_bnb_val){




		if(false){
			if(run_cosmic){
				test.plot_recomc(ftest, signal_cosmics, signal_cosmics->getBDTVariable(cosmic_bdt_info) , fcoscut,fbnbcut);
				test.plot_recomc(ftest, bnb_cosmics, bnb_cosmics->getBDTVariable(cosmic_bdt_info) , fcoscut,fbnbcut);
			}
			if(run_bnb){
				test.plot_recomc(ftest, bnb_cosmics, bnb_cosmics->getBDTVariable(bnb_bdt_info) , fcoscut,fbnbcut);
				test.plot_recomc(ftest, signal_cosmics, signal_cosmics->getBDTVariable(bnb_bdt_info) , fcoscut,fbnbcut);
			}
			return 0;

		}

		int h=0;
		for(auto &v:vars2){
			test.setRebin(false);
			if(v.name == "pi0_info.num_reco_showers_within_10cm_vertex") test.setRebin(false);
			test.plot_recomc(ftest, signal_cosmics, v, fcoscut, fbnbcut);
			test.plot_recomc(ftest, bnb_cosmics, v, fcoscut, fbnbcut);
			if(v.name == "pi0_info.num_reco_showers_within_10cm_vertex") test.setRebin(true);
			h++;
		}	

	}
		else if(mode_option == "sig"){


			TFile *fsig = new TFile(("significance_"+istrack+".root").c_str(),"recreate");
			std::vector<double> ans = scan_significance(fsig, {signal_cosmics} , {bnb_cosmics, intime}, cosmic_bdt_info, bnb_bdt_info);
			std::cout<<"Best Fit Significance: "<<ans.at(0)<<" "<<ans.at(1)<<" "<<ans.at(2)<<std::endl;
			fsig->Close();


		}else if(mode_option == "stack"){
			bdt_stack obs(istrack+"_stack");
			obs.addToStack(signal_cosmics);
			obs.addToStack(bnb_cosmics);
			obs.addToStack(intime);

			int ip=0;
			for(auto &v:vars2){
				ip++;
				obs.plotStacks(ftest,  v ,fcoscut,fbnbcut);
			}

			return 0;
			obs.plotBDTStacks(ftest, bnb_bdt_info, fcoscut, fbnbcut);
			obs.plotBDTStacks(ftest, cosmic_bdt_info, fcoscut, fbnbcut);
			return 0;

		}else if(mode_option == "datamc"){


			bdt_stack *obs = new bdt_stack(istrack+"_datamc");
			obs->plot_pot = 5e19;
			obs->addToStack(signal_cosmics);
			obs->addToStack(bnb_cosmics);
			obs->addToStack(intime);

			bdt_stack *obs2 = new bdt_stack(istrack+"_extintime");
			obs2->plot_pot = 5e19;
			obs2->addToStack(intime);

			bdt_stack *obs3 = new bdt_stack(istrack+"_overlaymc");
			obs3->plot_pot = 5e19;
			obs3->addToStack(bnb_cosmics);


			int ip=0;
			for(auto &v:vars){
				break;
				ip++;
				bdt_datamc tdatamc(data5e19, obs, istrack);	
				bdt_datamc tdatamc2(bnbext, obs2, istrack);	
				//bdt_datamc tdatamc3(overlay, obs3);	
				tdatamc.plotStacks(ftest,  v ,fcoscut,fbnbcut);
				tdatamc2.plotStacks(ftest,  v,fcoscut,fbnbcut);
				//tdatamc3.plotStacks(ftest,  v,fcoscut,fbnbcut);
				return 0;	

			}

			bdt_datamc datamc(data5e19, obs, istrack);	
			bdt_datamc datamc2(bnbext, obs2, istrack);	
			//bdt_datamc datamc3(overlay, obs3);	

			datamc.plotBDTStacks(ftest, bnb_bdt_info ,fcoscut,fbnbcut);
			datamc.plotBDTStacks(ftest, cosmic_bdt_info ,fcoscut,fbnbcut);
			datamc2.plotBDTStacks(ftest, bnb_bdt_info ,fcoscut,fbnbcut);
			datamc2.plotBDTStacks(ftest, cosmic_bdt_info ,fcoscut,fbnbcut);
			//datamc3.plotBDTStacks(ftest, bnb_bdt_info ,fcoscut,fbnbcut);
			//datamc3.plotBDTStacks(ftest, cosmic_bdt_info ,fcoscut,fbnbcut);



		}else if(mode_option == "vars"){

			std::vector<std::string> title = {"All Verticies","Pre-Selection Cuts"};

			if(run_cosmic){
				for(auto &v:vars){
					TCanvas *c_var = new TCanvas(("cvar_"+v.name+"_cosmo").c_str(), ("cvar_"+v.name+"_cosmo").c_str(),2200,1200);
					c_var->Divide(2,1);
					c_var->cd();

					for(int j=0; j<2;j++){	

						std::string cut_signal = signal_pure->getStageCuts(j,fcoscut,fbnbcut); 
						std::string cut_intime = intime->getStageCuts(j,fcoscut,fbnbcut); 


						TH1* sig = signal_pure->getTH1(v,cut_signal.c_str(),v.safe_name+"_sig_cosmo_var" ,1.0);
						TH1* bkg = intime->getTH1(v,cut_intime.c_str(),v.safe_name+"_bkg_cosmo_var" ,1.0);

						sig->Scale(1.0/sig->Integral());			
						bkg->Scale(1.0/bkg->Integral());			
						sig->SetLineColor(kRed-7);
						bkg->SetLineColor(kGreen-3);
						sig->SetLineWidth(2);
						bkg->SetLineWidth(2);


						sig->SetFillColor(kRed-7);
						bkg->SetFillColor(kGreen-3);
						sig->SetFillStyle(3445);
						bkg->SetFillStyle(3454);

						sig->SetTitle(title.at(j).c_str());
						c_var->cd(j+1);			

						sig->Draw("hist");
						bkg->Draw("hist same");
						//sig->GetXaxis()->SetTitle(v.unit.c_str());
						sig->GetYaxis()->SetTitle("Verticies [Area Normalized]");
						sig->GetYaxis()->SetTitleOffset(1.5);

						TLegend *l = new TLegend(0.11,0.75,0.89,0.89);
						l->SetLineColor(kWhite);
						l->SetFillStyle(0);
						l->SetNColumns(2);

						l->AddEntry(sig,"NC #Delta #gamma Signal","lf");	
						l->AddEntry(bkg,"Intime Cosmic Background","lf");	
						l->Draw();

						double max_height = std::max( sig->GetMaximum(), bkg->GetMaximum());
						sig->SetMaximum(max_height*1.3);



					}
					c_var->Print(("var/cosmic_"+v.safe_unit+".pdf").c_str(),"pdf");
				}

			}
			if(run_bnb){
				for(auto &v:vars){
					TCanvas *c_var = new TCanvas(("cvar_"+v.name+"_bnb").c_str(), ("cvar_"+v.name+"_bnb").c_str(),2200,1200);
					c_var->Divide(2,1);
					c_var->cd();

					for(int j=0; j<2;j++){	

						std::string cut_signal = signal_pure->getStageCuts(j,fcoscut,fbnbcut); 
						std::string cut_bnb = bnb_pure->getStageCuts(j,fcoscut,fbnbcut); 


						TH1* sig = signal_pure->getTH1(v,cut_signal.c_str(),v.safe_name+"_sig_bnb_var" ,1.0);
						TH1* bkg = bnb_pure->getTH1(v,cut_bnb.c_str(),v.safe_name+"_bkg_bnb_var" ,1.0);

						sig->Scale(1.0/sig->Integral());			
						bkg->Scale(1.0/bkg->Integral());			
						sig->SetLineColor(kRed-7);
						bkg->SetLineColor(kBlue-4);
						sig->SetLineWidth(2);
						bkg->SetLineWidth(2);

						sig->SetFillColor(kRed-7);
						bkg->SetFillColor(kBlue-4);
						sig->SetFillStyle(3445);
						bkg->SetFillStyle(3454);

						sig->SetTitle(title.at(j).c_str());
						c_var->cd(j+1);			

						sig->Draw("hist");
						sig->SetMaximum(sig->GetMaximum()*1.25);
						bkg->Draw("hist same");
						//sig->GetXaxis()->SetTitle(v.unit.c_str());
						sig->GetYaxis()->SetTitle("Verticies [Area Normalized]");
						sig->GetYaxis()->SetTitleOffset(1.5);

						TLegend *l = new TLegend(0.11,0.75,0.89,0.89);
						l->SetLineColor(kWhite);
						l->SetFillStyle(0);
						l->SetNColumns(2);

						l->AddEntry(sig,"NC #Delta #gamma Signal","lf");	
						l->AddEntry(bkg,"BNB Backgrounds","lf");	
						l->Draw();

						double max_height = std::max( sig->GetMaximum(), bkg->GetMaximum());
						sig->SetMaximum(max_height*1.1);



					}
					c_var->Print(("var/"+istrack+"_bnb_"+v.safe_unit+".pdf").c_str(),"pdf");
				}
			}







		} else if(mode_option == "eff"){


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
				std::string gencut = "1";
				if(bdt_files.at(i)->tag == "IntimeCosmics" || bdt_files.at(i)->tag == "Data5e19" || bdt_files.at(i)->tag == "BNBext"){
					gencut = "1";
				}	

				double nevents = bdt_files.at(i)->numberofevents*pot_scale;	
				double nv = bdt_files.at(i)->tvertex->GetEntries((gencut+"&&1").c_str())*pot_scale;	
				double ns = bdt_files.at(i)->tvertex->GetEntries( bdt_files.at(i)->getStageCuts(0,fcoscut,fbnbcut).c_str())*pot_scale;	

				start = nevents;
				sel = ns;
				std::cout<<"Stage Gen\t\t"<<nevents<<"\t\t"<<nevents/start*100<<std::endl;
				std::cout<<"Stage Vertex\t\t"<<nv<<"\t\t"<<nv/start*100<<std::endl;
				std::cout<<"Stage Topo\t\t"<<ns<<"\t\t"<<ns/start*100<<"\t\t"<<100<<std::endl;


				for(int m=0; m< bdt_files.at(i)->flow.vec_pre_cuts.size(); m++){
					std::string thiscut = bdt_files.at(i)->flow.vec_pre_cuts.at(m)+"&&"+bdt_files.at(i)->getStageCuts(0,fcoscut,fbnbcut);
					double np = bdt_files.at(i)->tvertex->GetEntries(thiscut.c_str())*pot_scale;
					std::cout<<"Precut: "<<bdt_files.at(i)->flow.vec_pre_cuts.at(m)<<"\t||\t"<<np<<"\t("<<np/ns*100<<")\%"<<std::endl;
				}

				std::cout<<"One by One"<<std::endl;
				std::string thiscut = bdt_files.at(i)->getStageCuts(0,fcoscut,fbnbcut);
				for(int m=0; m< bdt_files.at(i)->flow.vec_pre_cuts.size(); m++){
					thiscut += "&&"+ bdt_files.at(i)->flow.vec_pre_cuts.at(m);
					double np = bdt_files.at(i)->tvertex->GetEntries(thiscut.c_str())*pot_scale;
					std::cout<<" + "<<bdt_files.at(i)->flow.vec_pre_cuts.at(m)<<"\t||\t"<<np<<"\t("<<np/ns*100<<")\%"<<std::endl;
				}


				for(int j=0; j <5; j++){		
					std::string thiscut = bdt_files.at(i)->getStageCuts(j,fcoscut,fbnbcut); 
					double nvert =bdt_files.at(i)->tvertex->GetEntries(thiscut.c_str())*pot_scale;
					double eff =0;
					//	if(j==0){
					//		start = nvert;
					//		
					//	}

					eff = nvert/start*100;

					std::cout<<nvert<<"\t("<<nvert/sel*100<<" \%)"<<" ";	
				}

				std::cout<<std::endl;
				for(int j=0; j <5; j++){		
					std::string thiscut = bdt_files.at(i)->getStageCuts(j,fcoscut,fbnbcut); 
					double nvert =bdt_files.at(i)->tvertex->GetEntries(thiscut.c_str())*pot_scale;
					double eff =0;
					//	if(j==0){
					//		start = nvert;
					//		
					//	}

					eff = nvert/start*100;

					std::cout<<nvert<<"\t("<<eff<<" \%)"<<" ";	
				}
				std::cout<<std::endl;
			}


		}
		else if(mode_option == "effdata"){
			std::vector<bdt_file*> data_files = {data5e19, bnbext};



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

				double nevents = 0;//data_files.at(i)->tevent->GetEntries(gencut.c_str())*pot_scale;	
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

			//std::vector<bdt_file*> bdt_filesB = {bnb_pure,signal_pure};
			//std::vector<bdt_file*> bdt_filesB = {intime, data5e19, bnbext};
			std::vector<bdt_file*> bdt_filesB = {signal_pure, bnb_pure, intime, data5e19, bnbext};
			//std::vector<bdt_file*> bdt_filesB = {bnbext};

			bdt_precalc pre1(bdt_filesB.at(number));
			//pre1.genTrackInfo();
			pre1.genPi0Info();
			return 0;

			for(auto &f: bdt_filesB){
				bdt_precalc pre(f);
				//pre.genTrackInfo();
				pre.genPi0Info();
			}
		}

		else {
			std::cout << "WARNING: " << mode_option << " is an invalid option\n";
		}

		ftest->Close();

		return 0;

	}
