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


int main (int argc, char *argv[]){

    //This is a standardized location on /pnfs/ that everyone can use. 
    std::string dir = "/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_mcc9_v9/";
    std::string mydir = "/pnfs/uboone/persistent/users/amogan/singlePhoton/samples/";
    std::string dirv10 = "/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_mcc9_v10/";
    std::string dirv12 = "/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_mcc9_v12/";


    std::string mode_option = "fake"; 
    std::string xml = "../../xml/pi0_box_copy.xml";
    std::string topo_tag = "2g1p";
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
        {"cosmic",		no_argument,	0, 'c'},
        {"bnb",		    no_argument,	0, 'b'},
        {"sbnfit",		required_argument,	0, 's'},
        {"help",		required_argument,	0, 'h'},
        {"number",		required_argument,	0, 'n'},
        {0,			no_argument, 		0,  0},
    };

    int iarg = 0; opterr=1; int index;
    while(iarg != -1)
    {
        iarg = getopt_long(argc,argv, "x:o:d:t:bcn:rh?", longopts, &index);

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
    
    // Handy strings for shower indices
    std::string shower_index1 = "reco_shower_ordered_energy_index[0]";
    std::string shower_index2 = "reco_shower_ordered_energy_index[1]";

    //Get all the variables you want to use	
    vars = cosmic_bdt_info.train_vars;

    training_vars = cosmic_bdt_info.train_vars;
    plotting_vars = cosmic_bdt_info.spec_vars;

    std::cout<<"In  "<<cosmic_bdt_info.identifier<<" we have "<<cosmic_bdt_info.train_vars.size()<<" cosmic training variables and "<<cosmic_bdt_info.spec_vars.size()<<" spectators"<<std::endl;
    std::cout<<"In  "<<bnb_bdt_info.identifier<<" we have "<<bnb_bdt_info.train_vars.size()<<" bnb training variables and "<<bnb_bdt_info.spec_vars.size()<<" spectators"<<std::endl;

    //Train on "good" signals, defined as ones matched to the NC pi0 and have little "clutter" around.	
    std::string training_signal_cut = "sim_shower_pdg[0]==22 && sim_shower_pdg[1]==22 && sim_shower_parent_pdg[0]==111 && sim_shower_parent_pdg[1]==111 && sim_shower_overlay_fraction[0] < 1. && sim_shower_overlay_fraction[1]<1.";
    std::string training_bkg_cut = "sim_shower_overlay_fraction[0]<1. && sim_shower_overlay_fraction[1]<1. && !(sim_shower_pdg[0]==22 && sim_shower_pdg[1]==22 && sim_shower_parent_pdg[0]==111 && sim_shower_parent_pdg[1]==111)";
    std::string num_track_cut;

    if(analysis_tag == "2g1p"){
        training_signal_cut = training_signal_cut+ "&& sim_track_overlay_fraction[0]< 1.";
        training_bkg_cut = training_bkg_cut +"&& sim_track_overlay_fraction[0]<1.";
        num_track_cut =  "==1";

        bnb_bdt_info.setTopoName("2#gamma1p");
    }else if (analysis_tag == "2g0p") {
        num_track_cut = "==0";
        bnb_bdt_info.setTopoName("2#gamma0p");
    }
    else {
      std::cout << "Invalid analysis tag" << std::endl;
      return 1;
    }

    std::string ZMIN = "0.0"; std::string ZMAX = "1036.8"; 	std::string XMIN = "0.0"; std::string XMAX = "256.35"; std::string YMIN = "-116.5"; std::string YMAX = "116.5";
    std::string pmass = "0.938272";
    std::string fid_cut = "(mctruth_nu_vertex_x >"+XMIN+"+10 && mctruth_nu_vertex_x < "+XMAX+"-10 && mctruth_nu_vertex_y >"+ YMIN+"+20 && mctruth_nu_vertex_y <"+ YMAX+"-20 && mctruth_nu_vertex_z >"+ ZMIN +" +10 && mctruth_nu_vertex_z < "+ZMAX+"-10)";

    // MC-truth signal cuts
    std::vector<std::string> v_denom = {"mctruth_cc_or_nc == 1",
                                        "mctruth_num_exiting_pi0==1", 
                                        "mctruth_pi0_leading_photon_energy > 0.02", 
                                        "mctruth_pi0_subleading_photon_energy > 0.02", 
                                        //"mctruth_leading_exiting_proton_energy > "+pmass+"+0.04",
                                        "Sum$(mctruth_exiting_proton_energy-0.93827>0.04)==1",
                                        fid_cut
    }; 

    std::string signal_definition = v_denom[0];

    for(int i=1; i< v_denom.size();i++){
        signal_definition += "&&" + v_denom[i];
    }

    std::string background_definition = "(mctruth_num_exiting_pi0!=1 || mctruth_cc_or_nc==0)";
    std::string topological_cuts = "(reco_vertex_size > 0 && reco_asso_showers == 2 && reco_asso_tracks"+num_track_cut+")";
    std::string postcuts = "1";  //We dont currently use postcuts

    //***************************************************************************************************/
    //***********	The bdt_flows define the "flow" of the analysis, i.e what cuts at what stage  *******/
    //***************************************************************************************************/
    std::cout<<"Defining all out bdt_flows."<<std::endl;
    bdt_flow signal_training_flow(topological_cuts,signal_definition +"&&"+ training_signal_cut,vec_precuts,postcuts,cosmic_bdt_info,bnb_bdt_info);
    bdt_flow signal_other_flow(topological_cuts, 	"!("+signal_definition +")", 	vec_precuts,	postcuts,	cosmic_bdt_info,	bnb_bdt_info);

    bdt_flow signal_flow(topological_cuts, 	signal_definition , 			vec_precuts,	postcuts,	cosmic_bdt_info,	bnb_bdt_info);
    bdt_flow bkg_flow(topological_cuts,		background_definition, 			vec_precuts,	postcuts,	cosmic_bdt_info,	bnb_bdt_info);
    bdt_flow bkg_training_flow(topological_cuts,	background_definition+"&&"+ training_bkg_cut ,	vec_precuts,	postcuts,	cosmic_bdt_info,	bnb_bdt_info);

    bdt_flow data_flow(topological_cuts,		"1",		vec_precuts,	postcuts,	cosmic_bdt_info, 	bnb_bdt_info);

    ///////////////// SAMPLES /////////////////////////
    std::cout<<"Defining all our bdt_files."<<std::endl;
    // MC+Overlay files
    bdt_file *training_signal=new bdt_file(dirv12,"ncpi0_overlay_v12.2.root","NCPi0Train","hist","singlephoton/", kRed-7, signal_training_flow);
    bdt_file *signal = new bdt_file(dirv12, "ncpi0_overlay_v12.2.root", "NCPi0Overlay", "hist","singlephoton/", kRed-7, signal_flow);
    bdt_file *signal_other = new bdt_file(dirv12,"ncpi0_overlay_v12.2.root","NCPi0OverlayOther","hist","singlephoton/",kRed-10,signal_other_flow);
    //signal_other->fillstyle = 3333;

    bdt_file *training_bnb = new bdt_file(dirv12, "bnb_overlay_v12.2.root", "BNBTrain",	  "hist","singlephoton/",  kAzure-9, bkg_training_flow);
    bdt_file *bnb = new bdt_file(dirv12, "bnb_overlay_v12.2.root", "BNBOverlays", "hist","singlephoton/",  kAzure-9, bkg_flow);
    // Color was kBlue-4 or kAzure-9

    //Data files
    bdt_file *OnBeamData    = new bdt_file(dirv12, "data5e19_v12.2.root",	"OnBeamData",	   "E1p","singlephoton/",  kBlack, data_flow);
    bdt_file *OffBeamData    = new bdt_file(dirv12, "bnbext_run1_v12.21.root",	"OffBeamData",	"E1p","singlephoton/",  kGreen-3, data_flow);
    bdt_file *dirt = new bdt_file(dirv12,"dirt_overlay_v12.2.root","Dirt","hist","singlephoton/", kOrange-7, data_flow);

    //For conviencance fill a vector with pointers to all the files to loop over.
    std::vector<bdt_file*> bdt_files = {signal, signal_other, training_signal, training_bnb, bnb, OnBeamData, OffBeamData, dirt};

    //int setAsOnBeamData(double in_tor860_wcut);
    //int setAsOffBeamData(double in_data_tor860_wcut, double in_data_spills_E1DCNT_wcut, double in_ext_spills_ext, double N_samweb_ext);

    // v10
    //OnBeamData->setAsOnBeamData(4.795e19);
    //OffBeamData->setAsOffBeamData(4.795e19,10708042.0,14073757.0);//,176093.0);
    
    // v12
    OnBeamData->setAsOnBeamData(4.552e+19);
    OffBeamData->setAsOffBeamData(4.552e+19,10096723.0,64275293.0);

    //OffBeamData->makeRunSubRunList();
    //return 0;
    std::cout<<"--------------------------------------------------------------------------"<<std::endl;
    std::cout<<"--------------------------------------------------------------------------"<<std::endl;
    for(auto &f: bdt_files){
        std::cout<<"On file "<<f->name<<std::endl;
        std::cout<<"Loading "<<f->tag<<"\t with "<<f->tvertex->GetEntries()<<"\t entries.(unweighted)"<<std::endl;
        f->calcPOT();
        std::cout<<"Scale factor is then: "<<f->scale_data<<std::endl;
    }	


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
    training_signal->addPlotName("NC #pi^{0}");
    signal->addPlotName("Signal NC #pi^{0}");
    signal_other->addPlotName("Other NC #pi^{0}");
    training_bnb->addPlotName("BNB Backgrounds");
    bnb->addPlotName("BNB Backgrounds");
    OnBeamData->addPlotName("On-Beam  Data");
    OffBeamData->addPlotName("Cosmic Backgrounds");
    dirt->addPlotName("Dirt Backgrounds");
    std::cout<<"--------------------------------------------------------------------------"<<std::endl;
    std::cout<<"--------------------------------------------------------------------------"<<std::endl;


    double fcoscut;
    double fbnbcut;
    if(analysis_tag == "2g1p"){
        fcoscut =   0.653728;
        fbnbcut = 0.6804;
    }else if(analysis_tag == "2g0p"){
        fcoscut = 0.5; //0.612701;//0.587101;
        fbnbcut =  0.569627;
    }

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

        std::vector<int> recomc_cols = {kRed-7, kRed-10, kRed+1, kRed-4, kAzure+2, kAzure+5, kGreen+1, kGreen-2, kGray+1};

        std::vector<std::string>recomc_names = {"BNB NC #pi^{0}", "Other NC #pi^{0}", "NC BNB Background", "NC #Delta Radiative", "BNB CC #pi^{0}", "CC BNB Background", "Two Cosmic Showers", "One Cosmic Shower", "Other"};
        // How are they defined, cutwise?

        // NC pi0 signal: two photon showers whose true parents are pi0's, all resulting from NC BNB interaction
        std::string signal_string = "mctruth_cc_or_nc==1 && mctruth_num_exiting_pi0>0 && sim_shower_pdg[0]==22 && sim_shower_pdg[1]==22 && sim_shower_parent_pdg[0]==111 && sim_shower_parent_pdg[1]==111 && sim_track_overlay_fraction[0]<0.5 && !(mctruth_leading_exiting_proton_energy<"+pmass+"+0.04 || mctruth_pi0_leading_photon_energy<0.02 || mctruth_pi0_subleading_photon_energy<0.02)";
        // Other signal
        std::string signal_other_string = "mctruth_cc_or_nc==1 && mctruth_num_exiting_pi0>0 && sim_shower_pdg[0]==22 && sim_shower_pdg[1]==22 && sim_shower_parent_pdg[0]==111 && sim_shower_parent_pdg[1]==111 && sim_track_overlay_fraction[0]<0.5 && (mctruth_leading_exiting_proton_energy<"+pmass+"+0.04 || mctruth_pi0_leading_photon_energy<0.02 || mctruth_pi0_subleading_photon_energy<0.02)";
        // Other NC BNB events where either (a) one of the showers isn't photon-induced, or (b) one doesn't come from pi0
        std::string nc_bnb = "mctruth_cc_or_nc == 1 && mctruth_is_delta_radiative!=1 && sim_shower_overlay_fraction[0]<0.5 && sim_shower_overlay_fraction[1]<0.5 && sim_track_overlay_fraction[0]<0.5 && !(sim_shower_pdg[0]==22 && sim_shower_pdg[1]==22 && sim_shower_parent_pdg==111 && sim_shower_parent_pdg[1]==111)";
        // NC delta radiative decay; very small subset of NC background
        std::string ncdelta = "sim_shower_pdg["+shower_index1+"] == 22 && sim_shower_parent_pdg["+shower_index1+"] != 111 && sim_shower_overlay_fraction["+shower_index1+"]<0.5 && sim_shower_overlay_fraction["+shower_index2+"]>0.5 && mctruth_is_delta_radiative == 1";
        // CC pi0 background
        std::string cc_pi0 = "mctruth_cc_or_nc==0 && sim_shower_overlay_fraction[0]<0.2 && sim_shower_overlay_fraction[1]<0.2 && mctruth_num_exiting_pi0==1 && sim_shower_pdg[0]==22 && sim_shower_pdg[1]==22 && sim_shower_parent_pdg[0]==111 && sim_shower_parent_pdg[1]==111 && sim_track_overlay_fraction[0]<0.2";
        // CC other
        std::string cc_bnb = "mctruth_cc_or_nc==0 && sim_shower_overlay_fraction[0]<0.2 && sim_shower_overlay_fraction[1]<0.2 && sim_track_overlay_fraction[0]<0.2 && (sim_shower_pdg[0] != 22 || sim_shower_pdg[1] != 22 || sim_shower_parent_pdg[0] != 111 || sim_shower_parent_pdg[1] != 111)";
        // Cosmics (both?)
        std::string two_cosmic = "sim_shower_overlay_fraction[0]>0.8 && sim_shower_overlay_fraction[1]>0.8";
        std::string one_cosmic = "(sim_shower_overlay_fraction[0]>0.8 || sim_shower_overlay_fraction[1]>0.8) && !(sim_shower_overlay_fraction[0]>0.8 && sim_shower_overlay_fraction[1]>0.8)";
        // Other; defined as "!" versions of above
        std::string other = "!("+signal_string+") && !("+signal_other_string+") && !("+nc_bnb+") && !("+ncdelta+") && !("+cc_pi0+") && !("+cc_bnb+") && !("+two_cosmic+") && !("+one_cosmic+")";
       
        //std::vector<std::string> recomc_cuts = {signal_string, bkg1, bkg2, bkg3, bkg4, bkg5, bkg6, other}; 
        std::vector<std::string> recomc_cuts = {signal_string, signal_other_string, nc_bnb, ncdelta, cc_pi0, cc_bnb, two_cosmic, one_cosmic, other}; 

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
        std::vector<double> ans = scan_significance(fsig, {signal} , {bnb, OffBeamData, dirt}, cosmic_bdt_info, bnb_bdt_info);
        //std::vector<double> ans = lin_scan({signal}, {bnb, OffBeamData}, cosmic_bdt_info, bnb_bdt_info,fcoscut,fbnbcut);

        std::cout<<"Best Fit Significance: "<<ans.at(0)<<" "<<ans.at(1)<<" "<<ans.at(2)<<std::endl;
        fsig->Close();


    }else if(mode_option == "stack"){
        bdt_stack histogram_stack(analysis_tag+"_stack");
        histogram_stack.addToStack(signal);
        histogram_stack.addToStack(signal_other);
        histogram_stack.addToStack(bnb);
        histogram_stack.addToStack(dirt);

        //Add OffBeamData but change the color and style first
        OffBeamData->col;	
        OffBeamData->fillstyle = 3333;
        histogram_stack.addToStack(OffBeamData);
        
        // Reverse order for last two stages
        /*
        OffBeamData->col;	
        OffBeamData->fillstyle = 3333;
        histogram_stack.addToStack(OffBeamData);
        histogram_stack.addToStack(dirt);
        histogram_stack.addToStack(bnb);
        histogram_stack.addToStack(signal_other);
        histogram_stack.addToStack(signal);
        */

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



    }else if(mode_option == "datamc"){
        std::cout<<"Starting datamc"<<std::endl;
        TFile * ftest = new TFile(("test+"+analysis_tag+".root").c_str(),"recreate");

        bdt_stack *histogram_stack = new bdt_stack(analysis_tag+"_datamc");
        histogram_stack->plot_pot = OnBeamData->pot;
        histogram_stack->addToStack(signal);
        histogram_stack->addToStack(signal_other);
        histogram_stack->addToStack(bnb);
        OffBeamData->fillstyle = 3333;
        histogram_stack->addToStack(OffBeamData);
        histogram_stack->addToStack(dirt);

        // Reverse order for last two stages
        /*
        histogram_stack->plot_pot = OnBeamData->pot;
        histogram_stack->addToStack(OffBeamData);
        histogram_stack->addToStack(dirt);
        histogram_stack->addToStack(bnb);
        histogram_stack->addToStack(signal_other);
        histogram_stack->addToStack(signal);
        OffBeamData->fillstyle = 3333;
        */

        int ip=0;
        std::vector<bool> subv = {false,false,true};
        if(!response_only){
            if(number != -1){
                bdt_datamc datamc(OnBeamData, histogram_stack, analysis_tag+"_datamc");	

                //datamc.printPassingDataEvents("tmp", 3, fcoscut, fbnbcut);

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

    }else if(mode_option == "vars"){

        std::vector<std::string> title = {"All Vertices","Pre-Selection Cuts"};

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


    } else if(mode_option == "eff"){

        std::vector<std::string> v_topo =  {"reco_vertex_size>0","reco_asso_showers==2","reco_asso_tracks==1"};

        bdt_efficiency(signal, v_denom, v_topo, vec_precuts, fcoscut, fbnbcut, 13.2e20);
        //bdt_efficiency(bnb, {"1"}, v_topo, vec_precuts, fcoscut, fbnbcut, 5e19);


    }else if(mode_option == "sbnfit"){
        if(number==-1) number ==0;

        bdt_file * file = bdt_files.at(number);

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
    }else {
        std::cout << "WARNING: " << mode_option << " is an invalid option\n";
    }


    return 0;

}
