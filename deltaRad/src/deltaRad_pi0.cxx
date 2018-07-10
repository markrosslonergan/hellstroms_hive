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
#include "bdt_datamc.h"

#include "bdt_precalc.h"



int main (int argc, char *argv[]){

	// Just some simple argument things
	//===========================================================================================
	std::string dir = "/home/amogan/singlePhotonCode/hellstroms_hive/";

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

	// Some optional argument stuff
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
	std::string fiducial_vertex = "reco_nuvertx > 10 && reco_nuvertx < 246 && reco_nuverty > -107 && reco_nuverty < 97 && reco_nuvertz > 10 && reco_nuvertz < 1026 ";
    std::string num_track_cut;
	if(istrack == "track"){
	    num_track_cut = " == 1 ";
        //new_precuts = "1";
        // Minimum shower energy cut in GeV
        new_precuts = "reco_shower_helper_energy[second_most_energetic_shower_index] > 0.03 && track_info.reco_track_good_calo[0]>0 && track_info.reco_track_mean_dEdx[0] > 1 &&"+fiducial_vertex; 

	}else if(istrack == "notrack"){
		//new_precuts = "reco_nu_vtx_dist_to_closest_tpc_wall > 10 && totalpe_ibg_sum > 50";
		new_precuts = "1";
		num_track_cut = "==0";
	}

	//Set up 2 bdt_info structs for passing information on what BDT we are running. 
	//MARK: Now with added binning here, so bdt_file->GetBDTvariable() is much simpler!
	bdt_info bnb_bdt_info("pi0bnb_"+istrack, "BNB focused BDT", "(50,0.37,0.57)");
	bdt_info cosmic_bdt_info("pi0cosmic_"+istrack, "Cosmic focused BDT", "(50,0.3,0.67)");
	

	// Our signal definition alongside any base cuts we want to make
    // UPDATE 4/30/18: base cuts now done externally in attempt to work
    // around array issue. Cuts kept here to avoid code breaking
	std::string base_cuts = "reco_asso_showers==2 && reco_asso_tracks==1";
	std::string cosmic_base_cuts = "reco_asso_showers==2 && reco_asso_tracks==1";

    // Signal: NC interaction, two photons from parent pi0, BNB interaction
    // Can use 0 and 1 for indices. Since two showers are already selected,
    // the two showers will always be indexed 0 and 1, though not necessarily
    // in order (0 doesn't have to be most energetic)
	std::string signal_definition = "ccnc==1 && true_shower_parent_pdg[0]==111 && true_shower_parent_pdg[1]==111 && true_shower_origin[0]==1 && true_shower_origin[1]==1 ";
	std::string background_definition = "!(" + signal_definition + ")";

	
	//This is a particular cut flow that a file will undergo. I.e base cuts, precuts, postcuts, and then the name of the Cosmic BDT and bnb bdt
	bdt_flow signal_flow( base_cuts,        signal_definition,	    new_precuts, "1", cosmic_bdt_info, bnb_bdt_info);
	bdt_flow cosmic_flow( cosmic_base_cuts, "1",  			        new_precuts, "1", cosmic_bdt_info, bnb_bdt_info);
	bdt_flow bkg_flow(    base_cuts,        background_definition,  new_precuts, "1", cosmic_bdt_info, bnb_bdt_info);
	bdt_flow data_flow(   base_cuts ,	    "1", 			        new_precuts, "1", cosmic_bdt_info, bnb_bdt_info);

	// BDT files, in the form (location, rootfile, name, hisotgram_options, tfile_folder, tag, color, bdt_flow ) 
	bdt_file *signal_pure    = new bdt_file(dir+"samples/mcc88", "vertexed_ncpi0_fltr_mcc8.9_fresh_v1.0.root", "NCpi0Pure",	   "hist", "",  kRed-7,   signal_flow);
	bdt_file *signal_cosmics = new bdt_file(dir+"samples/mcc88", "vertexed_ncpi0_fltr_mcc8.9_fresh_v1.0.root", "NCpi0Cosmics", "hist", "",  kRed-7,   signal_flow);
	bdt_file *bnb_pure    = new bdt_file(   dir+"samples/mcc88", "vertexed_bnbcosmics_mcc8.9_fresh_v3.0.root", "BNBPure",	   "hist", "",  kBlue-4,  bkg_flow);
	bdt_file *bnb_cosmics = new bdt_file(   dir+"samples/mcc88", "vertexed_bnbcosmics_mcc8.9_fresh_v3.0.root", "BNBCosmics",   "hist", "",  kBlue-4,  bkg_flow);
	bdt_file *intime = new bdt_file(        dir+"samples/mcc88", "vertexed_intime_mcc8.9_fresh_v2.0.root",     "IntimeCosmics","hist", "",  kGreen-3, cosmic_flow);
	// Data files
	bdt_file *data5e19 = new bdt_file(dir+"samples/data",   "vertexed_data5e19_mcc8.9_fresh_v3.0.root", "Data5e19", "hist ep", "", kBlack, data_flow);
	bdt_file *bnbext = new bdt_file(  dir+"samples/bnbext", "vertexed_bnbext_mcc8.9_fresh_v2.0.root",   "BNBext",   "hist ep", "", kBlack, data_flow);
    

	std::vector<bdt_file*> bdt_files = {data5e19, bnbext, signal_pure, signal_cosmics, bnb_pure, bnb_cosmics, intime};


	//you get access to these with track_info.XXX
	std::cout<<"--------------------------------------------------------------------------"<<std::endl;
	std::cout<<"Going to add any precomputed tree friends, or any trained BDT responses   "<<std::endl;
	std::cout<<"If you see warnings, but havenet yet ran app stage, thats ok!            "<<std::endl;
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



	// Define BDT variables, starting with some convenient strings
	std::string angle_track_shower1 ="(reco_track_dirx[longest_asso_track_index]*reco_shower_dirx[most_energetic_shower_index]+reco_track_diry[longest_asso_track_index]*reco_shower_diry[most_energetic_shower_index]+reco_track_dirz[longest_asso_track_index]*reco_shower_dirz[most_energetic_shower_index])";
	std::string angle_track_shower2 ="(reco_track_dirx[longest_asso_track_index]*reco_shower_dirx[second_most_energetic_shower_index]+reco_track_diry[longest_asso_track_index]*reco_shower_diry[second_most_energetic_shower_index]+reco_track_dirz[longest_asso_track_index]*reco_shower_dirz[second_most_energetic_shower_index])";
	std::string angle_shower1_shower2 ="reco_shower_dirx[0]*reco_shower_dirx[1]+reco_shower_diry[0]*reco_shower_diry[1]+reco_shower_dirz[0]*reco_shower_dirz[1]";
    std::string E1 = "reco_shower_helper_energy[most_energetic_shower_index]"; 
    std::string E2 = "reco_shower_helper_energy[second_most_energetic_shower_index]"; 
    // Invariant mass for two massless particles
    std::string invMass = "sqrt(2.0*"+E1+"*"+E2+"*(1.0-"+angle_shower1_shower2+"))";
    // Pion momentum components
    std::string p_pi_x = E1+"*reco_shower_dirx[most_energetic_shower_index]"+"+"+E2+"*reco_shower_dirx[second_most_energetic_shower_index]";
    std::string p_pi_y = E1+"*reco_shower_diry[most_energetic_shower_index]"+"+"+E2+"*reco_shower_diry[second_most_energetic_shower_index]";
    std::string p_pi_z = E1+"*reco_shower_dirz[most_energetic_shower_index]"+"+"+E2+"*reco_shower_dirz[second_most_energetic_shower_index]";
    std::vector<bdt_variable> vars;

    vars.push_back(bdt_variable("sqrt("+E1+"*"+E1+" + "+E2+"*"+E2+" + 2*"+E1+"*"+E2+"*"+angle_shower1_shower2+")", "(100, 100, 1500)", "Reconstructed Pion Momentum[MeV]", true, "d"));
	vars.push_back(bdt_variable("reco_shower_dedx_plane2[most_energetic_shower_index]","(48,0,15)", "Shower 1 dE/dx Collection Plane [MeV/cm]",false,"d"));
	vars.push_back(bdt_variable("reco_shower_dedx_plane2[second_most_energetic_shower_index]","(48,0,15)", "Shower 2 dE/dx Collection Plane [MeV/cm]",false,"d"));
	vars.push_back(bdt_variable("summed_associated_helper_shower_energy","(25,0,0.5)","Summed Shower Energy [GeV]", false,"d"));
	vars.push_back(bdt_variable("reco_shower_length[most_energetic_shower_index]","(25,0,125)","Most Energetic Shower Length [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_shower_length[second_most_energetic_shower_index]","(25,0,125)","Least Energetic Shower Length [cm]",false,"d"));
    vars.push_back(bdt_variable(angle_shower1_shower2,"(50, -1, 1)", "Angle between Showers", false, "d"));
    vars.push_back(bdt_variable(invMass,"(50, 0.03, 0.5)", "Two-shower Invariant Mass [GeV]", false, "d"));
    vars.push_back(bdt_variable("reco_shower_helper_energy[most_energetic_shower_index]", "(50, 0, 1)", "Most Energetic Shower Energy", false, "d"));
    vars.push_back(bdt_variable("reco_shower_helper_energy[second_most_energetic_shower_index]", "(50, 0, 1)", "Second Most Energetic Shower Energy", false, "d"));

	vars.push_back(bdt_variable("totalpe_ibg_sum","(25,0,2000)","Total in Beam-Gate PE",false,"d"));
	vars.push_back(bdt_variable("closest_asso_shower_dist_to_flashzcenter","(25,0,1000)","Distance from Shower to Flashcenter [cm]",false,"d"));

	vars.push_back(bdt_variable("reco_nu_vtx_dist_to_closest_tpc_wall","(25,0,125)","Reconstructed Vertex to TPC Wall Distance [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_shower_bp_dist_to_tpc[most_energetic_shower_index]","(25,0,1000)","Back Projected Distance from Shower 1 to TPC wall [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_shower_bp_dist_to_tpc[second_most_energetic_shower_index]","(25,0,1000)","Back Projected Distance from Shower 2 to TPC wall [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_nuvertx","(25,0,300)"," Reconstructed Vertex x-location [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_nuverty","(25,-300,+300)","Reconstructed Vertex y-location [cm]",false,"d"));
	vars.push_back(bdt_variable("reco_nuvertz","(25,0,1000)","Reconstructed Vertex z-location [cm]",false,"d"));

	vars.push_back(bdt_variable("cos(atan2(reco_shower_diry[most_energetic_shower_index],reco_shower_dirz[most_energetic_shower_index]))","(50,-1,1)","Reconstructed Shower 1 |Cosine Theta|", true,"d"));
	vars.push_back(bdt_variable("cos(atan2(reco_shower_diry[most_energetic_shower_index],reco_shower_dirx[most_energetic_shower_index]))","(50,-1,1)","Reconstructed Shower 1 |Cosine Phi|", true,"d"));
	vars.push_back(bdt_variable("cos(atan2(reco_shower_diry[second_most_energetic_shower_index],reco_shower_dirz[second_most_energetic_shower_index]))","(50,-1,1)","Reconstructed Shower 2 |Cosine Theta|", true,"d"));
	vars.push_back(bdt_variable("cos(atan2(reco_shower_diry[second_most_energetic_shower_index],reco_shower_dirx[second_most_energetic_shower_index]))","(50,-1,1)","Reconstructed Shower 2 |Cosine Phi|", true,"d"));
    
	if(istrack=="track"){ 
        vars.push_back(bdt_variable("pi0_info.reco_gamma_decay_angle", "(50, -1, 1)", "Reconstructed Cosine #pi-#gamma Angle (CM Frame)", true, "d"));
		vars.push_back(bdt_variable("reco_track_displacement[0]","(52,0,150)","Reconstructed Track Displacement [cm]", true,"d"));
		vars.push_back(bdt_variable("track_info.reco_track_mean_dEdx[0]", "(52,0,12)","Mean Track dE/dx", true,"d"));
		vars.push_back(bdt_variable("track_info.reco_track_start_mean_dEdx[0]/track_info.reco_track_end_mean_dEdx[0]", "(52,0,3.5)","Ratio of Mean Start/End Track dE/dx", true,"d"));
		vars.push_back(bdt_variable("track_info.reco_track_PIDA[0]","(52,0,24)","Track PIDA",true,"d"));
		vars.push_back(bdt_variable("track_info.reco_track_braggA[0]","(52,0,25)","Track Bragg A",true,"d"));
		vars.push_back(bdt_variable("shortest_asso_shower_to_vert_dist","(50,0,20)","Closest Photon Conversion Length from Reconstructed Vertex [cm]" ,false,"d"));
		vars.push_back(bdt_variable("cos(atan2(reco_track_diry[longest_asso_track_index],reco_track_dirz[longest_asso_track_index]))","(50,-1.0, 1.0)","Reconstructed Track Cosine Theta", true,"d"));
		vars.push_back(bdt_variable("cos(atan2(reco_track_diry[longest_asso_track_index],reco_track_dirx[longest_asso_track_index]))","(50,-1.0, 1.0)","Reconstructed Track Cosine Phi", true,"d"));
 
		vars.push_back(bdt_variable(angle_track_shower1,	"(50,-1,1)","|Cosine Track-Shower Angle (Most Energetic)| ",true,"d"));
		vars.push_back(bdt_variable(angle_track_shower2,	"(50,-1,1)","|Cosine Track-Shower Angle (Second Most Energetic)| ",true,"d"));
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

    // Determined from first two numbers in output of "./deltaRad_pi0 -o sig"
	double fcoscut;
	double fbnbcut;
	if(istrack == "track"){
		fcoscut = 0.54;
		fbnbcut = 0.515;
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


	}else if(mode_option == "app_bnb"){
		//Apply! This will update bnb_bdt_info, signal file and bkg file. As in update them PROPERLY!	

		std::vector<bdt_file*> app_files = {data5e19,bnbext,signal_pure, bnb_pure, intime, signal_cosmics, bnb_cosmics}; 
		//std::vector<bdt_file*> app_files = {signal_pure, bnb_pure, intime, signal_cosmics, bnb_cosmics}; 
		bdt_app(bnb_bdt_info, app_files, vars, TMVAmethods);

	}else if(mode_option == "app_cosmic"){
		//Apply! This will update cosmic_bdt_info, signal file and bkg file. As in update them PROPERLY!	

		std::vector<bdt_file*> app_files = {data5e19,bnbext,signal_pure, bnb_pure, intime, signal_cosmics, bnb_cosmics}; 
		//std::vector<bdt_file*> app_files = {signal_pure}; 
		bdt_app(cosmic_bdt_info, app_files, vars, TMVAmethods);
	}

	else if(mode_option == "response"){

		//Ok print out Cosmic BDT
	//	bdt_response cosmic_response(cosmic_bdt_info, signal_pure, intime);
	//	cosmic_response.plot_bdt_response(ftest);
        
		bdt_response bnb_response(bnb_bdt_info, signal_pure, bnb_pure);
		bdt_response cosmic_response(cosmic_bdt_info, signal_pure, intime);
        signal_pure->plot_name = "NC #pi^{0}";
        bnb_pure->plot_name = "BNB";
        intime->plot_name = "Intime Corsika Cosmics";
		bnb_response.plot_bdt_response(ftest);
        cosmic_response.plot_bdt_response(ftest);

	}	
	else if(mode_option == "recomc"){

	    // First off, what MC catagories do you want to stack?
    	std::vector<std::string>recomc_names = {"BNB NC #pi^{0}", "BNB CC #pi^{0}","NC BNB Background", "CC BNB Background", "NC #Delta Radiative", "Two Cosmic Showers", "One Cosmic Shower"};
	    // How are they defined, cutwise?
        // NC pi0 signal: two photon showers whose true parents are pi0's, all resulting from NC BNB interaction
        std::string signal = "ccnc == 1 && true_shower_pdg[0] == 22 && true_shower_pdg[1] == 22 && true_shower_parent_pdg[0] == 111 && true_shower_parent_pdg[1] == 111 && true_shower_origin[0]==1 && true_shower_origin[1]==1";
        // CC pi0 background
        std::string bkg1 = "ccnc == 0 && true_shower_pdg[0] == 22 && true_shower_pdg[1] == 22 && true_shower_parent_pdg[0] == 111 && true_shower_parent_pdg[1] == 111 && true_shower_origin[0]==1 && true_shower_origin[1]==1"; 
        // Other NC BNB events where either (a) one of the showers isn't photon-induced, or (b) one doesn't come from pi0
        std::string bkg2 = "(true_shower_pdg[0] != 22 || true_shower_pdg[1] != 22 || true_shower_parent_pdg[0] != 111 || true_shower_parent_pdg[1] != 111) && ccnc == 1 && true_shower_origin[0]==1 && true_shower_origin[1]==1";
        // Same as previous, but for CC events
        std::string bkg3 = "(true_shower_pdg[0] != 22 || true_shower_pdg[1] != 22 || true_shower_parent_pdg[0] != 111 || true_shower_parent_pdg[1] != 111) && ccnc == 0 && true_shower_origin[0]==1 && true_shower_origin[1]==1";
        // Delta radiative is now considered a (very small) background
        std::string bkg4 = "reco_asso_showers==1 && true_shower_pdg[most_energetic_shower_index] == 22 && true_shower_parent_pdg[most_energetic_shower_index] != 111 && is_delta_rad == 1 && true_shower_origin==1";
        // Cosmics
        std::string bkg5 = "true_shower_origin[0]==2 && true_shower_origin[1]==2";
        // Other. For now, just case where one shower is cosmic, but not both
        std::string bkg6 = "(true_shower_origin[0]==2 || true_shower_origin[1]==2) && !(true_shower_origin[0]==2 && true_shower_origin[1]==2)";
        // Other; defined as "!" versions of above
        std::string other = "!("+signal+"&&"+bkg1+"&&"+bkg2+"&&"+bkg3+"&&"+bkg4+"&&"+bkg5+"&&"+bkg6+")";
	    std::vector<std::string> recomc_cuts = {signal, bkg1,bkg2, bkg3, bkg4, bkg5, bkg6, other};

    
	//and what colors
    //{kRed-7, kRed+1, kYellow-7, kOrange-3, kBlue+3, kBlue,  kGreen+1,kBlue-7}
	//std::vector<int> recomc_cols = {kRed-7, kRed+1, kMagenta+1, kBlue+3, kPink+1, kGreen+1, kGreen+3};
	std::vector<int> recomc_cols = {kRed-7, kRed+1, kYellow-7, kOrange-3, kBlue+3, kBlue, kGreen+1, kBlue-7};

        std::cout << "Done adding TreeFiends" << std::endl;
		bdt_recomc test(recomc_names, recomc_cuts, recomc_cols,istrack);
		//plot_recomc(TFile *fout, bdt_file* file, bdt_variable var, double cut_cosmic_val, double cut_bnb_val)

	//test.plot_recomc(ftest, bnb_cosmics, bnb_cosmics->getBDTVariable(bnb_bdt_info) , fcoscut,fbnbcut);
	//test.plot_recomc(ftest, signal_cosmics, signal_cosmics->getBDTVariable(cosmic_bdt_info) , fcoscut,fbnbcut);
    
		int h=0;
		for(auto &v:vars){
            test.plot_recomc(ftest, signal_cosmics, v, fcoscut, fbnbcut);
            test.plot_recomc(ftest, bnb_cosmics, v, fcoscut, fbnbcut);
            h++;
            if (h > 1) break;  
		}	
    }  

	//test.plot_recomc(ftest, bnb_cosmics, vars.at(1), fcoscut, fbnbcut);
					//test.plot_recomc(ftest, signal_cosmics, vars.at(1), fcoscut, fbnbcut);

		//		test.plot_recomc(ftest, signal_cosmics, signal_cosmics->getBDTVariable(cosmic_bdt_info), usecut1, usecut2);
		//		test.plot_recomc(ftest, signal_cosmics, signal_cosmics->getBDTVariable(bnb_bdt_info), usecut1, usecut2);

		//		for(int i=1; i<=4; i++){
		//			test.plot_recomc(ftest, bnb_cosmics, vars.at(i), usecut1, usecut2);
		//			test.plot_recomc(ftest, signal_cosmics, vars.at(i), usecut1, usecut2);
		//		}

	
	else if(mode_option == "sig"){

		TFile *fsig = new TFile(("significance_"+istrack+".root").c_str(),"recreate");
		std::vector<double> ans = scan_significance(fsig, {signal_cosmics} , {bnb_cosmics, intime}, cosmic_bdt_info, bnb_bdt_info);
		std::cout<<"Best Fit Significance: "<<ans.at(0)<<" "<<ans.at(1)<<" "<<ans.at(2)<<std::endl;
		fsig->Close();


	}else if(mode_option == "stack"){
        signal_cosmics->col=kRed-7;
        bnb_cosmics->col=kBlue;
        bnbext->col=kGreen+2;
		bdt_stack obs("obs");
		obs.addToStack(signal_cosmics);
		obs.addToStack(bnb_cosmics);
		obs.addToStack(intime);

		for(auto &v:vars){
            std::cout << "In obs.plotStacks loop" << std::endl;
			obs.plotStacks(ftest, v, fcoscut, fbnbcut);
		}
        std::cout << "Finished obs.plotStacks" << std::endl;
		obs.plotBDTStacks(ftest,bnb_bdt_info, fcoscut, fbnbcut);
        std::cout << "Done" << std::endl;

	}else if(mode_option == "datamc"){
        
        //signal_cosmics->col=kRed-7;
        //bnb_cosmics->col=kBlue;
        bnbext->col=kGreen-3;
	    bnbext->fillstyle = 3333;
        bdt_stack *obs = new bdt_stack(istrack+"_datamc");
        //obs->plot_pot = 5e19;
        obs->plot_pot = 4.801e19;
        obs->addToStack(signal_cosmics);
        obs->addToStack(bnb_cosmics);
        obs->addToStack(bnbext);

        //bdt_stack *obs2 = new bdt_stack(istrack+"_extintime");
        //obs2->plot_pot = 5e19;
        //obs2->addToStack(intime);

        bdt_stack *obs3 = new bdt_stack(istrack+"_overlaymc");
        obs3->plot_pot = 5e19;
        obs3->addToStack(bnbext);


        int ip=0;
        //for(auto &v:vars){
        //for(auto &v = vars.at(2); v < 100; v++ ) {
            //break;
            ip++;
            bdt_datamc tdatamc(data5e19, obs);
            //bdt_datamc tdatamc2(bnbext, obs2);
            bdt_datamc tdatamc3(data5e19, obs3);
            //tdatamc.plotStacks(ftest,  v,fcoscut,fbnbcut);
            tdatamc.plotStacks(ftest,  vars.at(6),fcoscut,fbnbcut);
            //tdatamc2.plotStacks(ftest,  v,fcoscut,fbnbcut);
            //tdatamc3.plotStacks(ftest,  v,fcoscut,fbnbcut);
            //return 0;

        //}

        bdt_datamc datamc(data5e19, obs);
        //bdt_datamc datamc2(bnbext, obs2);
        bdt_datamc datamc3(data5e19, obs3);

        datamc.plotBDTStacks(ftest, bnb_bdt_info ,fcoscut,fbnbcut);
        datamc.plotBDTStacks(ftest, cosmic_bdt_info ,fcoscut,fbnbcut);
        //datamc2.plotBDTStacks(ftest, bnb_bdt_info ,fcoscut,fbnbcut);
        //datamc2.plotBDTStacks(ftest, cosmic_bdt_info ,fcoscut,fbnbcut);
        //datamc3.plotBDTStacks(ftest, bnb_bdt_info ,fcoscut,fbnbcut);
        //datamc3.plotBDTStacks(ftest, cosmic_bdt_info ,fcoscut,fbnbcut);
} 

    else if(mode_option == "vars"){

			std::vector<std::string> title = {"All Vertices","Pre-Selection Cuts", "Post Cosmic BDT", "Post BNB BDT"};

			if(true){
				for(auto &v:vars){
					TCanvas *c_var = new TCanvas(("cvar_"+v.name+"_cosmo").c_str(), ("cvar_"+v.name+"_cosmo").c_str(),2200,1200);
					c_var->Divide(2,1);
					c_var->cd();

					for(int j=0; j<4;j++){	

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
						sig->GetYaxis()->SetTitle("Vertices [Area Normalized]");
						sig->GetYaxis()->SetTitleOffset(1.5);

						TLegend *l = new TLegend(0.11,0.75,0.89,0.89);
						l->SetLineColor(kWhite);
						l->SetFillStyle(0);
						l->SetNColumns(2);

						l->AddEntry(sig,"NC #pi^{0} Signal","lf");	
						l->AddEntry(bkg,"Intime Cosmic Background","lf");	
						l->Draw();
						double max_height = std::max( sig->GetMaximum(), bkg->GetMaximum());
						sig->SetMaximum(max_height*1.3);



					}
					c_var->Print(("var/"+istrack+"_cosmic_"+v.safe_unit+".pdf").c_str(),"pdf");
				}

			}
			if(true){
				for(auto &v:vars){
					TCanvas *c_var = new TCanvas(("cvar_"+v.name+"_bnb").c_str(), ("cvar_"+v.name+"_bnb").c_str(),2200,1200);
					c_var->Divide(2,1);
					c_var->cd();

					for(int j=0; j<4;j++){	

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

						l->AddEntry(sig,"NC #pi^{0} Signal","lf");	
						l->AddEntry(bkg,"BNB Backgrounds","lf");	
						l->Draw();

						double max_height = std::max( sig->GetMaximum(), bkg->GetMaximum());
						sig->SetMaximum(max_height*1.1);
					}
					c_var->Print(("var/"+istrack+"_bnb_"+v.safe_unit+".pdf").c_str(),"pdf");
				}
			}
    }



                    
                

    
                        /*
    else if(mode_option == "vars"){

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
                    */


	/*else if(mode_option == "eff"){

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
    else if(mode_option == "precalc"){

			std::vector<bdt_file*> precalc_files = {signal_pure};
			for(auto &f: precalc_files){
				bdt_precalc pre(f);
                // Only uncomment one calcluation at a time, otherwise memory leaks!
				//pre.genBNBcorrectionInfo();
			    //pre.genTrackInfo();
			    pre.genPi0BoostAngle();
			}
		}
		  
    else {
		std::cout << "WARNING: " << mode_option << " is an invalid option\n";
	}

	ftest->Close();

	return 0;

}
