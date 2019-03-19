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


int validateOverlay(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, bdt_file* data, std::string datacut, std::string pdfname);
int validateOverlay(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, bdt_file* data, std::string datacut, std::string pdfname,bool islog);
int validateOverlay(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, std::vector<bdt_file*> datas, std::string datacut, std::string pdfname, bool islog, bool cutall);
int validateOverlay(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, std::vector<bdt_file *> datas, std::string datacut, TCanvas *c, int which_c, std::string pdfname, bool islog, bool cutup);


int validateOverlay2(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, std::string pdfname);
int makeIncrementPlots(std::string name, bdt_variable variable, std::string cut_variable, bdt_file* bnb_cosmics, bdt_file* data5e19, bdt_file* bnbext, int n_increments, double cut_val_max, double cut_val_min, std::string base_data_conditions, std::string base_mc_conditions);

std::string getAnglewrtWire(int plane, std::string var_in_x, std::string var_in_y);

int main (int argc, char *argv[]){

    //This is a standardized location on /pnfs/ that everyone can use. 
    std::string dir = "/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_mcc9_v7/";
    std::string dir8 = "/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_mcc9_v8/";
    std::string dir9 = "/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_mcc9_v9/";

    std::string olddir5 = "/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_mcc9_v5/";
    std::string olddir6 = "/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_mcc9_v6/";
    std::string amogondir = "/pnfs/uboone/persistent/users/amogan/singlePhoton/samples/";


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

    std::string base_cuts = "reco_vertex_size==1 && reco_asso_showers > 0";
    std::string signal_definition = "1";
    std::string background_definition = "1";
    std::vector<std::string> vec_precuts = {"1"};

    //***************************************************************************************************/
    //***********	The bdt_flows define the "flow" of the analysis, i.e what cuts at what stage  *******/
    //***************************************************************************************************/
    bdt_flow signal_flow(base_cuts, 	signal_definition , 			vec_precuts,	postcuts,	cosmic_bdt_info,	bnb_bdt_info);
    bdt_flow bkg_flow(base_cuts,		background_definition, 			vec_precuts,	postcuts,	cosmic_bdt_info,	bnb_bdt_info);
    bdt_flow data_flow(base_cuts,		"1",					vec_precuts,	postcuts,	cosmic_bdt_info, 	bnb_bdt_info);

    bdt_file *signal_cosmics = new bdt_file(olddir5, "ncdeltarad_overlay_mcc9_v5.0.root", "NCDeltaRadCosmics", "hist","singlephoton/",  kRed-7, signal_flow);
    //bdt_file *bnb_cosmics = new bdt_file(dir, "bnb_overlay_combined_v7.3.root", "BNBCosmics", "hist","singlephoton/",  kBlue-4, bkg_flow);
    bdt_file *bnb_cosmics = new bdt_file(dir9, "bnb_overlay_v9.2.root", "BNBCosmics", "hist","singlephoton/",  kBlue-4, bkg_flow);
    bdt_file *data5e19    = new bdt_file(dir9, "data5e19_v9.0.root",	"Data5e19",	   "E1p","singlephoton/",  kBlack, data_flow);
    bdt_file *bnbext    = new bdt_file(dir, "bnbext_run1_v7.1.root",	"BNBext",	"hist","singlephoton/",  kBlack, data_flow);

    std::vector<bdt_file *> files = {signal_cosmics, bnb_cosmics, data5e19, bnbext};

    for(auto &f: files){
            f->calcPOT();
    }

    //this is
//    bdt_file *bnb_cosmics = new bdt_file(olddir5, "bnb_overlay_combined_mcc9_v5.0.root", "BNBCosmics", "hist","singlephoton/",  kBlue-4, bkg_flow);
//    bdt_file *data5e19    = new bdt_file(olddir5, "data_mcc9_v5.0.root",	"Data5e19",	   "E1p","singlephoton/",  kBlack, data_flow);
//    bdt_file *bnbext    = new bdt_file(olddir5, "bnbext_mcc9_v5.0.root",	"BNBext",	"hist","singlephoton/",  kBlack, data_flow);


    std::vector<bdt_file*> bdt_files = {bnb_cosmics, data5e19, bnbext};


    std::cout<<"--------------------------------------------------------------------------"<<std::endl;
    std::cout<<"--------------------------------------------------------------------------"<<std::endl;
    for(auto &f: bdt_files){
        std::cout<<"Loading "<<f->tag<<"\t with "<<f->tvertex->GetEntries()<<"\t verticies. (unweighted)"<<std::endl;
        std::cout<<"POT of file loaded is: "<<f->pot<<"\t\t "<<std::endl;
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

            std::cout<<"Filling Base EntryLists on File  "<<f->tag<<std::endl;
            if(mode_option != "train" && mode_option != "eff"){
                f->calcBaseEntryList(analysis_tag);
            }
        }
    }
    std::cout<<"--------------------------------------------------------------------------"<<std::endl;
    std::cout<<"--------------------------------------------------------------------------"<<std::endl;

    //Adding plot names
    //	signal_cosmics->addPlotName("LEE NC #Delta Rad w/ Overlay");
    bnb_cosmics->addPlotName("BNB w/ Overlay");
    data5e19->addPlotName("4.8e19 POT Data");
    bnbext->addPlotName("BNBext");

    std::cout<<"--------------------------------------------------------------------------"<<std::endl;
    std::cout<<"--------------------------------------------------------------------------"<<std::endl;

    //===========================================================================================
    //===========================================================================================
    //		Main flow of the program , using OPTIONS
    //===========================================================================================
    //===========================================================================================

    if(mode_option == "valid"){


        std::string s_reco_vertex_res ="(sqrt(pow(reco_vertex_x - mctruth_nu_vertex_x,2)-pow(reco_vertex_y - mctruth_nu_vertex_y,2)-pow(reco_vertex_z - mctruth_nu_vertex_z,2)))";
        bdt_variable v_reco_vertex_res (s_reco_vertex_res,"(54,0,100)","Vertex Resolutione cm", false,"d");

        validateOverlay({v_reco_vertex_res},{bnb_cosmics}, {"reco_vertex_size>0 && reco_asso_showers>0 && reco_asso_tracks> 0"}, {},"reco_vertex_size > 0 && reco_asso_showers>0 && reco_asso_tracks>0", "vertex_res",true,true);


        //        std::string s_reco_shower_angle_wire_plane2 = "abs((3.14/2) - acos(reco_shower_dirx[0]*0 + reco_shower_diry[0]*1.0))";
        //        std::string s_reco_shower_angle_wire_plane1 = "abs((3.14/2) - acos(reco_shower_dirx[0]*(-0.5) + reco_shower_diry[0]*sqrt(3)/2))";
        //        std::string s_reco_shower_angle_wire_plane0 = "abs((3.14/2) - acos(reco_shower_dirx[0]*0.5 + reco_shower_diry[0]*sqrt(3)/2))";

        std::string s_reco_shower_angle_wire_plane2 = getAnglewrtWire(2,"reco_shower_diry[0]", "reco_shower_dirz[0]");
        std::string s_reco_shower_angle_wire_plane1 = getAnglewrtWire(1,"reco_shower_diry[0]", "reco_shower_dirz[0]");
        std::string s_reco_shower_angle_wire_plane0 = getAnglewrtWire(0,"reco_shower_diry[0]", "reco_shower_dirz[0]");



        bdt_variable v_reco_shower_angle_wire_plane2 (s_reco_shower_angle_wire_plane2,"(48,0,1.57)", "Angle Between Reco Shower and Wires Plane 2",false,"d");
        validateOverlay({v_reco_shower_angle_wire_plane2 },{bnb_cosmics}, {"reco_asso_showers>0 && sim_shower_is_true_shower"}, data5e19,"reco_asso_showers>0  ", "shower_angle_wire_plane2",true);

        bdt_variable v_reco_shower_angle_wire_plane1 (s_reco_shower_angle_wire_plane1,"(48,0,1.57)", "Angle Between Reco Shower and Wires Plane 1",false,"d");
        validateOverlay({v_reco_shower_angle_wire_plane1 },{bnb_cosmics}, {"reco_asso_showers>0 && sim_shower_is_true_shower"}, data5e19,"reco_asso_showers>0  ", "shower_angle_wire_plane1",true);

        bdt_variable v_reco_shower_angle_wire_plane0 (s_reco_shower_angle_wire_plane0,"(48,0,1.57)", "Angle Between Reco Shower and Wires Plane 0",false,"d");
        validateOverlay({v_reco_shower_angle_wire_plane0 },{bnb_cosmics}, {"reco_asso_showers>0 && sim_shower_is_true_shower"}, data5e19,"reco_asso_showers>0  ", "shower_angle_wire_plane0",true);


        // Strings to calculate variables
        std::string shower_index1 = "(reco_shower_ordered_energy_index[0])";
        std::string shower_index2 = "(reco_shower_ordered_energy_index[1])";

        std::string s_reco_shower_dedx_plane2 ="reco_shower_dEdx_plane2_median[0]";
        bdt_variable v_reco_shower_dedx_plane2 (s_reco_shower_dedx_plane2,"(24,0,6)", "Median Shower dE/dx plane 2 [MeV/cm]",false,"d");

        std::string s_reco_shower_dedx_plane1 ="reco_shower_dEdx_plane1_median[0]";
        bdt_variable v_reco_shower_dedx_plane1 (s_reco_shower_dedx_plane1,"(24,0,6)", "Median Shower dE/dx plane 1 [MeV/cm]",false,"d");

        std::string s_reco_shower_dedx_plane0 ="reco_shower_dEdx_plane0_median[0]";
        bdt_variable v_reco_shower_dedx_plane0 (s_reco_shower_dedx_plane0,"(24,0,6)", "Median Shower dE/dx plane 0 [MeV/cm]",false,"d");

        /* double n_increments = 10;
           double angle_increment = 3.14/(2*n_increments);
           double angle_min = 0;
           double angle_max = angle_min + angle_increment;
           */

        std::string proton_mass = "0.938272";
        std::string E1 = "reco_shower_energy["+shower_index1+"]";
        std::string E2 = "reco_shower_energy["+shower_index2+"]";
        std::string Eratio = "("+E1+")/("+E2+")";
        std::string two_shower_opening_angle = "(reco_shower_dirx[0]*reco_shower_dirx[1] + reco_shower_diry[0]*reco_shower_diry[1] + reco_shower_dirz[0]*reco_shower_dirz[1])";
        std::string invariant_mass = "sqrt(2.0*"+E1+"*"+E2+"*(1.0-"+two_shower_opening_angle+"))/1000";

        std::string p_pi_x = "(reco_shower_energy["+shower_index1+"]*reco_shower_dirx["+shower_index1+"] + reco_shower_energy["+shower_index2+"]*reco_shower_dirx["+shower_index2+"])";
        std::string p_pi_y = "(reco_shower_energy["+shower_index1+"]*reco_shower_diry["+shower_index1+"] + reco_shower_energy["+shower_index2+"]*reco_shower_diry["+shower_index2+"])";
        std::string p_pi_z = "(reco_shower_energy["+shower_index1+"]*reco_shower_dirz["+shower_index1+"] + reco_shower_energy["+shower_index2+"]*reco_shower_dirz["+shower_index2+"])";
        std::string p_pi = "sqrt("+p_pi_x+"*"+p_pi_x+" + "+p_pi_y+"*"+p_pi_y+" + "+p_pi_z+"*"+p_pi_z+")";

        //std::string testcut = "test_matched_hits > 40";
        std::string testcut = "1";


        bdt_variable v_matchedzoom("test_matched_hits","(100,0,50)","Number of hits that get matched to something MC","false","d");
        validateOverlay({v_matchedzoom},{bnb_cosmics}, {"1"}, {}, "0", "matched_hits_zoom",false,false);

        bdt_variable v_matched("test_matched_hits","(100,0,2000)","Number of hits that get matched to something MC","false","d");
        validateOverlay({v_matched},{bnb_cosmics}, {"1"}, {}, "0", "matched_hits",false,false);



        bdt_variable v_showerMult("reco_asso_showers","(5,0,5)","Number of reconstructed showers in Neutrino Slice","false","d");
        validateOverlay({v_showerMult},{bnb_cosmics}, {"reco_vertex_size>0"}, {data5e19,bnbext}, "reco_vertex_size>0", "shower_multiplicity",true,false);

        bdt_variable v_trackMult("reco_asso_tracks","(5,0,5)","Number of reconstructed tracks in Neutrino Slice","false","d");
        validateOverlay({v_trackMult},{bnb_cosmics}, {"reco_vertex_size>0"}, {data5e19,bnbext}, "reco_vertex_size>0", "track_multiplicity",true,false);

        bdt_variable v_showertrackMult("reco_asso_tracks+reco_asso_showers","(10,0,10)","Number of reco tracks +reco showers in Neutrino Slice","false","d");
        validateOverlay({v_showertrackMult},{bnb_cosmics}, {"reco_vertex_size>0"}, {data5e19,bnbext}, "reco_vertex_size>0", "showertrack_multiplicity",true,false);

        bdt_variable v_vertexMult("reco_vertex_size","(5,0,5)","Number of Pandora Neutrino-Slices in event","false","d");
        validateOverlay({v_vertexMult},{bnb_cosmics}, {testcut}, {data5e19,bnbext}, "1", "vertex_multiplicity",false,true);

        bdt_variable v_overlayfrac("sim_shower_overlay_fraction","(100,0,1)","Shower hit overlay fraction","false","d");
        validateOverlay({v_overlayfrac},{bnb_cosmics}, {"1"}, {data5e19,bnbext}, "1", "shower_overlay_frac",true,false);

        bdt_variable v_overlayfrac2("reco_shower_startx","(50,0,270)","Shower Start X [cm]","false","d");
        validateOverlay({v_overlayfrac2},{bnb_cosmics}, {"reco_asso_showers>0"}, {data5e19,bnbext}, "reco_asso_showers>0", "shower_start_x",false,false);

        bdt_variable v_overlayfrac3("reco_shower_starty","(50,-125,125)","Shower Start Y [cm]","false","d");
        validateOverlay({v_overlayfrac3},{bnb_cosmics}, {"reco_asso_showers>0"}, {data5e19,bnbext}, "reco_asso_showers>0", "shower_start_y",false,false);

        bdt_variable v_overlayfrac4("reco_shower_startz","(50,0,1050)","Shower Start Z [cm]","false","d");
        validateOverlay({v_overlayfrac4},{bnb_cosmics}, {"reco_asso_showers>0"}, {data5e19,bnbext}, "reco_asso_showers>0", "shower_start_z",false,false);

        bdt_variable v_snumhits0("reco_shower_num_hits_plane0","(50,0,200)","Num Hits in Shower Plane 0","false","d");
        validateOverlay({v_snumhits0},{bnb_cosmics}, {"reco_asso_showers>0"}, {data5e19,bnbext}, "reco_asso_showers>0", "shower_hits0",false,false);

        bdt_variable v_snumhits1("reco_shower_num_hits_plane1","(50,0,200)","Num Hits in Shower Plane 1","false","d");
        validateOverlay({v_snumhits1},{bnb_cosmics}, {"reco_asso_showers>0"}, {data5e19,bnbext}, "reco_asso_showers>0", "shower_hits1",false,false);

        bdt_variable v_snumhits2("reco_shower_num_hits_plane2","(50,0,200)","Num Hits in Shower Plane 2","false","d");
        validateOverlay({v_snumhits2},{bnb_cosmics}, {"reco_asso_showers>0"}, {data5e19,bnbext}, "reco_asso_showers>0", "shower_hits2",false,false);

        bdt_variable v_sopen("reco_shower_opening_angle","(50,0,1)","Opening angle of shower (pandora)","false","d");
        validateOverlay({v_sopen},{bnb_cosmics}, {"reco_asso_showers>0"}, {data5e19,bnbext}, "reco_asso_showers>0", "shower_openingnangle",false,false);

        bdt_variable v_slength("reco_shower_length","(50,0,100)","Shower Length","false","d");
        validateOverlay({v_slength},{bnb_cosmics}, {"reco_asso_showers>0"}, {data5e19,bnbext}, "reco_asso_showers>0", "shower_length",false,false);


        bdt_variable v_trk_length("reco_track_displacement","(40,0,300)","Track Length [cm]","false","d");
        validateOverlay({v_trk_length},{bnb_cosmics}, {"reco_asso_tracks>0"}, {data5e19,bnbext}, "reco_asso_tracks>0", "track_length",false,false);

        bdt_variable v_tgoodcalo0("reco_track_good_calo_plane0","(40,0,100)","Track Good Calo Hits Plane0","false","d");
        validateOverlay({v_tgoodcalo0},{bnb_cosmics}, {"reco_asso_tracks>0"}, {data5e19}, "reco_asso_tracks>0", "track_goodcalo_plane0",true,false);

        bdt_variable v_tgoodcalo1("reco_track_good_calo_plane1","(40,0,100)","Track Good Calo Hits Plane1","false","d");
        validateOverlay({v_tgoodcalo1},{bnb_cosmics}, {"reco_asso_tracks>0"}, {data5e19}, "reco_asso_tracks>0", "track_goodcalo_plane1",true,false);

        bdt_variable v_tgoodcalo2("reco_track_good_calo_plane2","(40,0,100)","Track Good Calo Hits Plane2","false","d");
        validateOverlay({v_tgoodcalo2},{bnb_cosmics}, {"reco_asso_tracks>0"}, {data5e19}, "reco_asso_tracks>0", "track_goodcalo_plane2",true,false);

        bdt_variable v_tstopplane0("reco_track_mean_dEdx_start_half_plane0/reco_track_mean_dEdx_end_half_plane0","(40,0,3)","Ratio of end/start track mean dEdx plane0","false","d");
        validateOverlay({v_tstopplane0},{bnb_cosmics}, {"reco_asso_tracks>0"}, {data5e19}, "reco_asso_tracks>0", "track_stopping_plane0",false,false);

        bdt_variable v_tstopplane1("reco_track_mean_dEdx_start_half_plane1/reco_track_mean_dEdx_end_half_plane1","(40,0,3)","Ratio of end/start track mean dEdx plane1","false","d");
        validateOverlay({v_tstopplane1},{bnb_cosmics}, {"reco_asso_tracks>0"}, {data5e19}, "reco_asso_tracks>0", "track_stopping_plane1",false,false);

        bdt_variable v_tstopplane2("reco_track_mean_dEdx_start_half_plane2/reco_track_mean_dEdx_end_half_plane2","(40,0,3)","Ratio of end/start track mean dEdx plane2","false","d");
        validateOverlay({v_tstopplane2},{bnb_cosmics}, {"reco_asso_tracks>0"}, {data5e19}, "reco_asso_tracks>0", "track_stopping_plane2",false,false);

//        bdt_variable v_tdedx("reco_track_mean_dEdx","(40,0,10)","Mean Track dEdx","false","d");
  //      validateOverlay({v_tdedx},{bnb_cosmics}, {"reco_asso_tracks>0"}, {data5e19}, "reco_asso_tracks>0", "track_dedx",false,false);


//      std::string calcut = "(reco_track_startz>500 && reco_track_endz>500)";
        std::string calcut = "1";

//        bdt_variable v_tdedxtrun0("248.0/238.0*reco_track_mean_trunc_dEdx_plane0","(35,0,7)","Mean Track trunc_dEdx p0","false","d");
  //      bdt_variable v_tdedxtrun1("248.0/238.0*reco_track_mean_trunc_dEdx_plane1","(35,0,7)","Mean Track trunc_dEdx p1","false","d");
    //    bdt_variable v_tdedxtrun2("248.0/238.0*reco_track_mean_trunc_dEdx_plane2","(35,0,7)","Mean Track trunc_dEdx p2","false","d");

        
        bdt_variable v_tdedxtrun0("reco_track_mean_trunc_dEdx_plane0","(35,0,7)","Mean Track trunc_dEdx p0","false","d");
        bdt_variable v_tdedxtrun1("reco_track_mean_trunc_dEdx_plane1","(35,0,7)","Mean Track trunc_dEdx p1","false","d");
        bdt_variable v_tdedxtrun2("reco_track_mean_trunc_dEdx_plane2","(35,0,7)","Mean Track trunc_dEdx p2","false","d");

        bdt_variable v_tdedxtrun0D("reco_track_mean_trunc_dEdx_plane0","(35,0,7)","Mean Track trunc_dEdx p0","false","d");
        bdt_variable v_tdedxtrun1D("reco_track_mean_trunc_dEdx_plane1","(35,0,7)","Mean Track trunc_dEdx p1","false","d");
        bdt_variable v_tdedxtrun2D("reco_track_mean_trunc_dEdx_plane2","(35,0,7)","Mean Track trunc_dEdx p2","false","d");


        validateOverlay({v_tdedxtrun0,v_tdedxtrun0D},{bnb_cosmics}, {"reco_track_good_calo_plane0 > 0 && reco_asso_tracks>0&&"+calcut}, {data5e19}, "reco_track_good_calo_plane0 && reco_asso_tracks>0&&"+calcut, "track_trun_dedx_plane0",false,true);
        
        validateOverlay({v_tdedxtrun1,v_tdedxtrun1D},{bnb_cosmics}, {"reco_track_good_calo_plane1 > 0 && reco_asso_tracks>0 &&"+calcut}, {data5e19}, "reco_track_good_calo_plane1 && reco_asso_tracks>0 && "+calcut, "track_trun_dedx_plane1",false,true);

        validateOverlay({v_tdedxtrun2,v_tdedxtrun2D},{bnb_cosmics}, {"reco_track_good_calo_plane2 > 0 && reco_asso_tracks>0&& "+calcut}, {data5e19}, "reco_track_good_calo_plane2 && reco_asso_tracks>0 &&"+calcut, "track_trun_dedx_plane2",false,true);



        bdt_variable v_dedxtrun0("reco_track_mean_dEdx_plane0","(35,0,7)","Mean Track dEdx p0","false","d");
        bdt_variable v_dedxtrun1("reco_track_mean_dEdx_plane1","(35,0,7)","Mean Track dEdx p1","false","d");
        bdt_variable v_dedxtrun2("reco_track_mean_dEdx_plane2","(35,0,7)","Mean Track dEdx p2","false","d");
        validateOverlay({v_dedxtrun0},{bnb_cosmics}, {" reco_asso_tracks>0&&"+calcut}, {data5e19}, " reco_asso_tracks>0&&"+calcut, "track_dedx_plane0",false,false);
        validateOverlay({v_dedxtrun1},{bnb_cosmics}, {"reco_asso_tracks>0 &&"+calcut}, {data5e19}, "reco_asso_tracks>0 && "+calcut, "track_dedx_plane1",false,false);
        validateOverlay({v_dedxtrun2},{bnb_cosmics}, {"reco_asso_tracks>0&& "+calcut}, {data5e19}, "reco_asso_tracks>0 &&"+calcut, "track_dedx_plane2",false,false);


        /*
        bdt_variable v_tstop_trunc("reco_track_mean_trunc_dEdx_start_half/reco_track_mean_trunc_dEdx_end_half","(40,0,3)","Ratio of end/start track mean dEdx","false","d");
        validateOverlay({v_tstop_trunc},{bnb_cosmics}, {"reco_track_good_calo> 0 && reco_asso_tracks>0"}, {data5e19,bnbext}, "reco_track_good_calo>0  && reco_asso_tracks>0", "track_trunc_stopping",false,false);

        bdt_variable v_tstop_trunc2("max( reco_track_mean_trunc_dEdx_end_half/reco_track_mean_trunc_dEdx_start_half,  reco_track_mean_trunc_dEdx_start_half/reco_track_mean_trunc_dEdx_end_half)","(40,0,3)","Ratio of end/start track mean dEdx","false","d");
        validateOverlay({v_tstop_trunc2},{bnb_cosmics}, {"reco_track_good_calo> 0 && reco_asso_tracks>0"}, {data5e19,bnbext}, "reco_track_good_calo>0  && reco_asso_tracks>0", "track_MAX_trunc_stopping",false,false);
        */

        std::string cm_angle = "(fabs("+E1+" - "+E2+")/("+p_pi+"))";

        // Signal definition and cuts for MC plots
        std::string true_signal = "sim_shower_overlay_fraction[0]<0.1 && sim_shower_overlay_fraction[1]<0.1 && sim_track_matched[0]==1 && mctruth_num_exiting_pi0>0 && sim_shower_pdg[0]==22 && sim_shower_pdg[1]==22 && sim_shower_parent_pdg[0]==111 && sim_shower_parent_pdg[1]==111";
        std::string my_mcCuts = "reco_asso_showers==2 &&"+E2+">0.1";
        //std::string my_mcCuts = "reco_asso_showers==2 && reco_asso_tracks==1 && sim_shower_is_true_shower && "+E2+">0.1";
        std::string my_dataCuts = "reco_asso_showers==2 && reco_asso_tracks==1 && "+E2+">0.1";
        //std::string my_dataCuts = "reco_asso_showers==2 && reco_asso_tracks==1 && sim_shower_is_true_shower && "+E2+">0.1";


        //bdt_variable v_reco_shower_dedx_plane2 (s_reco_shower_dedx_plane2,"(48,0,6.5)", "Median Shower dE/dx plane 2 [MeV/cm]",false,"d");
        validateOverlay({v_reco_shower_dedx_plane2 },{bnb_cosmics}, {"reco_asso_showers>0 "}, {data5e19,bnbext},"reco_asso_showers>0  ", "shower_dedx_plane2",false,false);
        validateOverlay({v_reco_shower_dedx_plane1 },{bnb_cosmics}, {"reco_asso_showers>0 "}, {data5e19,bnbext},"reco_asso_showers>0  ", "shower_dedx_plane1", false,false);
        validateOverlay({v_reco_shower_dedx_plane0 },{bnb_cosmics}, {"reco_asso_showers>0 "}, {data5e19,bnbext},"reco_asso_showers>0  ", "shower_dedx_plane0",false,false);

        std::string data_conditions_shower = "reco_asso_showers>0";
        std::string mc_conditions_shower = data_conditions_shower + "&& sim_shower_is_true_shower" ;

        makeIncrementPlots ("median_shower_dedx_plane2_wrt_angle",  v_reco_shower_dedx_plane2, s_reco_shower_angle_wire_plane2, bnb_cosmics, data5e19, bnbext, 9, M_PI/2, 0., data_conditions_shower, mc_conditions_shower);
        makeIncrementPlots ("median_shower_dedx_plane1_wrt_angle",  v_reco_shower_dedx_plane1, s_reco_shower_angle_wire_plane1, bnb_cosmics, data5e19, bnbext, 9, M_PI/2, 0.,  data_conditions_shower, mc_conditions_shower);
        makeIncrementPlots ("median_shower_dedx_plane0_wrt_angle",  v_reco_shower_dedx_plane0, s_reco_shower_angle_wire_plane0, bnb_cosmics, data5e19, bnbext, 9, M_PI/2, 0.,  data_conditions_shower, mc_conditions_shower);

        std::string s_reco_shower_energy = "reco_shower_energy[0]/1000.0";
        makeIncrementPlots ("median_shower_dedx_plane2_wrt_energy",  v_reco_shower_dedx_plane2, s_reco_shower_energy, bnb_cosmics, data5e19,bnbext,  4, 400., 0., data_conditions_shower, mc_conditions_shower); 
        makeIncrementPlots ("median_shower_dedx_plane1_wrt_energy",  v_reco_shower_dedx_plane1, s_reco_shower_energy, bnb_cosmics, data5e19, bnbext, 4, 400., 0., data_conditions_shower, mc_conditions_shower); 
        makeIncrementPlots ("median_shower_dedx_plane0_wrt_energy",  v_reco_shower_dedx_plane0, s_reco_shower_energy, bnb_cosmics, data5e19, bnbext, 4, 400., 0., data_conditions_shower, mc_conditions_shower); 

        // ------------------------------------- Conversion Distance Stuff -----------------------------//


        
        bdt_variable v_reco_vertex_y("reco_vertex_y","(54,-125,125)","Vertex y [cm]","false","d");

        validateOverlay({v_reco_vertex_y},{bnb_cosmics}, {"reco_asso_showers>0 && reco_asso_tracks > 0 && sim_shower_is_true_shower"}, data5e19,"reco_asso_showers>0 && reco_asso_tracks>0", "reco_vertex_y");



        std::string s_reco_conv_dist = "sqrt(pow(reco_shower_startx - reco_vertex_x,2)-pow(reco_shower_starty - reco_vertex_y,2)-pow(reco_shower_startz - reco_vertex_z,2))";
        bdt_variable v_reco_conv_dist(s_reco_conv_dist,"(54,0,1000)","Conv Dist [cm]","false","d");

        validateOverlay({v_reco_conv_dist},{bnb_cosmics}, {"reco_asso_showers>0 && reco_asso_tracks > 0"}, {data5e19,bnbext}, "reco_asso_showers>0 && reco_asso_tracks>0", "conv_length_all",true,false);
        validateOverlay({v_reco_conv_dist},{bnb_cosmics}, {"reco_asso_showers>0 && reco_asso_tracks > 0 && sim_shower_pdg==22"}, {data5e19,bnbext}, "reco_asso_showers>0 && reco_asso_tracks>0", "conv_length_photon",true,false);
        validateOverlay({v_reco_conv_dist},{bnb_cosmics}, {"reco_asso_showers>0 && reco_asso_tracks > 0 && abs(sim_shower_pdg)==11"}, {data5e19,bnbext}, "reco_asso_showers>0 && reco_asso_tracks>0", "conv_length_electron",true,false);
        validateOverlay({v_reco_conv_dist},{bnb_cosmics}, {"reco_asso_showers>0 && reco_asso_tracks > 0 && abs(sim_shower_pdg)==11 && abs(sim_shower_parent_pdg) == 13"}, {data5e19,bnbext}, "reco_asso_showers>0 && reco_asso_tracks>0", "conv_length_electron_parent_muon",true,false);



        bdt_variable v_reco_shower_theta("reco_shower_theta_yz","(30,-3.2,3.2)","Reco Shower Theta yz [Rad]","false","d");
        bdt_variable v_reco_shower_phi("reco_shower_phi_yx","(30,-3.2,3.2)","Reco Shower Phi yx [Rad]","false","d");
        bdt_variable v_reco_shower_num_hits_plane0("reco_shower_num_hits_plane0", "(50,0,400)", "Reco Shower N Hits Plane 0", "false","i" ); 
        bdt_variable v_reco_shower_num_hits_plane1("reco_shower_num_hits_plane1", "(50,0,400)", "Reco Shower N Hits Plane 1", "false","i" ); 
        bdt_variable v_reco_shower_num_hits_plane2("reco_shower_num_hits_plane2", "(50,0,400)", "Reco Shower N Hits Plane 2", "false","i" ); 



        validateOverlay({v_reco_shower_num_hits_plane0},{bnb_cosmics}, {"reco_asso_showers>0 && reco_asso_tracks > 0"}, data5e19, "reco_asso_showers>0 && reco_asso_tracks>0", "shower_nhits_plane0",false);
        validateOverlay({v_reco_shower_num_hits_plane1},{bnb_cosmics}, {"reco_asso_showers>0 && reco_asso_tracks > 0"}, data5e19, "reco_asso_showers>0 && reco_asso_tracks>0", "shower_nhits_plane1",false);
        validateOverlay({v_reco_shower_num_hits_plane2},{bnb_cosmics}, {"reco_asso_showers>0 && reco_asso_tracks > 0"}, data5e19, "reco_asso_showers>0 && reco_asso_tracks>0", "shower_nhits_plane2",false);

        makeIncrementPlots ("shower_nhits_plane2_wrt_angle", v_reco_shower_num_hits_plane2, s_reco_shower_angle_wire_plane2, bnb_cosmics, data5e19, bnbext, 9, M_PI/2, 0., data_conditions_shower, mc_conditions_shower);
        makeIncrementPlots ("shower_nhits_plane1_wrt_angle",  v_reco_shower_num_hits_plane1, s_reco_shower_angle_wire_plane1, bnb_cosmics, data5e19, bnbext, 9, M_PI/2, 0.,  data_conditions_shower, mc_conditions_shower);
        makeIncrementPlots ("shower_nhits_plane0_wrt_angle",  v_reco_shower_num_hits_plane0, s_reco_shower_angle_wire_plane0, bnb_cosmics, data5e19, bnbext, 9, M_PI/2, 0.,  data_conditions_shower, mc_conditions_shower);



        validateOverlay({v_reco_shower_theta},{bnb_cosmics}, {"reco_asso_showers>0 && reco_asso_tracks > 0"}, data5e19, "reco_asso_showers>0 && reco_asso_tracks>0", "shower_theta_all",false);
        validateOverlay({v_reco_shower_theta},{bnb_cosmics}, {"reco_asso_showers>0 && reco_asso_tracks > 0 && sim_shower_pdg==22"}, data5e19, "reco_asso_showers>0 && reco_asso_tracks>0", "shower_theta_photon",false);
        validateOverlay({v_reco_shower_theta},{bnb_cosmics}, {"reco_asso_showers>0 && reco_asso_tracks > 0 && abs(sim_shower_pdg)==11"}, data5e19, "reco_asso_showers>0 && reco_asso_tracks>0", "shower_theta_electron",false);


        validateOverlay({v_reco_shower_phi},{bnb_cosmics}, {"reco_asso_showers>0 && reco_asso_tracks > 0"}, data5e19, "reco_asso_showers>0 && reco_asso_tracks>0", "shower_phi_all",true);
        validateOverlay({v_reco_shower_phi},{bnb_cosmics}, {"reco_asso_showers>0 && reco_asso_tracks > 0 && sim_shower_pdg==22"}, data5e19, "reco_asso_showers>0 && reco_asso_tracks>0", "shower_phi_photon",true);
        validateOverlay({v_reco_shower_phi},{bnb_cosmics}, {"reco_asso_showers>0 && reco_asso_tracks > 0 && abs(sim_shower_pdg)==11"}, data5e19, "reco_asso_showers>0 && reco_asso_tracks>0", "shower_phi_electron",true);


        // Plot stuff with increasing no. showers cuts
        bdt_variable overlay_fraction("sim_shower_overlay_fraction", "(50, 0, 1)", "Shower Overlay Fraction", "false", "d");
        std::vector<std::string> showerCuts = {"reco_asso_showers>0", "reco_asso_showers==1", "reco_asso_showers==2", "reco_asso_showers==3"};
        //        validateOverlay2({overlay_fraction}, {bnb_cosmics_v6}, showerCuts, "overlay_frac_v6");
        validateOverlay2({overlay_fraction}, {bnb_cosmics}, showerCuts, "overlay_frac_v7");



        //-------------------------------------Now some track stuff --------------------------------------------//

        std::string s_reco_track_angle_wire_plane2 = getAnglewrtWire(2,"reco_track_diry[0]", "reco_track_dirz[0]");
        std::string s_reco_track_angle_wire_plane1 = getAnglewrtWire(1,"reco_track_diry[0]", "reco_track_dirz[0]");
        std::string s_reco_track_angle_wire_plane0 = getAnglewrtWire(0,"reco_track_diry[0]", "reco_track_dirz[0]");

        //int validateOverlay(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, std::vector<bdt_file*> datas, std::string datacut, std::string pdfname, bool islog, bool cutall);

        std::string data_track_conditions = "reco_asso_tracks>0 && reco_track_good_calo > 0  ";
        std::string mc_track_conditions = data_track_conditions ;

        bdt_variable v_reco_track_angle_wire_plane2 (s_reco_track_angle_wire_plane2,"(48,0,1.57)", "Angle Between Reco Track and Wires Plane 2",false,"d");
        validateOverlay({v_reco_track_angle_wire_plane2 },{bnb_cosmics}, {mc_track_conditions}, {data5e19,bnbext},{data_track_conditions}, "track_angle_wire_plane2", false, false);

        bdt_variable v_reco_track_angle_wire_plane1 (s_reco_track_angle_wire_plane1,"(48,0,1.57)", "Angle Between Reco Track and Wires Plane 1",false,"d");
        validateOverlay({v_reco_track_angle_wire_plane1 },{bnb_cosmics}, {mc_track_conditions}, {data5e19,bnbext},{data_track_conditions}, "track_angle_wire_plane1",false, false);

        bdt_variable v_reco_track_angle_wire_plane0 (s_reco_track_angle_wire_plane0,"(48,0,1.57)", "Angle Between Reco Track and Wires Plane 0",false,"d");
        validateOverlay({v_reco_track_angle_wire_plane0 },{bnb_cosmics}, {mc_track_conditions}, {data5e19,bnbext}, {data_track_conditions}, "track_angle_wire_plane0",false, false);

        std::string s_reco_track_energy = "reco_track_proton_kinetic_energy[0]";
        bdt_variable v_reco_track_energy (s_reco_track_energy,"(48,0,2)","Reconstructed Proton Track Kinetic Energy [GeV]","true","d");
        validateOverlay( { v_reco_track_energy},{bnb_cosmics}, {mc_track_conditions}, {data5e19,bnbext}, {data_track_conditions}, "track_proton_kinetic_energy",false, false);

        bdt_variable v_reco_track_spacepointPCA0 ("reco_track_spacepoint_principal0","(48,0,1)","Reconstructed Track Spacepoint PCA0","true","d");
        validateOverlay( { v_reco_track_spacepointPCA0},{bnb_cosmics}, {mc_track_conditions}, {data5e19,bnbext}, {data_track_conditions}, "track_principal0",true, false);

        bdt_variable v_reco_track_spacepointPCA1 ("reco_track_spacepoint_principal1","(48,0,1)","Reconstructed Track Spacepoint PCA1","true","d");
        validateOverlay( { v_reco_track_spacepointPCA1},{bnb_cosmics}, {mc_track_conditions}, {data5e19,bnbext}, {data_track_conditions}, "track_principal1",true, false);

//        bdt_variable v_reco_track_spacepointPCA2 ("reco_track_spacepoint_principal2","(48,0,1)","Reconstructed Track Spacepoint PCA2","true","d");
  //      validateOverlay( { v_reco_track_spacepointPCA2},{bnb_cosmics}, {mc_track_conditions}, {data5e19,bnbext}, {data_track_conditions}, "track_principal2",false, false);


        makeIncrementPlots ("mean_trunc_track_dedx_wrt_angle_plane2",  v_tdedxtrun2, s_reco_track_angle_wire_plane2, bnb_cosmics, data5e19, bnbext, 9, M_PI/2, 0., data_track_conditions, mc_track_conditions);
        makeIncrementPlots ("mean_trunc_track_dedx_wrt_angle_plane1",  v_tdedxtrun1, s_reco_track_angle_wire_plane1, bnb_cosmics, data5e19, bnbext, 9, M_PI/2, 0.,  data_track_conditions, mc_track_conditions);
        makeIncrementPlots ("mean_trunc_track_dedx_wrt_angle_plane0",  v_tdedxtrun0, s_reco_track_angle_wire_plane0, bnb_cosmics, data5e19, bnbext, 9, M_PI/2, 0.,  data_track_conditions, mc_track_conditions);

        makeIncrementPlots ("mean_trunc_track_dedx_wrt_proton_KE",  v_tdedxtrun2,  s_reco_track_energy, bnb_cosmics, data5e19, bnbext, 9, 1., 0., data_track_conditions, mc_track_conditions);

        bdt_variable v_tln ("reco_track_displacement","(48,0,300)","Reconstructed Track Length [cm]","true","d");
        makeIncrementPlots ("mean_trunc_track_dedx_wrt_track_length_p0",  v_tdedxtrun0,  "reco_track_displacement", bnb_cosmics, data5e19, bnbext, 9, 300., 0., data_track_conditions, mc_track_conditions);
        makeIncrementPlots ("mean_trunc_track_dedx_wrt_track_length_p1",  v_tdedxtrun1,  "reco_track_displacement", bnb_cosmics, data5e19, bnbext, 9, 300., 0., data_track_conditions, mc_track_conditions);
        makeIncrementPlots ("mean_trunc_track_dedx_wrt_track_length_p2",  v_tdedxtrun2,  "reco_track_displacement", bnb_cosmics, data5e19, bnbext, 9, 300., 0., data_track_conditions, mc_track_conditions);






        // Do the stuff, make the plots
        /*
           bdt_variable invmass(invariant_mass,"(12, 0, 0.5)","#pi^{0} Invariant Mass [GeV]","false","d");
           validateOverlay({invmass},{bnb_cosmics}, {my_mcCuts}, data5e19, my_dataCuts, "pi0_invmass_v5");
           validateOverlay({invmass},{bnb_cosmics_v6}, {my_mcCuts}, data5e19_v6, my_dataCuts, "pi0_invmass_v6");

           <<<<<<< HEAD
           bdt_variable centerOfMass_angle(cm_angle,"(12, 0, 1.0)","#theta^{CM}_{#gamma #gamma}","false","d");
           validateOverlay({centerOfMass_angle},{bnb_cosmics}, {my_mcCuts}, data5e19, my_dataCuts, "cmangle_v5");
           validateOverlay({centerOfMass_angle},{bnb_cosmics_v6}, {my_mcCuts}, data5e19_v6, my_dataCuts, "cmangle_v6");
           =======
           std::string s_reco_shower_energy = "reco_shower_energy[0]/1000.0";
           makeIncrementPlots ("median_shower_dedx_plane2_wrt_shower_energy",  v_reco_shower_dedx_plane2, s_reco_shower_energy, bnb_cosmics, data5e19, 10, 1., 0., data_conditions_shower, mc_conditions_shower);
           >>>>>>> 083c262e968c45ea83518cae8001ea1f8d290d4e

           bdt_variable pion_momentum(p_pi, "(12, 0, 0.5)", "Reco. #pi^{0} Momentum [GeV]", "false", "d");
           validateOverlay({pion_momentum},{bnb_cosmics}, {my_mcCuts}, data5e19, my_dataCuts, "pimom_v5");
           validateOverlay({pion_momentum},{bnb_cosmics_v6}, {my_mcCuts}, data5e19_v6, my_dataCuts, "pimom_v6");

           <<<<<<< HEAD
           bdt_variable energy_ratio(Eratio, "(10, 1, 10)", "Ratio of Shower Energies [GeV]", "false", "d");
           validateOverlay({energy_ratio},{bnb_cosmics}, {my_mcCuts}, data5e19, my_dataCuts, "e_ratio_v5");
           validateOverlay({energy_ratio},{bnb_cosmics_v6}, {my_mcCuts}, data5e19_v6, my_dataCuts, "e_ratio_v6");
           */

    } //end valid
    return 0;

}

