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
#include "bdt_varplot.h"
#include "bdt_precalc.h"
#include "bdt_info.h"
#include "bdt_train.h"
#include "bdt_app.h"
#include "bdt_response.h"
#include "bdt_recomc.h"
#include "bdt_sig.h"
#include "bdt_boxcut.h"
#include "bdt_spec.h"




int main (int argc, char *argv[]){

	std::string dir = "/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_v3/";
	//std::string dir = "/home/mark/work/uBooNE/photon/tmva/samples/fresh_NCDR_bf_v2";

	std::string mode_option = "fake"; 
	std::string xml = "default.xml";
	std::string istrack ="track";

	bool run_cosmic = true;
	bool run_bnb = true;
	int number = -1;
	bool response_only = false;

	//All of this is just to load in command-line arguments, its not that important
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

	int iarg = 0; opterr=1; int index;
	while(iarg != -1)
	{
		iarg = getopt_long(argc,argv, "cbx:o:d:t:n:rh?", longopts, &index);

		switch(iarg)
		{
			case 'r':
				response_only = true;;
				break;
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





	//===========================================================================================
	//===========================================================================================
	//			Begininning of main program here!
	//===========================================================================================
	//===========================================================================================

	//Most TMVA arguments are loaded in here via XML
	MVALoader xml_methods(xml);
	std::vector<method_struct> TMVAmethods  = xml_methods.GetMethods(); 



	//A couple of variable definitions for convienance
	std::string angle_track_shower ="(reco_track_dirx[0]*reco_shower_dirx[0]+reco_track_diry[0]*reco_shower_diry[0]+reco_track_dirz[0]*reco_shower_dirz[0])";

	std::string fiducial_vertex = "reco_nuvertx > 10 && reco_nuvertx < 246 && reco_nuverty > -107 && reco_nuverty < 107.01 && reco_nuvertz > 10 && reco_nuvertz < 1026 ";
	std::string fiducial_shower_end = "reco_shower_endx > 10 && reco_shower_endx < 246 && reco_shower_endy > -107 && reco_shower_endy < 107.01 && reco_shower_endz > 10 && reco_shower_endz < 1026 ";
	std::string fiducial_track_end = "reco_track_endx > 10 && reco_track_endx < 246 && reco_track_endy > -107 && reco_track_endy < 107.01 && reco_track_endz > 10 && reco_track_endz < 1026 ";
	std::string fiducial_cut = fiducial_vertex;//"&&"+fiducial_shower_end;
	std::string bump_region = " reco_shower_helper_energy[0]>0.130 && reco_shower_helper_energy[0]<0.2";

	std::string new_precuts;
	std::string num_track_cut = "==1";

	std::string good_calo_cut = "track_info.reco_track_good_calo[0]>0 && track_info.reco_track_mean_dEdx[0] > 1";
	std::string track_length_cut = "reco_track_displacement[0]<100";
	std::string min_energy_cut = "reco_shower_helper_energy[0]>0.03 ";
	std::string min_conversion_cut = "shortest_asso_shower_to_vert_dist > 1";
	std::string track_direction_cut = "track_info.reco_track_start_mean_dEdx[0]/track_info.reco_track_end_mean_dEdx[0] >= 0.925";
	std::string back_to_back_cut = angle_track_shower+" > -0.95 &&"  + angle_track_shower + "< 0.95";
	std::string pe_cut = "totalpe_ibg_sum > 20";


	//This is a vector each containing a precut, they are all added together to make the whole "precut"
	std::vector<std::string> vec_precuts;


	//Precuts are different if its a track or notrack sample 
	if(istrack == "track"){
		fiducial_cut =  fiducial_cut;
		new_precuts = fiducial_cut + " &&" + track_length_cut +"&&"+ min_energy_cut +"&&" + min_conversion_cut + "&&"+ good_calo_cut +"&&" + track_direction_cut + "&&"+ back_to_back_cut;
		num_track_cut = "== 1";

		//We dont include the pe_cut for training
		if(mode_option != "train" && mode_option != "app" && mode_option !="vars" && mode_option!="response"){
			vec_precuts = {pe_cut, fiducial_cut, track_length_cut, min_energy_cut, min_conversion_cut, good_calo_cut, track_direction_cut, back_to_back_cut};
		}else{
			vec_precuts = {fiducial_cut, track_length_cut, min_energy_cut, min_conversion_cut, good_calo_cut, track_direction_cut, back_to_back_cut};

		}

	}else if(istrack == "notrack"){
		new_precuts = fiducial_cut+ "&&" + min_energy_cut ;
		num_track_cut = "== 0";
		vec_precuts = {fiducial_cut, min_energy_cut};
		if(mode_option!="response" &&mode_option != "train" && mode_option != "app" && mode_option != "vars") vec_precuts = {pe_cut, fiducial_cut, min_energy_cut};
	}


	//We dont currently use postcuts
	std::string postcuts = "1";


	//Set up some info about the BDTs to pass along
	bdt_info bnb_bdt_info("bnb_"+istrack, "BNB focused BDT","(56,0.37,0.565)");
	bdt_info cosmic_bdt_info("cosmic_"+istrack, "Cosmic focused BDT","(56,0.3,0.65)");


	std::string base_cuts = "reco_asso_showers == 1 && reco_asso_tracks "+num_track_cut;
	std::string signal_definition = "is_delta_rad == 1";
	std::string background_definition = "!(" +signal_definition+ ")";
	//	std::string background_definition = "is_delta_rad == 0 && ccnc== 0 &&abs(true_shower_pdg[0]) ==11 && abs(nu_pdg)==12 && (exiting_electron_number==1 || exiting_antielectron_number==1)";

	//Train on "good" signals, defined as ones matched to the ncdelta and have little "clutter" around.	
	std::string true_signal = "shower_matched_to_ncdeltarad_photon[0]==1";
	std::string true_bkg    = "true_shower_origin[0]==1";

	if(istrack == "track"){
		true_signal = true_signal+ "&& track_matched_to_ncdeltarad_proton[0]==1";
		true_bkg = true_bkg +"&& true_track_origin[0]==1";

		bnb_bdt_info.setTopoName("1#gamma1p");
		cosmic_bdt_info.setTopoName("1#gamma1p");
	}else{
		bnb_bdt_info.setTopoName("1#gamma0p");
		cosmic_bdt_info.setTopoName("1#gamma0p");
	}

	if(mode_option != "train" && mode_option !="vars" && mode_option !="app" && mode_option!="response"){
		new_precuts = "totalpe_ibg_sum > 20 &&" +new_precuts;
	}


	//***************************************************************************************************/
	//***********	The bdt_flows define the "flow" of the analysis, i.e what cuts at what stage  *******/
	//***************************************************************************************************/
	bdt_flow signal_pure_flow(base_cuts, signal_definition +"&&"+ true_signal, vec_precuts,	postcuts,cosmic_bdt_info,	bnb_bdt_info);
	bdt_flow signal_flow(base_cuts, signal_definition , vec_precuts,	postcuts,cosmic_bdt_info,	bnb_bdt_info);
	bdt_flow cosmic_flow(base_cuts,"1", vec_precuts , postcuts,	cosmic_bdt_info,bnb_bdt_info);
	bdt_flow bkg_flow(base_cuts,background_definition, vec_precuts,postcuts,	cosmic_bdt_info,	bnb_bdt_info);
	bdt_flow bkg_pure_flow(base_cuts,background_definition+"&&"+ true_bkg ,vec_precuts,postcuts,	cosmic_bdt_info,	bnb_bdt_info);
	bdt_flow data_flow(base_cuts,"1",	vec_precuts, postcuts,	cosmic_bdt_info, 	bnb_bdt_info);

	// BDt files , bdt_file::bdt_file(std::string indir,std::string inname, std::string intag, std::string inops, std::string inrootdir, int incol, bdt_flow inflow) :
	//bdt_file *signal_pure    = new bdt_file(dir, "vertexed_ncdeltarad_fresh_v1.0.root",	"NCDeltaRad",	   "hist","",  kRed-7, signal_pure_flow);
	bdt_file *signal_pure    = new bdt_file(dir, "vertexed_ncdeltaradcosmics_fresh_v4.root",	"NCDeltaRad",	   "hist","",  kRed-7, signal_pure_flow);
	bdt_file *signal_cosmics = new bdt_file(dir, "vertexed_ncdeltaradcosmics_fresh_v4.root", "NCDeltaRadCosmics", "hist","",  kRed-7, signal_flow);
	bdt_file *bnb_pure    = new bdt_file(dir, "vertexed_bnbcosmics_fresh_v4.root", "BNBPure",	  "hist","",  kBlue-4, bkg_pure_flow);
	bdt_file *bnb_cosmics = new bdt_file(dir, "vertexed_bnbcosmics_fresh_v4.root", "BNBCosmics", "hist","",  kBlue-4, bkg_flow);
	bdt_file *intime = new bdt_file(dir, "vertexed_intime_fresh_v4.root" ,"IntimeCosmics","hist","", kGreen-3, cosmic_flow);
	//Data files
	bdt_file *data5e19    = new bdt_file(dir, "vertexed_data5e19_fresh_v4.root",	"Data5e19",	   "E1p","",  kBlack, data_flow);
	bdt_file *bnbext    = new bdt_file(dir, "vertexed_bnbext_fresh_v4.root",	"BNBext",	"E1p","",  kBlack, data_flow);


	bdt_file *lee = new bdt_file(dir,"vertexed_elikeleecosmics_fresh_v4.root","LEEsignal","hist","",kRed-7, signal_flow);

	//For conviencance fill a vector with pointers to all the files to loop over.
	std::vector<bdt_file*> bdt_files = {signal_cosmics, signal_pure, bnb_pure, bnb_cosmics, intime, data5e19, bnbext};

	//The LEE signal is bigger than the SM signal by this factor
	signal_pure->scale_data = 3.1;
	signal_cosmics->scale_data = 3.1;

	std::cout<<"--------------------------------------------------------------------------"<<std::endl;
	std::cout<<"--------------------------------------------------------------------------"<<std::endl;
	for(auto &f: bdt_files){
		std::cout<<"Loading "<<f->tag<<"\t with "<<f->tvertex->GetEntries()<<"\t verticies. (unweighted)"<<std::endl;
		std::cout<<"POT of file loaded is: "<<f->pot<<"\t\t "<<std::endl;
		std::cout<<"Scale factor is then: "<<f->scale_data<<std::endl;
	}
	std::cout<<"--------------------------------------------------------------------------"<<std::endl;
	std::cout<<"--------------------------------------------------------------------------"<<std::endl;




	std::cout<<"--------------------------------------------------------------------------"<<std::endl;
	std::cout<<" Going to add any precomputed tree friends, or any trained BDT responses   "<<std::endl;
	std::cout<<" If you see warnings, but havenet yet ran app stage, thats ok!            "<<std::endl;
	std::cout<<"--------------------------------------------------------------------------"<<std::endl;
	if(mode_option != "precalc" ){
		for(auto &f: bdt_files){
			addPreFriends(f,"track");
			addPreFriends(f,"pi0");
			if(f->tag != "Data5e19" && f->tag != "BNBext") addPreFriends(f,"bnbcorrection");
			addPreFriends(f,"shower");
			//addPreFriends(f,"pi0boost");
			
			if(mode_option != "app" && mode_option != "train") f->addBDTResponses(cosmic_bdt_info, bnb_bdt_info, TMVAmethods);
			
			std::cout<<"Filling PrecutEntryList on File "<<f->tag<<std::endl;
			f->calcPrecutEntryList();

		}
	}
	std::cout<<"--------------------------------------------------------------------------"<<std::endl;
	std::cout<<"--------------------------------------------------------------------------"<<std::endl;




	//Adding plot names
	signal_pure->addPlotName("NC Delta Radiative");
	signal_cosmics->addPlotName("LEE NC #Delta Rad w/ Corsika");
	bnb_pure->addPlotName("BNB Backgrounds");
	bnb_cosmics->addPlotName("BNB w/ Corsika");
	intime->addPlotName("Intime Corsika cosmics");
	data5e19->addPlotName("4.8e19 POT Data");
	bnbext->addPlotName("External BNB Data");

	std::cout<<"--------------------------------------------------------------------------"<<std::endl;
	std::cout<<"--------------------------------------------------------------------------"<<std::endl;
	//Variables!


	std::vector<bdt_variable> vars;

	std::string proton_mass = "0.938272";
	std::string reco_shower_momentum = "(-0.017962 + 1.3719*reco_shower_helper_energy[0])"; 
	std::string reco_track_energy = "track_info.reco_track_kinetic_from_length[0]+"+proton_mass;
	std::string reco_track_momentum  = "sqrt("+reco_track_energy+"*"+reco_track_energy+"-"+proton_mass +"*"+proton_mass+")";
	std::string invariant_mass = proton_mass+"*"+proton_mass+"+2.0*("+reco_track_energy+"*"+reco_shower_momentum+"-"+reco_track_momentum+"*"+reco_shower_momentum+"*"+angle_track_shower+")";


	vars.push_back(bdt_variable("shower_info.amalgamated_shower_dEdx[0]","(52,0,7)", "Amalgamated Shower dE/dx [MeV/cm]",false,"d"));
//	vars.push_back(bdt_variable("reco_shower_helper_energy[0]","(52,0,0.6)","Reconstructed Shower Energy [GeV]", false,"d"));
	vars.push_back(bdt_variable(reco_shower_momentum,"(52,0,0.6)","Reconstructed Shower Energy (Corrected) [GeV]", false,"d"));
	vars.push_back(bdt_variable("reco_shower_length[0]","(52,0,100)","Shower Length [cm]",false,"d"));

	vars.push_back(bdt_variable("reco_shower_opening_angle[0]","(52,0,0.8)","Shower Opening Angle",false,"d"));
	vars.push_back(bdt_variable("reco_shower_dist_to_closest_flashzcenter[0]","(52,0,520)","Distance from Shower to Flashcenter [cm]",false,"d"));

	vars.push_back(bdt_variable("reco_nu_vtx_dist_to_closest_tpc_wall","(52,0,120)","Reconstructed Vertex to TPC Wall Distance [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_shower_bp_dist_to_tpc[0]","(52,0,550)","Back Projected Distance from Shower to TPC wall [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_nuvertx","(52,0,250)"," Reconstructed Vertex X pos [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_nuverty","(52,-110,+110)","Reconstructed Vertex Y pos [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_nuvertz","(52,0,1050)","Reconstructed Vertex Z pos [cm]",false,"d"));

//	vars.push_back(bdt_variable("cos(atan2(reco_shower_diry[0],reco_shower_dirz[0]))","(52,-1,1)","Reconstructed Shower - Cosine Theta", true,"d"));
//	vars.push_back(bdt_variable("cos(atan2(reco_shower_diry[0], reco_shower_dirx[0]))","(52,-1,1)","Reconstructed Shower - Cosine Phi", true,"d"));
	vars.push_back(bdt_variable("atan2(reco_shower_diry[0],reco_shower_dirz[0])","(52,-3.14159, 3.14159)","Reconstructed Shower - Theta", true,"d"));
	vars.push_back(bdt_variable("atan2(reco_shower_diry[0], reco_shower_dirx[0])","(52,-3.14159,-3.14159)","Reconstructed Shower - Phi", true,"d"));

	vars.push_back(bdt_variable("reco_shower_endx[0]","(52,0,250)"," Reconstructed Shower End X pos [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_shower_endy[0]","(52,-110,+110)","Reconstructed Shower End Y pos [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_shower_endz[0]","(52,0,1050)","Reconstructed Shower End Z pos [cm]",false,"d"));

	//New Variables for Showers!
	//vars.push_back(bdt_variable("reco_shower_helpernew_energy[0]","(52,0,0.4)","Reconstructed Shower Energy (NEW) [GeV]", false,"d"));
	//vars.push_back(bdt_variable("reco_shower_dedxnew_plane2[0]","(52,0,6)", "Shower dE/dx Collection Plane (NEW) [MeV/cm]",false,"d"));

	if(istrack=="track"){

		//Proton Straightness (not currently feasible)
		//Ratio of Track energy via length to track energy via Calometery
		vars.push_back(bdt_variable( reco_track_energy +"*(reco_track_dirx[0])+ "+reco_shower_momentum+"*reco_shower_dirx[0]","(52,-1.5,2)","Reconstructed Delta Momentum X [GeV]",true,"d"));
		vars.push_back(bdt_variable( reco_track_energy +"*(reco_track_diry[0])+ "+reco_shower_momentum+"*reco_shower_diry[0]","(52,-1.5,2)","Reconstructed Delta Momentum Y [GeV]",true,"d"));
		vars.push_back(bdt_variable( reco_track_energy +"*(reco_track_dirz[0])+ "+reco_shower_momentum+"*reco_shower_dirz[0]","(52,-1.5,2)","Reconstructed Delta Momentum Z [GeV]",true,"d"));

		vars.push_back(bdt_variable( "sqrt( pow("+reco_track_energy +"*(reco_track_dirx[0])+ "+reco_shower_momentum+"*reco_shower_dirx[0],2)+ pow("+reco_track_energy +"*(reco_track_diry[0])+"+reco_shower_momentum +"*reco_shower_diry[0],2))","(52,0,2)","Reconstucted Delta Transverse (YX) Momentum [GeV]",true,"d"));

		vars.push_back(bdt_variable("reco_track_displacement[0]","(52,0,150)","Reconstructed Track Displacement [cm]", true,"d"));

		vars.push_back(bdt_variable("track_info.reco_track_mean_dEdx[0]", "(52,0,12)","Mean Track dE/dx [MeV/cm]", true,"d"));
		//vars.push_back(bdt_variable("track_info.reco_track_end_mean_dEdx[0]", "(52,0,12)","Mean End Track dE/dx", true,"d"));
		//vars.push_back(bdt_variable("track_info.reco_track_start_mean_dEdx[0]", "(52,0,12)","Mean Start Track dE/dx", true,"d"));
		vars.push_back(bdt_variable("track_info.reco_track_start_mean_dEdx[0]/track_info.reco_track_end_mean_dEdx[0]", "(52,0,3.5)","Ratio of Mean Start/End Track dE/dx", true,"d"));
		vars.push_back(bdt_variable("track_info.reco_track_PIDA[0]","(52,0,24)","Track PIDA",true,"d"));
		vars.push_back(bdt_variable("track_info.reco_track_braggA[0]","(52,0,25)","Track Bragg A",true,"d"));

		vars.push_back(bdt_variable("shortest_asso_shower_to_vert_dist","(52,0,30)","Photon Conversion Length from Reconstructed Vertex [cm]" ,false,"d"));

		vars.push_back(bdt_variable(invariant_mass,"(52,1,2.5)","Invariant Mass",true,"d"));
	
		vars.push_back(bdt_variable("(atan2(reco_track_diry[0],reco_track_dirz[0]))","(52,-3.14159,3.14159)","Reconstructed Track -  Theta", true,"d"));
		vars.push_back(bdt_variable("(atan2(reco_track_diry[0],reco_track_dirx[0]))","(52,-3.14159,3.14159)","Reconstructed Track -  Phi", true,"d"));

	//	vars.push_back(bdt_variable("cos(atan2(reco_track_diry[0],reco_track_dirz[0]))","(52,-1,1)","Reconstructed Track - Cosine Theta", true,"d"));
	//	vars.push_back(bdt_variable("cos(atan2(reco_track_diry[0],reco_track_dirx[0]))","(52,-1,1)","Reconstructed Track - Cosine Phi", true,"d"));

		vars.push_back(bdt_variable(angle_track_shower,	"(52,-1,1)","Cosine Track-Shower Angle ",true,"d"));

		vars.push_back(bdt_variable("reco_track_endx[0]","(52,0,250)"," Reconstructed Track End X pos [cm]",false,"d"));
		vars.push_back(bdt_variable("reco_track_endy[0]","(52,-110,+110)","Reconstructed Track End Y pos [cm]",false,"d"));
		vars.push_back(bdt_variable("reco_track_endz[0]","(52,0,1050)","Reconstructed Track End Z pos [cm]",false,"d"));

		//NEW variables for tracks
		vars.push_back(bdt_variable("reco_track_energy_new_legacy[0]", "(52,0,1)","Track Energy (Best Plane)",true, "d"));
		vars.push_back(bdt_variable("track_info.reco_track_kinetic[0]","(52,0,1)","Reconstructed Track Kinetic Energy (Calo) [cm]", true,"d"));
		vars.push_back(bdt_variable("track_info.reco_track_kinetic_from_length[0]","(52,0,1)","Reconstructed Track Kinetic Energy (Length) [cm]", true,"d"));
		vars.push_back(bdt_variable("track_info.reco_track_kinetic_from_length[0]/track_info.reco_track_kinetic[0]","(52,0,5)","Ratio of Reco Track Kinetic (Length/Calo)", true,"d"));


	}

	if(mode_option != "train" && mode_option != "app"){
		//We don't want to train using these variables, but we would like to plot them just to see how things are

	vars.push_back(bdt_variable("pi0_info.num_reco_showers_within_10cm_vertex","(10,0,10)","Num Showers within 10cm",false,"i"));
	vars.push_back(bdt_variable("pi0_info.num_reco_showers_within_20cm_vertex","(10,0,10)","Num Showers within 20cm",false,"i"));
	vars.push_back(bdt_variable("pi0_info.num_reco_showers_within_30cm_vertex","(10,0,10)","Num Showers within 30cm",false,"i"));

	vars.push_back(bdt_variable("pi0_info.num_reco_tracks_within_10cm_vertex","(10,0,10)","Num tracks within 10cm",false,"i"));
	vars.push_back(bdt_variable("pi0_info.num_reco_tracks_within_20cm_vertex","(10,0,10)","Num tracks within 20cm",false,"i"));
	vars.push_back(bdt_variable("pi0_info.num_reco_tracks_within_30cm_vertex","(10,0,10)","Num tracks within 30cm",false,"i"));

	vars.push_back(bdt_variable("pi0_info.num_reco_showers_bp_within_10cm_vertex","(10,0,10)","Num Showers bp within 10cm",false,"i"));
	vars.push_back(bdt_variable("pi0_info.num_reco_showers_bp_within_20cm_vertex","(10,0,10)","Num Showers bp within 20cm",false,"i"));
	vars.push_back(bdt_variable("pi0_info.num_reco_showers_bp_within_30cm_vertex","(10,0,10)","Num Showers bp within 30cm",false,"i"));

	vars.push_back(bdt_variable("pi0_info.num_reco_tracks_bp_within_10cm_vertex","(10,0,10)","Num tracks bp within 10cm",false,"i"));
	vars.push_back(bdt_variable("pi0_info.num_reco_tracks_bp_within_20cm_vertex","(10,0,10)","Num tracks bp within 20cm",false,"i"));
	vars.push_back(bdt_variable("pi0_info.num_reco_tracks_bp_within_30cm_vertex","(10,0,10)","Num tracks bp within 30cm",false,"i"));





		if(false && mode_option != "train"){
			//We don't want to train using these variables, but we would like to plot them just to see how things are
			//Hmm. This is needed in track..	
			vars.push_back(bdt_variable("track_info.reco_track_range[0]","(52,0,150)","Reconstructed Track Range [cm]", true,"d"));
			vars.push_back(bdt_variable("reco_track_energy_new[0][0]", "(52,0,2)","Track Energy (New -Plane 0)",true, "d"));
			vars.push_back(bdt_variable("reco_track_energy_new[0][1]", "(52,0,2)","Track Energy (New -Plane 1)",true, "d"));
			vars.push_back(bdt_variable("reco_track_energy_new[0][2]", "(52,0,2)","Track Energy (New -Plane 2)",true, "d"));

			vars.push_back(bdt_variable("reco_track_energy_from_dEdx[0][0]", "(52,0,2)","Track Energy (dEdx - Plane 0)",true, "d"));
			vars.push_back(bdt_variable("reco_track_energy_from_dEdx[0][1]", "(52,0,2)","Track Energy (dEdx - Plane 1)",true, "d"));
			vars.push_back(bdt_variable("reco_track_energy_from_dEdx[0][2]", "(52,0,2)","Track Energy (dEdx - Plane 2)",true, "d"));

			vars.push_back(bdt_variable("reco_track_energy[0]", "(52,0,2)","Track Energy (0ld)",true, "d"));
			vars.push_back(bdt_variable("totalpe_ibg_sum","(52,0,15000)","Total in Beam-Gate PE",false,"d"));
			vars.push_back(bdt_variable("totalpe_bbg_sum","(52,0,15000)","Total Before Beam-Gate PE",false,"d"));
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
	if(false && mode_option == "stack"){
		vars2.push_back(bdt_variable("reco_shower_startx[0]","(52,0,250)"," Shower Start X pos [cm]",false,"d"));
		vars2.push_back(bdt_variable("reco_shower_starty[0]","(52,-110,+110)","Shower Start Y pos [cm]",false,"d"));
		vars2.push_back(bdt_variable("reco_shower_startz[0]","(52,0,1050)","Shower Start Z pos [cm]",false,"d"));

		vars2.push_back(bdt_variable("reco_shower_startx[0] + reco_shower_dirx[0]*reco_shower_length[0]","(52,0,250)"," Shower End X pos [cm]",false,"d"));
		vars2.push_back(bdt_variable("reco_shower_starty[0] + reco_shower_diry[0]*reco_shower_length[0]","(52,-110,+110)","Shower End Y pos [cm]",false,"d"));
		vars2.push_back(bdt_variable("reco_shower_startz[0] + reco_shower_dirz[0]*reco_shower_length[0]","(52,0,1050)","Shower End Z pos [cm]",false,"d"));

	}


	if(mode_option == "recomc"){

		vars2.push_back(bdt_variable("reco_true_nuvert_dist","(50,0,450)","Distance from Reco to Truth Vertex [cm]",false,"d"));

	}	


	std::vector<bdt_variable> public_vars = {vars.at(0), vars.at(1), vars.at(6)};
	if(istrack=="track") public_vars.push_back(vars.at(17));

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

	//OLD 
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

	//NEW track direction fix ones
	//Best Fit Significance: 0.540882 0.527059 1.01866
	//Best Fit Significance: 0.564 0.521 1.63638

	/*
	 *************************** TrackOnly *********************
	 Best is at track_cosmicut: 0.564 track_bnbcut: 0.521
	 track sig: 11.3775 track bkg 48.3415 1.63639
	 *************************** NoTrackOnly *********************
	 Best is at notrack_cosmicut: 0.540882 notrack_bnbcut: 0.527059
	 notrack sig: 19.4657 notrack bkg 365.157 1.01866
	 *************************** Combined *********************
	 Best is at track_cosmicut: 0.564 track_bnbcut: 0.516
	 Best is at notrack.g_cosmicut: 0.551176 notrack_bnbcut: 0.538824
	 track sig: 16.7411 track bkg 114.767 1.5627
	 notrack sig: 7.71812 notrack bkg 88.0266 0.82263
Combined: 1.71757 with sig 24.4592 202.794 s/sqrtb 1.71757

*/
	//Running on ext
	//TRACK: 0.518333  #signal: 14.5031 #bkg: 84.5341 ||  bnb: 81.4658 cos: 3.06832 || 1.5774


	double fcoscut;
	double fbnbcut;
	if(istrack == "track"){
//0.554 bcut: 0.534
		fcoscut = 0.554;
		fbnbcut = 0.534;

	}else if(istrack == "notrack"){
		fcoscut = 0.556667;
		fbnbcut = 0.536667;
		//Best Fit Significance: 0.556667 0.536667 0.924408

	}

	if(mode_option != "precalc" && mode_option != "train" && mode_option != "app" && mode_option != "sig"){
		for(auto &f: bdt_files){
			std::cout<<"Filling BDTList on File "<<f->tag<<std::endl;
			f->calcCosmicBDTEntryList(fcoscut, fbnbcut);
			f->calcBNBBDTEntryList(fcoscut, fbnbcut);
		}
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
		return 0;



	}else if(mode_option == "app"){

		//Apply! This will update cosmic_bdt_info, signal file and bkg file. As in update them PROPERLY!	
		std::vector<bdt_file*> app_files = {bnbext}; 
		if(run_cosmic) bdt_app(cosmic_bdt_info, bdt_files, vars, TMVAmethods);
		if(run_bnb)    bdt_app(bnb_bdt_info, bdt_files, vars, TMVAmethods);

		return 0;
	}
	else if(mode_option == "response"){

		TFile * ftest = new TFile(("test+"+istrack+".root").c_str(),"recreate");
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

		std::vector<int> recomc_cols = {kRed-7, kBlue+3, kBlue, kBlue-7, kMagenta-3, kYellow-7, kOrange-3, kGreen+1 ,kGray};
		std::vector<std::string> recomc_names = {"NC #Delta Radiative #gamma", "CC #pi^{0} #rightarrow #gamma", "NC #pi^{0} #rightarrow #gamma","Non #pi^{0} #gamma","Intrinsic #nu_{e} electron","BNB Michel e^{#pm}","BNB Other","Cosmic Michel e^{#pm}", "Cosmic Other"};

		std::string  nue = "abs(true_shower_pdg[0]) ==11 && abs(nu_pdg)==12 && (exiting_electron_number==1 || exiting_antielectron_number==1)";
		std::string  michel = "abs(true_shower_pdg[0]) ==11 && abs(true_shower_parent_pdg[0])==13";
		std::vector<std::string> recomc_cuts = {
			"true_shower_origin[0]==1 && true_shower_pdg[0] == 22 && true_shower_parent_pdg[0] !=111 && is_delta_rad ==1 ",
			"true_shower_pdg[0] == 22 && true_shower_parent_pdg[0] == 111 && true_shower_origin[0]==1 && ccnc==0",
			"true_shower_pdg[0] == 22 && true_shower_parent_pdg[0] == 111 && true_shower_origin[0]==1 && ccnc==1",
			"true_shower_pdg[0] == 22 && true_shower_parent_pdg[0] != 111 && is_delta_rad!=1 && true_shower_origin[0]==1",
			"true_shower_origin[0] ==1 && "+ nue,
			"true_shower_origin[0] ==1 && "+ michel,
			"true_shower_origin[0]==1 && true_shower_pdg[0]!=22 &&  (( abs(true_shower_pdg[0])!=11)  ||( abs(true_shower_pdg[0])==11 && !(abs(nu_pdg)==12 && (exiting_electron_number==1 || exiting_antielectron_number==1)) &&!(abs(true_shower_parent_pdg[0])==13)    ))     ",
			"true_shower_origin[0] ==2 && abs(true_shower_parent_pdg[0])==13",
			"true_shower_origin[0] ==2 && abs(true_shower_parent_pdg[0])!=13"
		};

		bdt_recomc recomc(recomc_names, recomc_cuts, recomc_cols,istrack);

		TFile * ftest = new TFile(("test+"+istrack+".root").c_str(),"recreate");
		if(!response_only){
			int h=0;

			if(number !=-1){
				recomc.plot_recomc(ftest, bnb_cosmics, vars.at(number), fcoscut, fbnbcut);
				return 0;

			}else{

				for(auto &v:vars){
					recomc.plot_recomc(ftest, bnb_cosmics, v, fcoscut, fbnbcut);
					h++;
				}
			}	
		}

		if(true){
			recomc.is_log = true;
			if(run_cosmic){
				recomc.plot_recomc(ftest, signal_cosmics, signal_cosmics->getBDTVariable(cosmic_bdt_info) , fcoscut,fbnbcut);
				recomc.plot_recomc(ftest, bnb_cosmics, bnb_cosmics->getBDTVariable(cosmic_bdt_info) , fcoscut,fbnbcut);
			}
			if(run_bnb){
				recomc.plot_recomc(ftest, bnb_cosmics, bnb_cosmics->getBDTVariable(bnb_bdt_info) , fcoscut,fbnbcut);
				recomc.plot_recomc(ftest, signal_cosmics, signal_cosmics->getBDTVariable(bnb_bdt_info) , fcoscut,fbnbcut);
			}

			recomc.is_log = false;
		}

	}
	else if(mode_option == "sig"){


		TFile *fsig = new TFile(("significance_"+istrack+".root").c_str(),"recreate");
		std::vector<double> ans = scan_significance(fsig, {signal_cosmics} , {bnb_cosmics, bnbext}, cosmic_bdt_info, bnb_bdt_info);
		//std::vector<double> ans = lin_scan({signal_cosmics}, {bnb_cosmics, bnbext}, cosmic_bdt_info, bnb_bdt_info,fcoscut,fbnbcut);

		std::cout<<"Best Fit Significance: "<<ans.at(0)<<" "<<ans.at(1)<<" "<<ans.at(2)<<std::endl;
		fsig->Close();


	}else if(mode_option == "stack"){
		bdt_stack histogram_stack(istrack+"_stack");
		histogram_stack.addToStack(signal_cosmics);
		histogram_stack.addToStack(bnb_cosmics);

		//Add bnbext but change the color and style first
		bnbext->col = intime->col;	
		bnbext->fillstyle = 3333;
		histogram_stack.addToStack(bnbext);

		TFile * ftest = new TFile(("test+"+istrack+".root").c_str(),"recreate");
		int ip=0;

		if(number == -1){
			for(auto &v:vars){
				ip++;
				histogram_stack.plotStacks(ftest,  v ,fcoscut,fbnbcut);
			}
		}else{
			std::cout<<"Starting to make a stack of : "<<vars.at(number).name<<std::endl;
			histogram_stack.plotStacks(ftest,  vars.at(number) ,fcoscut,fbnbcut);
			return 0;
		}

		histogram_stack.plotBDTStacks(ftest, bnb_bdt_info, fcoscut, fbnbcut);
		histogram_stack.plotBDTStacks(ftest, cosmic_bdt_info, fcoscut, fbnbcut);
		return 0;

	}else if(mode_option == "datamc"){

		TFile * ftest = new TFile(("test+"+istrack+".root").c_str(),"recreate");
		//Obsolete
		/*bdt_stack *obs = new bdt_stack(istrack+"_olddatamc");
		  obs->plot_pot =4.801e19;
		  obs->addToStack(signal_cosmics);
		  obs->addToStack(bnb_cosmics);
		  obs->addToStack(intime);*/

		bdt_stack *cosmic_stack = new bdt_stack(istrack+"_extintime");
		cosmic_stack->plot_pot = 4.801e19;
		cosmic_stack->addToStack(intime);

		bdt_stack *histogram_stack = new bdt_stack(istrack+"_datamc");
		histogram_stack->plot_pot = 4.801e19;
		histogram_stack->addToStack(signal_cosmics);
		histogram_stack->addToStack(bnb_cosmics);
		bnbext->col = intime->col;	
		bnbext->fillstyle = 3333;
		histogram_stack->addToStack(bnbext);

		int ip=0;


		if(!response_only){
			if(number != -1){
				bdt_datamc datamc(data5e19, histogram_stack, istrack+"_extintime");	
				datamc.plotStacks(ftest,  vars.at(number) ,fcoscut,fbnbcut);
			}else{


				for(auto &v:vars){
					ip++;
					//bdt_datamc cosmic_datamc(bnbext, cosmic_stack, istrack);	
					//cosmic_datamc.plotStacks(ftest,  v,fcoscut,fbnbcut);
					bdt_datamc real_datamc(data5e19, histogram_stack, istrack+"_datamc");	
					real_datamc.plotStacks(ftest, v,fcoscut,fbnbcut);
				}
			}
		}else{
			//bdt_datamc cosmic_datamc(bnbext, cosmic_stack, istrack+"_extintime");	
			bdt_datamc real_datamc(data5e19, histogram_stack, istrack+"_datamc");	

			real_datamc.plotBDTStacks(ftest, bnb_bdt_info ,fcoscut,fbnbcut);
			real_datamc.plotBDTStacks(ftest, cosmic_bdt_info ,fcoscut,fbnbcut);
		}

	}else if(mode_option == "vars"){

		std::vector<std::string> title = {"All Verticies","Pre-Selection Cuts"};

		if(run_cosmic){

			if(number != -1){
				plot_bdt_variable(signal_pure, intime, vars.at(number), cosmic_bdt_info);
			}else{
				plot_bdt_variables(signal_pure, intime, vars, cosmic_bdt_info);
			}

		}
		if(run_bnb){


			if(number != -1){
				plot_bdt_variable(signal_pure, bnb_pure, vars.at(number), bnb_bdt_info);
			}else{
				plot_bdt_variables(signal_pure, bnb_pure, vars, bnb_bdt_info);
			}

		}




	} else if(mode_option == "eff"){

		double plot_pot = 6.6e20;
		std::cout<<"Starting efficiency study: coscut @ "<<fcoscut<<" bnbcut@: "<<fbnbcut<<std::endl;

		std::cout<<"########################################################################"<<std::endl;
		std::cout<<"###############################Efficiency###############################"<<std::endl;


		TFile * feff = new TFile(("Eff_"+istrack+".root").c_str(),"recreate");
		TCanvas *ceff = new TCanvas();		
		ceff->cd();

		std::vector<double> eff_sig;

		//std::vector<bdt_file*> eff_files = {signal_pure, bnb_cosmics, bnbext};
		std::vector<bdt_file*> eff_files = {signal_cosmics, bnb_cosmics, bnbext};
		std::vector<std::vector<double>> effs;
		std::vector<std::vector<double>> pres;

		for(int i=0; i< eff_files.size(); i++){
			std::vector<double> tmp;
			effs.push_back(tmp);
			pres.push_back(tmp);

			std::cout<<eff_files.at(i)->tag<<std::endl;
			std::vector<double> prev;
			double start;
			double sel;		

			double pot_scale = (plot_pot/eff_files.at(i)->pot )*eff_files.at(i)->scale_data;
			std::string gencut = "1";
			if(eff_files.at(i)->tag == "IntimeCosmics" || eff_files.at(i)->tag == "Data5e19" || eff_files.at(i)->tag == "BNBext"){
				gencut = "1";
			}	

			double nevents = eff_files.at(i)->numberofevents*pot_scale;	
			double nv = eff_files.at(i)->GetEntries((gencut+"&&1").c_str())*pot_scale;	
			double ns = eff_files.at(i)->GetEntries( eff_files.at(i)->getStageCuts(0,fcoscut,fbnbcut).c_str())*pot_scale;	

			start = nevents;
			sel = ns;
			std::cout<<"Stage Gen\t\t"<<nevents<<"\t\t"<<nevents/start*100<<std::endl;
			std::cout<<"Stage Vertex\t\t"<<nv<<"\t\t"<<nv/start*100<<std::endl;
			std::cout<<"Stage Topo\t\t"<<ns<<"\t\t"<<ns/start*100<<"\t\t"<<100<<std::endl;

			if(eff_files.at(i)->tag=="NCDeltaRadCosmics"){
				eff_sig.push_back(nevents/start*100);
				eff_sig.push_back(ns/start*100);
			}

			effs.back().push_back(nevents);
			effs.back().push_back(nv);
			effs.back().push_back(ns);

			for(int m=0; m< eff_files.at(i)->flow.vec_pre_cuts.size(); m++){
				std::string thiscut = eff_files.at(i)->flow.vec_pre_cuts.at(m)+"&&"+eff_files.at(i)->getStageCuts(0,fcoscut,fbnbcut);
				double np = eff_files.at(i)->GetEntries(thiscut.c_str())*pot_scale;
				std::cout<<"Precut: "<<eff_files.at(i)->flow.vec_pre_cuts.at(m)<<"\t||\t"<<np<<"\t("<<np/ns*100<<")\%"<<std::endl;
				pres.back().push_back(np);
			}

			std::cout<<"One by One"<<std::endl;
			std::string thiscut = eff_files.at(i)->getStageCuts(0,fcoscut,fbnbcut);
			for(int m=0; m< eff_files.at(i)->flow.vec_pre_cuts.size(); m++){
				thiscut += "&&"+ eff_files.at(i)->flow.vec_pre_cuts.at(m);
				double np = eff_files.at(i)->GetEntries(thiscut.c_str())*pot_scale;
				std::cout<<" + "<<eff_files.at(i)->flow.vec_pre_cuts.at(m)<<"\t||\t"<<np<<"\t("<<np/ns*100<<")\%"<<std::endl;

				if(eff_files.at(i)->tag=="NCDeltaRadCosmics"){
					eff_sig.push_back(np/start*100);
				}
				effs.back().push_back(np);

			}

			if(eff_files.at(i)->tag=="NCDeltaRadCosmics"){
				eff_sig.push_back(  eff_files.at(i)->GetEntries(eff_files.at(i)->getStageCuts(2,fcoscut,fbnbcut).c_str())*pot_scale/start*100);
				eff_sig.push_back(  eff_files.at(i)->GetEntries(eff_files.at(i)->getStageCuts(3,fcoscut,fbnbcut).c_str())*pot_scale/start*100);
			}


			for(int j=1; j <4; j++){		
				std::string thiscut = eff_files.at(i)->getStageCuts(j,fcoscut,fbnbcut); 
				double nvert =eff_files.at(i)->GetEntries(thiscut.c_str())*pot_scale;
				double eff =0;
				//	if(j==0){
				//		start = nvert;
				//		
				//	}

				eff = nvert/start*100;
				effs.back().push_back(nvert);
				std::cout<<nvert<<"\t("<<nvert/sel*100<<" \%)"<<" ";	
			}

			std::cout<<std::endl;

			//break;
		}


		//pe_cut, fiducial_cut, track_length_cut, min_energy_cut, min_conversion_cut, good_calo_cut, track_direction_cut, back_to_back_cut
		std::vector<std::string> sname;


		if(istrack == "notrack"){
			sname = {"Generated","Vertexed","Topological","Total PE $>20$","Fiducial cut","Reco $E_{\\gamma}$ $> 30$MeV"};
		}else{
			sname = {"Generated","Vertexed","Topological","Total PE $>20$","Fiducial cut","Track $< 100$cm","Reco $E_{\\gamma}$ $> 30$MeV","Shower gap $> 1$cm","Good calo cut","Flipped track cut","Back-to-back cut"};
		}
		//	for(int i=0; i< pres.back().size(); i++){
		//		sname.push_back( std::to_string(i));
		//	}
		sname.push_back("All Precuts");
		sname.push_back("Cosmic BDT");
		sname.push_back("BNB BDT");

		std::cout<<"\\begin{table}[h!]"<<std::endl;
		std::cout<<"\\centering"<<std::endl;
		std::cout<<"\\begin{tabular}{|l||c|c||c|c||c|c|}"<<std::endl;
		std::cout<<"\\hline"<<std::endl;
		for(int i=0; i< effs.back().size();i++){
			std::cout<<sname.at(i);
			for(int j=0; j< eff_files.size(); j++){

				if(i>2 && i < 3+pres.back().size() ){
					double num = effs.at(j).at(i);
					double per  = num/effs.at(j).at(0)*100.0;
					double single_per = pres.at(j).at(i-3)/effs.at(j).at(2)*100.0;

					//std::cout<<"&"<<to_string_prec(num,1)<<"&"<<to_string_prec(per,1)<<"\\\%&("<<to_string_prec(single_per,1)<<"\\\%)";
					std::cout<<"&"<<to_string_prec(num,1)<<"&"<<to_string_prec(per,1)<<"\\\% ("<<to_string_prec(single_per,1)<<"\\\%)";

				}else{
					//std::cout<<"&"<<to_string_prec(effs.at(j).at(i),1)<<"&"<<to_string_prec(effs.at(j).at(i)/effs.at(j).at(0)*100.0,2)<<"\\\%&";
					std::cout<<"&"<<to_string_prec(effs.at(j).at(i),1)<<"&"<<to_string_prec(effs.at(j).at(i)/effs.at(j).at(0)*100.0,2)<<"\\\%";
				}


			}
			std::cout<<"\\\\"<<std::endl;
			if(i==2 || (effs.back().size()-i) == 4)std::cout<<"\\hline"<<std::endl;

		}	
		std::cout<<"\\hline"<<std::endl;

		std::cout<<"\\end{tabular}"<<std::endl;
		std::cout<<"\\end{table}"<<std::endl;










		//END********************************************



		feff->cd();			
		std::vector<double> eff_num;
		for(int i=0; i< eff_sig.size(); i++){ eff_num.push_back((double)i);}
		std::reverse(eff_sig.begin(),eff_sig.end());
		TGraph *g_eff_sig = new TGraph(eff_sig.size(),&eff_sig[0],&eff_num[0]);

		ceff->cd();
		g_eff_sig->Draw("alp");
		ceff->Write();

		feff->Close();


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
			double nv = data_files.at(i)->GetEntries((gencut+"&&1").c_str())*pot_scale;	
			double ns = data_files.at(i)->GetEntries((gencut+"&&  reco_asso_showers==1 && reco_asso_tracks "+num_track_cut).c_str())*pot_scale;	

			start = nevents;
			sel = ns;
			std::cout<<"Stage G\t\t"<<nevents<<"\t\t"<<nevents/start*100<<std::endl;
			std::cout<<"Stage V\t\t"<<nv<<"\t\t"<<nv/start*100<<std::endl;
			std::cout<<"Stage S\t\t"<<ns<<"\t\t"<<ns/start*100<<"\t\t"<<100<<std::endl;

			for(int j=0; j <4; j++){		
				std::string thiscut = data_files.at(i)->getStageCuts(j,fcoscut,fbnbcut); 
				double nvert =data_files.at(i)->GetEntries(thiscut.c_str())*pot_scale;
				double eff =2;
				eff = nvert/start*100;
				std::cout<<"Stage "<<std::to_string(j)<<"\t\t"<<nvert<<"\t\t"<<eff<<"\t\t"<<nvert/sel*100<<std::endl;	
				if(j==3){data_files.at(i)->tvertex->Scan( "run_number:subrun_number:event_number:reco_shower_dedx_plane2",thiscut.c_str());};
			}
		}


	} else if(mode_option == "precalc"){

		std::vector<bdt_file*> bdt_filesA = {bnb_pure};
		std::vector<bdt_file*> bdt_filesB = {data5e19, bnbext};
		std::vector<bdt_file*> bdt_filesC = {lee};

		for(auto &f: bdt_filesC){
			bdt_precalc pre(f);
		
		if(number==1)	pre.genBNBcorrectionInfo();
		if(number==1)	pre.genPi0Info();
		if(number==1)	pre.genShowerInfo();
		if(number==1)	pre.genTrackInfo();
		}
	}
	else if(mode_option == "sbnfit"){

		bdt_stack *obs = new bdt_stack(istrack+"_datamc");
		obs->plot_pot =  6.6e20;
		//obs->plot_pot =  4.801e19;
		obs->addToStack(signal_cosmics);
		obs->addToStack(bnb_cosmics);
		obs->addToStack(intime);
		obs->addToStack(data5e19);

		std::vector<std::string> hnam = {"nu_uBooNE_singlephoton_signal","nu_uBooNE_singlephoton_bkg","nu_uBooNE_singlephoton_intime","nu_uBooNE_singlephoton_data"};
		//std::vector<std::string> hnam = {"nu_uBooNE_singlephoton_signal","nu_uBooNE_singlephoton_bkg","nu_uBooNE_singlephoton_intime"};
		obs->makeSBNspec("test", vars.at(1), fcoscut, fbnbcut,hnam );






	}else if( mode_option =="test"){

		signal_cosmics->writeStageFriendTree("stage_friend.root", fcoscut, fbnbcut);


		return 0;


		std::vector<int> recomc_cols = {kRed-7, kBlue+3, kBlue, kBlue-7, kMagenta-3, kYellow-7, kOrange-3, kGreen+1 ,kGray};
		std::vector<std::string> recomc_names = {"NC #Delta Radiative #gamma", "CC #pi^{0} #rightarrow #gamma", "NC #pi^{0} #rightarrow #gamma","Non #pi^{0} #gamma","Intrinsic #nu_{e} electron","BNB Michel e^{#pm}","BNB Other","Cosmic Michel e^{#pm}", "Cosmic Other"};


		std::string  nue = "abs(true_shower_pdg[0]) ==11 && abs(nu_pdg)==12 && (exiting_electron_number==1 || exiting_antielectron_number==1)";
		std::string  michel = "abs(true_shower_pdg[0]) ==11 && abs(true_shower_parent_pdg[0])==13";
		std::vector<std::string> recomc_cuts = {
			"true_shower_origin[0]==1 && true_shower_pdg[0] == 22 && true_shower_parent_pdg[0] !=111 && is_delta_rad ==1 ",
			"true_shower_pdg[0] == 22 && true_shower_parent_pdg[0] == 111 && true_shower_origin[0]==1 && ccnc==0",
			"true_shower_pdg[0] == 22 && true_shower_parent_pdg[0] == 111 && true_shower_origin[0]==1 && ccnc==1",
			"true_shower_pdg[0] == 22 && true_shower_parent_pdg[0] != 111 && is_delta_rad!=1 && true_shower_origin[0]==1",
			"true_shower_origin[0] ==1 && "+ nue,
			"true_shower_origin[0] ==1 && "+ michel,
			"true_shower_origin[0]==1 && true_shower_pdg[0]!=22 &&  (( abs(true_shower_pdg[0])!=11)  ||( abs(true_shower_pdg[0])==11 && !(abs(nu_pdg)==12 && (exiting_electron_number==1 || exiting_antielectron_number==1)) &&!(abs(true_shower_parent_pdg[0])==13)    ))     ",
			"true_shower_origin[0] ==2 && abs(true_shower_parent_pdg[0])==13",
			"true_shower_origin[0] ==2 && abs(true_shower_parent_pdg[0])!=13"
		};

		bdt_recomc test(recomc_names, recomc_cuts, recomc_cols,istrack);


		TFile * ftest = new TFile(("test+"+istrack+".root").c_str(),"recreate");
		test.plot_recomc(ftest, bnb_cosmics, vars.at(1) , fcoscut,fbnbcut);

		return 0;	
		bnb_cosmics->tvertex->Scan("run_number:subrun_number:event_number:reco_shower_dedx_plane2[0]:reco_shower_helper_energy[0]:reco_track_displacement[0]:shortest_asso_shower_to_vert_dist:BNBCosmics_bnb_track.mva", bnb_cosmics->getStageCuts(3,fcoscut,fbnbcut).c_str()  );
		return 0;




	}else if(mode_option == "boxcut"){
		TFile * fout = new TFile("box.root","recreate");

		std::vector<std::string> boxcuts;

		boxcuts.push_back("reco_shower_dedx_plane2[0]>2"); // reco_shower_dedx_plane2[0]
		boxcuts.push_back("reco_shower_helper_energy[0]>0.2"); // reco_shower_helper_energy[0]
		boxcuts.push_back("1"); // reco_shower_length[0]
		boxcuts.push_back("1"); // reco_shower_opening_angle[0]
		boxcuts.push_back("reco_shower_dist_to_closest_flashzcenter[0]<100"); // reco_shower_dist_to_closest_flashzcenter[0e	
		boxcuts.push_back(fiducial_vertex); // reco_nu_vtx_dust to closset wal
		boxcuts.push_back("1"); // showe bp to wall
		boxcuts.push_back("1"); // vrtx X
		boxcuts.push_back("1"); // vrt Y
		boxcuts.push_back("1"); //vert z
		boxcuts.push_back("cos(atan2(reco_shower_diry[0],reco_shower_dirz[0]))> 0.6"); // shower cos theta
		boxcuts.push_back("cos(atan2(reco_shower_diry[0],reco_shower_dirx[0]))> 0.6"); // shower cos phi
		boxcuts.push_back("1"); //shower end x
		boxcuts.push_back("1"); // shower end y
		boxcuts.push_back("1"); // showr end z
		boxcuts.push_back("1"); //pi0num near 
		boxcuts.push_back("reco_track_displacement[0]<100"); //  track length
		boxcuts.push_back("track_info.reco_track_mean_dEdx[0]>2"); // mean dedx
		boxcuts.push_back("track_info.reco_track_start_mean_dEdx[0]/track_info.reco_track_end_mean_dEdx[0]> 1"); //  ratio mean
		boxcuts.push_back("track_info.reco_track_PIDA[0] >  10"); // pida
		boxcuts.push_back("track_info.reco_track_braggA[0]> 10"); // brag A
		boxcuts.push_back("shortest_asso_shower_to_vert_dist[0]>1"); // photon conversion
		boxcuts.push_back("1"); //  invariant mass
		boxcuts.push_back("cos(atan2(reco_track_diry[0],reco_track_dirz[0]))>0"); // track theta
		boxcuts.push_back("1"); // track theta
		boxcuts.push_back(angle_track_shower+">-0.95 && "+ angle_track_shower +"<0.95"); //  track showr angle
		boxcuts.push_back("1"); // track end x
		boxcuts.push_back("1"); // track end y
		boxcuts.push_back("1"); //  track end z
		boxcuts.push_back("1"); // track eneryy

		bdt_stack *obs3 = new bdt_stack(istrack+"_box");
		obs3->plot_pot = 6.6e20;
		obs3->addToStack(signal_cosmics);
		obs3->addToStack(bnb_cosmics);
		bnbext->col = intime->col;	
		bnbext->fillstyle = 3333;
		obs3->addToStack(bnbext);



		bdt_boxcut BOX("boxcut", obs3, boxcuts, vars);
		BOX.core_cuts = "totalpe_ibg_sum > 20 && track_info.reco_track_good_calo[0]>0";
		BOX.doCuts(fout);







	}else {
		std::cout << "WARNING: " << mode_option << " is an invalid option\n";
	}


	return 0;

}
