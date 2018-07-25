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



int main (int argc, char *argv[]){

	//This is a standardized location on /pnfs/ that everyone can use. 
	std::string dir = "/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_v3/";


	std::string mode_option = "fake"; 
	std::string xml = "default.xml";
	std::string analysis_tag ="ncdelta1g1p";


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
	MVALoader xml_methods(xml);
	std::vector<method_struct> TMVAmethods  = xml_methods.GetMethods(); 


	//Load up variables and precut object ! ATTN: Found in variable_list.cxx in parent src/ folder
	variable_list var_list(analysis_tag);

	//Get all the variables you want to use	
	std::vector<bdt_variable> vars = var_list.all_vars;

	//This is a vector each containing a precut, they are all added together to make the whole "precut"
	std::vector<std::string> vec_precuts = var_list.all_precuts;

	//We dont currently use postcuts
	std::string postcuts = "1";


	//We have 2 BDT's one for cosmics and one for BNB related backgrounds only
	//Set up some info about the BDTs to pass along
	bdt_info bnb_bdt_info("bnb_"+analysis_tag, "BNB focused BDT","(45,0.3,0.6)");
	bdt_info cosmic_bdt_info("cosmic_"+analysis_tag, "Cosmic focused BDT","(45,0.2,0.75)");

	//Train on "good" signals, defined as ones matched to the ncdelta and have little "clutter" around.	
	std::string true_signal = "shower_matched_to_ncdeltarad_photon[0]==1";
	std::string true_bkg    = "true_shower_origin[0]==1";
	std::string num_track_cut ;

	if(analysis_tag == "ncdelta1g1p"){
			true_signal = true_signal+ "&& track_matched_to_ncdeltarad_proton[0]==1";
			true_bkg = true_bkg +"&& true_track_origin[0]==1";
			num_track_cut =  "==1";

				bnb_bdt_info.setTopoName("1#gamma1p");
			cosmic_bdt_info.setTopoName("1#gamma1p");
		}else{
			num_track_cut = "==0";
				bnb_bdt_info.setTopoName("1#gamma0p");
			cosmic_bdt_info.setTopoName("1#gamma0p");
		}

	std::string base_cuts = "reco_asso_showers == 1 && reco_asso_tracks "+num_track_cut;
	std::string signal_definition = "is_delta_rad == 1";
	std::string background_definition = "!(" +signal_definition+ ")";


	//***************************************************************************************************/
	//***********	The bdt_flows define the "flow" of the analysis, i.e what cuts at what stage  *******/
	//***************************************************************************************************/
	bdt_flow signal_pure_flow(base_cuts, 	signal_definition +"&&"+ true_signal, 	vec_precuts,	postcuts,	cosmic_bdt_info,	bnb_bdt_info);
	bdt_flow signal_flow(base_cuts, 	signal_definition , 			vec_precuts,	postcuts,	cosmic_bdt_info,	bnb_bdt_info);
	bdt_flow cosmic_flow(base_cuts,		"1", 					vec_precuts,	postcuts,	cosmic_bdt_info,	bnb_bdt_info);
	bdt_flow bkg_flow(base_cuts,		background_definition, 			vec_precuts,	postcuts,	cosmic_bdt_info,	bnb_bdt_info);
	bdt_flow bkg_pure_flow(base_cuts,	background_definition+"&&"+ true_bkg ,	vec_precuts,	postcuts,	cosmic_bdt_info,	bnb_bdt_info);
	bdt_flow data_flow(base_cuts,		"1",					vec_precuts,	postcuts,	cosmic_bdt_info, 	bnb_bdt_info);

	// BDt files , bdt_file::bdt_file(std::string indir,std::string inname, std::string intag, std::string inops, std::string inrootdir, int incol, bdt_flow inflow) :
	bdt_file *signal_pure    = new bdt_file(dir, "vertexed_ncdeltaradcosmics_fresh_v4.1.root",	"NCDeltaRad",	   "hist","",  kRed-7, signal_pure_flow);
	bdt_file *signal_cosmics = new bdt_file(dir, "vertexed_ncdeltaradcosmics_fresh_v4.1.root", "NCDeltaRadCosmics", "hist","",  kRed-7, signal_flow);
	bdt_file *bnb_pure    = new bdt_file(dir, "vertexed_bnbcosmics_fresh_v4.1.root", "BNBPure",	  "hist","",  kBlue-4, bkg_pure_flow);
	bdt_file *bnb_cosmics = new bdt_file(dir, "vertexed_bnbcosmics_fresh_v4.1.root", "BNBCosmics", "hist","",  kBlue-4, bkg_flow);
	bdt_file *intime = new bdt_file(dir, "vertexed_intime_fresh_v4.1.root" ,"IntimeCosmics","hist","", kGreen-3, cosmic_flow);
	//Data files
	bdt_file *data5e19    = new bdt_file(dir, "vertexed_data5e19_fresh_v4.1.root",	"Data5e19",	   "E1p","",  kBlack, data_flow);
	bdt_file *bnbext    = new bdt_file(dir, "vertexed_bnbext_fresh_v4.1.root",	"BNBext",	"E1p","",  kBlack, data_flow);

	bdt_file *lee = new bdt_file(dir,"vertexed_elikeleecosmics_fresh_v4.root","LEEsignal","hist","",kRed-7, signal_flow);
	bdt_file *intrinsics = new bdt_file(dir,"vertexed_nueintrinsic_fresh_v4.1.root","NueIntrinsicCosmics","hist","",kRed-7, signal_flow);
	bdt_file *ncpi0 = new bdt_file(dir,"vertexed_ncpi0cosmics_fltr_fresh_v4.1.root","NCpi0Cosmics","hist","",kRed-7, signal_flow);

	//For conviencance fill a vector with pointers to all the files to loop over.
	std::vector<bdt_file*> bdt_files = {signal_cosmics, signal_pure, bnb_pure, bnb_cosmics, intime, data5e19, bnbext};

	//The LEE signal is bigger than the SM signal by this factor
	signal_pure->scale_data = 3.1;
	signal_cosmics->scale_data = 3.1;


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
			addPreFriends(f,"track");
			addPreFriends(f,"pi0");
			if(f->tag != "Data5e19" && f->tag != "BNBext") addPreFriends(f,"bnbcorrection");
			addPreFriends(f,"shower");

			if(mode_option != "app" && mode_option != "train") f->addBDTResponses(cosmic_bdt_info, bnb_bdt_info, TMVAmethods);

			std::cout<<"Filling Base EntryLists on File  "<<f->tag<<std::endl;
			if(mode_option != "train" && mode_option != "app"){
				f->calcBaseEntryList(cosmic_bdt_info);
			}
		}
	}
	std::cout<<"--------------------------------------------------------------------------"<<std::endl;
	std::cout<<"--------------------------------------------------------------------------"<<std::endl;


	//Adding plot names
	signal_pure->addPlotName("NC Delta Radiative");
	signal_cosmics->addPlotName("LEE NC #Delta Rad w/ Corsika");
	bnb_pure->addPlotName("BNB Backgrounds");
	bnb_cosmics->addPlotName("BNB w/ Corsika");
	intime->addPlotName("Intime Corsika cosmics");
	data5e19->addPlotName("4.8e19 POT Data");
	bnbext->addPlotName("External BNB Data");

	std::cout<<"--------------------------------------------------------------------------"<<std::endl;
	std::cout<<"--------------------------------------------------------------------------"<<std::endl;




	//MELD: Best Fit Significance: 0.591875 0.5325 1.74915
	double fcoscut;
	double fbnbcut;
	if(analysis_tag == "ncdelta1g1p"){
		fcoscut = 0.591875;
		fbnbcut = 0.5325;

		//Reduced
		//fcoscut =0.475;

	}else if(analysis_tag == "ncdelta1g0p"){
		fcoscut = 0.5525;
		fbnbcut = 0.533625;
		//	Best Fit Significance: 0.5525 0.533625 1.1

	}

	//===========================================================================================
	//===========================================================================================
	//		Main flow of the program , using OPTIONS
	//===========================================================================================
	//===========================================================================================

	if(mode_option == "train") {
		std::cout<<"**********************Starting COSMIC BDT Training*************************"<<std::endl;
		if(run_cosmic) bdt_train(cosmic_bdt_info, signal_pure, intime, vars, TMVAmethods);
		std::cout<<"**********************Starting BNB BDT Training*************************"<<std::endl;
		if(run_bnb) bdt_train(bnb_bdt_info, signal_pure, bnb_pure, vars, TMVAmethods);
		return 0;



	}else if(mode_option == "app"){

		//Apply! This will update cosmic_bdt_info, signal file and bkg file. As in update them PROPERLY!	
		if(run_cosmic) bdt_app(cosmic_bdt_info, bdt_files, vars, TMVAmethods);
		if(run_bnb)    bdt_app(bnb_bdt_info, bdt_files, vars, TMVAmethods);
		return 0;
	}
	else if(mode_option == "response"){

		TFile * ftest = new TFile(("test+"+analysis_tag+".root").c_str(),"recreate");
		//Ok print out Cosmic BDT
		if(run_cosmic){
			bdt_response cosmic_response(cosmic_bdt_info, signal_pure, intime);
			cosmic_response.plot_bdt_response(ftest);
		}

		if(run_bnb){
			bdt_response bnb_response(bnb_bdt_info, signal_pure, bnb_pure);
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
				recomc.plot_recomc(ftest, bnb_cosmics, tmp, fcoscut, fbnbcut);
				return 0;

			}else{
					recomc.plot_recomc(ftest, bnb_cosmics, vars, fcoscut, fbnbcut);
			}	
		}

		if(response_only){
			recomc.is_log = true;
			if(run_cosmic){
				recomc.plot_recomc(ftest, signal_cosmics, (std::vector<bdt_variable>){signal_cosmics->getBDTVariable(cosmic_bdt_info)} , fcoscut,fbnbcut);
				recomc.plot_recomc(ftest, bnb_cosmics, (std::vector<bdt_variable>){bnb_cosmics->getBDTVariable(cosmic_bdt_info)} , fcoscut,fbnbcut);
			}
			if(run_bnb){
				recomc.plot_recomc(ftest, bnb_cosmics, (std::vector<bdt_variable>){bnb_cosmics->getBDTVariable(bnb_bdt_info)} , fcoscut,fbnbcut);
				recomc.plot_recomc(ftest, signal_cosmics, (std::vector<bdt_variable>){signal_cosmics->getBDTVariable(bnb_bdt_info)} , fcoscut,fbnbcut);
			}

			recomc.is_log = false;
		}

	}
	else if(mode_option == "sig"){


		TFile *fsig = new TFile(("significance_"+analysis_tag+".root").c_str(),"recreate");
		std::vector<double> ans = scan_significance(fsig, {signal_cosmics} , {bnb_cosmics, bnbext}, cosmic_bdt_info, bnb_bdt_info);
		//std::vector<double> ans = lin_scan({signal_cosmics}, {bnb_cosmics, bnbext}, cosmic_bdt_info, bnb_bdt_info,fcoscut,fbnbcut);

		std::cout<<"Best Fit Significance: "<<ans.at(0)<<" "<<ans.at(1)<<" "<<ans.at(2)<<std::endl;
		fsig->Close();


	}else if(mode_option == "stack"){
		bdt_stack histogram_stack(analysis_tag+"_stack");
		histogram_stack.addToStack(signal_cosmics);
		histogram_stack.addToStack(bnb_cosmics);

		//Add bnbext but change the color and style first
		bnbext->col = intime->col;	
		bnbext->fillstyle = 3333;
		histogram_stack.addToStack(bnbext);

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

		TFile * ftest = new TFile(("test+"+analysis_tag+".root").c_str(),"recreate");
		//Obsolete
		/*bdt_stack *obs = new bdt_stack(analysis_tag+"_olddatamc");
		  obs->plot_pot =4.801e19;
		  obs->addToStack(signal_cosmics);
		  obs->addToStack(bnb_cosmics);
		  obs->addToStack(intime);*/

		bdt_stack *cosmic_stack = new bdt_stack(analysis_tag+"_extintime");
		cosmic_stack->plot_pot = 4.393e19;
		cosmic_stack->addToStack(intime);

		bdt_stack *histogram_stack = new bdt_stack(analysis_tag+"_datamc");
		histogram_stack->plot_pot = 4.393e19;
		histogram_stack->addToStack(signal_cosmics);
		histogram_stack->addToStack(bnb_cosmics);
		bnbext->col = intime->col;	
		bnbext->fillstyle = 3333;
		histogram_stack->addToStack(bnbext);

		int ip=0;


		if(!response_only){
			if(number != -1){
				bdt_datamc datamc(data5e19, histogram_stack, analysis_tag+"_datamc");	
				std::vector<bdt_variable> tmp_var = {vars.at(number)};
				datamc.plotStacks(ftest,  tmp_var ,fcoscut,fbnbcut);
			}else{

				bdt_datamc real_datamc(data5e19, histogram_stack, analysis_tag+"_datamc");	
				real_datamc.plotStacks(ftest, vars,fcoscut,fbnbcut);
			}
		}else{
			//bdt_datamc cosmic_datamc(bnbext, cosmic_stack, analysis_tag+"_extintime");	
			bdt_datamc real_datamc(data5e19, histogram_stack, analysis_tag+"_datamc");	

			if(run_cosmic) real_datamc.plotBDTStacks(ftest, bnb_bdt_info ,fcoscut,fbnbcut);
			if(run_bnb) real_datamc.plotBDTStacks(ftest, cosmic_bdt_info ,fcoscut,fbnbcut);
		}

	}else if(mode_option == "vars"){

		std::vector<std::string> title = {"All Verticies","Pre-Selection Cuts"};

		if(run_cosmic){

			if(number != -1){
				plot_bdt_variable(signal_pure, intime, vars.at(number), cosmic_bdt_info);
				//bdt_variable true_en("delta_photon_energy","(52,0,1.2)","True Shower Energy [GeV]",false,"d");
				//plot_bdt_variable(signal_pure, intime, true_en, cosmic_bdt_info);
			}else{
				plot_bdt_variables(signal_pure, intime, vars, cosmic_bdt_info);
			}

		}
		if(run_bnb){


			if(number != -1){
				plot_bdt_variable(signal_pure, bnb_pure, vars.at(number), bnb_bdt_info);
			}else{
				plot_bdt_variables(signal_pure, bnb_pure, vars, bnb_bdt_info);
			}

		}




	} else if(mode_option == "eff"){

		std::string ZMIN = "0.0"; std::string ZMAX = "1036.8";
		std::string XMIN = "0.0"; std::string XMAX = "256.35";
		std::string YMIN = "-116.5"; std::string YMAX = "116.5";
		std::string pmass = "0.938272";



		std::string en_cut_trk = "delta_photon_energy > 0.02 && delta_proton_energy-"+pmass+" > 0.04";
		std::string en_cut_notrk = "delta_photon_energy > 0.02";
		std::string fid_cut = "(true_nuvertx >"+XMIN+"+10 && true_nuvertx < "+XMAX+"-10 && true_nuverty >"+ YMIN+"+20 && true_nuverty <"+ YMAX+"-20 && true_nuvertz >"+ ZMIN +" +10 && true_nuvertz < "+ZMAX+"-10)";

		std::string is_delta_rad = "1";
		std::string notrk = "exiting_photon_number==1 && exiting_proton_number==0 &&"+is_delta_rad;
		std::string trk = "exiting_photon_number==1 && exiting_proton_number==1 &&"+is_delta_rad;


		std::string denom  = en_cut_trk + "&&"+ fid_cut + "&& "+ trk;


		bdt_efficiency(signal_cosmics, denom, fcoscut,fbnbcut);


		//everything below here is older
		return 0;

		double plot_pot = 6.6e20;
		std::cout<<"Starting efficiency study: coscut @ "<<fcoscut<<" bnbcut@: "<<fbnbcut<<std::endl;

		std::cout<<"########################################################################"<<std::endl;
		std::cout<<"###############################Efficiency###############################"<<std::endl;


		TFile * feff = new TFile(("Eff_"+analysis_tag+".root").c_str(),"recreate");
		TCanvas *ceff = new TCanvas();		
		ceff->cd();

		std::vector<double> eff_sig;

		//std::vector<bdt_file*> eff_files = {signal_pure, bnb_cosmics, bnbext};
		std::vector<bdt_file*> eff_files = {signal_cosmics, bnb_cosmics, bnbext};
		std::vector<std::vector<double>> effs;
		std::vector<std::vector<double>> pres;

		for(int i=0; i< eff_files.size(); i++){
			std::vector<double> tmp;
			effs.push_back(tmp);
			pres.push_back(tmp);

			std::cout<<eff_files.at(i)->tag<<std::endl;
			std::vector<double> prev;
			double start;
			double sel;		

			double pot_scale = (plot_pot/eff_files.at(i)->pot )*eff_files.at(i)->scale_data;
			std::string gencut = "1";
			if(eff_files.at(i)->tag == "IntimeCosmics" || eff_files.at(i)->tag == "Data5e19" || eff_files.at(i)->tag == "BNBext"){
				gencut = "1";
			}	

			double nevents = eff_files.at(i)->numberofevents*pot_scale;	

			if(eff_files.at(i)->tag == "NCDeltaRadCosmics"){
				if(analysis_tag == "ncdelta1g1p") nevents = 76.6162;
				if(analysis_tag == "ncdelta1g0p") nevents = 81.7591;
			}


			double nv = eff_files.at(i)->GetEntries((gencut+"&&1").c_str())*pot_scale;	
			double ns = eff_files.at(i)->GetEntries( eff_files.at(i)->getStageCuts(0,fcoscut,fbnbcut).c_str())*pot_scale;	

			start = nevents;
			sel = ns;
			std::cout<<"Stage Gen\t\t"<<nevents<<"\t\t"<<nevents/start*100<<std::endl;
			std::cout<<"Stage Vertex\t\t"<<nv<<"\t\t"<<nv/start*100<<std::endl;
			std::cout<<"Stage Topo\t\t"<<ns<<"\t\t"<<ns/start*100<<"\t\t"<<100<<std::endl;

			if(eff_files.at(i)->tag=="NCDeltaRadCosmics"){
				eff_sig.push_back(nevents/start*100);
				eff_sig.push_back(ns/start*100);
			}

			effs.back().push_back(nevents);
			effs.back().push_back(nv);
			effs.back().push_back(ns);

			for(int m=0; m< eff_files.at(i)->flow.vec_pre_cuts.size(); m++){
				std::string thiscut = eff_files.at(i)->flow.vec_pre_cuts.at(m)+"&&"+eff_files.at(i)->getStageCuts(0,fcoscut,fbnbcut);
				double np = eff_files.at(i)->GetEntries(thiscut.c_str())*pot_scale;
				std::cout<<"Precut: "<<eff_files.at(i)->flow.vec_pre_cuts.at(m)<<"\t||\t"<<np<<"\t("<<np/ns*100<<")\%"<<std::endl;
				pres.back().push_back(np);
			}

			std::cout<<"One by One"<<std::endl;
			std::string thiscut = eff_files.at(i)->getStageCuts(0,fcoscut,fbnbcut);
			for(int m=0; m< eff_files.at(i)->flow.vec_pre_cuts.size(); m++){
				thiscut += "&&"+ eff_files.at(i)->flow.vec_pre_cuts.at(m);
				double np = eff_files.at(i)->GetEntries(thiscut.c_str())*pot_scale;
				std::cout<<" + "<<eff_files.at(i)->flow.vec_pre_cuts.at(m)<<"\t||\t"<<np<<"\t("<<np/ns*100<<")\%"<<std::endl;

				if(eff_files.at(i)->tag=="NCDeltaRadCosmics"){
					eff_sig.push_back(np/start*100);
				}
				effs.back().push_back(np);

			}

			if(eff_files.at(i)->tag=="NCDeltaRadCosmics"){
				eff_sig.push_back(  eff_files.at(i)->GetEntries(eff_files.at(i)->getStageCuts(2,fcoscut,fbnbcut).c_str())*pot_scale/start*100);
				eff_sig.push_back(  eff_files.at(i)->GetEntries(eff_files.at(i)->getStageCuts(3,fcoscut,fbnbcut).c_str())*pot_scale/start*100);
			}


			for(int j=1; j <4; j++){		
				std::string thiscut = eff_files.at(i)->getStageCuts(j,fcoscut,fbnbcut); 
				double nvert =eff_files.at(i)->GetEntries(thiscut.c_str())*pot_scale;
				double eff =0;
				//	if(j==0){
				//		start = nvert;
				//		
				//	}

				eff = nvert/start*100;
				effs.back().push_back(nvert);
				std::cout<<nvert<<"\t("<<nvert/sel*100<<" \%)"<<" ";	
			}

			std::cout<<std::endl;

			//break;
		}


		//pe_cut, fiducial_cut, track_length_cut, min_energy_cut, min_conversion_cut, good_calo_cut, track_direction_cut, back_to_back_cut
		std::vector<std::string> sname;


		if(analysis_tag == "ncdeltarad1g0p"){
			sname = {"Generated","Vertexed","Topological","Total PE $>20$","Fiducial cut","Reco $E_{\\gamma}$ $> 30$MeV"};
		}else{
			sname = {"Generated","Vertexed","Topological","Total PE $>20$","Fiducial cut","Track $< 100$cm","Reco $E_{\\gamma}$ $> 30$MeV","Shower gap $> 1$cm","Good calo cut","Flipped track cut","Back-to-back cut"};
		}
		//	for(int i=0; i< pres.back().size(); i++){
		//		sname.push_back( std::to_string(i));
		//	}
		sname.push_back("All Precuts");
		sname.push_back("Cosmic BDT");
		sname.push_back("BNB BDT");

		std::cout<<"\\begin{table}[h!]"<<std::endl;
		std::cout<<"\\centering"<<std::endl;
		std::cout<<"\\begin{tabular}{|l||c|c||c|c||c|c|}"<<std::endl;
		std::cout<<"\\hline"<<std::endl;
		for(int i=0; i< effs.back().size();i++){
			std::cout<<sname.at(i);
			for(int j=0; j< eff_files.size(); j++){

				if(i>2 && i < 3+pres.back().size() ){
					double num = effs.at(j).at(i);
					double per  = num/effs.at(j).at(0)*100.0;
					double single_per = pres.at(j).at(i-3)/effs.at(j).at(2)*100.0;

					//std::cout<<"&"<<to_string_prec(num,1)<<"&"<<to_string_prec(per,1)<<"\\\%&("<<to_string_prec(single_per,1)<<"\\\%)";
					std::cout<<"&"<<to_string_prec(num,1)<<"&"<<to_string_prec(per,1)<<"\\\% ("<<to_string_prec(single_per,1)<<"\\\%)";

				}else{
					//std::cout<<"&"<<to_string_prec(effs.at(j).at(i),1)<<"&"<<to_string_prec(effs.at(j).at(i)/effs.at(j).at(0)*100.0,2)<<"\\\%&";
					std::cout<<"&"<<to_string_prec(effs.at(j).at(i),1)<<"&"<<to_string_prec(effs.at(j).at(i)/effs.at(j).at(0)*100.0,2)<<"\\\%";
				}


			}
			std::cout<<"\\\\"<<std::endl;
			if(i==2 || (effs.back().size()-i) == 4)std::cout<<"\\hline"<<std::endl;

		}	
		std::cout<<"\\hline"<<std::endl;

		std::cout<<"\\end{tabular}"<<std::endl;
		std::cout<<"\\end{table}"<<std::endl;










		//END********************************************



		feff->cd();			
		std::vector<double> eff_num;
		for(int i=0; i< eff_sig.size(); i++){ eff_num.push_back((double)i);}
		std::reverse(eff_sig.begin(),eff_sig.end());
		TGraph *g_eff_sig = new TGraph(eff_sig.size(),&eff_sig[0],&eff_num[0]);

		ceff->cd();
		g_eff_sig->Draw("alp");
		ceff->Write();

		feff->Close();


	}
	else if(mode_option == "effdata"){
		std::vector<bdt_file*> data_files = {data5e19, bnbext};

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

			double nevents = 0;//data_files.at(i)->tevent->GetEntries(gencut.c_str())*pot_scale;	
			double nv = data_files.at(i)->GetEntries((gencut+"&&1").c_str())*pot_scale;	
			double ns = data_files.at(i)->GetEntries((gencut+"&&  reco_asso_showers==1 && reco_asso_tracks "+num_track_cut).c_str())*pot_scale;	

			start = nevents;
			sel = ns;
			std::cout<<"Stage G\t\t"<<nevents<<"\t\t"<<nevents/start*100<<std::endl;
			std::cout<<"Stage V\t\t"<<nv<<"\t\t"<<nv/start*100<<std::endl;
			std::cout<<"Stage S\t\t"<<ns<<"\t\t"<<ns/start*100<<"\t\t"<<100<<std::endl;

			for(int j=0; j <4; j++){		
				std::string thiscut = data_files.at(i)->getStageCuts(j,fcoscut,fbnbcut); 
				double nvert =data_files.at(i)->GetEntries(thiscut.c_str())*pot_scale;
				double eff =2;
				eff = nvert/start*100;
				std::cout<<"Stage "<<std::to_string(j)<<"\t\t"<<nvert<<"\t\t"<<eff<<"\t\t"<<nvert/sel*100<<std::endl;	
				if(j==3){data_files.at(i)->tvertex->Scan( "run_number:subrun_number:event_number:reco_shower_dedx_plane2",thiscut.c_str());};
			}
		}


	} else if(mode_option == "precalc"){
		std::cout<<"mode: precalc: Begininning precalculation"<<std::endl;

		std::vector<bdt_file*> bdt_filesA;
		if(number == -1){
			 bdt_filesA = bdt_files;
		}else{
			 bdt_filesA = {bdt_files.at(number)};
		}

		for(auto &f: bdt_filesA){
			std::cout<<"On file: "<<f->tag<<std::endl;
			bdt_precalc pre(f);
			pre.genTrackInfo();
			pre.genBNBcorrectionInfo();
			pre.genPi0Info();
			pre.genShowerInfo();

		}
		return 0;
	}
	else if(mode_option == "sbnfit"){

		bdt_stack *obs = new bdt_stack(analysis_tag+"_datamc");
		obs->plot_pot =  6.6e20;
		//obs->plot_pot =  4.801e19;
		obs->addToStack(signal_cosmics);
		obs->addToStack(bnb_cosmics);
		obs->addToStack(intime);
		obs->addToStack(data5e19);

		std::vector<std::string> hnam = {"nu_uBooNE_singlephoton_signal","nu_uBooNE_singlephoton_bkg","nu_uBooNE_singlephoton_intime","nu_uBooNE_singlephoton_data"};
		//std::vector<std::string> hnam = {"nu_uBooNE_singlephoton_signal","nu_uBooNE_singlephoton_bkg","nu_uBooNE_singlephoton_intime"};
		obs->makeSBNspec("test", vars.at(1), fcoscut, fbnbcut,hnam );




	}else if( mode_option =="test"){

		//signal_cosmics->writeStageFriendTree("stage_friend.root", fcoscut, fbnbcut);
		//bnb_cosmics->writeStageFriendTree("stage_friend.root", fcoscut, fbnbcut);


		double true_nuvertx = 0;
		double true_nuverty = 0;
		double true_nuvertz = 0;
		int  event_number=0;
		int  run_number=0;
		int  subrun_number=0;
		double delta_proton_energy =0;
		double delta_photon_energy =0;
		double wei=0;

		std::vector<int> vec_enum;
		std::vector<int> vec_rnum;
		std::vector<int> vec_snum;
		int pass_denom = 0;
		double check_num = 0;


		signal_cosmics->tvertex->SetBranchAddress("true_nuvertx",&true_nuvertx);
		signal_cosmics->tvertex->SetBranchAddress("true_nuverty",&true_nuverty);
		signal_cosmics->tvertex->SetBranchAddress("true_nuvertz",&true_nuvertz);

		signal_cosmics->tvertex->SetBranchAddress("event_number",&event_number);
		signal_cosmics->tvertex->SetBranchAddress("delta_proton_energy",&delta_proton_energy);
		signal_cosmics->tvertex->SetBranchAddress("delta_photon_energy",&delta_photon_energy);
		signal_cosmics->tvertex->SetBranchAddress("bnbcorrection_info.weight",&wei);

		//TPCActive
		//Z: 0 to 1036.8
		//X: 0 to 256.35
		//Y: -116.5 to 116.5
		std::string ZMIN = "0.0"; std::string ZMAX = "1036.8";
		std::string XMIN = "0.0"; std::string XMAX = "256.35";
		std::string YMIN = "-116.5"; std::string YMAX = "116.5";
		std::string pmass = "0.938272";

		std::set<int> eventIDs;
		TEntryList * tlist = new TEntryList(signal_cosmics->tvertex);

		for(int k=0; k< signal_cosmics->tvertex->GetEntries();k++){
			signal_cosmics->tvertex->GetEntry(k);
			if(k%10000==0) std::cout<<k<<std::endl;	
			if(eventIDs.count(event_number)==0){
				eventIDs.insert(event_number);
				tlist->Enter(k,signal_cosmics->tvertex);
				check_num+=wei;		
			}	
		}

		double MOD =signal_cosmics->scale_data*6.6e20/signal_cosmics->pot;
		double volCryo = 199668.427885;
		double volTPC = 101510.0;
		double  volTPCActive=  86698.6;


		signal_cosmics->tvertex->SetEntryList(tlist);

		std::string en_cut_trk = "delta_photon_energy > 0.02 && delta_proton_energy-"+pmass+" > 0.04";
		std::string en_cut_notrk = "delta_photon_energy > 0.02";
		std::string fid_cut = "(true_nuvertx >"+XMIN+"+10 && true_nuvertx < "+XMAX+"-10 && true_nuverty >"+ YMIN+"+20 && true_nuverty <"+ YMAX+"-20 && true_nuvertz >"+ ZMIN +" +10 && true_nuvertz < "+ZMAX+"-10)";

		//std::string is_delta_rad = "is_delta_rad==1";
		std::string is_delta_rad = "1";
		std::string notrk = "exiting_photon_number==1 && exiting_proton_number==0 &&"+is_delta_rad;
		std::string trk = "exiting_photon_number==1 && exiting_proton_number==1 &&"+is_delta_rad;

		std::cout<<"====================Raw Numbers of Events==================="<<std::endl;
		//std::cout<<"Generated in Cryo: "<<signal_cosmics->GetEntries("1")<<" and "<<check_num<<std::endl;
		double MOD2=volTPCActive/volCryo;
		std::cout<<"Generated in Cryo: "<<signal_cosmics->GetEntries(is_delta_rad.c_str())<<std::endl;
		std::cout<<"Generated in ActiveTPC: "<<MOD2*signal_cosmics->GetEntries(is_delta_rad.c_str())<<std::endl;
		std::cout<<"PASS: 1g1p: "<<signal_cosmics->GetEntries((trk).c_str())<<std::endl;
		std::cout<<"PASS: 1g1p + Fid: "<<signal_cosmics->GetEntries((trk+"&&"+fid_cut).c_str())<<std::endl;
		std::cout<<"PASS: 1g1p + Fid + Energy: "<<signal_cosmics->GetEntries((trk+"&&"+en_cut_trk+"&&"+fid_cut).c_str())<<std::endl;
		std::cout<<"PASS: 1g0p: "<<signal_cosmics->GetEntries((notrk).c_str())<<std::endl;
		std::cout<<"PASS: 1g0p + Fid: "<<signal_cosmics->GetEntries((notrk+"&&"+ fid_cut).c_str())<<std::endl;
		std::cout<<"PASS: 1g0p + Fid+ Energy: "<<signal_cosmics->GetEntries((notrk+"&&"+en_cut_notrk +"&&"+en_cut_notrk).c_str())<<std::endl;

		std::cout<<"====================Scaled to 6.6e20 LEE Events==================="<<std::endl;

		std::cout<<"Generated in TPCActive Volume: "<<signal_cosmics->numberofevents_raw*MOD*volTPCActive/volTPC<<std::endl;
		std::cout<<"Vertexed in Fiducial Volume: "<<signal_cosmics->GetEntries((is_delta_rad + "&&"+fid_cut).c_str())*MOD<<std::endl;
		std::cout<<"PASS: 1g1p + Fid: "<<signal_cosmics->GetEntries((trk+"&&"+fid_cut).c_str())*MOD<<std::endl;
		std::cout<<"PASS: 1g1p + Fid + Energy: "<<signal_cosmics->GetEntries((trk+"&&"+en_cut_trk+"&&"+fid_cut).c_str())*MOD<<std::endl;
		std::cout<<"PASS: 1g0p + Fid: "<<signal_cosmics->GetEntries((notrk+"&&"+fid_cut).c_str())*MOD<<std::endl;
		std::cout<<"PASS: 1g0p + Fid + Energy: "<<signal_cosmics->GetEntries((notrk+"&&"+en_cut_notrk + "&&"+ fid_cut).c_str())*MOD<<std::endl;




		TFile * fout = new TFile("eff.root","update");

		if(number==-1)number=1;	

		bdt_variable true_en("delta_photon_energy","(52,0,1.2)","True Shower Energy [GeV]",false,"d");
		std::string ct;
		if(analysis_tag=="ncdeltarad1g1p") ct = (trk+"&&"+en_cut_trk+"&&"+fid_cut);
		if(analysis_tag=="ncdeltarad1g0p") ct = (notrk+"&&"+en_cut_notrk+"&&"+fid_cut);

		TH1* pre = (TH1*)signal_cosmics->getTH1(true_en,ct , "pre_"+analysis_tag+signal_cosmics->tag, 6.6e20, 2);
		std::cout<<pre->GetSumOfWeights()<<std::endl;
		std::cout<<"Before: "<<pre->GetSumOfWeights()<<std::endl;


		signal_cosmics->calcBNBBDTEntryList(fcoscut, fbnbcut);
		signal_cosmics->tvertex->SetEntryList(0);
		signal_cosmics->setStageEntryList(3);
		TH1* post = (TH1*)signal_cosmics->getTH1(true_en, "1", "post_"+analysis_tag+signal_cosmics->tag, 6.6e20, 2);

		std::cout<<"Post: "<<post->GetSumOfWeights()<<std::endl;

		TH1* ratio = (TH1*)post->Clone(("ratio_"+analysis_tag).c_str());
		ratio->Divide(pre);

		for(int i=1; i<= ratio->GetNbinsX(); i++){
			std::cout<<i<<" "<<ratio->GetBinContent(i)<<std::endl;	
		}
		TCanvas *cobs = new TCanvas("test","test",1800,1600);
		ratio->Draw();
		cobs->SaveAs("tester.pdf","pdf");

		fout->cd();
		ratio->Write();
		pre->Write();
		post->Write();
		fout->Close();

		return 0;


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

		bdt_recomc test(recomc_names, recomc_cuts, recomc_cols,analysis_tag);

		TFile * ftest = new TFile(("test+"+analysis_tag+".root").c_str(),"recreate");
		test.plot_recomc(ftest, bnb_cosmics, vars.at(1) , fcoscut,fbnbcut);

		return 0;	
		bnb_cosmics->tvertex->Scan("run_number:subrun_number:event_number:reco_shower_dedx_plane2[0]:reco_shower_helper_energy[0]:reco_track_displacement[0]:shortest_asso_shower_to_vert_dist:BNBCosmics_bnb_track.mva", bnb_cosmics->getStageCuts(3,fcoscut,fbnbcut).c_str()  );
		return 0;


	}else {
		std::cout << "WARNING: " << mode_option << " is an invalid option\n";
	}


	return 0;

}