int validateOverlay(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, bdt_file * data, std::string datacut, std::string pdfname){
    std::vector<bdt_file *> tmp = {data};
    validateOverlay(vars, files, cuts, tmp,datacut,pdfname,false,true);
    return 0;
}

int validateOverlay(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, bdt_file * data, std::string datacut, std::string pdfname,bool islog){
    std::vector<bdt_file *> tmp = {data};
    validateOverlay(vars, files, cuts, tmp,datacut,pdfname,islog,true);
    return 0;
}



int validateOverlay(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, std::vector<bdt_file *> datas, std::string datacut, std::string pdfname, bool islog, bool cutup){

    TCanvas *c = new TCanvas();
    c->cd();
    if(islog) c->SetLogy();

    validateOverlay(vars, files, cuts, datas, datacut, c, 0, pdfname, islog, cutup);

    c->cd();
    c->SaveAs((pdfname+".pdf").c_str(),"pdf");    

    return 0;

}


int validateOverlay(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, std::vector<bdt_file *> datas, std::string datacut, TCanvas *c, int which_c, std::string pdfname, bool islog, bool cutup){

    double maxval = -9999;
    TLegend* leg=new TLegend(0.7,0.7,0.9,0.9);

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
            th1_overlay =  (TH1*) files[i]->getTH1(vars[i], testcut+"&&"+cuts[i] +"&& sim_track_overlay_fraction > 0.5" , "photon_truth_overlay"+std::to_string(i), 6.6e20, 1);
            th1_mcish =  (TH1*) files[i]->getTH1(vars[i], testcut+"&&"+cuts[i] +"&& sim_track_overlay_fraction < 0.5" , "photon_truth_mcish"+std::to_string(i), 6.6e20, 1);
        }else{
            th1_overlay =  (TH1*) files[i]->getTH1(vars[i], testcut+"&&"+cuts[i] +"&& 1" , "photon_truth_overlay"+std::to_string(i), 6.6e20, 1);
            th1_mcish =  (TH1*) files[i]->getTH1(vars[i], testcut+"&&"+cuts[i] +"&& 0" , "photon_truth_mcish"+std::to_string(i), 6.6e20, 1);
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

        leg->AddEntry(th1_overlay,"BNB w/ Overlay: >50\% overlay","f");
        leg->AddEntry(th1_mcish,"BNB w/ Overlay: <50\% overlay","f");
    }

    if(datas.size()==2){
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
    TH1* h_data =  (TH1*) datas[0]->getTH1(vars[v], datacut , "data_truth_overlay", 0, 1);
    c->cd(which_c);
    h_data->SetLineColor(kBlack);
    h_data->SetMarkerStyle(20);
    h_data->Draw("E1 same");


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
    if (plane ==1 ){
        s_var = "abs( acos(" + var_in_x + "*(-0.5) + " + var_in_y+ "*(sqrt(3)/2)))";
    }
    if (plane == 0){
        s_var = "abs( acos(" + var_in_x + "*(0.5) + " + var_in_y+ "*(sqrt(3)/2)))";
    } 

    return s_var;
}






