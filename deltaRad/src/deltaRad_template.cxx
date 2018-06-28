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
#include "bdt_boxcut.h"
#include "bdt_spec.h"

#include <sys/stat.h> //for line 518 "Create a folder for pdf."


int main (int argc, char *argv[]){

	// Just some simple argument things
	//===========================================================================================

	//This is the default base directory that data files are stored in, shouldn't need to change unless we update the vertexing
	std::string dir = "/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_v2/"; //Detaulf directory of root files (MC).


	std::string mode_option = "fake"; 
	std::string xml = "default.xml";
	std::string run_tag = "template";
	TFile * ftest = new TFile(("test_"+run_tag+".root").c_str(),"recreate");


	bool run_cosmic = true;
	bool run_bnb = true;
	int number = 0;
	bool response_only = false;

	//All of this is just to load in command-line arguments, its not that important
	const struct option longopts[] = 
	{
		{"dir", 		required_argument, 	0, 'd'},
		{"option",		required_argument,	0, 'o'},
		{"xml"	,		required_argument,	0, 'x'},
		{"help",		required_argument,	0, 'h'},
		{"number",		required_argument,	0, 'n'},
		{"cosmic",		no_argument,		0, 'c'},
		{"bnb",			no_argument,		0, 'b'},
		{0,			no_argument, 		0,  0},
	};//./deltaRad --option === ./deltaRad.cxx -o


	int iarg = 0; opterr=1; int index;
	while(iarg != -1)
	{
		iarg = getopt_long(argc,argv, "cbx:o:d:n:rh?", longopts, &index);

		switch(iarg)
		{
			case 'r':
				response_only = true;
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
			case '?':
			case 'h':
				std::cout<<"Allowed arguments:"<<std::endl;
				std::cout<<"\t-d\t--dir\t\tDirectory for file inputs"<<std::endl;
				std::cout<<"\t-o\t--option\t\tWhich mode to run the program in: Available options are;"<<std::endl;
				std::cout<<" --option train "<<std::endl;
				std::cout<<"\t\t Train either (if --cosmic or --bnb is flagged) or both (default) the BDTs using the input.xml "<<std::endl;
				std::cout<<" --option app "<<std::endl;
				std::cout<<"\t\t Apply the trained BDT to all the files in bdt_files"<<std::endl;
				std::cout<<" --option response "<<std::endl;
				std::cout<<"\t\t Prints out the responses of the trained BDT's on the training samples"<<std::endl;
	
				std::cout<<"-------------------------------------------------------"<<std::endl;
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
	MVALoader xml_methods(xml);//multiple variable analysis MVA;  
	std::vector<method_struct> TMVAmethods  = xml_methods.GetMethods(); //./deltaRad --xml default.xml   read the "variabls" given in the files. default.xml is the default one.

	//A couple of variable definitions for convienance
	std::string angle_track_shower ="(reco_track_dirx[0]*reco_shower_dirx[0]+reco_track_diry[0]*reco_shower_diry[0]+reco_track_dirz[0]*reco_shower_dirz[0])";
	std::string fiducial_vertex = "reco_nuvertx > 10 && reco_nuvertx < 246 && reco_nuverty > -107 && reco_nuverty < 107.01 && reco_nuvertz > 10 && reco_nuvertz < 1026 ";
	std::string fiducial_shower_end ="reco_shower_endx > 10 && reco_shower_endx < 246 && reco_shower_endy > -107 && reco_shower_endy < 107.01 && reco_shower_endz > 10 && reco_shower_endz < 1026 ";
	std::string fiducial_track_end = "reco_track_endx > 10 && reco_track_endx < 246 && reco_track_endy > -107 && reco_track_endy < 107.01 && reco_track_endz > 10 && reco_track_endz < 1026 ";
	std::string fiducial_cut = fiducial_vertex;//"&&"+fiducial_shower_end;
	
	std::string good_calo_cut = "track_info.reco_track_good_calo[0]>0 && track_info.reco_track_mean_dEdx[0] > 1";
	std::string track_length_cut = "reco_track_displacement[0]<100";
	std::string min_energy_cut = "reco_shower_helper_energy[0]>0.03";
	//std::string min_conversion_cut = "shortest_asso_shower_to_vert_dist > 1";
	std::string track_direction_cut = "track_info.reco_track_start_mean_dEdx[0]/track_info.reco_track_end_mean_dEdx[0] >= 0.925";
	std::string back_to_back_cut = angle_track_shower+" > -0.95 &&"  + angle_track_shower + "< 0.95";
	std::string pe_cut = "totalpe_ibg_sum > 20";


	//New pre-cut for single_electron selection.
	std::string min_conversion_cut = "shortest_asso_shower_to_vert_dist < 20";// 150 for including all events.


	//Set up some info about the BDTs to pass along; bdt_info is the type of variable. This stores information of the bdt: name, location, ariables... 

	bdt_info bnb_bdt_info("bnb_"+run_tag, "BNB focused BDT","(56,0.37,0.565)");
	bdt_info cosmic_bdt_info("cosmic_"+run_tag, "Cosmic focused BDT","(56,0.3,0.65)");

	bnb_bdt_info.setTopoName("1e1p");//name of the output plots.
	cosmic_bdt_info.setTopoName("1e1p");
	

	//Right now, for this example, we have 1 shower 1 track only
	std::string base_cuts = "reco_asso_showers == 1 && reco_asso_tracks == 1"; //1 shower and track in each vertex.
	std::string signal_definition = "((nu_pdg == 12 && lep_pdg == 11)||(nu_pdg == -12 && lep_pdg == -11))";//it means it select nu_e&e- event or anti-nu_e&e+ event.
	std::string background_definition = "abs(nu_pdg) != 12";//events that contains no nu_e or anti-nu_e

	//Train on "good" signals, defined as ones matched to the ncdelta and have little "clutter" around.	
	std::string true_signal    = "true_shower_origin[0]==1 && true_track_origin[0]==1";
	std::string true_bkg    = "true_shower_origin[0]==1 && true_track_origin[0]==1";

	//This is a vector each containing a precut, they are all added together to make the whole "precut"
	std::vector<std::string> vec_precuts;

	//We dont include the pe_cut for training
	if(mode_option != "train" && mode_option != "app" && mode_option !="vars" && mode_option!="response"){
		vec_precuts = {pe_cut, fiducial_cut, track_length_cut, min_energy_cut, min_conversion_cut, good_calo_cut, track_direction_cut, back_to_back_cut};
	}else{
		vec_precuts = {fiducial_cut, track_length_cut, min_energy_cut, min_conversion_cut, good_calo_cut, track_direction_cut, back_to_back_cut}; //see line 124-138
	}


	//We dont currently use postcuts,  but in theory we can here
	std::string postcuts = "1";//1 means pass, not doing it.





	//***************************************************************************************************/
	//***********	The bdt_flows define the "flow" of the analysis, i.e what cuts at what stage  *******/
	//***************************************************************************************************/
	// Pass in the following (base cuts, definition cuts, vector of precuts, postcuts, cosmid_bdt_info, bnb_bdt_info)
	// This flow corresponds to  
	// stage (0) just the basecuts and definition cuts; All Vertices
	// stage (1) After applying ALL precuts; Pre-selection
	// stage (2) After applying the BDT cosmic cut
	// stage (3) After applying the BDT BNB cuts
	// stage (4) After applying ALL postcuts
	bdt_flow signal_pure_flow(base_cuts, signal_definition +"&&"+ true_signal, vec_precuts,	postcuts, cosmic_bdt_info,	bnb_bdt_info);
	bdt_flow signal_flow(base_cuts, signal_definition , vec_precuts,	postcuts,cosmic_bdt_info,	bnb_bdt_info);
	bdt_flow cosmic_flow(base_cuts,"1", vec_precuts , postcuts,	cosmic_bdt_info,bnb_bdt_info);
	bdt_flow bkg_flow(base_cuts,background_definition, vec_precuts,postcuts,	cosmic_bdt_info,	bnb_bdt_info);
	bdt_flow bkg_pure_flow(base_cuts,background_definition+"&&"+ true_bkg ,vec_precuts,postcuts,	cosmic_bdt_info,	bnb_bdt_info);
	bdt_flow data_flow(base_cuts,"1",	vec_precuts, postcuts,	cosmic_bdt_info, 	bnb_bdt_info);
	

	//***************************************************************************************************/
	//***********	Each bdt_file correcponds to a single sample we want to run over	      *******/
	//***************************************************************************************************/
	// BDt files , bdt_file::bdt_file(std::string indir,std::string inname, std::string intag, std::string inops, std::string inrootdir, int incol, bdt_flow inflow) :
	// when creating, 
	// 1st argument, is the directory in which the file is located
	// 2nd argument, is the filename
	// 3rd argument, is a unique TAG that is used in saving information
	// 4th argument, is any plotting arguments for root plotting subrutines
	// 5th argument, is any internal directory in the rootfile; blank, nothing.
	// 6th argument, is a color (in roots TColor wheel) that it is plotted with
	// 7th argument, is the bdt_flow that the file g ; a function (cut) applied to each sample.
//IntrinsicNue change to IntrinsicNue
	bdt_file *signal_pure    = new bdt_file(dir, "vertexed_intrinsicnuecosmics_mcc8.9_fresh_v1.0.root",	"IntrinsicNue",	   "hist","",  kRed-7, signal_pure_flow);//change this for single electron, this one apply signal_pure_flow. label them as "NCDeltaRad".
	bdt_file *signal_cosmics = new bdt_file(dir, "vertexed_intrinsicnuecosmics_mcc8.9_fresh_v1.0.root", "IntrinsicNueCosmics", "hist","",  kRed-7, signal_flow);
	bdt_file *bnb_pure    = new bdt_file(dir, "vertexed_bnbcosmics_mcc8.9_fresh_v3.0.root", "BNBPure",	  "hist","",  kBlue-4, bkg_pure_flow);
	bdt_file *bnb_cosmics = new bdt_file(dir, "vertexed_bnbcosmics_mcc8.9_fresh_v3.0.root", "BNBCosmics", "hist","",  kBlue-4, bkg_flow);
	bdt_file *intime = new bdt_file(dir, "vertexed_intime_mcc8.9_fresh_v2.0.root" ,"IntimeCosmics","hist","", kGreen-3, cosmic_flow);
	bdt_file *data5e19    = new bdt_file(dir, "vertexed_data5e19_mcc8.9_fresh_v3.0.root",	"Data5e19",	   "E1p","",  kBlack, data_flow);
	bdt_file *bnbext    = new bdt_file(dir, "vertexed_bnbext_mcc8.9_fresh_v2.0.root",	"BNBext",	"E1p","",  kBlack, data_flow);


    
	//For conviencance fill a vector with pointers to all the files to loop over.
	std::vector<bdt_file*> bdt_files = {signal_cosmics, signal_pure, bnb_pure, bnb_cosmics, intime, data5e19, bnbext};


	//In plots, the bdt_file TAG will be used for labels/etc. Here we can add a "plotting name" which is a prettier string for plotting purposes only. If missing revert to tag
	signal_pure->addPlotName("CC #nu_e Intrinsics");//NAME the plot.
	signal_cosmics->addPlotName("LEE CC #nu_e Intrinsics w/ Corsika");
	bnb_pure->addPlotName("BNB Backgrounds");
	bnb_cosmics->addPlotName("BNB w/ Corsika");
	intime->addPlotName("Intime Corsika cosmics");
	data5e19->addPlotName("4.8e19 POT Data");
	bnbext->addPlotName("External BNB Data");


	//Just say which files are loaded and some brief data
	std::cout<<"--------------------------------------------------------------------------"<<std::endl;
	std::cout<<"--------------------------------------------------------------------------"<<std::endl;
	for(auto &f: bdt_files){
		std::cout<<"Loading "<<f->tag<<"\t with "<<f->tvertex->GetEntries()<<"\t verticies. (unweighted)"<<std::endl;
		std::cout<<"POT of file loaded is: "<<f->pot<<"\t\t "<<std::endl;
		std::cout<<"Scale factor is then: "<<f->scale_data<<std::endl;
	}
	std::cout<<"--------------------------------------------------------------------------"<<std::endl;
	std::cout<<"--------------------------------------------------------------------------"<<std::endl;


	//We need to add in any other precomputed friends here, as well as the app section (if training has already taken place)
	std::cout<<"--------------------------------------------------------------------------"<<std::endl;
	std::cout<<" Going to add any precomputed tree friends, or any trained BDT responses   "<<std::endl;
	std::cout<<" If you see warnings, but havenet yet ran app stage, thats ok!            "<<std::endl;
	std::cout<<"--------------------------------------------------------------------------"<<std::endl;
	if(mode_option != "precalc"){
		for(auto &f: bdt_files){
			addPreFriends(f,"track");
			addPreFriends(f,"pi0");
			addPreFriends(f,"bnbcorrection");
			f->addBDTResponses(cosmic_bdt_info, bnb_bdt_info, TMVAmethods);
		}
	}
	std::cout<<"--------------------------------------------------------------------------"<<std::endl;
	std::cout<<"--------------------------------------------------------------------------"<<std::endl;


	//Variables!
	//The order of the variables in the vector vars is important. Each variable information is stored in a class (see inc/bdt_variable.h) that is constructed with the following
	//bdt_variable(std::string inname, std::string inbin, std::string inunit,bool intrack, std::string intype) : 
	//1st argument, the variable as a string usable by root's TTrees
	//2nd argument, the binning with which to plot said variable (num bins, min, max)
	//3rd argument, a human readable label w/ units
	//4th argument, if the variable is track like only (a bit obsolete here)
	//5th argument, is the variable double "d" or integer "i"

	std::vector<bdt_variable> vars;

	//Just a few strings for simplier variable names
/*	std::string shower_mom = "reco_shower_helper_energy[0]"; 
	std::string reco_track_energy = "reco_track_energy_new_legacy[0]+0.938272"; //assuming a proton
	std::string track_mom  = "sqrt("+reco_track_energy+"*"+reco_track_energy+"-0.938272*0.938272)";
	std::string invariant_mass = "0.938272*0.938272+2.0*("+reco_track_energy+"*reco_shower_helper_energy[0]-"+track_mom+"*"+shower_mom+"*"+angle_track_shower+")";//of proton photon pair
*/

	vars.push_back(bdt_variable("reco_shower_dedx_plane2[0]","(52,0,7)", "Shower dE/dx Collection Plane [MeV/cm]",false,"d"));//52 beams from 0 to 7. True for track, flase for shower.d for double, i for integer.
	
	vars.push_back(bdt_variable("reco_shower_helper_energy[0]","(52,0,0.6)","Reconstructed Shower Energy [GeV]", false,"d"));
	vars.push_back(bdt_variable("reco_shower_length[0]","(52,0,100)","Shower Length [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_shower_opening_angle[0]","(52,0,0.8)","Shower Opening Angle",false,"d"));
	vars.push_back(bdt_variable("reco_shower_dist_to_closest_flashzcenter[0]","(52,0,520)","Distance from Shower to Flashcenter [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_nu_vtx_dist_to_closest_tpc_wall","(52,0,120)","Reconstructed Vertex to TPC Wall Distance [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_shower_bp_dist_to_tpc[0]","(52,0,550)","Back Projected Distance from Shower to TPC wall [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_nuvertx","(52,0,250)"," Reconstructed Vertex X pos [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_nuverty","(52,-110,+110)","Reconstructed Vertex Y pos [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_nuvertz","(52,0,1050)","Reconstructed Vertex Z pos [cm]",false,"d"));
	vars.push_back(bdt_variable("cos(atan2(reco_shower_diry[0],reco_shower_dirz[0]))","(52,-1,1)","Reconstructed Shower - Cosine Theta", true,"d"));
	vars.push_back(bdt_variable("cos(atan2(reco_shower_diry[0], reco_shower_dirx[0]))","(52,-1,1)","Reconstructed Shower - Cosine Phi", true,"d"));
	vars.push_back(bdt_variable("reco_shower_endx[0]","(52,0,250)"," Reconstructed Shower End X pos [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_shower_endy[0]","(52,-110,+110)","Reconstructed Shower End Y pos [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_shower_endz[0]","(52,0,1050)","Reconstructed Shower End Z pos [cm]",false,"d"));
	//vars.push_back(bdt_variable("pi0_info.num_reco_showers_within_10cm_vertex","(10,0,10)","Num Showers within 10cm",false,"i"));

	//track like ones
	vars.push_back(bdt_variable("reco_track_displacement[0]","(52,0,150)","Reconstructed Track Displacement [cm]", true,"d"));
	vars.push_back(bdt_variable("track_info.reco_track_mean_dEdx[0]", "(52,0,12)","Mean Track dE/dx [MeV/cm]", true,"d"));
	vars.push_back(bdt_variable("track_info.reco_track_start_mean_dEdx[0]/track_info.reco_track_end_mean_dEdx[0]", "(52,0,3.5)","Ratio of Mean Start/End Track dE/dx", true,"d"));
	vars.push_back(bdt_variable("track_info.reco_track_PIDA[0]","(52,0,24)","Track PIDA",true,"d"));
	vars.push_back(bdt_variable("track_info.reco_track_braggA[0]","(52,0,25)","Track Bragg A",true,"d"));
	//vars.push_back(bdt_variable("shortest_asso_shower_to_vert_dist","(52,0,30)","Photon Conversion Length from Reconstructed Vertex [cm]" ,false,"d"));
	////vars.push_back(bdt_variable(invariant_mass,"(52,1,2.5)","Invariant Mass",true,"d"));//delete this one.
	vars.push_back(bdt_variable("cos(atan2(reco_track_diry[0],reco_track_dirz[0]))","(52,-1,1)","Reconstructed Track - Cosine Theta", true,"d"));
	//vars.push_back(bdt_variable("cos(atan2(reco_track_diry[0],reco_track_dirx[0]))","(52,-1,1)","Reconstructed Track - Cosine Phi", true,"d"));
	vars.push_back(bdt_variable(angle_track_shower,	"(52,-1,1)","Cosine Track-Shower Angle ",true,"d"));
	vars.push_back(bdt_variable("reco_track_endx[0]","(52,0,250)"," Reconstructed Track End X pos [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_track_endy[0]","(52,-110,+110)","Reconstructed Track End Y pos [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_track_endz[0]","(52,0,1050)","Reconstructed Track End Z pos [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_track_energy_new_legacy[0]", "(52,0,1)","Track Energy (Best Plane)",true, "d"));

//Add some variables:
	
	//vars.push_back(bdt_variable("reco_shower_helper_energy[0]", "(52,0,1)","Reconstructed Shower Energy [GeV]",true, "d"));

	vars.push_back(bdt_variable("shortest_asso_shower_to_vert_dist","(52,0,20)","Photon Conversion Length from Reconstructed Vertex [cm]" ,false,"d"));//changes value only

	vars.push_back(bdt_variable("(min ( (reco_track_endz[0]-reco_shower_startz[0]),(reco_track_startz[0]-reco_shower_startz[0])))","(52,-100,50)",
			"Distance of a Track Ahead of Shower[cm]",true,"d"));//only works well on z-direction.

//go to line 29 for variables.
	
 

	//Once you have trained and studied the bdt outputs, here you can fix the value of the BDT cut (stage 2 and 3)
	double fcoscut = 0.5419;//found by -o sig
	double fbnbcut = 0.4992;
	
	signal_cosmics->writeStageFriendTree("stage_friend.root", fcoscut, fbnbcut);
	bnb_cosmics->writeStageFriendTree("stage_friend.root", fcoscut, fbnbcut);


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
		
		return 0;


	}else if(mode_option == "app"){

		//Apply! This will update cosmic_bdt_info, signal file and bkg file. As in update them PROPERLY!	
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
	else if(mode_option == "recomc"){//Labels backgrounds, adjust the name and values of true signal events.

		std::vector<int> recomc_cols = {kRed-7, kBlue+3, kBlue, kBlue-7, kMagenta-3, kYellow-7, kOrange-3, kGreen+1 ,kGray};
		std::vector<std::string> recomc_names = {"CC #nu_e Intrinsic", "CC #pi^{0} #rightarrow #gamma", "NC #pi^{0} #rightarrow #gamma","Non #pi^{0} #gamma","Intrinsic #nu_{e} electron","BNB Michel e^{#pm}","BNB Other","Cosmic Michel e^{#pm}", "Cosmic Other"};


		std::string  nue = "abs(true_shower_pdg[0]) ==11 && abs(nu_pdg)==12 && (exiting_electron_number==1 || exiting_antielectron_number==1)";
		std::string  michel = "abs(true_shower_pdg[0]) ==11 && abs(true_shower_parent_pdg[0])==13";
		std::vector<std::string> recomc_cuts = {
			"true_shower_origin[0]==1 && true_shower_pdg[0] == 22 && true_shower_parent_pdg[0] !=111 && "+ signal_definition,
			"true_shower_pdg[0] == 22 && true_shower_parent_pdg[0] == 111 && true_shower_origin[0]==1 && ccnc==0",
			"true_shower_pdg[0] == 22 && true_shower_parent_pdg[0] == 111 && true_shower_origin[0]==1 && ccnc==1",
			"true_shower_pdg[0] == 22 && true_shower_parent_pdg[0] != 111 && is_delta_rad!=1 && true_shower_origin[0]==1",
			"true_shower_origin[0] ==1 && "+ nue,
			"true_shower_origin[0] ==1 && "+ michel,
			"true_shower_origin[0]==1 && true_shower_pdg[0]!=22 &&  (( abs(true_shower_pdg[0])!=11)  ||( abs(true_shower_pdg[0])==11 && !(abs(nu_pdg)==12 && (exiting_electron_number==1 || exiting_antielectron_number==1)) &&!(abs(true_shower_parent_pdg[0])==13)    ))     ",
			"true_shower_origin[0] ==2 && abs(true_shower_parent_pdg[0])==13",
			"true_shower_origin[0] ==2 && abs(true_shower_parent_pdg[0])!=13"
		};



		bdt_recomc test(recomc_names, recomc_cuts, recomc_cols,run_tag);


		if(true){
			test.is_log = true;
			if(run_cosmic){
				test.plot_recomc(ftest, signal_cosmics, signal_cosmics->getBDTVariable(cosmic_bdt_info) , fcoscut,fbnbcut);
				test.plot_recomc(ftest, bnb_cosmics, bnb_cosmics->getBDTVariable(cosmic_bdt_info) , fcoscut,fbnbcut);
			}
			if(run_bnb){
				test.plot_recomc(ftest, bnb_cosmics, bnb_cosmics->getBDTVariable(bnb_bdt_info) , fcoscut,fbnbcut);
				test.plot_recomc(ftest, signal_cosmics, signal_cosmics->getBDTVariable(bnb_bdt_info) , fcoscut,fbnbcut);
			}

			test.is_log = false;
		}
		if(!response_only){
			int h=0;
			for(auto &v:vars){
				test.setRebin(true);
				if(v.name == "pi0_info.num_reco_showers_within_10cm_vertex") test.setRebin(false);
			//	test.plot_recomc(ftest, signal_cosmics, v, fcoscut, fbnbcut);
				test.plot_recomc(ftest, bnb_cosmics, vars.at(1), fcoscut, fbnbcut);
				if(v.name == "pi0_info.num_reco_showers_within_10cm_vertex") test.setRebin(true);
				h++;
				return 0;
			}	
		}

	}
	else if(mode_option == "sig"){


		TFile *fsig = new TFile(("significance_"+run_tag+".root").c_str(),"recreate");
		std::vector<double> ans = scan_significance(fsig, {signal_cosmics} , {bnb_cosmics, bnbext}, cosmic_bdt_info, bnb_bdt_info);
		//std::vector<double> ans = lin_scan({signal_cosmics}, {bnb_cosmics, bnbext}, cosmic_bdt_info, bnb_bdt_info,fcoscut,fbnbcut);

		std::cout<<"Best Fit Significance: "<<ans.at(0)<<" "<<ans.at(1)<<" "<<ans.at(2)<<std::endl;
		fsig->Close();


	}else if(mode_option == "stack"){
		
		if (access("stack",F_OK) == -1){
	    	    mkdir("stack",0777);//Create a folder for pdf.
	    	}

		bdt_stack obs(run_tag+"_stack");
		obs.addToStack(signal_cosmics);
		obs.addToStack(bnb_cosmics);
		bnbext->col = intime->col;	
		bnbext->fillstyle = 3333;
		obs.addToStack(bnbext);

		int ip=0;
		for(auto &v:vars){
			ip++;
			obs.plotStacks(ftest, v ,fcoscut,fbnbcut);//vars.at(n)
			if(number==1) return 0;// -n 1; means pick the first variable.
		}

		obs.plotBDTStacks(ftest, bnb_bdt_info, fcoscut, fbnbcut);
		obs.plotBDTStacks(ftest, cosmic_bdt_info, fcoscut, fbnbcut);
		return 0;

	}else if(mode_option == "datamc"){

		bdt_stack *obs = new bdt_stack(run_tag+"_datamc");
		obs->plot_pot =4.801e19;
		obs->addToStack(signal_cosmics);
		obs->addToStack(bnb_cosmics);
		obs->addToStack(intime);

		bdt_stack *obs2 = new bdt_stack(run_tag+"_extintime");
		obs2->plot_pot = 4.801e19;
		obs2->addToStack(intime);

		bdt_stack *obs3 = new bdt_stack(run_tag+"_twodata");
		obs3->plot_pot = 4.801e19;
		obs3->addToStack(signal_cosmics);
		obs3->addToStack(bnb_cosmics);
		bnbext->col = intime->col;	
		bnbext->fillstyle = 3333;
		obs3->addToStack(bnbext);

		int ip=0;

	
		if(!response_only){
			for(auto &v:vars){
				ip++;
				if(number==0){
					bdt_datamc datamc(data5e19, obs, run_tag);	
					datamc.plotStacks(ftest,  v ,fcoscut,fbnbcut);
				}
				if(number==1){
					bdt_datamc datamc2(bnbext, obs2, run_tag);	
					datamc2.plotStacks(ftest,  v,fcoscut,fbnbcut);
				}
				if(number==2){
					bdt_datamc t2datamc(data5e19, obs3, run_tag+"_2data");	
					t2datamc.plotStacks(ftest, v,fcoscut,fbnbcut);
				}
			}
		}else{
			bdt_datamc datamc(data5e19, obs, run_tag);	
			bdt_datamc datamc2(bnbext, obs2, run_tag);	
			bdt_datamc datamc3(data5e19, obs3, run_tag+"_2data");	

			if(number==2){
				datamc3.plotBDTStacks(ftest, bnb_bdt_info ,fcoscut,fbnbcut);
				datamc3.plotBDTStacks(ftest, cosmic_bdt_info ,fcoscut,fbnbcut);
			}
			if(number==0){
				datamc.plotBDTStacks(ftest, bnb_bdt_info ,fcoscut,fbnbcut);
				datamc.plotBDTStacks(ftest, cosmic_bdt_info ,fcoscut,fbnbcut);
			}
			if(number==1){
				datamc2.plotBDTStacks(ftest, bnb_bdt_info ,fcoscut,fbnbcut);
				datamc2.plotBDTStacks(ftest, cosmic_bdt_info ,fcoscut,fbnbcut);
			}
		}

	}else if(mode_option == "vars"){

		std::vector<std::string> title = {"All Verticies","Pre-Selection Cuts"};

	    	if (access("var",F_OK) == -1){
	    	    mkdir("var",0777);//Create a folder for pdf.
	    	}
	    	else{
	    	    std::cout<<"vars has been executed; the results locate in the folder: var/"<<std::endl;
	    	    return 0;
	    	}

		if(run_cosmic){
			for(auto &v:vars){

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
				TCanvas *c_var = new TCanvas(("cvar_"+v.name+"_cosmo").c_str(), ("cvar_"+v.name+"_cosmo").c_str(),1200,1200);
				c_var->cd();


					sig->SetFillColor(kRed-7);
					bkg->SetFillColor(kGreen-3);
					sig->SetFillStyle(3445);
					bkg->SetFillStyle(3454);

					if(j!=1){
						sig->SetTitle(title.at(j).c_str());
					}else{
						sig->SetTitle(" ");
					}
					
					c_var->cd();			

					sig->Draw("hist");
					bkg->Draw("hist same");
					//sig->GetXaxis()->SetTitle(v.unit.c_str());
					sig->GetYaxis()->SetTitle("Verticies [Area Normalized]");
					sig->GetYaxis()->SetTitleOffset(1.5);

					TLegend *l = new TLegend(0.11,0.75,0.89,0.89);
					l->SetLineColor(kWhite);
					l->SetFillStyle(0);
					l->SetNColumns(2);

					l->AddEntry(sig,"CC #nu_e Intrinsic Signal","lf");	
					l->AddEntry(bkg,"Intime Cosmic Background","lf");	
					l->Draw();

					TText *pre = drawPrelim(0.1,0.915,0.03,"MicroBooNE Simulation Preliminary");
					pre->Draw();

					TLatex latex;
					latex.SetTextSize(0.06);
					latex.SetTextAlign(13);  //align at top
					latex.SetNDC();
					latex.DrawLatex(.7,.71, bnb_bdt_info.topo_name.c_str());


					double max_height = std::max( sig->GetMaximum(), bkg->GetMaximum());
					sig->SetMaximum(max_height*1.3);


				c_var->Print(("var/"+run_tag+"_cosmic_"+v.safe_unit+"_stage_"+std::to_string(j)+".pdf").c_str(),"pdf");
				}
				if(number==1) return 0;
			}

		}
		if(run_bnb){
			for(auto &v:vars){
//				auto v = vars.at(21); 				

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
					//c_var->cd(1);			

					TCanvas *c_var = new TCanvas(("cvar_"+v.name+"_"+std::to_string(j)+"_bnb").c_str(), ("cvar_"+v.name+"_"+std::to_string(j)+"_bnb").c_str(),1200,1200);
					c_var->cd();


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

					l->AddEntry(sig,"NC #nu_e Instrinsic Signal","lf");	
					l->AddEntry(bkg,"BNB Backgrounds","lf");	
					l->Draw();

					TLatex latex;
					latex.SetTextSize(0.06);
					latex.SetTextAlign(13);  //align at top
					latex.SetNDC();
					latex.DrawLatex(.7,.71, bnb_bdt_info.topo_name.c_str());

					double max_height = std::max( sig->GetMaximum(), bkg->GetMaximum());
					sig->SetMaximum(max_height*1.1);


					c_var->Print(("var/"+run_tag+"_bnb_"+v.safe_unit+"_stage_"+std::to_string(j)+".pdf").c_str(),"pdf");
				}
				if(number==1) return 0;
			}
		}







	} else if(mode_option == "eff"){

		double plot_pot = 6.6e20;
		std::cout<<"Starting efficiency study: coscut @ "<<fcoscut<<" bnbcut@: "<<fbnbcut<<std::endl;

		std::cout<<"########################################################################"<<std::endl;
		std::cout<<"###############################Efficiency###############################"<<std::endl;


		TFile * feff = new TFile(("Eff_"+run_tag+".root").c_str(),"recreate");
		TCanvas *ceff = new TCanvas();		
		ceff->cd();

		std::vector<double> eff_sig;

		std::vector<bdt_file*> eff_files = {signal_pure, bnb_cosmics, bnbext};
		//std::vector<bdt_file*> eff_files = {signal_cosmics, bnb_cosmics, bnbext};
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

			if(eff_files.at(i)->tag=="IntrinsicNueCosmics"){
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

				if(eff_files.at(i)->tag=="IntrinsicNueCosmics"){
					eff_sig.push_back(np/start*100);
				}
				effs.back().push_back(np);

			}

			if(eff_files.at(i)->tag=="IntrinsicNueCosmics"){
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

		
			sname = {"Generated","Vertexed","Topological","Total PE $>20$","Fiducial cut","Track $< 100$cm","Reco $E_{\\gamma}$ $> 30$MeV","Shower gap $> 1$cm","Good calo cut","Flipped track cut","Back-to-back cut"};
		
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
			double nv = data_files.at(i)->GetEntries((gencut+"&& 1").c_str())*pot_scale;	
			double ns = data_files.at(i)->GetEntries((gencut+"&&"+  base_cuts).c_str())*pot_scale;	

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

		//std::vector<bdt_file*> bdt_filesB = {bnb_pure,signal_pure};
		//std::vector<bdt_file*> bdt_filesB = {intime, data5e19, bnbext};
		std::vector<bdt_file*> bdt_filesB = {signal_pure};
		//std::vector<bdt_file*> bdt_filesB = {bnbext};

		//bdt_precalc pre1(bdt_filesB.at(number));
		//pre1.genTrackInfo();

		//pre1.genBNBcorrectionInfo();
		//pre1.genPi0Info();

		for(auto &f: bdt_filesB){
			bdt_precalc pre(f);
			//pre.genBNBcorrectionInfo();
			//pre.genTrackInfo();
			pre.genPi0Info();
		}
	}
	else if(mode_option == "sbnfit"){

		bdt_stack *obs = new bdt_stack(run_tag+"_datamc");
		obs->plot_pot =  6.6e20;
		//obs->plot_pot =  4.801e19;
		obs->addToStack(signal_cosmics);
		obs->addToStack(bnb_cosmics);
		obs->addToStack(intime);
		obs->addToStack(data5e19);

		std::vector<std::string> hnam = {"nu_uBooNE_singleelectron_signal","nu_uBooNE_singleelectron_bkg","nu_uBooNE_singleelectron_intime","nu_uBooNE_singleelectron_data"};
		//std::vector<std::string> hnam = {"nu_uBooNE_singlephoton_signal","nu_uBooNE_singlephoton_bkg","nu_uBooNE_singlephoton_intime"};
		obs->makeSBNspec("test", vars.at(1), fcoscut, fbnbcut,hnam );






	}else if( mode_option =="test"){//adjust the name and values of true signal events.



		std::vector<int> recomc_cols = {kRed-7, kBlue+3, kBlue, kBlue-7, kMagenta-3, kYellow-7, kOrange-3, kGreen+1 ,kGray};
		std::vector<std::string> recomc_names = {"CC #nu_e Intrinsic", "CC #pi^{0} #rightarrow #gamma", "NC #pi^{0} #rightarrow #gamma","Non #pi^{0} #gamma","Intrinsic #nu_{e} electron","BNB Michel e^{#pm}","BNB Other","Cosmic Michel e^{#pm}", "Cosmic Other"};


		std::string  nue = "abs(true_shower_pdg[0]) ==11 && abs(nu_pdg)==12 && (exiting_electron_number==1 || exiting_antielectron_number==1)";
		std::string  michel = "abs(true_shower_pdg[0]) ==11 && abs(true_shower_parent_pdg[0])==13";
		std::vector<std::string> recomc_cuts = {
			"true_shower_origin[0]==1 && true_shower_pdg[0] == 22 && true_shower_parent_pdg[0] !=111 && "+ signal_definition,
			"true_shower_pdg[0] == 22 && true_shower_parent_pdg[0] == 111 && true_shower_origin[0]==1 && ccnc==0",
			"true_shower_pdg[0] == 22 && true_shower_parent_pdg[0] == 111 && true_shower_origin[0]==1 && ccnc==1",
			"true_shower_pdg[0] == 22 && true_shower_parent_pdg[0] != 111 && is_delta_rad!=1 && true_shower_origin[0]==1",
			"true_shower_origin[0] ==1 && "+ nue,
			"true_shower_origin[0] ==1 && "+ michel,
			"true_shower_origin[0]==1 && true_shower_pdg[0]!=22 &&  (( abs(true_shower_pdg[0])!=11)  ||( abs(true_shower_pdg[0])==11 && !(abs(nu_pdg)==12 && (exiting_electron_number==1 || exiting_antielectron_number==1)) &&!(abs(true_shower_parent_pdg[0])==13)    ))     ",
			"true_shower_origin[0] ==2 && abs(true_shower_parent_pdg[0])==13",
			"true_shower_origin[0] ==2 && abs(true_shower_parent_pdg[0])!=13"
		};

		bdt_recomc test(recomc_names, recomc_cuts, recomc_cols, run_tag);


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

		bdt_stack *obs3 = new bdt_stack("track_box");
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

	ftest->Close();

	return 0;

}
