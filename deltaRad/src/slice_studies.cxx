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
#include "bdt_vertex_eff.h"


int validateOverlay(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, bdt_file* data, std::string datacut, std::string pdfname, std::string analysis_tag);
int validateOverlay(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, bdt_file* data, std::string datacut, std::string pdfname,bool islog,std::string analysis_tag);
//int validateOverlay(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, std::vector<bdt_file*> datas, std::string datacut, std::string pdfname, bool islog, bool cutall);
int validateOverlay(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, std::vector<bdt_file *> datas, std::string datacut, TCanvas *c, int which_c, std::string pdfname, bool islog, bool cutup);

int validateOverlay(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, std::vector<bdt_file *> datas, std::string datacut, std::string pdfname, bool islog, bool cutup, std::string analysis_tag);

int validateOverlay2(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, std::string pdfname);
int makeIncrementPlots(std::string name, bdt_variable variable, std::string cut_variable, bdt_file* bnb_cosmics, bdt_file* data5e19, bdt_file* bnbext, int n_increments, double cut_val_max, double cut_val_min, std::string base_data_conditions, std::string base_mc_conditions);

std::string getAnglewrtWire(int plane, std::string var_in_x, std::string var_in_y);

int compareQuick(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, std::string name);

int validateOverlay3(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts,bool islog, bool cutup,  std::string pdfname, std::string xlabel,std::string ylabel, std::string analysis_tag);


