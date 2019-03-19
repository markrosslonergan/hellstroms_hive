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


    std::string mode_option = "fake"; 
    std::string xml = "default.xml";
    std::string analysis_tag ="track";


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
        {"tag",			required_argument,	0, 't'},
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
                analysis_tag = optarg;
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
                std::cout<<"\t-t\t--tag\t\tAnalysis tag used to keep all things clean!"<<std::endl;
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

    //Load up variables and precut object ! ATTN: Found in variable_list.cxx in parent src/ folder
    variable_list var_list(analysis_tag);

    //Get all the variables you want to use	
    std::vector<bdt_variable> vars = var_list.all_vars;
    std::vector<bdt_variable> training_vars = var_list.train_vars;
    std::vector<bdt_variable> plotting_vars = var_list.plot_vars;   

    std::cout<<"Adding a total of "<<vars.size()<<" vars!"<<std::endl;

    //This is a vector each containing a precut, they are all added together to make the whole "precut"
    std::vector<std::string> vec_precuts = var_list.all_precuts;

    //We dont currently use postcuts
    std::string postcuts = "1";

    //We have 2 BDT's one for cosmics and one for BNB related backgrounds only
    //Set up some info about the BDTs to pass along
    bdt_info bnb_bdt_info("bnb_"+analysis_tag, "BNB focused BDT","(80,0.3,0.6)");
    bdt_info cosmic_bdt_info("cosmic_"+analysis_tag, "Cosmic focused BDT","(80,0.2,0.75)");

    //Train on "good" signals, defined as ones matched to the ncdelta and have little "clutter" around.	
    std::string training_signal_cut = "sim_shower_pdg[0]==22 && sim_shower_parent_pdg[0] ==-1 && sim_shower_overlay_fraction[0] < 0.5";
    std::string training_bkg_cut    = "sim_shower_overlay_fraction[0]<0.5";
    std::string num_track_cut;

    if(analysis_tag == "track"){
        training_signal_cut = training_signal_cut+ "&& sim_track_overlay_fraction[0]< 0.5";
        training_bkg_cut = training_bkg_cut +"&& sim_track_overlay_fraction[0]<0.5";
        num_track_cut =  "==1";

        bnb_bdt_info.setTopoName("1#gamma1p");
        cosmic_bdt_info.setTopoName("1#gamma1p");
    }else{
        num_track_cut = "==0";
        bnb_bdt_info.setTopoName("1#gamma0p");
        cosmic_bdt_info.setTopoName("1#gamma0p");
    }

    std::string ZMIN = "0.0"; std::string ZMAX = "1036.8"; 	std::string XMIN = "0.0"; std::string XMAX = "256.35"; std::string YMIN = "-116.5"; std::string YMAX = "116.5";
    std::string pmass = "0.938272";
    std::string fid_cut = "(mctruth_nu_vertex_x >"+XMIN+"+10 && mctruth_nu_vertex_x < "+XMAX+"-10 && mctruth_nu_vertex_y >"+ YMIN+"+20 && mctruth_nu_vertex_y <"+ YMAX+"-20 && mctruth_nu_vertex_z >"+ ZMIN +" +10 && mctruth_nu_vertex_z < "+ZMAX+"-10)";

    std::vector<std::string> v_denom = {"mctruth_cc_or_nc == 1","mctruth_is_delta_radiative" ,"mctruth_num_exiting_pi0==0", "mctruth_exiting_photon_energy > 0.02", "mctruth_leading_exiting_proton_energy > "+pmass+"+0.04",fid_cut}; 

    std::string signal_definition = v_denom[0];

    for(int i=1; i< v_denom.size();i++){
        signal_definition += "&&" + v_denom[i];
    }

    std::string background_definition = "1";
    std::string topological_cuts = "reco_vertex_size > 0 && reco_asso_showers == 1 && reco_asso_tracks "+num_track_cut;



    //***************************************************************************************************/
    //***********	The bdt_flows define the "flow" of the analysis, i.e what cuts at what stage  *******/
    //***************************************************************************************************/
    bdt_flow signal_training_flow(topological_cuts, 	signal_definition +"&&"+ training_signal_cut, 	vec_precuts,	postcuts,	cosmic_bdt_info,	bnb_bdt_info);
    bdt_flow signal_other_flow(topological_cuts, 	"!("+signal_definition +")", 	vec_precuts,	postcuts,	cosmic_bdt_info,	bnb_bdt_info);

    bdt_flow signal_flow(topological_cuts, 	signal_definition , 			vec_precuts,	postcuts,	cosmic_bdt_info,	bnb_bdt_info);
    bdt_flow bkg_flow(topological_cuts,		background_definition, 			vec_precuts,	postcuts,	cosmic_bdt_info,	bnb_bdt_info);
    bdt_flow bkg_training_flow(topological_cuts,	background_definition+"&&"+ training_bkg_cut ,	vec_precuts,	postcuts,	cosmic_bdt_info,	bnb_bdt_info);

    bdt_flow data_flow(topological_cuts,		"1",					vec_precuts,	postcuts,	cosmic_bdt_info, 	bnb_bdt_info);

    // BDt files , bdt_file::bdt_file(std::string indir,std::string inname, std::string intag, std::string inops, std::string inrootdir, int incol, bdt_flow inflow) :
    bdt_file *training_signal    = new bdt_file(dir, "ncdeltarad_overlay_collins_v9.1.root",	"NCDeltaRadTrain",	   "hist","singlephoton/",  kRed-7, signal_training_flow);
    bdt_file *signal = new bdt_file(dir, "ncdeltarad_overlay_collins_v9.1.root", "NCDeltaRadOverlay", "hist","singlephoton/",  kRed-7, signal_flow);
    bdt_file *signal_other = new bdt_file(dir, "ncdeltarad_overlay_collins_v9.1.root", "NCDeltaRadOverlayOther", "hist","singlephoton/",  kRed-7, signal_other_flow);
    signal_other->fillstyle = 3333;

    bdt_file *training_bnb    = new bdt_file(dir, "bnb_overlay_collins_v9.1.root", "BNBTrain",	  "hist","singlephoton/",  kBlue-4, bkg_training_flow);
    bdt_file *bnb = new bdt_file(dir, "bnb_overlay_collins_v9.1.root", "BNBOverlays", "hist","singlephoton/",  kBlue-4, bkg_flow);

    //Data files
    bdt_file *OnBeamData    = new bdt_file(dir, "data5e19_v9.0.root",	"OnBeamData",	   "E1p","singlephoton/",  kBlack, data_flow);
    bdt_file *OffBeamData    = new bdt_file(dir, "bnbext_run1_v9.0.root",	"OffBeamData",	"E1p","singlephoton/",  kGreen-3, data_flow);

    //For conviencance fill a vector with pointers to all the files to loop over.
    std::vector<bdt_file*> bdt_files = {signal, signal_other, training_signal, training_bnb, bnb, OnBeamData, OffBeamData};

    //The LEE signal is bigger than the SM signal by this factor
    training_signal->scale_data = 3.0;
    signal->scale_data = 3.0;

    //int setAsOnBeamData(double in_tor860_wcut);
    //int setAsOffBeamData(double in_data_tor860_wcut, double in_data_spills_E1DCNT_wcut, double in_ext_spills_ext, double N_samweb_ext);
    OnBeamData->setAsOnBeamData(4.795e19);
    OffBeamData->setAsOffBeamData(4.795e19,10708042.0,10078674.0);//,176093.0);

    //OffBeamData->makeRunSubRunList();

    std::cout<<"--------------------------------------------------------------------------"<<std::endl;
    std::cout<<"--------------------------------------------------------------------------"<<std::endl;
    for(auto &f: bdt_files){
        std::cout<<"Loading "<<f->tag<<"\t with "<<f->tvertex->GetEntries()<<"\t verticies. (unweighted)"<<std::endl;
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
    training_signal->addPlotName("NC Delta Radiative");
    signal->addPlotName("LEE NC #Delta Rad w/ Overlays");
    signal_other->addPlotName("Other w/ Overlays");
    training_bnb->addPlotName("BNB Backgrounds");
    bnb->addPlotName("BNB w/Overlays");
    OnBeamData->addPlotName("On-Beam  Data");
    OffBeamData->addPlotName("Off-Beam Data");

    std::cout<<"--------------------------------------------------------------------------"<<std::endl;
    std::cout<<"--------------------------------------------------------------------------"<<std::endl;


    double fcoscut;
    double fbnbcut;
    if(analysis_tag == "track"){
        fcoscut =   0.45;
        fbnbcut = 0.54;
    }else if(analysis_tag == "notrack"){
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
        if(run_cosmic) bdt_train(cosmic_bdt_info, training_signal, OffBeamData, training_vars, plotting_vars, TMVAmethods);
        std::cout<<"**********************Starting BNB BDT Training*************************"<<std::endl;
        if(run_bnb) bdt_train(bnb_bdt_info, training_signal, training_bnb, training_vars,  plotting_vars, TMVAmethods);
        return 0;

    }else if(mode_option == "app"){
        //Apply! This will update cosmic_bdt_info, signal file and bkg file. As in update them PROPERLY!	

        if(number != -1){
            if(run_cosmic) bdt_app(cosmic_bdt_info, bdt_files, training_vars,  plotting_vars, TMVAmethods);
            if(run_bnb)    bdt_app(bnb_bdt_info, bdt_files, training_vars,  plotting_vars, TMVAmethods);
        }else{
            if(run_cosmic) bdt_app(cosmic_bdt_info, {bdt_files[number]}, training_vars, plotting_vars, TMVAmethods);
            if(run_bnb)    bdt_app(bnb_bdt_info, {bdt_files[number]}, training_vars, plotting_vars, TMVAmethods);
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
        std::vector<double> ans = scan_significance(fsig, {signal} , {bnb, OffBeamData}, cosmic_bdt_info, bnb_bdt_info);
        //std::vector<double> ans = lin_scan({signal}, {bnb, OffBeamData}, cosmic_bdt_info, bnb_bdt_info,fcoscut,fbnbcut);

        std::cout<<"Best Fit Significance: "<<ans.at(0)<<" "<<ans.at(1)<<" "<<ans.at(2)<<std::endl;
        fsig->Close();


    }else if(mode_option == "stack"){
        bdt_stack histogram_stack(analysis_tag+"_stack");
        histogram_stack.addToStack(signal);
        histogram_stack.addToStack(signal_other);
        histogram_stack.addToStack(bnb);

        //Add OffBeamData but change the color and style first
        OffBeamData->col;	
        OffBeamData->fillstyle = 3333;
        histogram_stack.addToStack(OffBeamData);
        //histogram_stack.addToStack(dirt);

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

        std::vector<std::string> title = {"All Verticies","Pre-Selection Cuts"};

        if(run_cosmic){

            if(number != -1){
                plot_bdt_variable(training_signal, OffBeamData, vars.at(number), cosmic_bdt_info);
            }else{
                plot_bdt_variables(training_signal, OffBeamData, vars, cosmic_bdt_info);
            }

        }
        if(run_bnb){


            if(number != -1){
                plot_bdt_variable(training_signal, training_bnb, vars.at(number), bnb_bdt_info);
            }else{
                plot_bdt_variables(training_signal, training_bnb, vars, bnb_bdt_info);
            }

        }


    } else if(mode_option == "eff"){

        std::vector<std::string> v_topo =  {"reco_vertex_size>0","reco_asso_showers==1","reco_asso_tracks==1"};

        bdt_efficiency(signal, v_denom, v_topo, vec_precuts, fcoscut, fbnbcut, 13.2e20);
        //bdt_efficiency(bnb, {"1"}, v_topo, vec_precuts, fcoscut, fbnbcut, 5e19);


    }else {
        std::cout << "WARNING: " << mode_option << " is an invalid option\n";
    }


    return 0;

}
