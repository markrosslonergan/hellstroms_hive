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
int validateOverlay(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, bdt_file* data, std::string datacut, std::string pdfname, bool islog);
int validateOverlay2(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, std::string pdfname);
int makeIncrementPlots(std::string name, bdt_variable variable, std::string cut_variable, bdt_file* bnb_cosmics, bdt_file* data5e19, int n_increments, double cut_val_max, double cut_val_min, std::string base_data_conditions, std::string base_mc_conditions);

int main (int argc, char *argv[]){

    //This is a standardized location on /pnfs/ that everyone can use. 
    //std::string dir = "/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_mcc9_v5/";
    std::string dir = "/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_mcc9_v6/";
    std::string olddir = "/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_mcc9_v5/";
    std::string mydir = "/pnfs/uboone/persistent/users/amogan/singlePhoton/samples/";

    std::string dir5 = "/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_mcc9_v5/";

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

    bdt_file *signal_cosmics = new bdt_file(olddir, "ncdeltarad_overlay_mcc9_v5.0.root", "NCDeltaRadCosmics", "hist","singlephoton/",  kRed-7, signal_flow);
    bdt_file *bnb_cosmics = new bdt_file(olddir, "bnb_overlay_combined_mcc9_v5.0.root", "BNBCosmics", "hist","singlephoton/",  kBlue-4, bkg_flow);
    bdt_file *data5e19    = new bdt_file(olddir, "data_mcc9_v5.0.root",	"Data5e19",	   "E1p","singlephoton/",  kBlack, data_flow);
    bdt_file *bnbext    = new bdt_file(olddir, "bnbext_mcc9_v5.0.root",	"BNBext",	"E1p","singlephoton/",  kGreen-3, data_flow);
    bdt_file *bnb_cosmics_v6 = new bdt_file(dir, "bnb_overlay_run1_v6.0.root", "BNBCosmics", "hist","singlephoton/",  kBlue-4, bkg_flow);
    bdt_file *data5e19_v6    = new bdt_file(mydir, "vertexed_data5e19_v6.root",  "Data5e19",    "E1p","singlephoton/",  kBlack, data_flow);

    std::vector<bdt_file*> bdt_files = {signal_cosmics, bnb_cosmics, bnbext, data5e19, bnb_cosmics_v6, data5e19_v6};

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
            //if(mode_option != "train" && mode_option != "app"){
            if(mode_option != "train" && mode_option != "eff"){
                f->calcBaseEntryList(analysis_tag);
            }
        }
        }
        std::cout<<"--------------------------------------------------------------------------"<<std::endl;
        std::cout<<"--------------------------------------------------------------------------"<<std::endl;

        //Adding plot names
        signal_cosmics->addPlotName("LEE NC #Delta Rad w/ Overlay");
        bnb_cosmics->addPlotName("BNB w/ Overlay");
        data5e19->addPlotName("4.8e19 POT Data");
        bnbext->addPlotName("External BNB Data");

        std::cout<<"--------------------------------------------------------------------------"<<std::endl;
        std::cout<<"--------------------------------------------------------------------------"<<std::endl;

        //===========================================================================================
        //===========================================================================================
        //		Main flow of the program , using OPTIONS
        //===========================================================================================
        //===========================================================================================

        if(mode_option == "valid"){




            /*
               std::string s_reco_vertex_res ="(sqrt(pow(reco_vertex_x - mctruth_nu_vertex_x,2)-pow(reco_vertex_y - mctruth_nu_vertex_y,2)-pow(reco_vertex_z - mctruth_nu_vertex_z,2)))";
               bdt_variable v_reco_vertex_res (s_reco_vertex_res,"(54,0,100)","Vertex Resolution cm", false,"d");
               validateOverlay({v_reco_vertex_res},{bnb_cosmics}, {"reco_asso_showers>0 && reco_asso_tracks > 0 && sim_shower_is_true_shower"}, data5e19,"reco_asso_showers>0 && reco_asso_tracks>0", "vertex_res",true);

               <<<<<<< HEAD

               bdt_variable v_reco_vertex_y("reco_vertex_y","(54,-125,125)","Vertex y [cm]","false","d");
               validateOverlay({v_reco_vertex_y},{bnb_cosmics}, {"reco_asso_showers>0 && reco_asso_tracks > 0 && sim_shower_is_true_shower"}, data5e19,"reco_asso_showers>0 && reco_asso_tracks>0", "reco_vertex_y");
               =======
               bdt_file *signal_cosmics = new bdt_file(olddir, "ncdeltarad_overlay_mcc9_v5.0.root", "NCDeltaRadCosmics", "hist","singlephoton/",  kRed-7, signal_flow);
               bdt_file *bnb_cosmics = new bdt_file(olddir, "bnb_overlay_combined_mcc9_v5.0.root", "BNBCosmics", "hist","singlephoton/",  kBlue-4, bkg_flow);
               bdt_file *bnb_cosmics_v6 = new bdt_file(dir, "bnb_overlay_run1_v6.0.root", "BNBCosmics", "hist","singlephoton/",  kBlue-4, bkg_flow);
               bdt_file *data5e19  = new bdt_file(olddir, "data_mcc9_v5.0.root",	"Data5e19",	   "E1p","singlephoton/",  kBlack, data_flow);
               bdt_file *data5e19_v6    = new bdt_file(mydir, "vertexed_data5e19_v6.root",	"Data5e19",	   "E1p","singlephoton/",  kBlack, data_flow);
               bdt_file *bnbext    = new bdt_file(olddir, "bnbext_mcc9_v5.0.root",	"BNBext",	"E1p","singlephoton/",  kGreen-3, data_flow);

               TFile *fin_data_v6 = new TFile("/pnfs/uboone/persistent/users/amogan/singlePhoton/samples/vertexed_data5e19_v6.root", "READ");
            //std::vector<bdt_file*> bdt_files = {signal_cosmics, bnb_cosmics, bnbext, data5e19};
            std::vector<bdt_file*> bdt_files = {bnb_cosmics, bnb_cosmics_v6, data5e19, data5e19_v6};
            >>>>>>> 3784da52d67421a237be6451e04a473c734482be

            std::string s_reco_conv_dist = "log10(sqrt(pow(reco_shower_startx - reco_vertex_x,2)-pow(reco_shower_starty - reco_vertex_y,2)-pow(reco_shower_startz - reco_vertex_z,2)))";
            bdt_variable v_reco_conv_dist(s_reco_conv_dist,"(54,-3,4)","Conv Dist Log10[cm]","false","d");
            validateOverlay({v_reco_conv_dist},{bnb_cosmics}, {"reco_asso_showers>0 && reco_asso_tracks > 0 && sim_shower_is_true_shower"}, data5e19,"reco_asso_showers>0 && reco_asso_tracks>0", "conv_length");
            */

            //std::vector<double> wire_angle_plane2_xy{0., 1.};
            //Adding plot names
            //	signal_cosmics->addPlotName("LEE NC #Delta Rad w/ Overlay");
            bnb_cosmics->addPlotName("BNB w/ Overlay");
            bnb_cosmics_v6->addPlotName("BNB w/ Overlay");
            data5e19->addPlotName("4.8e19 POT Data");
            data5e19_v6->addPlotName("4.8e19 POT Data");
            bnbext->addPlotName("External BNB Data");



            std::string s_reco_shower_angle_wire_plane2 = "abs((3.14/2) - acos(reco_shower_dirx[0]*0 + reco_shower_diry[0]*1.0))";
            std::string s_reco_shower_angle_wire_plane1 = "abs((3.14/2) - acos(reco_shower_dirx[0]*(-0.5) + reco_shower_diry[0]*sqrt(3)/2))";
            std::string s_reco_shower_angle_wire_plane0 = "abs((3.14/2) - acos(reco_shower_dirx[0]*0.5 + reco_shower_diry[0]*sqrt(3)/2))";

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
            bdt_variable v_reco_shower_dedx_plane2 (s_reco_shower_dedx_plane2,"(48,0,6.5)", "Median Shower dE/dx plane 2 [MeV/cm]",false,"d");

            std::string s_reco_shower_dedx_plane1 ="reco_shower_dEdx_plane1_median[0]";
            bdt_variable v_reco_shower_dedx_plane1 (s_reco_shower_dedx_plane1,"(48,0,6.5)", "Median Shower dE/dx plane 1 [MeV/cm]",false,"d");

            std::string s_reco_shower_dedx_plane0 ="reco_shower_dEdx_plane0_median[0]";
            bdt_variable v_reco_shower_dedx_plane0 (s_reco_shower_dedx_plane0,"(48,0,6.5)", "Median Shower dE/dx plane 0 [MeV/cm]",false,"d");

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

            bdt_variable v_showerMult("reco_asso_showers","(5,0,5)","Number of reconstructed showers associated","false","d");
            validateOverlay({v_showerMult},{bnb_cosmics}, {"reco_vertex_size>=0"}, data5e19, "reco_vertex_size>=0", "shower_multiplicity",true);

            bdt_variable v_trackMult("reco_asso_tracks","(5,0,5)","Number of reconstructed tracks associated","false","d");
            validateOverlay({v_trackMult},{bnb_cosmics}, {"reco_vertex_size>=0"}, data5e19, "reco_vertex_size>=0", "track_multiplicity",true);

            bdt_variable v_vertexMult("reco_vertex_size","(5,0,5)","Number of reconstructed vertexs associated","false","d");
            validateOverlay({v_vertexMult},{bnb_cosmics}, {"1"}, data5e19, "1", "vertex_multiplicity",true);

            bdt_variable v_overlayfrac("sim_shower_overlay_fraction","(100,0,1)","Shower hit overlay fraction","false","d");
            validateOverlay({v_overlayfrac},{bnb_cosmics}, {"1"}, data5e19, "1", "shower_overlay_frac",true);

            bdt_variable v_overlayfrac2("reco_shower_startx","(100,0,270)","Shower Start X [cm]","false","d");
            validateOverlay({v_overlayfrac2},{bnb_cosmics}, {"reco_asso_showers>0"}, data5e19, "reco_asso_showers>0", "shower_start_x",false);

            bdt_variable v_overlayfrac3("reco_shower_starty","(100,-125,125)","Shower Start Y [cm]","false","d");
            validateOverlay({v_overlayfrac3},{bnb_cosmics}, {"reco_asso_showers>0"}, data5e19, "reco_asso_showers>0", "shower_start_y",false);

            bdt_variable v_trk_length("reco_track_displacement","(100,0,300)","Track Length Y [cm]","false","d");
            validateOverlay({v_trk_length},{bnb_cosmics}, {"reco_asso_tracks>0"}, data5e19, "reco_asso_tracks>0", "shower_track_length",false);


            std::string cm_angle = "(fabs("+E1+" - "+E2+")/("+p_pi+"))";

            // Signal definition and cuts for MC plots
            std::string true_signal = "sim_shower_overlay_fraction[0]<0.1 && sim_shower_overlay_fraction[1]<0.1 && sim_track_matched[0]==1 && mctruth_num_exiting_pi0>0 && sim_shower_pdg[0]==22 && sim_shower_pdg[1]==22 && sim_shower_parent_pdg[0]==111 && sim_shower_parent_pdg[1]==111";
            std::string my_mcCuts = "reco_asso_showers==2 &&"+E2+">0.1";
            //std::string my_mcCuts = "reco_asso_showers==2 && reco_asso_tracks==1 && sim_shower_is_true_shower && "+E2+">0.1";
            std::string my_dataCuts = "reco_asso_showers==2 && reco_asso_tracks==1 && "+E2+">0.1";
            //std::string my_dataCuts = "reco_asso_showers==2 && reco_asso_tracks==1 && sim_shower_is_true_shower && "+E2+">0.1";

            // Do the stuff, make the plots
            /*
               bdt_variable invmass(invariant_mass,"(12, 0, 0.5)","#pi^{0} Invariant Mass [GeV]","false","d");
               validateOverlay({invmass},{bnb_cosmics}, {my_mcCuts}, data5e19, my_dataCuts, "pi0_invmass_v5");
               validateOverlay({invmass},{bnb_cosmics_v6}, {my_mcCuts}, data5e19_v6, my_dataCuts, "pi0_invmass_v6");

               bdt_variable centerOfMass_angle(cm_angle,"(12, 0, 1.0)","#theta^{CM}_{#gamma #gamma}","false","d");
               validateOverlay({centerOfMass_angle},{bnb_cosmics}, {my_mcCuts}, data5e19, my_dataCuts, "cmangle_v5");
               validateOverlay({centerOfMass_angle},{bnb_cosmics_v6}, {my_mcCuts}, data5e19_v6, my_dataCuts, "cmangle_v6");

               bdt_variable pion_momentum(p_pi, "(12, 0, 0.5)", "Reco. #pi^{0} Momentum [GeV]", "false", "d");
               validateOverlay({pion_momentum},{bnb_cosmics}, {my_mcCuts}, data5e19, my_dataCuts, "pimom_v5");
               validateOverlay({pion_momentum},{bnb_cosmics_v6}, {my_mcCuts}, data5e19_v6, my_dataCuts, "pimom_v6");

               bdt_variable energy_ratio(Eratio, "(10, 1, 10)", "Ratio of Shower Energies [GeV]", "false", "d");
               validateOverlay({energy_ratio},{bnb_cosmics}, {my_mcCuts}, data5e19, my_dataCuts, "e_ratio_v5");
               validateOverlay({energy_ratio},{bnb_cosmics_v6}, {my_mcCuts}, data5e19_v6, my_dataCuts, "e_ratio_v6");
               */
            std::string s_reco_vertex_res ="(sqrt(pow(reco_vertex_x - mctruth_nu_vertex_x,2)-pow(reco_vertex_y - mctruth_nu_vertex_y,2)-pow(reco_vertex_z - mctruth_nu_vertex_z,2)))";
            bdt_variable v_reco_vertex_res (s_reco_vertex_res,"(54,0,100)","Vertex Resolutione cm", false,"d");

            //validateOverlay(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, bdt_file* data, std::string datacut, std::string pdfname, bool islog);

            validateOverlay({v_reco_vertex_res},{bnb_cosmics}, {"reco_asso_showers>0 && reco_asso_tracks > 0 && sim_shower_is_true_shower"}, data5e19,"reco_asso_showers>0 && reco_asso_tracks>0", "vertex_res",true);


            bdt_variable v_reco_vertex_y("reco_vertex_y","(54,-125,125)","Vertex y [cm]","false","d");

            validateOverlay({v_reco_vertex_y},{bnb_cosmics}, {"reco_asso_showers>0 && reco_asso_tracks > 0 && sim_shower_is_true_shower"}, data5e19,"reco_asso_showers>0 && reco_asso_tracks>0", "reco_vertex_y");


            std::string s_reco_conv_dist = "sqrt(pow(reco_shower_startx - reco_vertex_x,2)-pow(reco_shower_starty - reco_vertex_y,2)-pow(reco_shower_startz - reco_vertex_z,2))";
            bdt_variable v_reco_conv_dist(s_reco_conv_dist,"(54,0,100)","Conv Dist [cm]","false","d");

            validateOverlay({v_reco_conv_dist},{bnb_cosmics}, {"reco_asso_showers>0 && reco_asso_tracks > 0"}, data5e19, "reco_asso_showers>0 && reco_asso_tracks>0", "conv_length_all",true);
            validateOverlay({v_reco_conv_dist},{bnb_cosmics}, {"reco_asso_showers>0 && reco_asso_tracks > 0 && sim_shower_pdg==22"}, data5e19, "reco_asso_showers>0 && reco_asso_tracks>0", "conv_length_photon",true);
            validateOverlay({v_reco_conv_dist},{bnb_cosmics}, {"reco_asso_showers>0 && reco_asso_tracks > 0 && abs(sim_shower_pdg)==11"}, data5e19, "reco_asso_showers>0 && reco_asso_tracks>0", "conv_length_electron",true);
            validateOverlay({v_reco_conv_dist},{bnb_cosmics}, {"reco_asso_showers>0 && reco_asso_tracks > 0 && abs(sim_shower_pdg)==11 && abs(sim_shower_parent_pdg) == 13"}, data5e19, "reco_asso_showers>0 && reco_asso_tracks>0", "conv_length_electron_parent_muon",true);


            bdt_variable v_reco_shower_theta("reco_shower_theta_yz","(30,-3.2,3.2)","Reco Shower Theta yz [Rad]","false","d");
            bdt_variable v_reco_shower_phi("reco_shower_phi_yx","(30,-3.2,3.2)","Reco Shower Phi yx [Rad]","false","d");

            validateOverlay({v_reco_shower_theta},{bnb_cosmics}, {"reco_asso_showers>0 && reco_asso_tracks > 0"}, data5e19, "reco_asso_showers>0 && reco_asso_tracks>0", "shower_theta_all",false);
            validateOverlay({v_reco_shower_theta},{bnb_cosmics}, {"reco_asso_showers>0 && reco_asso_tracks > 0 && sim_shower_pdg==22"}, data5e19, "reco_asso_showers>0 && reco_asso_tracks>0", "shower_theta_photon",false);
            validateOverlay({v_reco_shower_theta},{bnb_cosmics}, {"reco_asso_showers>0 && reco_asso_tracks > 0 && abs(sim_shower_pdg)==11"}, data5e19, "reco_asso_showers>0 && reco_asso_tracks>0", "shower_theta_electron",false);

            validateOverlay({v_reco_shower_phi},{bnb_cosmics}, {"reco_asso_showers>0 && reco_asso_tracks > 0"}, data5e19, "reco_asso_showers>0 && reco_asso_tracks>0", "shower_phi_all",false);
            validateOverlay({v_reco_shower_phi},{bnb_cosmics}, {"reco_asso_showers>0 && reco_asso_tracks > 0 && sim_shower_pdg==22"}, data5e19, "reco_asso_showers>0 && reco_asso_tracks>0", "shower_phi_photon",false);
            validateOverlay({v_reco_shower_phi},{bnb_cosmics}, {"reco_asso_showers>0 && reco_asso_tracks > 0 && abs(sim_shower_pdg)==11"}, data5e19, "reco_asso_showers>0 && reco_asso_tracks>0", "shower_phi_electron",false);


            // Plot stuff with increasing no. showers cuts
            bdt_variable overlay_fraction("sim_shower_overlay_fraction", "(50, 0, 1)", "Shower Overlay Fraction", "false", "d");
            std::vector<std::string> showerCuts = {"reco_asso_showers>0", "reco_asso_showers==1", "reco_asso_showers==2", "reco_asso_showers==3"};
            validateOverlay2({overlay_fraction}, {bnb_cosmics}, showerCuts, "overlay_frac_v5");
            validateOverlay2({overlay_fraction}, {bnb_cosmics_v6}, showerCuts, "overlay_frac_v6");







            //bdt_variable v_reco_shower_dedx_plane2 (s_reco_shower_dedx_plane2,"(48,0,6.5)", "Median Shower dE/dx plane 2 [MeV/cm]",false,"d");
            validateOverlay({v_reco_shower_dedx_plane2 },{bnb_cosmics}, {"reco_asso_showers>0 && sim_shower_is_true_shower"}, data5e19,"reco_asso_showers>0  ", "shower_dedx_plane2",false);
            validateOverlay({v_reco_shower_dedx_plane1 },{bnb_cosmics}, {"reco_asso_showers>0 && sim_shower_is_true_shower"}, data5e19,"reco_asso_showers>0  ", "shower_dedx_plane1", false);
            validateOverlay({v_reco_shower_dedx_plane0 },{bnb_cosmics}, {"reco_asso_showers>0 && sim_shower_is_true_shower"}, data5e19,"reco_asso_showers>0  ", "shower_dedx_plane0",false);

            std::string data_conditions_shower = "reco_asso_showers>0";
            std::string mc_conditions_shower = data_conditions_shower + "&& sim_shower_is_true_shower" ;

            makeIncrementPlots ("median_shower_dedx_plane2",  v_reco_shower_dedx_plane2, s_reco_shower_angle_wire_plane2, bnb_cosmics, data5e19, 10, M_PI/2, 0., data_conditions_shower, mc_conditions_shower);
            makeIncrementPlots ("median_shower_dedx_plane1",  v_reco_shower_dedx_plane1, s_reco_shower_angle_wire_plane1, bnb_cosmics, data5e19,  10, M_PI/2, 0.,  data_conditions_shower, mc_conditions_shower);
            makeIncrementPlots ("median_shower_dedx_plane0",  v_reco_shower_dedx_plane0, s_reco_shower_angle_wire_plane0, bnb_cosmics, data5e19,  10, M_PI/2, 0.,  data_conditions_shower, mc_conditions_shower);


            std::string s_reco_shower_energy = "reco_shower_energy[0]/1000.0";
            makeIncrementPlots ("median_shower_dedx_plane2_wrt_shower_energy",  v_reco_shower_dedx_plane2, s_reco_shower_energy, bnb_cosmics, data5e19, 10, 1., 0., data_conditions_shower, mc_conditions_shower);




            //Plot 1: ===========================================================================================
            /*
               TCanvas *c0 = new TCanvas(); 
               c0->cd();

               TH1* h_reco_vertex_res_photon =  (TH1*) bnb_cosmics->getTH1(v_reco_vertex_res,   "reco_asso_showers > 0 && reco_asso_tracks > 0 && sim_shower_is_true_shower && sim_shower_matched && sim_shower_pdg == 22", "photon_truth", 0, 1);
               TH1* h_reco_vertex_res_photon_notrack =  (TH1*) bnb_cosmics->getTH1(v_reco_vertex_res,   "reco_asso_showers > 0 && reco_asso_tracks == 0 && sim_shower_is_true_shower && sim_shower_matched && sim_shower_pdg == 22", "photon_truth_notrack", 0, 1);
               c0->cd();
               TH1* h_reco_vertex_res_electron =  (TH1*) bnb_cosmics->getTH1(v_reco_vertex_res, "reco_asso_showers > 0 && reco_asso_tracks > 0 && sim_shower_is_true_shower && sim_shower_matched && abs(sim_shower_pdg) == 11", "electron_truth", 0, 1);
               TH1* h_reco_vertex_res_electron_notrack =  (TH1*) bnb_cosmics->getTH1(v_reco_vertex_res, "reco_asso_showers > 0 && reco_asso_tracks == 0 && sim_shower_is_true_shower && sim_shower_matched && abs(sim_shower_pdg) == 11 ", "electron_truth_notrack", 0, 1);

               c0->cd();
               c0->SetLogy();

               h_reco_vertex_res_photon->SetLineColor(kRed);
               h_reco_vertex_res_photon_notrack->SetLineColor(kRed);
               h_reco_vertex_res_photon_notrack->SetLineStyle(9);

               h_reco_vertex_res_electron->SetLineColor(kBlue);
               h_reco_vertex_res_electron_notrack->SetLineColor(kBlue);
               h_reco_vertex_res_electron_notrack->SetLineStyle(9);

               h_reco_vertex_res_photon->Draw("hist");
               h_reco_vertex_res_photon_notrack->Draw("hist same");
               h_reco_vertex_res_electron->Draw("hist same");
               h_reco_vertex_res_electron_notrack->Draw("hist same");

               h_reco_vertex_res_electron->SetMaximum(std::max(h_reco_vertex_res_photon->GetMaximum(),h_reco_vertex_res_electron->GetMaximum())*1.3);

               c0->SaveAs("hope_conv.pdf","pdf");



            //Plot 1: ===========================================================================================

            std::string s_reco_conv_dist ="log10(sqrt(pow(reco_shower_startx - reco_vertex_x,2)-pow(reco_shower_starty - reco_vertex_y,2)-pow(reco_shower_startz - reco_vertex_z,2)))";
            std::string s_true_conv_dist = "log10(sqrt(pow(sim_shower_start_x - mctruth_nu_vertex_x,2)-pow(sim_shower_start_y - mctruth_nu_vertex_y,2)-pow(sim_shower_start_z - mctruth_nu_vertex_z,2)))";

            bdt_variable v_reco_conv_dist (s_reco_conv_dist,"(54,-2,4)","Reconstructed Conversion Distance Log[cm]", false,"d");
            bdt_variable v_true_conv_dist (s_true_conv_dist,"(54,-2,4)","True Conversion Distance Log[cm]", false,"d");

            TCanvas *c = new TCanvas(); 
            c->cd();

            TH1* h_reco_conv_dist =  (TH1*) bnb_cosmics->getTH1(v_reco_conv_dist, "reco_asso_showers > 0 && reco_asso_tracks > 0 && sim_shower_is_true_shower && sim_shower_matched ", "photon_truth", 0, 1);
            c->cd();
            TH1* h_true_conv_dist =  (TH1*) bnb_cosmics->getTH1(v_true_conv_dist, "reco_asso_showers > 0 && reco_asso_tracks > 0 && sim_shower_is_true_shower && sim_shower_matched", "true_truth", 0, 1);
            //TH1* h_true_conv_dist =  (TH1*) bnb_cosmics->getTH1(v_true_conv_dist, "reco_asso_showers > 0 && reco_asso_tracks > 0 ", "ee_truth", 0, 1);
            c->cd();
            TH1* h_data_conv_dist =  (TH1*) data5e19->getTH1(v_reco_conv_dist, "reco_asso_showers > 0 && reco_asso_tracks > 0 ", "data", 0, 1);
            c->cd();

            h_reco_conv_dist->SetLineColor(kRed);
            h_true_conv_dist->SetLineColor(kBlue);
            h_data_conv_dist->SetLineColor(kBlack);

            h_reco_conv_dist->Draw("hist");
            h_true_conv_dist->Draw("hist same");
            h_data_conv_dist->Draw("E1 same");

            h_reco_conv_dist->SetMaximum(std::max(h_true_conv_dist->GetMaximum(),h_reco_conv_dist->GetMaximum())*1.3);

            c->SaveAs("hope.pdf","pdf");

*/

        }
        return 0;

    }

    int validateOverlay(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, bdt_file * data, std::string datacut, std::string pdfname){
        validateOverlay(vars, files, cuts, data,datacut,pdfname,false);
        return 0;
    }

    int validateOverlay(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, bdt_file * data, std::string datacut, std::string pdfname, bool islog){

        TCanvas *c = new TCanvas();
        c->cd();
        if(islog) c->SetLogy();

        double maxval = -9999;
        TLegend* leg=new TLegend(0.7,0.7,0.9,0.9);

        for(int i=0; i<files.size();i++){

            c->cd(); 
            //       TH1* th1_overlay =  (TH1*) files[i]->getTH1(vars[i], cuts[i] +"&& 1" , "photon_truth_overlay"+std::to_string(i), 6.6e20, 1);
            //      TH1* th1_mcish =  (TH1*) files[i]->getTH1(vars[i], cuts[i] +"&& 0" , "photon_truth_mcish"+std::to_string(i), 6.6e20, 1);
            TH1* th1_overlay =  (TH1*) files[i]->getTH1(vars[i], cuts[i] +"&& sim_shower_overlay_fraction > 0.2" , "photon_truth_overlay"+std::to_string(i), 6.6e20, 1);
            TH1* th1_mcish =  (TH1*) files[i]->getTH1(vars[i], cuts[i] +"&& sim_shower_overlay_fraction < 0.2" , "photon_truth_mcish"+std::to_string(i), 6.6e20, 1);
            THStack * ts1 = new THStack();

            th1_overlay->SetFillColor(files[i]->col);
            th1_overlay->SetFillStyle(3244);
            th1_mcish->SetFillColor(files[i]->col);

            if (th1_mcish->Integral() == 0. || th1_overlay->Integral() == 0) {
                std::cout << "MC hist integral in " << vars[i].unit.c_str() << " is 0!" << std::endl;
                return 1;
            }
            if (th1_overlay->Integral() == 0.) {
                std::cout << "Overlay hist integral in " << vars[i].unit.c_str() << " is 0!" << std::endl;
                return 1;
            }
            double norm = th1_overlay->Integral()+ th1_mcish->Integral();
            th1_overlay->Scale(1.0/norm);
            th1_mcish->Scale(1.0/norm);

            ts1->Add(th1_overlay);
            ts1->Add(th1_mcish);
            c->cd();
            maxval  = std::max(maxval, ts1->GetMaximum());
            ts1->Draw("hist");
            ts1->SetMaximum(maxval*1.5);
            ts1->GetXaxis()->SetTitle(vars[i].unit.c_str());


            leg->AddEntry(th1_overlay,"BNB Overlay","f");
        }
        c->cd();
        TH1* h_data =  (TH1*) data->getTH1(vars[0], datacut , "data_truth_overlay", 0, 1);
        c->cd();
        h_data->SetLineColor(kBlack);
        h_data->SetMarkerStyle(20);
        h_data->Draw("E1 same");
        leg->AddEntry(h_data,"Data5e19","lp");
        leg->Draw("same");


        c->cd();
        c->SaveAs((pdfname+".pdf").c_str(),"pdf");    

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

    int makeIncrementPlots (std::string name, bdt_variable variable, std::string cut_variable, bdt_file* bnb_cosmics, bdt_file* data5e19, int n_increments, double cut_val_max, double cut_val_min,std::string base_data_conditions, std::string base_mc_conditions ){

        

        double increment = (cut_val_max - cut_val_min)/(n_increments);
        double min = cut_val_min;
        double max = cut_val_min + increment;


        std::string s_min = std::to_string(min);
        std::string s_max= std::to_string(max);


        for (int i = 1; i <= n_increments; i++){

            std::string cuts = cut_variable+ " > " +s_min + "     && " + cut_variable  + " <= " + s_max;
            std::string data_conditions = base_data_conditions + "&& " + cuts;
            std::string mc_conditions= base_mc_conditions + "&&" + cuts ;


           validateOverlay({variable },{bnb_cosmics}, {mc_conditions}, data5e19,{data_conditions}, name + "_" + std::to_string(i) ,false);

            min = max;
            max = min+increment;
            s_min =  std::to_string(min);
            s_max= std::to_string(max);

        } 

        return 0;
    }