int main (int argc, char *argv[]){

    //location of the input file, should move to pnfs really...
    //    std::string dir = "uboone/app/users/ksutton/mcc9_singlephoton_v4/srcs/ubana/ubana/SinglePhotonAnalysis/v12_vertexed_singlephoton.root";
    std::string dir = "uboone/app/users/ksutton/mcc9_singlephoton_v4/srcs/ubana/ubana/SinglePhotonAnalysis/v12_1000_vertexed_v3.root";


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



    //We dont currently use postcuts
    std::string postcuts = "1";

    bdt_info bnb_bdt_info("bnb_"+analysis_tag, "BNB focused BDT","(60,0.2,0.7");
    bdt_info cosmic_bdt_info("cosmic_"+analysis_tag, "Cosmic focused BDT","(80,0.1,0.8)");

    // std::string base_cuts = "reco_vertex_size==1 && reco_asso_showers > 0";
    // std::string signal_definition = "1";
    // std::string background_definition = "1";
    // std::vector<std::string> vec_precuts = {"1"};

    //Train on "good" signals, defined as ones matched to the ncdelta and have little "clutter" around.	
    std::string training_signal_cut = "sim_shower_pdg[0]==22 && sim_shower_overlay_fraction[0] < 0.5";
    std::string training_bkg_cut    = "sim_shower_overlay_fraction[0]<1.0";
    std::string num_track_cut;

    if(analysis_tag == "track"){
        training_signal_cut = training_signal_cut+ "&& sim_track_overlay_fraction[0]< 0.5";
        training_bkg_cut = training_bkg_cut +"&& sim_track_overlay_fraction[0]<1";
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

    std::vector<std::string> v_denom;
    if(analysis_tag == "notrack"){
        v_denom = {"mctruth_cc_or_nc == 1","mctruth_is_delta_radiative" ,"mctruth_num_exiting_pi0==0", "mctruth_exiting_photon_energy > 0.02",     "mctruth_leading_exiting_proton_energy < "+pmass+"+0.04",fid_cut}; 

    } else{
        //        v_denom = {"mctruth_cc_or_nc == 1","mctruth_is_delta_radiative" ,"mctruth_num_exiting_pi0==0", "mctruth_exiting_photon_energy > 0.02",     "mctruth_leading_exiting_proton_energy > "+pmass+"+0.04",fid_cut}; 
        v_denom = {"mctruth_cc_or_nc == 1", "mctruth_is_delta_radiative", "mctruth_num_exiting_pi0==0", "mctruth_leading_exiting_proton_energy > "+pmass+"+0.04",fid_cut};       
        //  v_denom = {"mctruth_cc_or_nc == 1", "mctruth_is_delta_radiative", "mctruth_num_exiting_pi0==0", fid_cut};       
    }


    std::string signal_definition = v_denom[0];

    for(int i=1; i< v_denom.size();i++){
        signal_definition += "&&" + v_denom[i];
    }

    //temporarily checking for all events in file
    // signal_definition = "1";
    std::string background_definition = "!mctruth_is_delta_radiative";

    std::string topological_cuts = "(reco_vertex_size > 0 && reco_asso_showers == 1 && reco_asso_tracks "+num_track_cut+")";
    //std::string topological_cuts = "1";

    std::vector<std::string> vec_precuts = {"1"}; //will need to put in the real precuts here



    //***************************************************************************************************/
    //***********	The bdt_flows define the "flow" of the analysis, i.e what cuts at what stage  *******/
    //***************************************************************************************************/
    bdt_flow signal_flow(topological_cuts, 	signal_definition, vec_precuts,	postcuts,	cosmic_bdt_info,	bnb_bdt_info);
    bdt_file *signal= new bdt_file("", dir, "NCDeltaRad", "hist","singlephoton/",  kRed-7, signal_flow);

    std::vector<bdt_file *> files = {signal};

    for(auto &f: files){
        f->calcPOT();
    }

    std::vector<bdt_file*> bdt_files = files ;//{bnb_cosmics, data5e19, bnbext};


    std::cout<<"--------------------------------------------------------------------------"<<std::endl;
    std::cout<<"--------------------------------------------------------------------------"<<std::endl;
    for(auto &f: bdt_files){
        std::cout<<"Loading "<<f->tag<<"\t with "<<f->tvertex->GetEntries()<<"\t verticies. (unweighted)"<<std::endl;
        std::cout<<"\t with "<<f->tslice->GetEntries()<<"\t entries in slice tree.(unweighted)"<<std::endl;
        std::cout<<"POT of file loaded is: "<<f->pot<<"\t\t "<<std::endl;
        std::cout<<"Scale factor is then: "<<f->scale_data<<std::endl;
        //add the vertex tree as a friend to the slice tree 
        f->tslice->AddFriend(f->tvertex);
    }	

    std::cout<<"--------------------------------------------------------------------------"<<std::endl;
    std::cout<<"--------------------------------------------------------------------------"<<std::endl;

    if(mode_option != "precalc" ){
        std::cout<<"--------------------------------------------------------------------------"<<std::endl;
        std::cout<<" Going to add any precomputed tree friends, or any trained BDT responses   "<<std::endl;
        std::cout<<" If you see warnings, but havenet yet ran app stage, thats ok!            "<<std::endl;
        std::cout<<"--------------------------------------------------------------------------"<<std::endl;
        /*  for(auto &f: bdt_files){

            std::cout<<"Filling Base EntryLists on File  "<<f->tag<<std::endl;
            if(mode_option != "train" && mode_option != "eff"){
            f->calcBaseEntryList(analysis_tag);
            }*/
    }
    std::cout<<"--------------------------------------------------------------------------"<<std::endl;
    std::cout<<"--------------------------------------------------------------------------"<<std::endl;

    //Adding plot names
    signal->addPlotName("NC #Delta Rad w/ Overlay");

    std::cout<<"--------------------------------------------------------------------------"<<std::endl;
    std::cout<<"--------------------------------------------------------------------------"<<std::endl;

    //===========================================================================================
    //===========================================================================================
    //		Main flow of the program , using OPTIONS
    //===========================================================================================
    //===========================================================================================

    if(mode_option == "valid"){

        bdt_variable v_mctruth_exiting_photon_energy("mctruth_exiting_photon_energy","(20,0,0.4)","mctruth_exiting_photon_energy",false,"d");
        validateOverlay({ v_mctruth_exiting_photon_energy},{signal}, {signal_definition}, {},"1", "mctruth_exiting_photon_energy",false,false, analysis_tag);


        std::string s_is_matched_1g0p = "is_matched_1g0p";
        bdt_variable v_is_matched_1g0p(s_is_matched_1g0p,"(4,-1,3)","Is matched 1g0p",false,"i");
        validateOverlay({ v_is_matched_1g0p},{signal}, {signal_definition}, {},"1", "matched_1g0p",false,false, analysis_tag);


        std::string s_is_matched_1g0p_in_nuslice = " reco_1g0p_is_nuslice";
        bdt_variable v_is_matched_1g0p_in_nuslice(s_is_matched_1g0p_in_nuslice,"(4,-1,3)","Is matched 1g0p in Selected Neutrino Slice",false,"i");
        validateOverlay({v_is_matched_1g0p_in_nuslice},{signal}, {signal_definition}, {},"1", "matched_1g0p_nuslice",false,false, analysis_tag);

        std::string s_is_matched_1g1p = "is_matched_1g1p";
        bdt_variable v_is_matched_1g1p(s_is_matched_1g1p,"(4,-1,3)","Is matched 1g1p",false,"i");
        validateOverlay({ v_is_matched_1g1p},{signal}, {signal_definition}, {},"1", "matched_1g1p",false,false, analysis_tag);


        std::string s_is_matched_1g1p_in_nuslice = " reco_1g1p_is_nuslice";
        bdt_variable v_is_matched_1g1p_in_nuslice(s_is_matched_1g1p_in_nuslice,"(4,-1,3)","Is matched 1g1p in Selected Neutrino Slice",false,"i");
        validateOverlay({v_is_matched_1g1p_in_nuslice},{signal}, {signal_definition}, {},"1", "matched_1g1p_nuslice",false,false, analysis_tag);


        std::string s_no_matched_showers = "no_matched_showers";
        bdt_variable v_no_matched_showers(s_no_matched_showers,"(4,-1,3)","no matched showers in the event",false,"i");
        validateOverlay({v_no_matched_showers},{signal}, {signal_definition}, {},"1", "no_matched_showers",false,false, analysis_tag);

        std::string s_matched_signal_shower_nuscore = "matched_signal_shower_nuscore";
        bdt_variable v_matched_signal_shower_nuscore(s_matched_signal_shower_nuscore,"(20,0,1)","matched_signal_shower_nuscore",false,"d");
        validateOverlay({v_matched_signal_shower_nuscore},{signal}, {signal_definition}, {},"1", "matched_signal_shower_nuscore",false,false, analysis_tag);

        bdt_variable v_matched_signal_is_nuslice("matched_signal_shower_is_nuslice","(4,-1,3)","matched_signal_shower_is_nuslice",false,"d");
        validateOverlay({ v_matched_signal_is_nuslice},{signal}, {"1"}, {},"1", "matched_signal_shower_is_nuslice",false,false, analysis_tag);



        std::string proton_cut = "sim_track_pdg== 2212 ";
        bdt_variable v_reco_track_trackscore("reco_track_trackscore","(20,0,1)","reco_track_trackscore",false,"d");
        validateOverlay({v_reco_track_trackscore},{signal}, {signal_definition, proton_cut}, {},"1", "reco_track_trackscore_proton",false,false, analysis_tag);

        bdt_variable v_reco_shower_trackscore("reco_shower_trackscore","(20,0,1)","reco_shower_trackscore",false,"d");
        validateOverlay({v_reco_shower_trackscore},{signal}, {signal_definition, proton_cut}, {},"1", "reco_shower_trackscore_proton",false,false, analysis_tag);

        if (analysis_tag == "track"){
            //need to stack the track-shower score for protons
            validateOverlay3({v_reco_shower_trackscore, v_reco_track_trackscore},{signal}, {signal_definition, proton_cut, "mctruth_leading_exiting_proton_energy > "+pmass+"+0.04"},false, false, "reco_trackscore_proton_signaldef","Track/Shower score for true protons","Number of reconstructed objects", analysis_tag);

            std::string sig_no_energy =  "mctruth_cc_or_nc == 1 &&mctruth_is_delta_radiative && mctruth_num_exiting_pi0==0 &&" + fid_cut;
            validateOverlay3({v_reco_shower_trackscore, v_reco_track_trackscore},{signal}, {sig_no_energy, proton_cut},false, false, "reco_trackscore_proton_signaldef_noprotonKEcut","Track/Shower score for true protons","Number of reconstructed objects", analysis_tag);
        } else{

            validateOverlay3({v_reco_shower_trackscore, v_reco_track_trackscore},{signal}, {signal_definition, proton_cut},false, false, "reco_trackscore_proton_signaldef","Track/Shower score for true protons","Number of reconstructed objects", analysis_tag);

        }


        //validateOverlay3({v_reco_shower_trackscore, v_reco_track_trackscore},{signal}, {signal_definition, topological_cuts, proton_cut},false, false, "reco_trackscore_proton_topodef","Track/Shower score for true protons","Number of reconstructed true protons", analysis_tag);



        std::string photon_cut = "sim_shower_pdg== 22";
        //   bdt_variable v_reco_track_trackscore("reco_track_trackscore","(20,0,1)","reco_track_trackscore_photon",false,"d");
        validateOverlay({v_reco_track_trackscore},{signal}, {signal_definition, photon_cut}, {},"1", "reco_track_trackscore_photon",false,false, analysis_tag);

        // bdt_variable v_reco_shower_trackscore("reco_shower_trackscore","(20,0,1)","reco_shower_trackscore_photon",false,"d");
        validateOverlay({v_reco_shower_trackscore},{signal}, {signal_definition, photon_cut}, {},"1", "reco_shower_trackscore_photon",false,false, analysis_tag);

        validateOverlay3({v_reco_shower_trackscore, v_reco_track_trackscore},{signal}, {signal_definition, photon_cut},false, false, "reco_trackscore_photon_signaldef","Track/Shower score for true photons","Number of reconstructed objects", analysis_tag);


        // std::string photon_shower_reco_cuts  = "sim_shower_pdg== 22";
        std::string photon_shower_reco_cuts  = "sim_shower_pdg== 22 && reco_shower_energy_max > 0.02";
        bdt_variable v_matched_signal_shower_is_nuslice("reco_shower_is_nuslice","(4,-1,2)","matched_signal_shower_isnuslice",false,"d");
        validateOverlay({ v_matched_signal_shower_is_nuslice},{signal}, {signal_definition, photon_shower_reco_cuts}, {},"1", "matched_signal_shower_is_nuslice",false,false, analysis_tag);

        std::string photon_shower_nuslice_reco_cuts  = "sim_shower_pdg== 22 && reco_shower_energy_max > 0.02 && reco_shower_is_nuslice";
        bdt_variable v_reco_shower_nuscore("reco_shower_nuscore","(20,0,1)","matched_signal_shower_nuscore",false,"d");
        validateOverlay({ v_reco_shower_nuscore},{signal}, {signal_definition, photon_shower_nuslice_reco_cuts}, {},"1", "reco_shower_nuscore",false,false, analysis_tag);


        bdt_variable v_reco_shower_energy("reco_shower_energy_max","(20,0,1000)","reco_shower_energy",false,"d");

        // validateOverlay({ v_reco_shower_energy},{signal}, {signal_definition}, {},"1", "reco_shower_energy",false,false, analysis_tag);


        bdt_variable v_num_reco_objects_per_slice("reco_slice_num_showers + reco_slice_num_tracks","(10,0,10)","num_reco_objects_per_slice",false,"d");
        bdt_variable v_num_reco_showers_per_slice("reco_slice_num_showers","(10,0,10)","num_reco_showers_per_slice",false,"d");
        bdt_variable v_num_reco_tracks_per_slice("reco_slice_num_tracks","(10,0,10)","num_reco_tracks_per_slice",false,"d");


        validateOverlay({v_num_reco_showers_per_slice},{signal}, {signal_definition}, {},"1", "num_reco_showers_per_slice",false,false, analysis_tag);
        validateOverlay({v_num_reco_tracks_per_slice},{signal}, {signal_definition}, {},"1", "num_reco_tracks_per_slice",false,false, analysis_tag);
        validateOverlay({v_num_reco_objects_per_slice},{signal}, {signal_definition}, {},"1", "num_reco_objects_per_slice",false,false, analysis_tag);





    } //end valid

    if(mode_option == "eff"){

        std::cout<<"------------Starting slice eff --------------"<<std::endl;
        int total_entries = signal->tvertex->GetEntries();
        int signal_def_entries = signal->tvertex->GetEntries((signal_definition).c_str());
        std::cout<<"The total number events in the file: "<<total_entries<<std::endl;
        std::cout<<"The number of events that pass the 1g1p signal definition: "<< signal_def_entries<<std::endl;

        int signal_def_entries_1g1p ;
        int signal_def_entries_1g1p_same_slice;
        int signal_def_entries_1g1p_nuslice;
        int signal_def_entries_1g1p_nuslice_1shower ;
        int signal_def_entries_1g1p_nuslice_1shower1track ;


        int signal_def_entries_0shower = signal->tslice->GetEntries(("matched_signal_shower_num == 0 && "+signal_definition).c_str());
        int signal_def_entries_1shower0track = signal->tslice->GetEntries(("matched_signal_track_num == 0 && matched_signal_shower_num == 1 && "+signal_definition).c_str());
        int signal_def_entries_1shower2track = signal->tslice->GetEntries(("matched_signal_track_num > 1 && matched_signal_shower_num == 1 && "+signal_definition).c_str());



        std::cout<<"--------------------------------------------------"<<std::endl;
        std::cout<<"The total number of events with 0 signal showers: "<< signal->tslice->GetEntries("matched_signal_shower_num == 0")<<"/ "<<signal->tslice->GetEntries(("matched_signal_shower_num == 0 && "+signal_definition).c_str()) <<std::endl;
        std::cout<<"The total number of events with 1 signal  showers: "<< signal->tslice->GetEntries("matched_signal_shower_num == 1")<<"/ "<<signal->tslice->GetEntries(("    matched_signal_shower_num == 1 && "+signal_definition).c_str()) <<std::endl;
        std::cout<<"The total number of events with 2+ signal showers: "<< signal->tslice->GetEntries("matched_signal_shower_num > 1")<<"/ "<<signal->tslice->GetEntries(("    matched_signal_shower_num > 1 && "+signal_definition).c_str()) <<std::endl;

        std::cout<<"--------------------------------------------------"<<std::endl;
        std::cout<<"The total number of events with 0 signal tracks: "<< signal->tslice->GetEntries("matched_signal_track_num == 0") <<std::endl;
        std::cout<<"The total number of events with 1 signal tracks: "<< signal->tslice->GetEntries("matched_signal_track_num == 1") <<std::endl;
        std::cout<<"The total number of events with 2+ signal tracks: "<< signal->tslice->GetEntries("matched_signal_track_num > 1") <<std::endl;


        std::cout<<"--------------------------------------------------"<<std::endl;
        if (analysis_tag == "notrack"){
            std::cout<<"The total number of events with 1 signal shower and 0 signal tracks (1g0p candidates): "<< signal->tslice->GetEntries("matched_signal_shower_num == 1 && matched_signal_track_num == 0") <<std::endl;



        }else {
            int entries_1g1p = signal->tslice->GetEntries("matched_signal_shower_num == 1 && matched_signal_track_num == 1");
            int entries_1g1p_same_slice = signal->tslice->GetEntries("matched_signal_shower_num == 1 && matched_signal_track_num == 1 && reco_1g1p_is_same_slice");
            int entries_1g1p_nuslice = signal->tslice->GetEntries("matched_signal_shower_num == 1 && matched_signal_track_num == 1 && reco_1g1p_is_nuslice");
            int entries_1g1p_nuslice_1shower = signal->tslice->GetEntries("matched_signal_shower_num == 1 && matched_signal_track_num == 1 && reco_1g1p_is_nuslice && reco_slice_num_showers ==1");
            int entries_1g1p_nuslice_1shower1track = signal->tslice->GetEntries("matched_signal_shower_num == 1 && matched_signal_track_num == 1 && reco_1g1p_is_nuslice && reco_slice_num_showers ==1 && reco_slice_num_tracks ==1 ");



            signal_def_entries_1g1p = signal->tslice->GetEntries(("matched_signal_shower_num == 1 && matched_signal_track_num == 1 &&" + signal_definition).c_str());

            signal_def_entries_1g1p_same_slice = signal->tslice->GetEntries(("matched_signal_shower_num == 1 && matched_signal_track_num == 1 && reco_1g1p_is_same_slice&& "+ signal_definition).c_str());
            signal_def_entries_1g1p_nuslice = signal->tslice->GetEntries(("matched_signal_shower_num == 1 && matched_signal_track_num == 1 && reco_1g1p_is_nuslice &&" + signal_definition).c_str());
            signal_def_entries_1g1p_nuslice_1shower = signal->tslice->GetEntries(("matched_signal_shower_num == 1 && matched_signal_track_num == 1 && reco_1g1p_is_nuslice && reco_slice_num_showers ==1 && "+ signal_definition).c_str());
            signal_def_entries_1g1p_nuslice_1shower1track = signal->tslice->GetEntries(("matched_signal_shower_num == 1 && matched_signal_track_num == 1 && reco_1g1p_is_nuslice && reco_slice_num_showers ==1 && reco_slice_num_tracks ==1 && "+ signal_definition).c_str());



            std::cout<<"The total number of events with 1 signal shower and 1 signal tracks (1g1p candidates): "<<entries_1g1p<<" ("<<entries_1g1p*100/total_entries<<"%)"<<"/  " <<signal_def_entries_1g1p<<" ("<<signal_def_entries_1g1p*100/signal_def_entries<<"%)" <<std::endl;

            std::cout<<"The total number of events with 1 signal shower and 1 signal tracks (1g1p candidates) in the same slice: "<< entries_1g1p_same_slice<<" ("<<entries_1g1p_same_slice*100/total_entries<<"%)"<<"/  " <<signal_def_entries_1g1p_same_slice<<" ("<<signal_def_entries_1g1p_same_slice*100/signal_def_entries<<"%)"  <<std::endl;

            std::cout<<"The total number of events with 1 signal shower and 1 signal tracks (1g1p candidates) both in the neutrino slice: "<< entries_1g1p_nuslice<<" ("<<entries_1g1p_nuslice*100/total_entries<<"%)"<<"/  " <<signal_def_entries_1g1p_nuslice<<" ("<<signal_def_entries_1g1p_nuslice*100/signal_def_entries<<"%)"  <<std::endl;

            std::cout<<"The total number of events with 1 signal shower and 1 signal tracks (1g1p candidates) both in the neutrino slice with only 1 shower in the slice: "<< entries_1g1p_nuslice_1shower<<" ("<<entries_1g1p_nuslice_1shower*100/total_entries<<"%)"<<"/  " <<signal_def_entries_1g1p_nuslice_1shower<<" ("<<signal_def_entries_1g1p_nuslice_1shower*100/signal_def_entries<<"%)"  <<std::endl;

            std::cout<<"The total number of events with 1 signal shower and 1 signal tracks (1g1p candidates) both in the neutrino slice with only 1 shower and 1 track in the slice: "<< entries_1g1p_nuslice_1shower1track<<" ("<<entries_1g1p_nuslice_1shower1track*100/total_entries<<"%)"<<"/  " <<signal_def_entries_1g1p_nuslice_1shower1track<<" ("<<signal_def_entries_1g1p_nuslice_1shower1track*100/signal_def_entries<<"%)"  <<std::endl;


        }//if track

        std::cout<<"-----------------------Failure Modes For Signal Def---------------------------"<<std::endl;
        std::cout<<"No shower was reconstructed: "<<signal_def_entries_0shower<<"/ "<<" ("<<signal_def_entries_0shower*100/signal_def_entries<<"%)" <<std::endl;   

        if (analysis_tag == "notrack"){

        }else {
            std::cout<<"One shower was reconstructed but no track was reconstructed: "<<signal_def_entries_1shower0track<<"/ "<<" ("<<(double)signal_def_entries_1shower0track*100/signal_def_entries<<"%)" <<std::endl;   
            std::cout<<"One shower was reconstructed but multiple tracks were reconstructed: "<<signal_def_entries_1shower2track<<"/ "<<" ("<<(double)signal_def_entries_1shower2track*100/signal_def_entries<<"%)" <<std::endl;  
            std::cout<<"One shower and one track was reconstructed but in different slices: "<<signal_def_entries_1g1p - signal_def_entries_1g1p_same_slice<<"/ "<<" ("<<(double)(signal_def_entries_1g1p -signal_def_entries_1g1p_same_slice)*100/signal_def_entries<<"%)" <<std::endl;  

         std::cout<<"One shower and one track was reconstructed in same slice but it wasn't selected as the neutrino slice: "<<signal_def_entries_1g1p_same_slice - signal_def_entries_1g1p_nuslice<<"/ "<<" ("<<(double)(signal_def_entries_1g1p_same_slice - signal_def_entries_1g1p_nuslice)*100/signal_def_entries<<"%)" <<std::endl;  

        std::cout<<"One shower and one track was reconstructed in neutrino slice but there are more than 1 track and 1 shower inside the slice: "<<signal_def_entries_1g1p_nuslice - signal_def_entries_1g1p_nuslice_1shower1track<<"/ "<<" ("<<(double)(signal_def_entries_1g1p_nuslice- signal_def_entries_1g1p_nuslice_1shower1track)*100/signal_def_entries<<"%)" <<std::endl;  





        }

    }//end eff


    return 0;

    }

    int validateOverlay(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, bdt_file * data, std::string datacut, std::string pdfname, std::string analysis_tag){
        std::vector<bdt_file *> tmp = {data};
        validateOverlay(vars, files, cuts, tmp,datacut,pdfname,false,true, analysis_tag);
        return 0;
    }

    int validateOverlay(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, bdt_file * data, std::string datacut, std::string pdfname,bool islog, std::string analysis_tag){
        std::vector<bdt_file *> tmp = {data};
        validateOverlay(vars, files, cuts, tmp,datacut,pdfname,islog,true, analysis_tag);
        return 0;
    }



    int validateOverlay(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, std::vector<bdt_file *> datas, std::string datacut, std::string pdfname, bool islog, bool cutup, std::string analysis_tag){

        TCanvas *c = new TCanvas();
        c->cd();
        if(islog) c->SetLogy();

        validateOverlay(vars, files, cuts, datas, datacut, c, 0, pdfname, islog, cutup);

        c->cd();
        c->SaveAs((pdfname+"_"+analysis_tag+".pdf").c_str(),"pdf");    

        return 0;

    }


    int validateOverlay(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, std::vector<bdt_file *> datas, std::string datacut, TCanvas *c, int which_c, std::string pdfname, bool islog, bool cutup){

        double maxval = -9999;
        //  TLegend* leg=new TLegend(0.6,0.6,0.89,0.89);
        TLegend* leg=new TLegend(0.7,0.7,0.89,0.89);



        if(islog) c->SetLogy();

        int v = 0;
        if(vars.size()>1) v=1;


        double dval = 0;
        if(datas.size()>0){
            c->cd(which_c);
            TH1* h_data2 =  (TH1*) datas[0]->getTH1(vars[v], datacut , "data_truth_overlay_fake", 0, 1);
            dval = h_data2->GetMaximum();
            c->cd(which_c);
        }

        for(int i=0; i<files.size();i++){

            c->cd(which_c); 
            TH1* th1_overlay;
            TH1* th1_mcish;

            //std::string testcut = "test_matched_hits > 0";
            std::string testcut = "1";


            if(cutup){
                //       TH1* th1_overlay =  (TH1*) files[i]->getTH1(vars[i], cuts[i] +"&& 1" , "photon_truth_overlay"+std::to_string(i), 6.6e20, 1);
                //      TH1* th1_mcish =  (TH1*) files[i]->getTH1(vars[i], cuts[i] +"&& 0" , "photon_truth_mcish"+std::to_string(i), 6.6e20, 1);
                th1_overlay =  (TH1*) files[i]->getTH1Slice(vars[i], testcut+"&&"+cuts[i] +"&& sim_track_overlay_fraction > 0.5" , "photon_truth_overlay"+std::to_string(i), 6.6e20, 1);
                th1_mcish =  (TH1*) files[i]->getTH1Slice(vars[i], testcut+"&&"+cuts[i] +"&& sim_track_overlay_fraction < 0.5" , "photon_truth_mcish"+std::to_string(i), 6.6e20, 1);
            }else{
                th1_overlay =  (TH1*) files[i]->getTH1Slice(vars[i], testcut+"&&"+cuts[i] +"&& 1" , "photon_truth_overlay"+std::to_string(i), 6.6e20, 1);
                th1_mcish =  (TH1*) files[i]->getTH1Slice(vars[i], testcut+"&&"+cuts[i] +"&& 0" , "photon_truth_mcish"+std::to_string(i), 6.6e20, 1);
            }

            THStack * ts1 = new THStack();

            th1_overlay->SetFillColor(files[i]->col);
            th1_overlay->SetFillStyle(3244);
            th1_mcish->SetFillColor(files[i]->col);

            if (th1_mcish->Integral() == 0. || th1_overlay->Integral() == 0) {
                std::cout << "MC hist integral in " << vars[i].unit.c_str() << " is 0!" << std::endl;
                //return 1;
            }
            if (th1_overlay->Integral() == 0.) {
                std::cout << "Overlay hist integral in " << vars[i].unit.c_str() << " is 0!" << std::endl;
                // return 1;
            }
            double norm = th1_overlay->Integral()+ th1_mcish->Integral();
            //th1_overlay->Scale(1.0/norm);
            //th1_mcish->Scale(1.0/norm);

            ts1->Add(th1_overlay);
            ts1->Add(th1_mcish);
            c->cd(which_c);
            maxval  = std::max(maxval, ts1->GetMaximum());
            ts1->Draw("hist");

            ts1->SetMaximum(std::max(maxval,dval)*1.3);
            ts1->GetXaxis()->SetTitle(vars[i].unit.c_str());
            ts1->SetTitle(pdfname.c_str());

            //leg->AddEntry(th1_overlay,"BNB w/ Overlay","f");
            //   leg->AddEntry(th1_overlay,"BNB w/ Overlay: >50\% overlay","f");
            // leg->AddEntry(th1_mcish,"BNB w/ Overlay: <50\% overlay","f");
            leg->AddEntry(th1_mcish,("NC Delta Rad w/ Overlay: " + std::to_string(th1_mcish->GetEntries())).c_str(),"");
            leg->Draw("same");
        }

        if(datas.size()>1){
            c->cd(which_c);
            TH1* h_bnbext =  (TH1*) datas[1]->getTH1Slice(vars[0], datacut , "bnbext", 0, 1);
            c->cd(which_c);
            h_bnbext->SetLineColor(kRed);
            h_bnbext->SetLineWidth(2);
            h_bnbext->Draw("hist same");

            leg->AddEntry(h_bnbext,"BNBext run1","lp");
            leg->Draw("same");
        }
        if(datas.size()>0){
            c->cd(which_c);
            TH1* h_data =  (TH1*) datas[0]->getTH1Slice(vars[v], datacut , "data_truth_overlay", 0, 1);
            c->cd(which_c);
            h_data->SetLineColor(kBlack);
            h_data->SetMarkerStyle(20);
            h_data->Draw("E1 same");

            if(datas.size()==3){
                c->cd(which_c);
                TH1* h_bnbcorsika =  (TH1*) datas[2]->getTH1Slice(vars[0], datacut , "bnbcorsika", 0, 1);
                c->cd(which_c);
                h_bnbcorsika->SetLineColor(kGreen-3);
                h_bnbcorsika->SetLineWidth(2);
                h_bnbcorsika->Draw("hist same");

                leg->AddEntry(h_bnbcorsika,"BNB w/ Corsika","lp");
                leg->Draw("same");
            }

            leg->AddEntry(h_data,"Data5e19","lp");
            leg->Draw("same");
        }

        return 0;

    }

    int validateOverlay3(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, bool islog, bool cutup,  std::string pdfname,std::string xlabel, std::string ylabel, std::string analysis_tag){

        TCanvas *c = new TCanvas();
        c->cd();
        // c->SetLogy();

        std::string all_cuts = "1";
        for (std::string cut:cuts){
            all_cuts += "&&"+ cut;
        }

        std::cout<<all_cuts<<std::endl;

        TH1 *th1 =  (TH1*) files[0]->getTH1Slice(vars[0], all_cuts, "", 0,1);
        c->cd();
        //th1->SetFillColor(kAzure+2);
        //th1->SetLineColor(kAzure+2);
        th1->SetLineColor(kRed);
        th1->SetLineWidth(2);
        th1->SetFillColor(files[0]->col);
        th1->SetFillStyle(3144);
        c->cd();
        th1->GetXaxis()->SetTitle((xlabel).c_str());
        th1->GetYaxis()->SetTitle((ylabel).c_str());
        th1->SetTitle("");
        c->cd();
        double  maxval1  = th1->GetMaximum();
        //th1->SetMaximum(maxval*1.3);
        th1->Draw("hist");
        c->cd();

        c->cd();
        TH1 *th2 =  (TH1*) files[0]->getTH1Slice(vars[1], all_cuts, "", 0,1);
        //th2->SetFillColor(kGreen+2);
        // th2->SetLineColor(kGreen+2);
        th2->SetLineColor(kRed);
        th2->SetLineWidth(2);
        th2->SetFillColor(files[0]->col);
        th2->SetFillStyle(3354);
        double maxval2 = th2->GetMaximum();

        c->cd();
        th1->SetMaximum(std::max(maxval1, maxval2)*1.3);

        th2->Draw("hist same");

        TH1 *th3 =  (TH1*) files[0]->getTH1Slice(vars[0], all_cuts + "&& sim_shower_overlay_fraction > 0.5", "", 0,1);
        c->cd();
        th3->SetFillColor(kGreen);
        th3->SetFillStyle(3001);
        c->cd();
        th3->Draw("hist same");
        c->cd();

        TH1 *th4 =  (TH1*) files[0]->getTH1Slice(vars[1], all_cuts + "&& sim_track_overlay_fraction > 0.5", "", 0,1);
        c->cd();
        th4->SetFillColor(kGreen);
        th4->SetFillStyle(3001);
        c->cd();
        th4->Draw("hist same");
        c->cd();




        // th1_overlay =  (TH1*) files[i]->getTH1Slice(vars[i], testcut+"&&"+cuts[i] +"&& sim_track_overlay_fraction > 0.5" , "photon_truth_overlay"+std::to_string(i),     6.6e20, 1);
        //th1_mcish =  (TH1*) files[i]->getTH1Slice(vars[i], testcut+"&&"+cuts[i] +"&& sim_track_overlay_fraction < 0.5" , "photon_truth_mcish"+std::to_string(i), 6.6e    20, 1);


        TLegend *leg = new TLegend(0.15, 0.7, 0.45, 0.9);
        leg->AddEntry(th1,( "Reco Shower: "+ std::to_string((int)th1->Integral()-(int)th3->Integral())).c_str());
        leg->AddEntry(th2, ("Reco Track: "+ std::to_string((int)th2->Integral()-(int)th4->Integral())).c_str());
        leg->AddEntry(th3, ("Reco Shower, overlay frac > 0.5: "+ std::to_string((int)th3->Integral())).c_str());
        leg->AddEntry(th3, ("Reco Track, overlay frac > 0.5: "+ std::to_string((int)th4->Integral())).c_str());
        //    leg->AddEntry(th4, "3 Showers");
        leg->Draw("same");


        c->SaveAs((pdfname+"_"+analysis_tag+".pdf").c_str(),"pdf");    



        return 0;

    }





    // Written by Andrew because editing other things is too much work
    int validateOverlay2(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, std::string pdfname){

        TCanvas *c = new TCanvas();
        c->cd();
        c->SetLogy();

        TH1 *th1 =  (TH1*) files[0]->getTH1(vars[0], cuts[0], "photon_overlay_frac_"+std::to_string(0), 0, 1);
        c->cd();
        //th1->SetFillColor(kAzure+2);
        th1->SetLineColor(kAzure+2);
        c->cd();
        th1->GetXaxis()->SetTitle("Shower Overlay Fraction");
        c->cd();
        th1->Draw("hist");
        c->cd();

        c->cd();
        TH1 *th2 =  (TH1*) files[0]->getTH1(vars[0], cuts[1], "photon_overlay_frac_"+std::to_string(1), 0, 1);
        //th2->SetFillColor(kGreen+2);
        th2->SetLineColor(kGreen+2);

        c->cd();
        th2->Draw("hist same");
        c->cd();

        c->cd();
        TH1 *th3 =  (TH1*) files[0]->getTH1(vars[0], cuts[2], "photon_overlay_frac_"+std::to_string(2), 0, 1);
        //th3->SetFillColor(kViolet+5);
        th3->SetLineColor(kViolet+5);
        c->cd();
        th3->Draw("hist same");
        c->cd();

        c->cd();
        TH1 *th4 =  (TH1*) files[0]->getTH1(vars[0], cuts[3], "photon_overlay_frac_"+std::to_string(3), 0, 1);
        //th4->SetFillColor(kRed+1);
        th4->SetLineColor(kRed+1);
        c->cd();
        th4->Draw("hist same");
        c->cd();

        TLegend *leg = new TLegend(0.15, 0.7, 0.45, 0.9);
        leg->AddEntry(th1, "All");
        leg->AddEntry(th2, "1 Shower");
        leg->AddEntry(th3, "2 Showers");
        leg->AddEntry(th4, "3 Showers");
        leg->Draw("same");


        c->SaveAs((pdfname+".png").c_str(),"png");    



        return 0;

    }

    int makeIncrementPlots (std::string name, bdt_variable variable, std::string cut_variable, bdt_file* bnb_cosmics, bdt_file* data5e19, bdt_file* bnbext, int n_increments, double cut_val_max, double cut_val_min,std::string base_data_conditions, std::string base_mc_conditions ){



        double increment = (cut_val_max - cut_val_min)/(n_increments);
        double min = cut_val_min;
        double max = cut_val_min + increment;
        char letter = 'a';

        std::string s_min = std::to_string(min);
        std::string s_max= std::to_string(max);


        TCanvas *c = new TCanvas(("tmp_"+name).c_str(),"tmp",800*sqrt(n_increments),800*sqrt(n_increments));
        c->Divide(sqrt(n_increments),sqrt(n_increments));
        c->cd();


        for (int i = 1; i <= n_increments; i++){

            std::string cuts = cut_variable+ " > " +s_min + "     && " + cut_variable  + " <= " + s_max;
            std::string data_conditions = base_data_conditions + "&& " + cuts;
            std::string mc_conditions= base_mc_conditions + "&&" + cuts ;

            validateOverlay({variable },{bnb_cosmics}, {mc_conditions}, {data5e19,bnbext},{data_conditions}, c, i, s_min+" -> "+s_max,false,true);

            letter++;
            min = max;
            max = min+increment;
            s_min =  std::to_string(min);
            s_max= std::to_string(max);

        } 

        c->cd();
        c->SaveAs((name+".pdf").c_str(),"pdf");


        return 0;
    }


    std::string getAnglewrtWire(int plane, std::string var_in_x, std::string var_in_y){

        std::string s_var;

        if (plane == 2){
            s_var = "abs((3.14/2) - acos(" + var_in_x + "*0 + " + var_in_y+ "*1.0))";
        }
        if (plane == 1){
            s_var = "abs((3.14/2) - acos(" + var_in_x + "*(0.5) + " + var_in_y+ "*(sqrt(3)/2)))";
        }
        if (plane == 0){
            s_var = "abs((3.14/2) - acos(" + var_in_x + "*(-0.5) + " + var_in_y+ "*(sqrt(3)/2)))";
        } 

        return s_var;
    }






    int compareQuick(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, std::string name){

        TCanvas *c = new TCanvas();
        c->cd(); 



        std::vector<int> cols = {kRed-7,  kBlue-7, kGreen+1 ,kBlack};
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

            th1->SetMaximum(th1->GetMaximum()*2.0);
            th1->GetXaxis()->SetTitle(vars[i].unit.c_str());
            th1->SetTitle(vars[i].unit.c_str());

            leg->AddEntry(th1,files[i]->tag.c_str(),"l");


        }

        leg->Draw();
        c->SaveAs((name+".pdf").c_str(),"pdf");

        return 0;
    };
