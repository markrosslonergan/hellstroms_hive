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
#include "bdt_spec.h"




int main (int argc, char *argv[]){

	// Just some simple argument things
	//===========================================================================================

	std::string dir = "/home/mark/work/uBooNE/photon/tmva/";
	//std::string dir = "/uboone/app/users/markrl/single_photon/hellstroms_hive/hellstroms_hive/";
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

	//some optioni/argument stuff
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


	std::string new_precuts;
	std::string num_track_cut = "==1";
	if(istrack == "track"){
		new_precuts =  "reco_nu_vtx_dist_to_closest_tpc_wall > 10 && shortest_asso_shower_to_vert_dist > 1 && reco_track_displacement[0] < 100 &&  reco_shower_helper_energy[0] > 0.03 && track_info.reco_track_good_calo[0]>0 ";
		num_track_cut = "==1";

	}else if(istrack == "notrack"){
		new_precuts = "reco_nu_vtx_dist_to_closest_tpc_wall > 10 && reco_shower_helper_energy[0] > 0.03 && totalpe_ibg_sum > 10";
		num_track_cut = "==0";
	}



	std::string postcuts = {"reco_asso_tracks == 1 "};//

	//Set up some info about the BDTs to pass along
	bdt_info bnb_bdt_info("bnb_"+istrack, "BNB focused BDT","(65,0.2,0.7)");
	bdt_info cosmic_bdt_info("cosmic_"+istrack, "Cosmic focused BDT","(65,0.2,0.7)");
	bdt_info ncpi0_bdt_info("ncpi0_"+istrack, "NCPi0 focused BDT","(65,0.2,0.7)");


	//std::string base_cuts = "totalpe_ibg_sum > 0 && reco_asso_showers == 1 && reco_asso_tracks "+num_track_cut;
	std::string base_cuts = "totalpe_ibg_sum > 30 && reco_asso_showers == 1 && reco_asso_tracks "+num_track_cut;
	std::string intime_base_cuts = "totalpe_ibg_sum > 30 &&"+base_cuts; ;//"reco_asso_showers == 1 && reco_asso_tracks "+num_track_cut;

	std::string signal_definition = "is_delta_rad == 1 && true_nu_vtx_fid_contained == 1";
	std::string background_definition = "is_delta_rad == 0";
	
	std::string true_signal = "shower_matched_to_ncdeltarad_photon[0]==1 && track_matched_to_ncdeltarad_proton[0]==1";

	std::string true_bkg    = "true_shower_origin[0]==1";
	if(istrack == "track"){
		true_signal = true_signal+ "&& track_matched_to_ncdeltarad_proton[0]==1";
		true_bkg = true_bkg +"&& true_track_origin[0]==1";
	}


	// takes 5 arguments ( 
	bdt_flow signal_pure_flow(base_cuts +"&&"+signal_definition +"&&"+ true_signal,	new_precuts+"&& passed_swtrigger ==1",	postcuts,cosmic_bdt_info,	bnb_bdt_info);
	bdt_flow signal_flow(base_cuts +"&&"+signal_definition ,	new_precuts+"&& passed_swtrigger ==1  ",	postcuts,cosmic_bdt_info,	bnb_bdt_info);
	bdt_flow cosmic_flow(intime_base_cuts, new_precuts , postcuts,	cosmic_bdt_info,bnb_bdt_info);
	bdt_flow bkg_flow(base_cuts +"&&"+background_definition,	new_precuts+ "&& passed_swtrigger ==1 ",postcuts,	cosmic_bdt_info,	bnb_bdt_info);
	bdt_flow bkg_pure_flow(base_cuts +"&&"+background_definition+"&&"+ true_bkg ,new_precuts+ "&& passed_swtrigger ==1",postcuts,	cosmic_bdt_info,	bnb_bdt_info);

	bdt_flow data_flow(base_cuts,				new_precuts+"&& passed_swtrigger ==1",postcuts,	cosmic_bdt_info, 	bnb_bdt_info);
	bdt_flow ncpi0_flow(base_cuts + " &&" + true_bkg + "&& totalpe_ibg_sum >0 ", new_precuts +"&& passed_swtrigger ==1",postcuts, cosmic_bdt_info, ncpi0_bdt_info); 


	// BDT files, in the form (location, rootfile, name, hisotgram_options, tfile_folder, tag, color, BDT_CUT )		
	bdt_file *signal_pure    = new bdt_file(dir+"samples/vectored/", "vertexed_ncdeltaradcosmics_mcc88_v1.0.root",	"NCDeltaRad",	   "hist","",  kRed-7, signal_pure_flow);
	bdt_file *signal_cosmics = new bdt_file(dir+"samples/vectored/", "vertexed_ncdeltaradcosmics_mcc88_v1.0.root", "NCDeltaRadCosmics", "hist","",  kRed-7, signal_flow);
	bdt_file *bnb_pure    = new bdt_file(dir+"samples/vectored/", "vertexed_bnbcosmic_mcc88_v2.0.root", "BNBPure",	  "hist","",  kBlue-4, bkg_pure_flow);
	bdt_file *bnb_cosmics = new bdt_file(dir+"samples/vectored/", "vertexed_bnbcosmic_mcc88_v2.0.root", "BNBCosmics", "hist","",  kBlue-4, bkg_flow);

	bdt_file *intime = new bdt_file(dir+"samples/vectored/", "vertexed_intime_v2.0_mcc88.root" ,"IntimeCosmics","hist","", kGreen-3, cosmic_flow);

	bdt_file *ncpi0 = new bdt_file(dir+"samples/vectored/", "vertexed_ncpi0cosmic_mcc88_v1.0.root" ,"NCpi0","hist","", kGreen-3, ncpi0_flow);


         bdt_file *overlay = new bdt_file(dir+"samples/vectored/", "vertexed_overlay_mcc88_v1.0.root",   "BNBOverlay",      "hist","",  kMagenta-3, bkg_flow);
 
         bdt_file *data5e19    = new bdt_file(dir+"samples/vectored/", "vertexed_data5e19_v10.root",     "Data5e19",        "E1p","",  kBlack, data_flow);
         bdt_file *bnbext    = new bdt_file(dir+"samples/vectored/", "vertexed_bnbext_mcc88_v8.0.root",  "BNBext",       "E1p","",  kBlack, data_flow);


	//std::vector<bdt_file*> bdt_files = {signal_pure, signal_cosmics, bnb_pure, bnb_cosmics, intime, data5e19,bnbext};
	std::vector<bdt_file*> bdt_files = {signal_pure, signal_cosmics, bnb_pure, bnb_cosmics, intime, overlay, data5e19, bnbext};

	signal_pure->scale_data = 3.1;
	signal_cosmics->scale_data = 3.1;



	//you get access to these with track_info.XXX
	for(auto &f: bdt_files){
		addPreFriends(f,"track");
	}


	//Adding plot names
	signal_pure->addPlotName("NC Delta Radiative");
	signal_cosmics->addPlotName("NC Delta Rad w/ Corsika");

	bnb_pure->addPlotName("BNB Backgrounds");
	bnb_cosmics->addPlotName("BNB w/ Corsika");

	intime->addPlotName("Intime Corsika cosmics");


	//Variables!

	std::string gx = "true_shower_startx[0]-true_shower_endx[0]";
	std::string gy = "true_shower_starty[0]-true_shower_endy[0]";
	std::string gz = "true_shower_startz[0]-true_shower_endz[0]";


	std::string px = "true_track_startx[0]-true_track_endx[0]";
	std::string py = "true_track_starty[0]-true_track_endy[0]";
	std::string pz = "true_track_startz[0]-true_track_endz[0]";

	std::string  cos_angle_true_t  = "("+gx+"*"+px+"+"+gy+"*"+py+"+"+gz+"*"+pz+")";
	std::string  cos_angle_true_b = "(sqrt("+px+"*"+px+"+"+py+"*"+py+"+"+pz+"*"+pz+")*sqrt("+gx+"*"+gx+"+"+gy+"*"+gy+"+"+gz+"*"+gz+"))";
	std::string  cos_angle_true = cos_angle_true_t+"/"+cos_angle_true_b;

	
	std::string angle_track_shower ="(reco_track_dirx[0]*reco_shower_dirx[0]+reco_track_diry[0]*reco_shower_diry[0]+reco_track_dirz[0]*reco_shower_dirz[0])";
	std::string shower_mom = "reco_shower_helper_energy[0]"; 
	std::string reco_track_energy = "track_info.reco_track_kinetic[0]+0.938272046 ";
	std::string track_mom  = "sqrt("+reco_track_energy+"*"+reco_track_energy+"-0.938272046*0.938272046)";

	std::string invariant_mass = "(0.938272046*0.938272046+2.0*("+reco_track_energy+"*reco_shower_helper_energy[0]-"+track_mom+"*"+shower_mom+"*"+angle_track_shower+"))";
	std::string invariant_mass_true = "0.938272046*0.938272046+2.0*(delta_proton_energy*delta_photon_energy-sqrt(delta_proton_energy*delta_proton_energy-0.938272046*0.938272046)*delta_photon_energy*"+cos_angle_true+")";


	bdt_variable invar_reco(invariant_mass,"(80,1,3)","Invariant Mass",true,"d");
	bdt_variable invar_true(invariant_mass_true,"(80,1,3)","Invariant Mass",true,"d");


	bdt_variable cos_reco(angle_track_shower,"(80,-1,1)","Cos",true,"d");
	bdt_variable cos_true(cos_angle_true,"(80,-1,1)","Cos ",true,"d");

	ftest->cd();
	TCanvas *c = new TCanvas("invar sig");
	c->Divide(2,1);
	c->cd(1);


	std::string cut_0 = signal_pure->getStageCuts(1,-9,-9);	
	std::string j = "invar";
	TH1 * hinvar_sig_true  = signal_pure->getTH1(invar_true, cut_0, j+"_pure_true", 6.6e20);
	TH1 * hinvar_sig_reco  = signal_pure->getTH1(invar_reco, cut_0, j+"_pure_reco", 6.6e20);
	TH1 * hcos_sig_reco  = signal_pure->getTH1(cos_reco, cut_0, j+"_cos_reco", 6.6e20);
	TH1 * hcos_sig_true  = signal_pure->getTH1(cos_true, cut_0, j+"_cos_true", 6.6e20);

	c->cd(1);
	hinvar_sig_true->Draw("hist");
	hinvar_sig_reco->Draw("same hist");

	hinvar_sig_true->GetXaxis()->SetRangeUser(1,3);
	hinvar_sig_reco->SetLineColor(kBlue);
	hinvar_sig_true->SetMaximum( std::max(hinvar_sig_reco->GetMaximum(), hinvar_sig_true->GetMaximum())*1.2);

	hinvar_sig_true->Write();
	hinvar_sig_reco->Write();

	c->cd(2);
	hcos_sig_true->Draw("hist");
	hcos_sig_reco->Draw("same hist");

	hcos_sig_true->GetXaxis()->SetRangeUser(-1,1);
	hcos_sig_reco->SetLineColor(kBlue);
	hcos_sig_true->SetMaximum( std::max(hcos_sig_reco->GetMaximum(), hcos_sig_true->GetMaximum())*1.2);



	c->Write();




	ftest->Close();













	return 0;

}
