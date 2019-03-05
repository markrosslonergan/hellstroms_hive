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



int main (int argc, char *argv[]){

	//This is a standardized location on /pnfs/ that everyone can use. 
	//std::string dir = "/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_mcc9_v5/";
	std::string dir = "/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_mcc9_v6/";
	std::string olddir = "/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_mcc9_v5/";
	std::string mydir = "/pnfs/uboone/persistent/users/amogan/singlePhoton/samples/";


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
	bdt_file *bnb_cosmics = new bdt_file(dir, "bnb_overlay_run1_v6.0.root", "BNBCosmics", "hist","singlephoton/",  kBlue-4, bkg_flow);
	bdt_file *data5e19    = new bdt_file(mydir, "vertexed_data5e19_v6.root",	"Data5e19",	   "E1p","singlephoton/",  kBlack, data_flow);
	bdt_file *bnbext    = new bdt_file(olddir, "bnbext_mcc9_v5.0.root",	"BNBext",	"E1p","singlephoton/",  kGreen-3, data_flow);

	std::vector<bdt_file*> bdt_files = {signal_cosmics, bnb_cosmics, bnbext, data5e19};

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

    // Strings to calculate variables
    std::string shower_index1 = "(reco_shower_ordered_energy_index[0])";
    std::string shower_index2 = "(reco_shower_ordered_energy_index[1])";

		std::string proton_mass = "0.938272";
    std::string E1 = "reco_shower_energy["+shower_index1+"]";
    std::string E2 = "reco_shower_energy["+shower_index2+"]";
    std::string Eratio = "("+E1+"/"+E2+")";
    std::string two_shower_opening_angle = "(reco_shower_dirx[0]*reco_shower_dirx[1] + reco_shower_diry[0]*reco_shower_diry[1] + reco_shower_dirz[0]*reco_shower_dirz[1])";
		std::string invariant_mass = "sqrt(2.0*"+E1+"*"+E2+"*(1.0-"+two_shower_opening_angle+"))/1000";

    std::string p_pi_x = "(reco_shower_energy["+shower_index1+"]*reco_shower_dirx["+shower_index1+"] + reco_shower_energy["+shower_index2+"]*reco_shower_dirx["+shower_index2+"])";
    std::string p_pi_y = "(reco_shower_energy["+shower_index1+"]*reco_shower_diry["+shower_index1+"] + reco_shower_energy["+shower_index2+"]*reco_shower_diry["+shower_index2+"])";
    std::string p_pi_z = "(reco_shower_energy["+shower_index1+"]*reco_shower_dirz["+shower_index1+"] + reco_shower_energy["+shower_index2+"]*reco_shower_dirz["+shower_index2+"])";
    std::string p_pi = "sqrt("+p_pi_x+"*"+p_pi_x+" + "+p_pi_y+"*"+p_pi_y+" + "+p_pi_z+"*"+p_pi_z+")";

    std::string cm_angle = "(fabs("+E1+" - "+E2+")/("+p_pi+"))";
    
    // Signal definition and cuts for MC plots
    std::string true_signal = "sim_shower_overlay_fraction[0]<0.1 && sim_shower_overlay_fraction[1]<0.1 && sim_track_matched[0]==1 && mctruth_num_exiting_pi0>0 && sim_shower_pdg[0]==22 && sim_shower_pdg[1]==22 && sim_shower_parent_pdg[0]==111 && sim_shower_parent_pdg[1]==111";
    std::string mycuts = "reco_asso_showers==2 && reco_asso_track==1 && sim_shower_is_true_shower && "+E2+">0.01";

    // Do the stuff, make the plots
    bdt_variable invmass(invariant_mass,"(12, 0, 0.5)","#pi^{0} Invariant Mass [GeV]","false","d");
    validateOverlay({invmass},{bnb_cosmics}, {mycuts}, data5e19,"reco_asso_showers==2 && reco_asso_tracks==1", "pi0_invmass");

    bdt_variable centerOfMass_angle(cm_angle,"(12, 0, 1.0)","#theta^{CM}_{#gamma #gamma}","false","d");
    validateOverlay({centerOfMass_angle},{bnb_cosmics}, {mycuts}, data5e19,"reco_asso_showers==2 && reco_asso_tracks==1", "cmangle");

    bdt_variable pion_momentum(p_pi, "(12, 0, 0.5)", "Reco. #pi^{0} Momentum [GeV]", "false", "d");
    validateOverlay({pion_momentum},{bnb_cosmics}, {mycuts}, data5e19,"reco_asso_showers==2 && reco_asso_tracks==1", "pimom");

    bdt_variable energy_ratio(Eratio, "(12, 0, 0.5)", "Ratio of Shower Energies [GeV]", "false", "d");
    validateOverlay({energy_ratio},{bnb_cosmics}, {mycuts}, data5e19,"reco_asso_showers==2 && reco_asso_tracks==1", "e_ratio");



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

     for(int i=0; i<files.size();i++){

         c->cd(); 
         TH1* th1_overlay =  (TH1*) files[i]->getTH1(vars[i], cuts[i] +"&& sim_shower_overlay_fraction > 0.2" , "photon_truth_overlay"+std::to_string(i), 6.6e20, 1);
         TH1* th1_mcish =  (TH1*) files[i]->getTH1(vars[i], cuts[i] +"&&   sim_shower_overlay_fraction < 0.2" , "photon_truth_mcish"+std::to_string(i), 6.6e20, 1);
         THStack * ts1 = new THStack();

         th1_overlay->SetFillColor(files[i]->col);
         th1_overlay->SetFillStyle(3244);
         th1_mcish->SetFillColor(files[i]->col);
         
         double norm = th1_overlay->Integral()+ th1_mcish->Integral();
         th1_overlay->Scale(1.0/norm);
         th1_mcish->Scale(1.0/norm);

         ts1->Add(th1_overlay);
         ts1->Add(th1_mcish);
         c->cd();
         maxval  = std::max(maxval, ts1->GetMaximum());
         ts1->Draw("hist");
         ts1->SetMaximum(maxval*1.3);
         ts1->GetXaxis()->SetTitle(vars[i].unit.c_str() );
     }

        c->cd();
        TH1* h_data =  (TH1*) data->getTH1(vars[0], datacut , "data_truth_overlay", 0, 1);
        c->cd();
        h_data->SetLineColor(kBlack);
        h_data->Draw("E1 same");
    



        c->cd();
        c->SaveAs((pdfname+".pdf").c_str(),"pdf");    

        return 0;

}





