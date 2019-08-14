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

int validateOverlay(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, bdt_file* data, std::string datacut, std::string pdfname);
int validateOverlay(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, bdt_file* data, std::string datacut, std::string pdfname,bool islog);
int validateOverlay(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, std::vector<bdt_file*> datas, std::string datacut, std::string pdfname, bool islog, bool cutall,double,double);
int validateOverlay(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, std::vector<bdt_file *> datas, std::string datacut, TCanvas *c, int which_c, std::string pdfname, bool islog, bool cutup,double, double);

int compareQuick(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, std::string name);



int validateOverlay2(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, std::string pdfname);
int makeIncrementPlots(std::string name, bdt_variable variable, std::string cut_variable, bdt_file* bnb_cosmics, bdt_file* data5e19, bdt_file* bnbext, int n_increments, double cut_val_max, double cut_val_min, std::string base_data_conditions, std::string base_mc_conditions);

std::string getAnglewrtWire(int plane, std::string var_in_x, std::string var_in_y);




int main (int argc, char *argv[]){

    //This is a standardized location on /pnfs/ that everyone can use. 
    std::string dir = "/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_mcc9_v9/";
    std::string dir12 = "/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_mcc9_v12/";


    std::string mode_option = "fake"; 
    std::string xml = "default.xml";
    std::string topo_tag = "track";
    std::string bdt_tag = "cosmic";
    std::string analysis_tag = topo_tag;

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
        {"topo_tag",	required_argument,	0, 't'},
        {"cosmic",		no_argument,	0, 'b'},
        {"bnb",		    no_argument,	0, 'c'},
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

    //Get all the variables you want to use	
    vars = cosmic_bdt_info.train_vars;
    training_vars = cosmic_bdt_info.train_vars;
    plotting_vars = cosmic_bdt_info.spec_vars;

    std::cout<<"In  "<<cosmic_bdt_info.identifier<<" we have "<<cosmic_bdt_info.train_vars.size()<<" cosmic training variables and "<<cosmic_bdt_info.spec_vars.size()<<" spectators"<<std::endl;
    std::cout<<"In  "<<bnb_bdt_info.identifier<<" we have "<<bnb_bdt_info.train_vars.size()<<" bnb training variables and "<<bnb_bdt_info.spec_vars.size()<<" spectators"<<std::endl;
       
    std::string base_cuts = "reco_vertex_size==1";
    std::string signal_definition = "1";
    std::string background_definition = "1";
    vec_precuts = {"1"};
    std::string postcuts = "1";

    //***************************************************************************************************/
    //***********	The bdt_flows define the "flow" of the analysis, i.e what cuts at what stage  *******/
    //***************************************************************************************************/
    bdt_flow signal_flow(base_cuts, 	signal_definition , 			vec_precuts,	postcuts,	cosmic_bdt_info,	bnb_bdt_info);
    bdt_flow bkg_flow(base_cuts,		background_definition, 			vec_precuts,	postcuts,	cosmic_bdt_info,	bnb_bdt_info);
    bdt_flow data_flow(base_cuts,		"1",					vec_precuts,	postcuts,	cosmic_bdt_info, 	bnb_bdt_info);

    
    bdt_file *bnb_cosmics_caliSCE = new bdt_file(dir12, "bnb_overlay_v12.0.root", "BNBOverlay_caliSCE", "hist","singlephoton/",  kBlue-4, bkg_flow);
    bdt_file *data5e19_caliSCE    = new bdt_file(dir12, "data5e19_v12.0.root",	"On-BeamData_caliSCE",	   "E1p","singlephoton/",  kBlack, data_flow);
    bdt_file *bnbext_caliSCE    = new bdt_file(dir12, "bnbext_run1_v12.0.root",	"Off-BeamData_caliSCE",	"hist","singlephoton/",  kRed, data_flow);
    bdt_file *bnb_corsika_caliSCE    = new bdt_file(dir, "bnb_corsika_v9.3.root",	"BNBCorsika_caliSCE",	"hist","singlephoton/",  kGreen-3, bkg_flow);
    

    //THIS IS ACTUALLY pandoracaloSCE excete corsika
    bdt_file *bnb_cosmics_calo = new bdt_file(dir12, "bnb_overlay_v12.1.root", "BNBOverlay_calo", "hist","singlephoton/",  kBlue-4, bkg_flow);
    bdt_file *data5e19_calo    = new bdt_file(dir12, "data5e19_v12.1.root",	"On-BeamData_calo",	   "E1p","singlephoton/",  kBlack, data_flow);
    bdt_file *bnbext_calo    = new bdt_file(dir12, "bnbext_run1_v12.1.root",	"Off-BeamData_calo",	"hist","singlephoton/",  kRed, data_flow);
    bdt_file *bnb_corsika_calo    = new bdt_file(dir, "bnb_corsika_v9.99.root",	"BNBCorsika_calo",	"hist","singlephoton/",  kGreen-3, bkg_flow);



    std::vector<bdt_file*> files = {bnb_cosmics_calo, data5e19_calo, bnbext_calo, bnb_corsika_calo,  bnb_cosmics_caliSCE, data5e19_caliSCE, bnbext_caliSCE, bnb_corsika_caliSCE};
    for(auto &f: files){
        f->calcPOT();
    }



        bdt_variable v_vertexMult("reco_vertex_size","(5,0,5)","Number of Pandora Neutrino-Slices in event","false","d");
        validateOverlay({v_vertexMult},{bnb_cosmics_caliSCE}, {"1"}, {data5e19_caliSCE,bnbext_caliSCE,bnb_corsika_caliSCE}, "1", "vertex_multiplicity",false,false,0.5,0.5);

        bdt_variable v_tdedxtrun0("reco_track_mean_trunc_dEdx_plane0","(35,0,7)","Truncated Mean dEdx Plane 0 [MeV/cm]","false","d");
        bdt_variable v_tdedxtrun1("reco_track_mean_trunc_dEdx_plane1","(35,0,7)","Truncated Mean dEdx Plane 1 [MeV/cm]","false","d");
        bdt_variable v_tdedxtrun2("reco_track_mean_trunc_dEdx_plane2","(35,0,7)","Truncated Mean dEdx Plane 2 [MeV/cm]","false","d");

        std::string s_reco_shower_dedx_plane2 ="reco_shower_dEdx_plane2_median[0]";
        bdt_variable v_reco_shower_dedx_plane2 (s_reco_shower_dedx_plane2,"(24,0,6)", "Median Shower dE/dx plane 2 [MeV/cm]",false,"d");
        std::string s_reco_shower_dedx_plane1 ="reco_shower_dEdx_plane1_median[0]";
        bdt_variable v_reco_shower_dedx_plane1 (s_reco_shower_dedx_plane1,"(24,0,6)", "Median Shower dE/dx plane 1 [MeV/cm]",false,"d");
        std::string s_reco_shower_dedx_plane0 ="reco_shower_dEdx_plane0_median[0]";
        bdt_variable v_reco_shower_dedx_plane0 (s_reco_shower_dedx_plane0,"(24,0,6)", "Median Shower dE/dx plane 0 [MeV/cm]",false,"d");




        std::string calcut = "1";
    
        validateOverlay({v_reco_shower_dedx_plane0},{bnb_cosmics_caliSCE}, {" (reco_shower_dEdx_plane0_nhits[0]>1) && reco_asso_showers>0&& "+calcut}, {data5e19_caliSCE,bnbext_caliSCE}, "(reco_shower_dEdx_plane0_nhits[0]>1) && reco_asso_showers>0 &&"+calcut, "median_shower_dedx_plane0",false,true,0.5,0.5);
        validateOverlay({v_reco_shower_dedx_plane1},{bnb_cosmics_caliSCE}, {" (reco_shower_dEdx_plane1_nhits[0]>1) && reco_asso_showers>0&& "+calcut}, {data5e19_caliSCE,bnbext_caliSCE}, "(reco_shower_dEdx_plane1_nhits[0]>1) && reco_asso_showers>0 &&"+calcut, "median_shower_dedx_plane1",false,true,0.5,0.5);
        validateOverlay({v_reco_shower_dedx_plane2},{bnb_cosmics_caliSCE}, {" (reco_shower_dEdx_plane2_nhits[0]>1) && reco_asso_showers>0&& "+calcut}, {data5e19_caliSCE,bnbext_caliSCE}, "(reco_shower_dEdx_plane2_nhits[0]>1) && reco_asso_showers>0 &&"+calcut, "median_shower_dedx_plane2",false,true,0.5,0.5);

 
        validateOverlay({v_reco_shower_dedx_plane0},{bnb_cosmics_caliSCE}, {" (reco_shower_dEdx_plane0_nhits[0]>1) && reco_asso_showers>0&& fabs(sim_shower_pdg)==11 && "+calcut}, {data5e19_caliSCE,bnbext_caliSCE}, "(reco_shower_dEdx_plane0_nhits[0]>1) && reco_asso_showers>0 &&"+calcut, "median_electron_shower_dedx_plane0",false,true,0.5,0.5);
        validateOverlay({v_reco_shower_dedx_plane1},{bnb_cosmics_caliSCE}, {" (reco_shower_dEdx_plane1_nhits[0]>1) && reco_asso_showers>0&& fabs(sim_shower_pdg)==11 &&"+calcut}, {data5e19_caliSCE,bnbext_caliSCE}, "(reco_shower_dEdx_plane1_nhits[0]>1) && reco_asso_showers>0 &&"+calcut, "median_electron_shower_dedx_plane1",false,true,0.5,0.5);
        validateOverlay({v_reco_shower_dedx_plane2},{bnb_cosmics_caliSCE}, {" (reco_shower_dEdx_plane2_nhits[0]>1) && reco_asso_showers>0&& fabs(sim_shower_pdg)==11 &&"+calcut}, {data5e19_caliSCE,bnbext_caliSCE}, "(reco_shower_dEdx_plane2_nhits[0]>1) && reco_asso_showers>0 &&"+calcut, "median_electron_shower_dedx_plane2",false,true,0.5,0.5);


        validateOverlay({v_reco_shower_dedx_plane0},{bnb_cosmics_caliSCE}, {" (reco_shower_dEdx_plane0_nhits[0]>1) && reco_asso_showers>0&& fabs(sim_shower_pdg)==22 && "+calcut}, {data5e19_caliSCE,bnbext_caliSCE}, "(reco_shower_dEdx_plane0_nhits[0]>1) && reco_asso_showers>0 &&"+calcut, "median_photon_shower_dedx_plane0",false,true,0.5,0.5);
        validateOverlay({v_reco_shower_dedx_plane1},{bnb_cosmics_caliSCE}, {" (reco_shower_dEdx_plane1_nhits[0]>1) && reco_asso_showers>0&& fabs(sim_shower_pdg)==22 &&"+calcut}, {data5e19_caliSCE,bnbext_caliSCE}, "(reco_shower_dEdx_plane1_nhits[0]>1) && reco_asso_showers>0 &&"+calcut, "median_photon_shower_dedx_plane1",false,true,0.5,0.5);
        validateOverlay({v_reco_shower_dedx_plane2},{bnb_cosmics_caliSCE}, {" (reco_shower_dEdx_plane2_nhits[0]>1) && reco_asso_showers>0&& fabs(sim_shower_pdg)==22 &&"+calcut}, {data5e19_caliSCE,bnbext_caliSCE}, "(reco_shower_dEdx_plane2_nhits[0]>1) && reco_asso_showers>0 &&"+calcut, "median_photon_shower_dedx_plane2",false,true,0.5,0.5);







        validateOverlay({v_tdedxtrun0},{bnb_cosmics_caliSCE}, {"reco_track_good_calo_plane0 > 0 && reco_asso_tracks>0&&"+calcut}, {data5e19_caliSCE,bnbext_caliSCE,bnb_corsika_caliSCE}, "reco_track_good_calo_plane0 && reco_asso_tracks>0&&"+calcut, "track_trun_dedx_plane0",false,true,0.5,0.5);

        validateOverlay({v_tdedxtrun1},{bnb_cosmics_caliSCE}, {"reco_track_good_calo_plane1 > 0 && reco_asso_tracks>0 &&"+calcut}, {data5e19_caliSCE,bnbext_caliSCE,bnb_corsika_caliSCE}, "reco_track_good_calo_plane1 && reco_asso_tracks>0 && "+calcut, "track_trun_dedx_plane1",false,true,0.5,0.5);

        validateOverlay({v_tdedxtrun2},{bnb_cosmics_caliSCE}, {"reco_track_good_calo_plane2 > 0 && reco_asso_tracks>0&& "+calcut}, {data5e19_caliSCE,bnbext_caliSCE,bnb_corsika_caliSCE}, "reco_track_good_calo_plane2 && reco_asso_tracks>0 &&"+calcut, "track_trun_dedx_plane2",false,true,0.5,0.5);


        //Plane 0
        calcut = "reco_track_startz<500 && reco_track_endz<500"; 
        validateOverlay({v_tdedxtrun0},{bnb_cosmics_caliSCE}, {"reco_track_good_calo_plane0 > 0 && reco_asso_tracks>0&& "+calcut}, {data5e19_caliSCE,bnbext_caliSCE,bnb_corsika_caliSCE}, "reco_track_good_calo_plane0 && reco_asso_tracks>0 &&"+calcut, "zone1_track_trun_dedx_plane0",false,true,0.5,0.5);

        calcut = "reco_track_startz>500 && reco_track_endz>500"; 
        validateOverlay({v_tdedxtrun0},{bnb_cosmics_caliSCE}, {"reco_track_good_calo_plane0 > 0 && reco_asso_tracks>0&& "+calcut}, {data5e19_caliSCE,bnbext_caliSCE,bnb_corsika_caliSCE}, "reco_track_good_calo_plane0 && reco_asso_tracks>0 &&"+calcut, "zone2_track_trun_dedx_plane0",false,true,0.5,0.5);

        
        
        //Plane 1
        calcut = "reco_track_startz<500 && reco_track_endz<500"; 
        validateOverlay({v_tdedxtrun1},{bnb_cosmics_caliSCE}, {"reco_track_good_calo_plane1 > 0 && reco_asso_tracks>0&& "+calcut}, {data5e19_caliSCE,bnbext_caliSCE,bnb_corsika_caliSCE}, "reco_track_good_calo_plane1 && reco_asso_tracks>0 &&"+calcut, "zone1_track_trun_dedx_plane1",false,true,0.5,0.5);

        calcut = "reco_track_startz>500 && reco_track_endz>500"; 
        validateOverlay({v_tdedxtrun1},{bnb_cosmics_caliSCE}, {"reco_track_good_calo_plane1 > 0 && reco_asso_tracks>0&& "+calcut}, {data5e19_caliSCE,bnbext_caliSCE,bnb_corsika_caliSCE}, "reco_track_good_calo_plane1 && reco_asso_tracks>0 &&"+calcut, "zone2_track_trun_dedx_plane1",false,true,0.5,0.5);



        //Plane 2
        calcut = "reco_track_startz<500 && reco_track_endz<500"; 
        validateOverlay({v_tdedxtrun2},{bnb_cosmics_caliSCE}, {"reco_track_good_calo_plane2 > 0 && reco_asso_tracks>0&& "+calcut}, {data5e19_caliSCE,bnbext_caliSCE,bnb_corsika_caliSCE}, "reco_track_good_calo_plane2 && reco_asso_tracks>0 &&"+calcut, "zone1_track_trun_dedx_plane2",false,true,0.5,0.5);

        calcut = "reco_track_startz>500 && reco_track_endz>500"; 
        validateOverlay({v_tdedxtrun2},{bnb_cosmics_caliSCE}, {"reco_track_good_calo_plane2 > 0 && reco_asso_tracks>0&& "+calcut}, {data5e19_caliSCE,bnbext_caliSCE,bnb_corsika_caliSCE}, "reco_track_good_calo_plane2 && reco_asso_tracks>0 &&"+calcut, "zone2_track_trun_dedx_plane2",false,true,0.5,0.5);




        //Calo and CaliSCE comparason:
       calcut = "1";
       compareQuick({v_tdedxtrun0,v_tdedxtrun2},{bnb_cosmics_caliSCE, bnb_cosmics_calo},{"reco_track_good_calo_plane0>0 && reco_asso_tracks>0&&"+calcut,"reco_track_good_calo_plane2>0 && reco_asso_tracks>0&&"+calcut} ,"compare_overlay_dedx_p0");
       compareQuick({v_tdedxtrun0,v_tdedxtrun2},{data5e19_caliSCE, data5e19_calo},{"reco_track_good_calo_plane0>0 && reco_asso_tracks>0&&"+calcut,"reco_track_good_calo_plane2>0 && reco_asso_tracks>0&&"+calcut} ,"compare_data_dedx_p0");
       compareQuick({v_tdedxtrun0,v_tdedxtrun2},{bnb_corsika_caliSCE, bnb_corsika_calo},{"reco_track_good_calo_plane0>0 && reco_asso_tracks>0&&"+calcut,"reco_track_good_calo_plane2>0 && reco_asso_tracks>0&&"+calcut} ,"compare_corsika_dedx_p0");
       compareQuick({v_tdedxtrun0,v_tdedxtrun2},{bnbext_caliSCE, bnbext_calo},{"reco_track_good_calo_plane0>0 && reco_asso_tracks>0&&"+calcut,"reco_track_good_calo_plane2>0 && reco_asso_tracks>0&&"+calcut} ,"compare_bnbext_dedx_p0");

       
       compareQuick({v_tdedxtrun1,v_tdedxtrun1},{bnb_cosmics_caliSCE, bnb_cosmics_calo},{"reco_track_good_calo_plane1>0 && reco_asso_tracks>0&&"+calcut,"reco_track_good_calo_plane1>0 && reco_asso_tracks>0&&"+calcut} ,"compare_overlay_dedx_p1");
       compareQuick({v_tdedxtrun1,v_tdedxtrun1},{data5e19_caliSCE, data5e19_calo},{"reco_track_good_calo_plane1>0 && reco_asso_tracks>0&&"+calcut,"reco_track_good_calo_plane1>0 && reco_asso_tracks>0&&"+calcut} ,"compare_data_dedx_p1");
       compareQuick({v_tdedxtrun1,v_tdedxtrun1},{bnb_corsika_caliSCE, bnb_corsika_calo},{"reco_track_good_calo_plane1>0 && reco_asso_tracks>0&&"+calcut,"reco_track_good_calo_plane1>0 && reco_asso_tracks>0&&"+calcut} ,"compare_corsika_dedx_p1");
       compareQuick({v_tdedxtrun1,v_tdedxtrun1},{bnbext_caliSCE, bnbext_calo},{"reco_track_good_calo_plane1>0 && reco_asso_tracks>0&&"+calcut,"reco_track_good_calo_plane1>0 && reco_asso_tracks>0&&"+calcut} ,"compare_bnbext_dedx_p1");


       compareQuick({v_tdedxtrun2,v_tdedxtrun0},{bnb_cosmics_caliSCE, bnb_cosmics_calo},{"reco_track_good_calo_plane0>0 && reco_asso_tracks>0&&"+calcut,"reco_track_good_calo_plane2>0 && reco_asso_tracks>0&&"+calcut} ,"compare_overlay_dedx_p2");
       compareQuick({v_tdedxtrun2,v_tdedxtrun0},{data5e19_caliSCE, data5e19_calo},{"reco_track_good_calo_plane0>0 && reco_asso_tracks>0&&"+calcut,"reco_track_good_calo_plane2>0 && reco_asso_tracks>0&&"+calcut} ,"compare_data_dedx_p2");
       compareQuick({v_tdedxtrun2,v_tdedxtrun0},{bnb_corsika_caliSCE, bnb_corsika_calo},{"reco_track_good_calo_plane0>0 && reco_asso_tracks>0&&"+calcut,"reco_track_good_calo_plane2>0 && reco_asso_tracks>0&&"+calcut} ,"compare_corsika_dedx_p2");
       compareQuick({v_tdedxtrun2,v_tdedxtrun0},{bnbext_caliSCE, bnbext_calo},{"reco_track_good_calo_plane0>0 && reco_asso_tracks>0&&"+calcut,"reco_track_good_calo_plane2>0 && reco_asso_tracks>0&&"+calcut} ,"compare_bnbext_dedx_p2");




    return 0;
};





