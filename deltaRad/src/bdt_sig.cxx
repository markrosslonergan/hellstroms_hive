#include "bdt_sig.h"



/*
How do I normalize..

Two cuts( on this single cut)

INTIME
RECO2 has 1013198 events in 31497 files
DETSIM has 1030055 events in 31980 files
I ended with 989875 events


BNBCOSMIC
DETSIM has 2412300 in 48246 files
RECO2 should have 2360950 events
I ended with 2360950 events //ooohh all of them 

Weight each intime cosmic event with a factor 10.279*N_gen_BNB/(N_gen_cosmic*my_rate)
= 10.279*2412300/(1030055*989875/1013198) = 24.639718178714663

times whatever POT scaling we need to put on the BNB events to get to 6.6e20
which for v3.0_with calo is 2.38091e+21
= 24.6397*6.6e20/2.38091e21  = 6.830246418386248

	precut--
	for loop over BDT1
	for loop over BDT2
		calc S/sqrt(S+S+BKG)

*/


std::vector<double> scan_significance(TFile * fout, std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files, bdt_info cosmic_focused_bdt, bdt_info bnb_focused_bdt){
	std::cout<<"Starting to Scan Significance"<<std::endl;
	double best_significance = 0;
	double best_mva_cut = DBL_MAX;
	double best_mva_cut2 = DBL_MAX;

	double plot_pot = 6.6e20;
	

	//for nice plots make the 50, 25 is quicker tho
	int nsteps_cosmic = 40;//50
	double cut_min_cosmic = 999;
	double cut_max_cosmic = -999;

	int nsteps_bnb = 40;//50
	double cut_min_bnb = 999;//0.52;
	double cut_max_bnb = -999;
	
	std::cout<<"Setting stage entry lists"<<std::endl;
	for(size_t i = 0; i < sig_files.size(); ++i) {
		sig_files.at(i)->setStageEntryList(1);
	}
	for(size_t i = 0; i < bkg_files.size(); ++i) {
		bkg_files.at(i)->setStageEntryList(1);
	}

	for(size_t i = 0; i < sig_files.size(); ++i) {
	//	double tmin_cos = sig_files.at(i)->tvertex->GetMinimum( (sig_files.at(i)->getBDTVariable(cosmic_focused_bdt).name + ">0").c_str()    );
		double tmax_cos = sig_files.at(i)->tvertex->GetMaximum( sig_files.at(i)->getBDTVariable(cosmic_focused_bdt).name.c_str()    );
		double tmax_bnb = sig_files.at(i)->tvertex->GetMaximum( sig_files.at(i)->getBDTVariable(bnb_focused_bdt).name.c_str()    );
	
		std::cout<<sig_files.at(i)->getBDTVariable(cosmic_focused_bdt).name<<" MaxCos: "<<tmax_cos<<" MaxBnb: "<<tmax_bnb<<std::endl;
		//if( tmin_cos <= cut_min_cosmic) cut_min_cosmic=tmin_cos;
		if( tmax_cos >= cut_max_cosmic) cut_max_cosmic=tmax_cos;
		if( tmax_bnb >= cut_max_bnb) cut_max_bnb=tmax_bnb;

	}
  // Normally *0.7 or *0.8
	cut_min_cosmic = cut_max_cosmic*0.7;
	cut_min_bnb = cut_max_bnb*0.6;

  // These are normally *1.0
	cut_max_cosmic =cut_max_cosmic*0.9;
	cut_max_bnb =cut_max_bnb*0.8; 

	//Zoomed in notrack
//	cut_min_cosmic = 0.54; cut_max_cosmic = 0.58;
//	cut_min_bnb = 0.51; cut_max_bnb = 0.552;

	//Best Fit Significance: 0.601552 0.533678 1.63658
	//Zoomed in track
	cut_min_cosmic = 0.643; cut_max_cosmic = 0.644;
	cut_min_bnb = 0.632; cut_max_bnb = 0.638;




	//Create 2 tempoary TEntryLists  at minimum
	std::vector<TEntryList*> sig_min_lists;
	std::vector<TEntryList*> bkg_min_lists;


	std::cout<<"Setting Min entry lists"<<std::endl;
	for(size_t i = 0; i < sig_files.size(); ++i) {
	

		std::string min_list_name  = "micam"+std::to_string(i);
		sig_files.at(i)->tvertex->Draw((">>"+min_list_name).c_str(), sig_files.at(i)->getStageCuts(3, cut_min_cosmic,cut_min_bnb).c_str() , "entrylist");
		sig_min_lists.push_back(  (TEntryList*)gDirectory->Get(min_list_name.c_str()) );
		sig_files.at(i)->tvertex->SetEntryList(sig_min_lists.back());


	}
	for(size_t i = 0; i < bkg_files.size(); ++i) {

		std::string min_list_name  = "mibam"+std::to_string(i);
		bkg_files.at(i)->tvertex->Draw((">>"+min_list_name).c_str(), bkg_files.at(i)->getStageCuts(3, cut_min_cosmic,cut_min_bnb).c_str() , "entrylist");
		bkg_min_lists.push_back(  (TEntryList*)gDirectory->Get(min_list_name.c_str()) );
		bkg_files.at(i)->tvertex->SetEntryList(bkg_min_lists.back());


	}	




	std::cout<<"BNB sig scan from: "<<cut_min_bnb<<" to "<<cut_max_bnb<<std::endl;
	std::cout<<"COSMIC sig scan from: "<<cut_min_cosmic<<" to "<<cut_max_cosmic<<std::endl;

	double step_cosmic = (cut_max_cosmic-cut_min_cosmic)/((double)nsteps_cosmic);
	double step_bnb = (cut_max_bnb-cut_min_bnb)/((double)nsteps_bnb);


	TH2D * h2_sig_cut = new TH2D( "significance_2D",  "significance_2D",nsteps_cosmic, cut_min_cosmic, cut_max_cosmic, nsteps_bnb, cut_min_bnb, cut_max_bnb);
	std::vector<double> vec_sig;//some vectors to store TGraph info;
	std::vector<double> vec_cut;	
  
  // Calculate total signal for efficiency 
  double total_sig = 0.;
  for(size_t i = 0; i < sig_files.size(); ++i) {
      double pot_scale = (plot_pot/sig_files.at(i)->pot )*sig_files.at(i)->scale_data;
      std::cout << "POT scale: " << pot_scale << std::endl;

      std::string bnbcut = sig_files.at(i)->getStageCuts(1,-9,-9); 
      total_sig += sig_files.at(i)->tvertex->GetEntries(bnbcut.c_str())*pot_scale;
  }

	for(int di=1; di<=nsteps_cosmic; di++) {
		double d  = (double)(di-1.0)*step_cosmic + cut_min_cosmic; ;	

		for(int di2=1; di2<=nsteps_bnb; di2++) {
			double d2  = (double)(di2-1.0)*step_bnb + cut_min_bnb ;	

			double signal = 0;
			double background = 0;
			std::vector<double> bkg;	

			for(size_t i = 0; i < sig_files.size(); ++i) {
				double pot_scale = (plot_pot/sig_files.at(i)->pot )*sig_files.at(i)->scale_data;
			
				std::string bnbcut = sig_files.at(i)->getStageCuts(3,d,d2); 
				signal += sig_files.at(i)->GetEntries(bnbcut.c_str())*pot_scale;

			}

			for(size_t i = 0; i < bkg_files.size(); ++i) {
				double pot_scale = (plot_pot/bkg_files.at(i)->pot)*bkg_files.at(i)->scale_data;
		
	
				std::string bnbcut = bkg_files.at(i)->getStageCuts(3,d,d2); 
				bkg.push_back(	bkg_files.at(i)->GetEntries(bnbcut.c_str())*pot_scale);			

				background += bkg.back();
			}



			double significance =0;
			if(signal==0){
				 significance =0;
			}else if(background !=0){
        significance = signal/(signal+background)*signal/total_sig*100;
				//significance = signal/sqrt(background);
			}else{
				std::cout<<"method_best_significane_seperate || signal2+background2 == 0, so significance  = nan @ cut1: "<<d<<", cut2: "<<d2<<std::endl;
				break;
			}


			if(significance > best_significance) {
				best_significance = significance;
				best_mva_cut = d;
				best_mva_cut2 = d2;
			}


			std::cout<<"ccut: "<<d<<" bcut: "<<d2<<" "<<" #signal: "<<signal<<" #bkg: "<<background<<" || "<<" bnb: "<<bkg.at(0)<<" cos: "<<bkg.at(1)<<" || "<<significance<<std::endl;
			vec_sig.push_back(significance);
			vec_cut.push_back(d2);
			h2_sig_cut->SetBinContent(di,di2, significance);
		}

	}
		

	h2_sig_cut->SetStats(false);
	TCanvas * c_sig_cuts =  new TCanvas( "significance_cuts_colz", "significance_cuts_colz", 2000,1600 );
	c_sig_cuts->Divide(2,1);
	TPad *p1 = (TPad*)c_sig_cuts->cd(1);
	p1->SetRightMargin(0.13);
	h2_sig_cut->Draw("colz");
	h2_sig_cut->GetXaxis()->SetTitle("Cosmic Cut");
	h2_sig_cut->GetYaxis()->SetTitle("BNB Cut");
	
   	std::vector<double> vec_bf_cut1 = {best_mva_cut};
   	std::vector<double> vec_bf_cut2 = {best_mva_cut2};
	TGraph *graph_bf = new TGraph(vec_bf_cut1.size(), &vec_bf_cut1[0], &vec_bf_cut2[0]);
	graph_bf->SetMarkerStyle(29);
	graph_bf->SetMarkerSize(2);
	graph_bf->SetMarkerColor(kBlack);
	graph_bf->Draw("same p");
 

	TGraph * graph_cut = new TGraph(vec_sig.size(), &vec_cut[0], &vec_sig[0]);
	graph_cut->SetTitle("1D slices");
	c_sig_cuts->cd(2);
	graph_cut->Draw("alp");

	h2_sig_cut->Write();
	c_sig_cuts->Write();

	return std::vector<double>{best_mva_cut, best_mva_cut2, best_significance};

}



