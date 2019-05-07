#include "load_mva_param.h"
#include "tinyxml.h"

#include <getopt.h>

#include "variable_list.h"
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
#include "bdt_eff.h"
#include "bdt_test.h"
#include "TTreePlayer.h"
#include <sys/stat.h>

using namespace std;

int compareQuick(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, std::string name);



int main (int argc, char *argv[]){

    //This is a standardized location on /pnfs/ that everyone can use. 
    std::string dir = "/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_mcc9_v12/";
//    std::string dir9 = "/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_mcc9_v9/";


    std::string mode_option = "fake"; 
    std::string xml = "default.xml";
    std::string topo_tag = "1e1p";
    std::string bdt_tag = "cosmic";
    std::string analysis_tag = topo_tag;

    bool run_cosmic = true;
    bool run_bnb = true;
    int number = -1;
    bool response_only = false;
    int sbnfit_stage = 1;

    //All of this is just to load in command-line arguments, its not that important
    const struct option longopts[] = 
    {
        {"dir", 		required_argument, 	0, 'd'},
        {"option",		required_argument,	0, 'o'},
        {"xml"	,		required_argument,	0, 'x'},
        {"topo_tag",	required_argument,	0, 't'},
        {"cosmic",		no_argument,	0, 'b'},
        {"bnb",		    no_argument,	0, 'c'},
        {"sbnfit",		required_argument,	0, 's'},
        {"help",		required_argument,	0, 'h'},
        {"number",		required_argument,	0, 'n'},
        {0,			no_argument, 		0,  0},
    };

    int iarg = 0; opterr=1; int index;
    while(iarg != -1)
    {
        iarg = getopt_long(argc,argv, "x:o:d:s:t:bcn:rh?", longopts, &index);

        switch(iarg)
        {
            case 'r':
                response_only = true;;
                break;
            case 'n':
                number = strtof(optarg,NULL);
                break;
            case 'x':
                xml = optarg;
                break;
            case 'c':
                run_cosmic = true;
                run_bnb = false;
                break;
            case 'b':
                run_cosmic = false;
                run_bnb = true;
                break;
            case 'o':
                mode_option = optarg;
                break;
            case 'd':
                dir = optarg;
                break;
            case 's':
                mode_option = "sbnfit";
                sbnfit_stage = strtod(optarg,NULL);
                break;
            case 't':
                topo_tag = optarg;
                break;
            case '?':
            case 'h':
                std::cout<<"Allowed arguments:"<<std::endl;
                std::cout<<"\t-d\t--dir\t\tDirectory for file inputs"<<std::endl;
                std::cout<<"\t-x\t--xml\t\tInput .xml file for configuring what MVA/BDT & param"<<std::endl;
                std::cout<<"\t-o\t--option\t\tOptional mode to run, train, app..etc.."<<std::endl;
                std::cout<<"\t\t\t\t Options are:"<<std::endl;
                std::cout<<"\t\t\t\t train:"<<std::endl;
                std::cout<<"\t\t\t\t app:"<<std::endl;
                std::cout<<"\t\t\t\t response:"<<std::endl;
                std::cout<<"\t\t\t\t sig:"<<std::endl;
                std::cout<<"\t\t\t\t stack:"<<std::endl;
                std::cout<<"\t\t\t\t recomc:"<<std::endl;
                std::cout<<"\t\t\t\t datamc:"<<std::endl;
                std::cout<<"\t\t\t\t eff:"<<std::endl;
                std::cout<<"\t-c\t--cosmic\t\t Run only cosmic training/app"<<std::endl;
                std::cout<<"\t-b\t--bnb\t\t Run only BNB training/app"<<std::endl;
                std::cout<<"\t-r\t--response\t\t Run only BDT response plots for datamc/recomc"<<std::endl;
                std::cout<<"\t-t\t--topo_tag\t\tTopological Tag used to keep all things clean!"<<std::endl;
                std::cout<<"\t-s\t--sbnfit\t\tProduce a sbnfit for file N at stage S"<<std::endl;
                std::cout<<"\t-h\t--help\t\tThis help menu"<<std::endl;
                return 0;
        }
    }
//
    //===========================================================================================
    //===========================================================================================
    //			Begininning of main program here!
    //===========================================================================================
    //===========================================================================================

    //Most TMVA arguments are loaded in here via XML
    std::cout<<"Getting xml variables"<<std::endl;
    MVALoader xml_methods(xml);
    std::vector<method_struct> TMVAmethods  = xml_methods.GetMethods(); 

    //This is a vector each containing a precut, they are all added together to make the whole "precut"
    std::vector<std::string> vec_precuts = TMVAmethods[0].precuts;


    //Load up variables and precut object ! ATTN: Found in variable_list.cxx in parent src/ folder
    variable_list var_list(analysis_tag);

    //Get all the variables you want to use	
    std::vector<bdt_variable> vars = var_list.all_vars;
    std::vector<bdt_variable> training_vars = var_list.train_vars;
    std::vector<bdt_variable> plotting_vars = var_list.plot_vars;   

    //We have 2 BDT's one for cosmics and one for BNB related backgrounds only
    bdt_info cosmic_bdt_info(analysis_tag, TMVAmethods[0]);
    bdt_info bnb_bdt_info(analysis_tag, TMVAmethods[1]);

    //Get all the variables you want to use	
    vars = cosmic_bdt_info.train_vars;
    vars.insert( vars.end(), cosmic_bdt_info.spec_vars.begin(), cosmic_bdt_info.spec_vars.end() );

    training_vars = cosmic_bdt_info.train_vars;
    plotting_vars = cosmic_bdt_info.spec_vars;

    std::cout<<"In  "<<cosmic_bdt_info.identifier<<" we have "<<cosmic_bdt_info.train_vars.size()<<" cosmic training variables and "<<cosmic_bdt_info.spec_vars.size()<<" spectators"<<std::endl;
    std::cout<<"In  "<<bnb_bdt_info.identifier<<" we have "<<bnb_bdt_info.train_vars.size()<<" bnb training variables and "<<bnb_bdt_info.spec_vars.size()<<" spectators"<<std::endl;

    //Train on "good" signals, defined as ones matched to the ncdelta and have little "clutter" around.	
    std::string training_signal_cut = "abs(sim_shower_pdg[0])==11" 
		    "&& sim_shower_matched[0]==1"
		    "&& sim_shower_overlay_fraction[0] < 0.5" //Non cosmic events
		    "&&0.9*mctruth_lepton_E+0.05>reco_shower_energy_max[0]/1000"
		    "&&0.7*mctruth_lepton_E-0.1<reco_shower_energy_max[0]/1000"
//		    "&&(reco_shower_energy_max[0]/1000>(mctruth_lepton_E[0]^2+0.02))"//Shape shower energy
//		    "&&(reco_shower_energy_max[0]/1000<(mctruth_lepton_E[0]*0.8+0.1))";
		    "&&reco_track_proton_kinetic_energy[0]<(mctruth_leading_exiting_proton_energy[0]-0.93828)+0.05"//Shape track energy
		    "&&reco_track_proton_kinetic_energy[0]>(mctruth_leading_exiting_proton_energy[0]-0.93828)-0.05";

    std::string training_bkg_cut = "sim_shower_overlay_fraction[0]<0.5";
    std::string num_track_cut;

    if(analysis_tag == "1e1p"){
        training_signal_cut = training_signal_cut+ "&& sim_track_matched[0]==1 && sim_track_overlay_fraction[0]< 0.5";
        training_bkg_cut = training_bkg_cut +"&& sim_track_matched[0]==1  && sim_track_overlay_fraction[0]<0.5";
        num_track_cut =  "==1";

        bnb_bdt_info.setTopoName("1e1p");
        cosmic_bdt_info.setTopoName("1e1p");
    }else{
        num_track_cut = "==0";
        bnb_bdt_info.setTopoName("1e0p");
        cosmic_bdt_info.setTopoName("1e0p");
    }

    std::string ZMIN = "0.0"; std::string ZMAX = "1036.8"; 	std::string XMIN = "0.0"; std::string XMAX = "256.35"; std::string YMIN = "-116.5"; std::string YMAX = "116.5";
    std::string pmass = "0.938272";
    std::string fid_cut = "(mctruth_nu_vertex_x >"+XMIN+"+10 && mctruth_nu_vertex_x < "+XMAX+"-10 && mctruth_nu_vertex_y >"+ YMIN+"+10 && mctruth_nu_vertex_y <"+ YMAX+"-20 && mctruth_nu_vertex_z >"+ ZMIN +" +10 && mctruth_nu_vertex_z < "+ZMAX+"-10)";

    std::vector<std::string> v_denom = {"abs(mctruth_nu_pdg)==12"," ((mctruth_num_exiting_pi0+mctruth_num_exiting_pipm) ==0)", 
				"Sum$(mctruth_leading_exiting_proton_energy-0.93828>0.04)==1" ,"mctruth_lepton_E[0]>0.02" ,fid_cut};//,"mctruth_nu_E<0.8"}; 

    std::string signal_definition = v_denom[0];

    for(int i=1; i< v_denom.size();i++){
        signal_definition += "&&" + v_denom[i];
    }

    std::string background_definition = "abs(mctruth_nu_pdg)!=12";//||mctruth_cc_or_nc==1";
//    std::string intrinsic_background = "(abs(mctruth_nu_pdg)==12&&mctruth_cc_or_nc==0)";
    std::string topological_cuts = "(reco_vertex_size > 0 && reco_asso_showers == 1 && reco_asso_tracks "+num_track_cut+")";
    std::string postcuts = "1";  //We are not currently use postcuts

    //***************************************************************************************************/
    //***********	The bdt_flows define the "flow" of the analysis, i.e what cuts at what stage  *******/
    //***************************************************************************************************/
    std::cout<<"Defining all out bdt_flows."<<std::endl;
    bdt_flow signal_training_flow   (topological_cuts, 	signal_definition +"&&"+ training_signal_cut, 	vec_precuts,	postcuts,	cosmic_bdt_info,	bnb_bdt_info);
    bdt_flow signal_flow	    (topological_cuts, 	signal_definition , 			vec_precuts,	postcuts,	cosmic_bdt_info,	bnb_bdt_info);
    bdt_flow signal_other_flow	    (topological_cuts, 	"!("+signal_definition +")", 	vec_precuts,	postcuts,	cosmic_bdt_info,	bnb_bdt_info);

    bdt_flow bkg_flow(topological_cuts,		background_definition, 			vec_precuts,	postcuts,	cosmic_bdt_info,	bnb_bdt_info);
    bdt_flow bkg_training_flow(topological_cuts,	background_definition+"&&"+ training_bkg_cut ,	vec_precuts,	postcuts,	cosmic_bdt_info,	bnb_bdt_info);

    bdt_flow data_flow(topological_cuts,		"1",		vec_precuts,	postcuts,	cosmic_bdt_info, 	bnb_bdt_info);
//    bdt_flow intrinsic_flow(topological_cuts,	intrinsic_background	,		vec_precuts,	postcuts,	cosmic_bdt_info, 	bnb_bdt_info);
    

    std::cout<<"Defining all our bdt_files."<<std::endl;
    bdt_file *training_signal   = new bdt_file(dir, "nueintrinsic_overlay_v12.2.root"	, "LEEunfoldedTraining"	, "hist" ,  "singlephoton/",  kRed-7, signal_training_flow);

    bdt_file *signal		= new bdt_file(dir, "nueintrinsic_overlay_v12.2.root"	, "LEEunfolded"		, "hist","singlephoton/",  kRed-7, signal_flow);
    bdt_file *signal_other	= new bdt_file(dir, "nueintrinsic_overlay_v12.2.root"	, "LEEunfoldedOther"	, "hist","singlephoton/",  kRed-10, signal_other_flow);
    //signal_other->fillstyle = 3391;

    bdt_file *nueintrinsic	= new bdt_file(dir,"nueintrinsic_overlay_v12.2.root"	,"NueIntrinsics"	,"hist","singlephoton/",kCyan-8,data_flow); //intrinsic_flow);


    bdt_file *dirt		= new bdt_file(dir,"dirt_overlay_v12.2.root"		,"Dirt"			,"hist","singlephoton/", kOrange-7, data_flow);

    bdt_file *training_bnb	= new bdt_file(dir, "bnb_overlay_v12.2.root"		, "BNBTrain"		, "hist","singlephoton/",  kAzure-9, bkg_training_flow);
    bdt_file *bnb		= new bdt_file(dir, "bnb_overlay_v12.2.root"		, "BNBOverlays"		, "hist","singlephoton/",  kAzure-9, bkg_flow);

    //Data files
    bdt_file *OnBeamData	= new bdt_file(dir, "data5e19_v12.2.root"		,	"OnBeamData"	, "E1p","singlephoton/",  kBlack, data_flow);
    bdt_file *OffBeamData	= new bdt_file(dir, "bnbext_run1_v12.21.root"		,	"OffBeamData"	, "E1p","singlephoton/",  kGreen-3, data_flow);

    
    
    
 /*  bdt_file *bnb_cosmics_noabs = new bdt_file(dir, "bnb_overlay_NoAbsGain.root", "BNBOverlay_noabs", "hist","singlephoton/",  kBlue-4, data_flow);
   bdt_file *bnb_cosmics_nom = new bdt_file(dir, "bnb_overlay_nominal2.root", "BNBOverlay_Nomonal", "hist","singlephoton/",  kBlue-4, data_flow);
   bdt_file *bnb_cosmics_undo = new bdt_file(dir, "bnb_overlay_undo2.root", "BNBOverlay_undo", "hist","singlephoton/",  kBlue-4, data_flow);
   bdt_file *bnb_cosmics_noyz = new bdt_file(dir, "bnb_overlay_NoYZ.root", "BNBOverlay_noyz", "hist","singlephoton/",  kBlue-4, data_flow);
*/

    //For conviencance fill a vector with pointers to all the files to loop over.
    //std::vector<bdt_file*> bdt_files = {signal, signal_other, training_signal, training_bnb, bnb, OnBeamData, OffBeamData,dirt, bnb_cosmics_noabs, bnb_cosmics_nom, bnb_cosmics_undo, bnb_cosmics_noyz};
    std::vector<bdt_file*> bdt_files = {signal,training_signal, signal_other, training_bnb, bnb, OnBeamData, OffBeamData,dirt,nueintrinsic};


    //int setAsOnBeamData(double in_tor860_wcut);
    //int setAsOffBeamData(double in_data_tor860_wcut, double in_data_spills_E1DCNT_wcut, double in_ext_spills_ext, double N_samweb_ext);
    OnBeamData->setAsOnBeamData(4.552e+19);
    OffBeamData->setAsOffBeamData(4.552e+19,10096723.0,64275293.0);
    //OnBeamData->setAsOnBeamData(4.795e19);
    //OffBeamData->setAsOffBeamData(4.795e19,10708042.0,14073757.0);//,176093.0);

    //OffBeamData->makeRunSubRunList();
    //return 0;
    std::cout<<"--------------------------------------------------------------------------"<<std::endl;
    std::cout<<"--------------------------------------------------------------------------"<<std::endl;
    for(auto &f: bdt_files){
        std::cout<<"On file "<<f->name<<std::endl;
        std::cout<<"Loading "<<f->tag<<"\t with "<<f->tvertex->GetEntries()<<"\t entries.(unweighted)"<<std::endl;
        f->calcPOT();
        std::cout<<"Scale factor is then: "<<f->scale_data<<std::endl;


	 if(f->tag.compare(0,11,"LEEunfolded",0,11)==0){
	 
	 cout<<"Add Branch as weight\n\n"<<endl;

	 f->weight_branch ="(lee_signal_weights.lee_weights)";
	 
	 f->tvertex->AddFriend("lee_signal_weights",(dir+"lee_weights_friend_for_nueintrinsic_overlay_v12.2.root").c_str());
	}

    }	


    //===========================================================================================
    //===========================================================================================
    //		Main flow of the program , using OPTIONS
    //===========================================================================================
    //===========================================================================================

    /*
        std::cout<<"Hello"<<std::endl;
        bdt_variable v_vertexMult("reco_vertex_size","(5,0,5)","Number of Pandora Neutrino-Slices in event","false","d");
        compareQuick({v_vertexMult,v_vertexMult,v_vertexMult, v_vertexMult},{bnb_cosmics_nom, bnb_cosmics_undo,bnb_cosmics_noyz,bnb_cosmics_noabs},{"1","1","1","1"} ,"wescheck_0");
        return 0;
    */




    std::cout<<"--------------------------------------------------------------------------"<<std::endl;
    std::cout<<"--------------------------------------------------------------------------"<<std::endl;

    if(mode_option != "precalc" ){

        std::cout<<"--------------------------------------------------------------------------"<<std::endl;
        std::cout<<" Going to add any precomputed tree friends, or any trained BDT responses   "<<std::endl;
        std::cout<<" If you see warnings, but havenet yet ran app stage, thats ok!            "<<std::endl;
        std::cout<<"--------------------------------------------------------------------------"<<std::endl;

        for(auto &f: bdt_files){

            if(mode_option != "app" && mode_option != "train"){
                f->addBDTResponses(cosmic_bdt_info, bnb_bdt_info, TMVAmethods);
            }
            if(mode_option != "train" && mode_option != "app"){
                f->calcBaseEntryList(analysis_tag);
            }
        }
    }
    std::cout<<"--------------------------------------------------------------------------"<<std::endl;
    std::cout<<"--------------------------------------------------------------------------"<<std::endl;


    //Adding plot names
    training_signal->addPlotName("LEE #nu_{e}(#bar{#nu}_{e})");
    signal->addPlotName("Golden LEE #nu_{e}(#bar{#nu}_{e})");
    signal_other->addPlotName("Other LEE #nu_{e}(#bar{#nu}_{e})");
    training_bnb->addPlotName("BNB Neutrino Backgrounds");
    bnb->addPlotName("BNB Neutrino Backgrounds");
    nueintrinsic->addPlotName("#nu_{e}(#bar{#nu}_{e}) Intrinsics");
    OnBeamData->addPlotName("On-Beam Data");
    OffBeamData->addPlotName("Cosmic Background");
    dirt->addPlotName("Dirt");
    std::cout<<"--------------------------------------------------------------------------"<<std::endl;
    std::cout<<"--------------------------------------------------------------------------"<<std::endl;

    double fcoscut, fbnbcut;

    if (access("sig.txt",F_OK) == -1){//no file
	std::cout<<"Warning: No sig.txt is found in the current directory. Proceed without BDT cuts."<<std::endl;
    }
    else{
	std::cout<<"Read off significance from sig.txt for coscut & bnbcut."<<std::endl;

	std::fstream sig_file("sig.txt", std::ios_base::in);

	sig_file >> fcoscut >> fbnbcut;
	std::cout<<"coscut: "<<fcoscut<<"; bnbcut: "<<fbnbcut<<std::endl;
    }
//    double fcoscut;
//    double fbnbcut;
//    if(analysis_tag == "track"){
//        fcoscut =   0.658;
//        fbnbcut =  0.61;
//    }else if(analysis_tag == "notrack"){
//        fcoscut = 0.5; //0.612701;//0.587101;
//        fbnbcut =  0.569627;
//    }

    //===========================================================================================
    //===========================================================================================
    //		Main flow of the program , using OPTIONS
    //===========================================================================================
    //===========================================================================================

    if(mode_option == "train") {
        std::cout<<"**********************Starting COSMIC BDT Training*************************"<<std::endl;
        if(run_cosmic) bdt_train(cosmic_bdt_info, training_signal, OffBeamData);

        std::cout<<"**********************Starting BNB BDT Training*************************"<<std::endl;
        if(run_bnb) bdt_train(bnb_bdt_info, training_signal, training_bnb);
        return 0;

    }else if(mode_option == "app"){
        //Apply! This will update cosmic_bdt_info, signal file and bkg file. As in update them PROPERLY!	

        if(number == -1){
            if(run_cosmic) bdt_app(cosmic_bdt_info, bdt_files);
            if(run_bnb)    bdt_app(bnb_bdt_info, bdt_files);
        }else{
            std::vector<bdt_file*> tmp_f =  {bdt_files[number]};
            if(run_cosmic) bdt_app(cosmic_bdt_info,tmp_f);
            if(run_bnb)    bdt_app(bnb_bdt_info, tmp_f);
        }
        return 0;
    }
    else if(mode_option == "response"){

	if (access("response",F_OK) == -1){
	    mkdir("response",0777);//Create a folder for pdf.
	}
	else{
	    std::cout<<"Overwrite response/ in 2 seconds, 1 seconds, ..."<<std::endl;
	    sleep(2);
	}
	TFile * ftest = new TFile(("test+"+analysis_tag+".root").c_str(),"recreate");
	//Ok print out Cosmic BDT
        if(run_cosmic){
            bdt_response cosmic_response(cosmic_bdt_info, training_signal, OffBeamData);
            cosmic_response.plot_bdt_response(ftest);
        }

        if(run_bnb){
            bdt_response bnb_response(bnb_bdt_info, training_signal, training_bnb);
            bnb_response.plot_bdt_response(ftest);
        }
    }	
    else if(mode_option == "recomc"){

	if (access("recomc",F_OK) == -1){
	    mkdir("recomc",0777);//Create a folder for pdf.
	}
	else{
	    std::cout<<"Overwrite recomc/ in 2 seconds, 1 seconds, ..."<<std::endl;
	    sleep(2);
	}


	if(true){//This section is used to search evetns that pass through BDTs
	    for(auto &f: bdt_files){
		std::cout<<"\nLooking for events that pass cosmiccut in "<< f->tag << std::endl;
		std::string cosmiccut = f->getStageCuts(2, fcoscut, -9);
		if(f->tag.compare(0,11,"BNBOverlays",0,11) == 0){
//		    f->tvertex->Scan("run_number:subrun_number:event_number:reco_shower_helper_energy[0]:reco_track_displacement[0]", cosmiccut.c_str());
//		    f->tvertex->Scan("run_number:subrun_number:event_number:mctruth_interaction_type:mctruth_nu_pdg", cosmiccut.c_str());

		}else{
		    std::cout<<"Skip this file."<< std::endl;
		}
	    }
	    for(auto &f: bdt_files){
		std::cout<<"\nLooking for events that pass both cuts in "<< f->tag << std::endl;
		std::string bnbcut = f->getStageCuts(3, fcoscut, fbnbcut);
		if(f->tag.compare(0,11,"BNBOverlays",0,11) == 0){
//		    f->tvertex->Scan("run_number:subrun_number:event_number:reco_shower_helper_energy[0]:reco_track_displacement[0]",bnbcut.c_str());
			((TTreePlayer*)(f->tvertex->GetPlayer()))->SetScanRedirect(true); 
			((TTreePlayer*)(f->tvertex->GetPlayer()))->SetScanFileName("Sneaky_Events.txt"); 
		    f->tvertex->Scan("run_number:subrun_number:event_number:mctruth_interaction_type:mctruth_nu_pdg:mctruth_cc_or_nc:mctruth_mode", bnbcut.c_str());
		}else{
		    std::cout<<"Skip this file."<< std::endl;
		}
	    }
	    return 0;
	}


	std::vector<int> recomc_cols = {kRed-7, kBlue+3, kBlue, kBlue-7, kMagenta-3, kYellow-7, kOrange-3, kGreen+1 ,kGray};

/* https://nusoft.fnal.gov/larsoft/doxsvn/html/MCNeutrino_8h_source.html;
 *
 * Reference of interaction type:
 * 1000: CC Nuance Offset
 * 1001: CC CCQE
 * 1002: NC NCQE 
 * 1003: CC ResCCNuProtonPiPlus 
 * 1004: CC ResCCNuNeutronPi0 
 * 1005: CC ResCCNuNeutronPiPlus 
 * 1006: NC ResNCNuProtonPi0
 * 1008: NC ResNuNeutronPi0 
 * 1009: NC ResNCNuNeutronPiMinus 
 * 1091: CC CCDIS 
 * 1092: NC NCDIS
 * check this: https://github.com/LArbys/LArCV/wiki/ROI-Interaction-Information-Codes
 *
 * Refernce of mctruth_mode:
 * 0: QE
 * 1: Res
 * 2: DIS
 * 3: Coh
 * 10:??
 * 
 */

	vector< vector <string> > recomc_list = {
	   { "#nu_{#mu} CC Coh"	, "mctruth_mode==3  && mctruth_nu_pdg==14 && mctruth_cc_or_nc==0"},
	   { "#nu_{#mu} NC Coh"	, "mctruth_mode==3  && mctruth_nu_pdg==14 && mctruth_cc_or_nc==1"},
	   { "#nu_{#mu} CC QE"	, "mctruth_mode==0  && mctruth_nu_pdg==14 && mctruth_cc_or_nc==0"},
	   { "#nu_{#mu} NC QE"	, "mctruth_mode==0  && mctruth_nu_pdg==14 && mctruth_cc_or_nc==1"},
	   { "#nu_{#mu} CC Res.", "mctruth_mode==1  && mctruth_nu_pdg==14 && mctruth_cc_or_nc==0"},
	   { "#nu_{#mu} NC Res.", "mctruth_mode==1  && mctruth_nu_pdg==14 && mctruth_cc_or_nc==1"},
	   { "#nu_{#mu} DIS"	, "mctruth_mode==2  && mctruth_nu_pdg==14"},
	   { "#nu_{#mu} MEC"	, "mctruth_mode==10 && mctruth_nu_pdg==14"},
	};
	/*	vector< vector <string> > recomc_list = {
	   { "#nu_{#mu} Nuance Offset","mctruth_interaction_type==1000 && mctruth_nu_pdg==14"},
	   { "#nu_{#mu} CC QE"			, "mctruth_interaction_type==1001 && mctruth_nu_pdg==14 && mctruth_cc_or_nc==0"},
	   { "#nu_{#mu} NC QE"			, "mctruth_interaction_type==1002 && mctruth_nu_pdg==14 && mctruth_cc_or_nc==1"},
//	   { "#nu_{#mu} CC Res. #pi^{+}", "(mctruth_interaction_type==1003 || mctruth_interaction_type==1005) && mctruth_nu_pdg==14 && mctruth_cc_or_nc==0"},
	   { "#nu_{#mu} CC Res."		, "mctruth_interaction_type>1002 && mctruth_interaction_type<1006 && mctruth_nu_pdg==14 && mctruth_cc_or_nc==0"},
	   { "#nu_{#mu} NC Res."		, "mctruth_interaction_type>1005 && mctruth_interaction_type<1010 && mctruth_nu_pdg==14 && mctruth_cc_or_nc==1"},
//	   { "#nu_{#mu} NC Res. #pi^{0}", "(mctruth_interaction_type==1006 || mctruth_interaction_type==1008) && mctruth_nu_pdg==14 && mctruth_cc_or_nc==1"},
	   { "#nu_{#mu} CC DIS"			, "mctruth_interaction_type==1091 && mctruth_nu_pdg==14 && mctruth_cc_or_nc==0"},
	   { "#nu_{#mu} NC DIS"			, "mctruth_interaction_type==1092 && mctruth_nu_pdg==14 && mctruth_cc_or_nc==1"},
	};
*/	
	std::vector<std::string> recomc_names, recomc_cuts;
	//prepare "others" category.
	string recomc_other_cut = "!(";

	for(int i=0; i<recomc_list.size(); i++){
	recomc_names.push_back(recomc_list[i][0]);
	recomc_cuts.push_back(recomc_list[i][1]);

	if(i>0)recomc_other_cut+="||";
	recomc_other_cut +="("+recomc_list[i][1] + ")";
	}
	recomc_other_cut += ")";

	recomc_names.push_back("others");
	recomc_cuts.push_back(recomc_other_cut);
	
//	std::vector<std::string> recomc_names = {
//	    "#nu_{#mu} CC QE",
//	    "#nu_{#mu} NC QE",
//	    "#nu_{#mu} CC Res. #pi^{+}",
//	    "#nu_{#mu} CC Res. #pi^{0}",
//	    "#nu_{#mu} NC Res. #pi^{-}",
//	    "#nu_{#mu} NC Res. #pi^{0}",
//	    "#nu_{#mu} CC DIS",
//	    "#nu_{#mu} NC DIS",
//	    "Other",
//	};
//
//	std::vector<std::string> recomc_cuts = {//Look at number of hardrons && leptons only.
//	    "mctruth_interaction_type==1001 && mctruth_nu_pdg==14 && mctruth_cc_or_nc==0",
//	    "mctruth_interaction_type==1002 && mctruth_nu_pdg==14 && mctruth_cc_or_nc==1",
//	    "(mctruth_interaction_type==1003 || mctruth_interaction_type==1005) && mctruth_nu_pdg==14 && mctruth_cc_or_nc==0",
//	    "mctruth_interaction_type==1004 && mctruth_nu_pdg==14 && mctruth_cc_or_nc==0",
//	    "mctruth_interaction_type==1009 && mctruth_nu_pdg==14 && mctruth_cc_or_nc==1",
//	    "(mctruth_interaction_type==1006 || mctruth_interaction_type==1008) && mctruth_nu_pdg==14 && mctruth_cc_or_nc==1",
//	    "mctruth_interaction_type==1091 && mctruth_nu_pdg==14 && mctruth_cc_or_nc==0",
//	    "mctruth_interaction_type==1092 && mctruth_nu_pdg==14 && mctruth_cc_or_nc==1",
//	    "(mctruth_interaction_type<1001||mctruth_interaction_type==1007||mctruth_interaction_type>1009||mctruth_interaction_type<1091||mctruth_interaction_type>1092)&&mctruth_nu_pdg!=14",
//	};

	bdt_recomc recomc(recomc_names, recomc_cuts, recomc_cols,analysis_tag);

	TFile * ftest = new TFile(("test+"+analysis_tag+".root").c_str(),"recreate");
	if(!response_only){
	    int h=0;

	    if(number !=-1){
		std::vector<bdt_variable> tmp = {vars.at(number)};
		recomc.plot_recomc(ftest, bnb, tmp, fcoscut, fbnbcut);
		return 0;

	    }else{
		recomc.plot_recomc(ftest, bnb, vars, fcoscut, fbnbcut);
	    }	
	}

	if(response_only){
	    recomc.is_log = true;
	    if(run_cosmic){
		recomc.plot_recomc(ftest, signal, (std::vector<bdt_variable>){signal->getBDTVariable(cosmic_bdt_info)} , fcoscut,fbnbcut);
		recomc.plot_recomc(ftest, bnb, (std::vector<bdt_variable>){bnb->getBDTVariable(cosmic_bdt_info)} , fcoscut,fbnbcut);
	    }
	    if(run_bnb){
		recomc.plot_recomc(ftest, bnb, (std::vector<bdt_variable>){bnb->getBDTVariable(bnb_bdt_info)} , fcoscut,fbnbcut);
		recomc.plot_recomc(ftest, signal, (std::vector<bdt_variable>){signal->getBDTVariable(bnb_bdt_info)} , fcoscut,fbnbcut);
	    }

	    recomc.is_log = false;
	}

    }

    else if(mode_option == "sig"){

	TFile *fsig = new TFile(("significance_"+analysis_tag+".root").c_str(),"recreate");
	std::vector<double> ans = scan_significance(fsig, {signal} , {bnb, nueintrinsic , OffBeamData, dirt}, cosmic_bdt_info, bnb_bdt_info);
	//std::vector<double> ans = lin_scan({signal_cosmics}, {bnb_cosmics, bnbext}, cosmic_bdt_info, bnb_bdt_info,fcoscut,fbnbcut);

	std::ofstream save_sig("sig.txt");
	save_sig<<ans.at(0)<<" "<<ans.at(1)<<std::endl;

	std::cout<<"Best Fit Significance: cosmic_cut "<<ans.at(0)<<", bnb_cut: "<<ans.  at(1)<<" with significance: "<<ans.at(2)<<"."<<std::endl;
	fsig->Close();

    }else if(mode_option == "stack"){
 
	if (access("stack",F_OK) == -1){
	    mkdir("stack",0777);//Create a folder for pdf.
	}
	else{
	    std::cout<<"Overwrite stack/ in 2 seconds, 1 seconds, ..."<<std::endl;
	    sleep(2);
	}


        bdt_stack histogram_stack(analysis_tag+"_stack");
        histogram_stack.addToStack(signal);
        histogram_stack.addToStack(signal_other);
	histogram_stack.addToStack(nueintrinsic);
	histogram_stack.addToStack(bnb);

        //Add OffBeamData but change the color and style first
        OffBeamData->col;	
        OffBeamData->fillstyle = 3333;
        histogram_stack.addToStack(OffBeamData);
        histogram_stack.addToStack(dirt);

	histogram_stack.plot_pot = 13.2e20;
        TFile * ftest = new TFile(("test+"+analysis_tag+".root").c_str(),"recreate");
        int ip=0;


        if(!response_only){
            if(number == -1){
                histogram_stack.plotStacks(ftest,vars,fcoscut,fbnbcut);
            }else{
                std::cout<<"Starting to make a stack of : "<<vars.at(number).name<<std::endl;

                std::vector<bdt_variable> v_tmp = {vars.at(number)};
                histogram_stack.plotStacks(ftest,v_tmp,fcoscut,fbnbcut);
            }
        }else{
            histogram_stack.plotBDTStacks(ftest, bnb_bdt_info, fcoscut, fbnbcut);
            histogram_stack.plotBDTStacks(ftest, cosmic_bdt_info, fcoscut, fbnbcut);
            return 0;
        }

    }else if(mode_option == "sbnfit"){
        if(number==-1) number ==0;

        bdt_file * file = bdt_files.at(number);

        //have to first add the vertex tree as a friend to the eventweight tree, you will see why later.. if i get to those comments
        file->teventweight->AddFriend(file->tvertex);

        std::string output_file_name = "sbnfit_"+analysis_tag+"_stage_"+std::to_string(sbnfit_stage)+"_"+file->tag+".root";
        
        std::cout<<"Starting to make SBNFit output file named: "<<output_file_name<<std::endl;
        TFile* f_sbnfit = new TFile(output_file_name.c_str(),"recreate");
        

        std::cout<<"Creating directory structure"<<std::endl;
        TDirectory *cdtof = f_sbnfit->mkdir("singlephoton");
        cdtof->cd();    
        
        
        std::string sbnfit_cuts = file->getStageCuts(sbnfit_stage,fcoscut,fbnbcut);
        
        std::cout<<"Copying vertex tree"<<std::endl;
        TTree * t_sbnfit_tree = (TTree*)file->tvertex->CopyTree(sbnfit_cuts.c_str());
        std::cout<<"Copying POT tree"<<std::endl;
        TTree * t_sbnfit_pot_tree = (TTree*)file->tpot->CopyTree("1");
        std::cout<<"Copying eventweight tree (via friends)"<<std::endl;
        TTree * t_sbnfit_eventweight_tree = (TTree*)file->teventweight->CopyTree(sbnfit_cuts.c_str());


        
        std::cout<<"Writing to file"<<std::endl;
        cdtof->cd();
        t_sbnfit_tree->Write();
        t_sbnfit_pot_tree->Write();
        t_sbnfit_eventweight_tree->Write(); 

        f_sbnfit->Close();
        std::cout<<"Done!"<<std::endl;


        return 0;


    }else if(mode_option == "datamc"){

	if (access("datamc",F_OK) == -1){
	    mkdir("datamc",0777);//Create a folder for pdf.
	}
	else{
	    std::cout<<"Overwrite datamc/ in 2 seconds, 1 seconds, ..."<<std::endl;
	    sleep(2);
	}


        std::cout<<"Starting datamc"<<std::endl;
        TFile * ftest = new TFile(("test+"+analysis_tag+".root").c_str(),"recreate");

        bdt_stack *histogram_stack = new bdt_stack(analysis_tag+"_datamc");
        histogram_stack->plot_pot = OnBeamData->pot;
        histogram_stack->addToStack(signal);
        histogram_stack->addToStack(signal_other);
	histogram_stack->addToStack(nueintrinsic);

        histogram_stack->addToStack(bnb);
        OffBeamData->fillstyle = 3333;
        histogram_stack->addToStack(OffBeamData);
        histogram_stack->addToStack(dirt);
        
	int ip=0;
        std::vector<bool> subv = {false,false,true};
        if(!response_only){
            if(number != -1){
                bdt_datamc datamc(OnBeamData, histogram_stack, analysis_tag+"_datamc");	

                datamc.printPassingDataEvents("tmp", 3, fcoscut, fbnbcut);

                //datamc.setSubtractionVector(subv);
                std::vector<bdt_variable> tmp_var = {vars.at(number)};
                datamc.plotStacks(ftest,  tmp_var ,fcoscut,fbnbcut);
            }else{

                bdt_datamc real_datamc(OnBeamData, histogram_stack, analysis_tag+"_datamc");	
                //real_datamc.setSubtractionVector(subv);
                // real_datamc.plotStacks(ftest, vars,fcoscut,fbnbcut);
                real_datamc.plotStacks(ftest, training_vars,fcoscut,fbnbcut);
                real_datamc.SetSpectator();
                real_datamc.plotStacks(ftest, plotting_vars,fcoscut,fbnbcut);
            }
        }else{
            bdt_datamc real_datamc(OnBeamData, histogram_stack, analysis_tag+"_datamc");	

            if(run_bnb) real_datamc.plotBDTStacks(ftest, bnb_bdt_info ,fcoscut,fbnbcut);
            if(run_cosmic) real_datamc.plotBDTStacks(ftest, cosmic_bdt_info ,fcoscut,fbnbcut);
        }

    }else if(mode_option == "vars"|| mode_option == "var"){


	if (access("var",F_OK) == -1){
	    mkdir("var",0777);//Create a folder for pdf.
	}
	else{
	    std::cout<<"Overwrite var/ in 2 seconds, 1 seconds, ..."<<std::endl;
	    sleep(2);
	}


	std::vector<std::string> title = {"All Verticies","Pre-Selection Cuts"};

	if(run_cosmic){

	    if(number != -1){
		//plot_bdt_variable(training_signal, OffBeamData, vars.at(number), cosmic_bdt_info);
		plot_bdt_variable(training_signal, OffBeamData, training_vars.at(number), cosmic_bdt_info, false);
		plot_bdt_variable(training_signal, OffBeamData, plotting_vars.at(number), cosmic_bdt_info, true);
	    }else{
		//plot_bdt_variables(training_signal, OffBeamData, vars, cosmic_bdt_info);
		plot_bdt_variables(training_signal, OffBeamData, training_vars, cosmic_bdt_info, false);
		plot_bdt_variables(training_signal, OffBeamData, plotting_vars, cosmic_bdt_info, true);
	    }

	}
	if(run_bnb){


	    if(number != -1){
		plot_bdt_variable(training_signal, training_bnb, training_vars.at(number), bnb_bdt_info, false);
		plot_bdt_variable(training_signal, training_bnb, plotting_vars.at(number), bnb_bdt_info, true);
	    }else{
		plot_bdt_variables(training_signal, training_bnb, training_vars, bnb_bdt_info, false);
		plot_bdt_variables(training_signal, training_bnb, plotting_vars, bnb_bdt_info, true);
	    }

	}


    }else if(mode_option == "eff"){

        std::vector<std::string> v_topo =  {"reco_vertex_size>0","reco_asso_showers==1","reco_asso_tracks==1"};

        bdt_efficiency(signal, v_denom, v_topo, vec_precuts, fcoscut, fbnbcut, 13.2e20);
        //bdt_efficiency(bnb, {"1"}, v_topo, vec_precuts, fcoscut, fbnbcut, 5e19);
	//bdt_efficiency(signal, signal_definition, fcoscut,fbnbcut, true);
	        


    }else {
        std::cout << "WARNING: " << mode_option << " is an invalid option\n";
    }


    return 0;

}


int compareQuick(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, std::string name){

    TCanvas *c = new TCanvas();
    c->cd(); 



    std::vector<int> cols = {kRed-7,  kBlue-7, kGreen+1 ,kGray};
    TLegend* leg=new TLegend(0.55,0.55,0.9,0.9);


    std::string testcut = "1";
    for(int i=0; i< files.size();i++){

        c->cd();
        TH1* th1 =  (TH1*) files[i]->getTH1(vars[i], testcut+"&&"+cuts[i], "photon_truth_overlay"+std::to_string(i), 6.6e20, 1);
        c->cd();

        th1->SetLineColor(cols[i]);
        th1->SetLineWidth(2);

        double norm = th1->Integral();
        th1->Scale(1.0/norm);


        th1->Draw("hist same");

        th1->SetMaximum(th1->GetMaximum()*1.5);
        th1->GetXaxis()->SetTitle(vars[i].unit.c_str());
        th1->SetTitle(vars[i].unit.c_str());

        leg->AddEntry(th1,files[i]->tag.c_str(),"l");


    }

    leg->Draw();
    c->SaveAs((name+".pdf").c_str(),"pdf");

    return 0;
};