int validateOverlay(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, bdt_file * data, std::string datacut, std::string pdfname){
    std::vector<bdt_file *> tmp = {data};
    validateOverlay(vars, files, cuts, tmp,datacut,pdfname,false,true,0.5,0.5);
    return 0;
}

int validateOverlay(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, bdt_file * data, std::string datacut, std::string pdfname,bool islog){
    std::vector<bdt_file *> tmp = {data};
    validateOverlay(vars, files, cuts, tmp,datacut,pdfname,islog,true,0.5,0.5);
    return 0;
}



int validateOverlay(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, std::vector<bdt_file *> datas, std::string datacut, std::string pdfname, bool islog, bool cutup, double a, double b){

    TCanvas *c = new TCanvas();
    c->cd();
    if(islog) c->SetLogy();

    validateOverlay(vars, files, cuts, datas, datacut, c, 0, pdfname, islog, cutup,a,b);

    c->cd();
    c->SaveAs((pdfname+".pdf").c_str(),"pdf");    

    return 0;

}


int validateOverlay(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, std::vector<bdt_file *> datas, std::string datacut, TCanvas *c, int which_c, std::string pdfname, bool islog, bool cutup, double a, double b){

   
    double maxval = -9999;
    TLegend* leg=new TLegend(0.55,0.55,0.9,0.9);

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
            th1_overlay =  (TH1*) files[i]->getTH1(vars[i], testcut+"&&"+cuts[i] +"&& sim_track_overlay_fraction > "+std::to_string(a) , "photon_truth_overlay"+std::to_string(i), 6.6e20, 1);
            th1_mcish =  (TH1*) files[i]->getTH1(vars[i], testcut+"&&"+cuts[i] +"&& sim_track_overlay_fraction < "+std::to_string(b) , "photon_truth_mcish"+std::to_string(i), 6.6e20, 1);
        }else{
            th1_overlay =  (TH1*) files[i]->getTH1(vars[i], testcut+"&&"+cuts[i] +"&& 1" , "photon_truth_overlay"+std::to_string(i), 6.6e20, 1);
            th1_mcish =  (TH1*) files[i]->getTH1(vars[i], testcut+"&&"+cuts[i] +"&& 0" , "photon_truth_mcish"+std::to_string(i), 6.6e20, 1);
        }

        
        THStack * ts1 = new THStack();

        th1_overlay->SetFillColor(files[i]->col);
        th1_overlay->SetFillStyle(3244);
        th1_mcish->SetFillColor(files[i]->col);


        std::cout<<pdfname<<" : "<<files[i]->tag<<" "<<th1_overlay->Integral()<<std::endl;

        if (th1_mcish->Integral() == 0. || th1_overlay->Integral() == 0) {
            std::cout << "MC hist integral in " << vars[i].unit.c_str() << " is 0!" << std::endl;
            //return 1;
        }
        if (th1_overlay->Integral() == 0.) {
            std::cout << "Overlay hist integral in " << vars[i].unit.c_str() << " is 0!" << std::endl;
            // return 1;
        }
        double norm = th1_overlay->Integral()+ th1_mcish->Integral();
        th1_overlay->Scale(1.0/norm);
        th1_mcish->Scale(1.0/norm);

        ts1->Add(th1_overlay);
        ts1->Add(th1_mcish);
        c->cd(which_c);
        maxval  = std::max(maxval, ts1->GetMaximum());
        ts1->Draw("hist");

        ts1->SetMaximum(std::max(maxval,dval)*1.3);
        ts1->GetXaxis()->SetTitle(vars[i].unit.c_str());
        ts1->SetTitle(pdfname.c_str());

        if(cutup){
        leg->AddEntry(th1_overlay,"BNB w/ Overlay: >50\% overlay","f");
        leg->AddEntry(th1_mcish,"BNB w/ Overlay: <50\% overlay","f");
        }else{
          leg->AddEntry(th1_overlay,"BNB w/ Overlay","f");

        }
    }

    if(datas.size()>1){
        c->cd(which_c);
        TH1* h_bnbext =  (TH1*) datas[1]->getTH1(vars[0], datacut , "bnbext", 0, 1);
        c->cd(which_c);
        h_bnbext->SetLineColor(kRed);
        h_bnbext->SetLineWidth(2);
        h_bnbext->Draw("hist same");

        leg->AddEntry(h_bnbext,"BNBext run1","lp");
        leg->Draw("same");
    }
    if(datas.size()>0){
        c->cd(which_c);
        TH1* h_data =  (TH1*) datas[0]->getTH1(vars[v], datacut , "data_truth_overlay", 6.6e20, 1);
        std::cout<<pdfname<<" : Data: "<<h_data->Integral()<<std::endl;
        c->cd(which_c);
        h_data->Scale(1/h_data->Integral());

        h_data->SetLineColor(kBlack);
        h_data->SetMarkerStyle(20);
        h_data->Draw("E1 same");


    if(datas.size()==3){
        c->cd(which_c);
        TH1* h_bnbcorsika =  (TH1*) datas[2]->getTH1(vars[0], datacut , "bnbcorsika", 0, 1);
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

        validateOverlay({variable },{bnb_cosmics}, {mc_conditions}, {data5e19,bnbext},{data_conditions}, c, i, s_min+" -> "+s_max,false,true,0.5,0.5);

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
    if (plane ==1 ){
        s_var = "abs( acos(" + var_in_x + "*(-0.5) + " + var_in_y+ "*(sqrt(3)/2)))";
    }
    if (plane == 0){
        s_var = "abs( acos(" + var_in_x + "*(0.5) + " + var_in_y+ "*(sqrt(3)/2)))";
    } 

    return s_var;
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