std::vector<double> lin_scan(std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files, bdt_info cosmic_focused_bdt, bdt_info bnb_focused_bdt, double c1, double c2){
	std::cout<<"Starting to Scan Significance"<<std::endl;
	double best_significance = 0;
	double best_mva_cut = DBL_MAX;
	double best_mva_cut2 = DBL_MAX;

	double plot_pot = 6.6e20;
	

	std::vector<double> vec_sig;//some vectors to store TGraph info;
	std::vector<double> vec_cut;	


	double d1 = c1;
	double d2 = c2; 


	for(int i=0; i< 100; i++){

			d1 = d1*1.0001;
			//d1 = d1*0.99999;

			double signal = 0;
			double background = 0;
			std::vector<double> bkg;	

			for(size_t i = 0; i < sig_files.size(); ++i) {
				double pot_scale = (plot_pot/sig_files.at(i)->pot )*sig_files.at(i)->scale_data;
			
				std::string bnbcut = sig_files.at(i)->getStageCuts(3,d1,d2); 
				signal += sig_files.at(i)->GetEntries(bnbcut.c_str())*pot_scale;

			}

			for(size_t i = 0; i < bkg_files.size(); ++i) {
				double pot_scale = (plot_pot/bkg_files.at(i)->pot)*bkg_files.at(i)->scale_data;
		
	
				std::string bnbcut = bkg_files.at(i)->getStageCuts(3,d1,d2); 
				bkg.push_back(	bkg_files.at(i)->GetEntries(bnbcut.c_str())*pot_scale);			

				background += bkg.back();
			}
			double significance =0;
			if(signal==0){
				 significance =0;
			}else if(background !=0){
				significance = signal/sqrt(background);
			}else{
				std::cout<<"method_best_significane_seperate || signal2+background2 == 0, so significance  = nan @ cut1: "<<d1<<", cut2: "<<d2<<std::endl;
				break;
			}


			if(significance > best_significance) {
				best_significance = significance;
				best_mva_cut = d1;
				best_mva_cut2 = d2;
			}


			std::cout<<"ccut: "<<d1<<" bcut: "<<d2<<" "<<" #signal: "<<signal<<" #bkg: "<<background<<" || "<<" bnb: "<<bkg.at(0)<<" cos: "<<bkg.at(1)<<" || "<<significance<<std::endl;
			vec_sig.push_back(significance);
			vec_cut.push_back(d1);
			vec_cut.push_back(d2);
		}

		
	return std::vector<double>{best_mva_cut, best_mva_cut2, best_significance};

}

