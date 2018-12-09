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


    bool quick_search = true;//use the original detail search via false value;
    //the if statement is the old program; change quick_search to enable it.
    if(!quick_search){

	std::cout<<"Starting to Scan Significance"<<std::endl;
	double best_significance = 0;
	double best_mva_cut = DBL_MAX;
	double best_mva_cut2 = DBL_MAX;

	double plot_pot = 6.6e20;


	//for nice plots make the 50, 25 is quicker tho
	int nsteps_cosmic = 4;//50
	double cut_min_cosmic = 999;
	double cut_max_cosmic = -999;

	int nsteps_bnb = 4;//50
	double cut_min_bnb = 999;//0.52;
	double cut_max_bnb = -999;



	for(size_t i = 0; i < sig_files.size(); ++i) {
	    //	double tmin_cos = sig_files.at(i)->tvertex->GetMinimum( (sig_files.at(i)->getBDTVariable(cosmic_focused_bdt).name + ">0").c_str()    );
	    double tmax_cos = sig_files.at(i)->tvertex->GetMaximum( sig_files.at(i)->getBDTVariable(cosmic_focused_bdt).name.c_str()    );
	    double tmax_bnb = sig_files.at(i)->tvertex->GetMaximum( sig_files.at(i)->getBDTVariable(bnb_focused_bdt).name.c_str()    );

	    //if( tmin_cos <= cut_min_cosmic) cut_min_cosmic=tmin_cos;
	    if( tmax_cos >= cut_max_cosmic) cut_max_cosmic=tmax_cos;
	    if( tmax_bnb >= cut_max_bnb) cut_max_bnb=tmax_bnb;

	}
	//cut_min_cosmic = cut_max_cosmic*0.8;
	//cut_min_bnb = cut_max_bnb*0.8;

	//cut_max_cosmic =cut_max_cosmic*1.0;
	//cut_max_bnb =cut_max_bnb*1.0;

	//Zoomed in notrack
	cut_min_cosmic = 0.44; cut_max_cosmic = 0.455;
	cut_min_bnb = 0.44; cut_max_bnb = 0.455;
	//Zoomed in track
	//	cut_min_cosmic = 0.54; cut_max_cosmic = 0.58;
	//	cut_min_bnb = 0.515; cut_max_bnb = 0.53;


	std::cout<<"BNB sig scan from: "<<cut_min_bnb<<" to "<<cut_max_bnb<<std::endl;
	std::cout<<"COSMIC sig scan from: "<<cut_min_cosmic<<" to "<<cut_max_cosmic<<std::endl;

	double step_cosmic = (cut_max_cosmic-cut_min_cosmic)/((double)nsteps_cosmic);
	double step_bnb = (cut_max_bnb-cut_min_bnb)/((double)nsteps_bnb);


	TH2D * h2_sig_cut = new TH2D( "significance_2D",  "significance_2D",nsteps_cosmic, cut_min_cosmic, cut_max_cosmic, nsteps_bnb, cut_min_bnb, cut_max_bnb);
	std::vector<double> vec_sig;//some vectors to store TGraph info;
	std::vector<double> vec_cut;	

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
		    significance = signal/sqrt(background);
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

    }else{
	std::cout<<"Starting to Scan Significance quickly: "<<std::endl;
	double zero = 0 ;
	double max = DBL_MAX;
	double max2 = DBL_MAX;
	double *best_sig;   best_sig = &zero;
	double *best_cut;   best_cut = &max;
	double *best_cut2;  best_cut2 = &max2;//allocate memory for the best significance info.

	double plot_pot = 6.6e20;

	double cut_min_cosmic = 999;
	double cut_max_cosmic = -999;

	double cut_min_bnb = 999;
	double cut_max_bnb = -999;  //initialize maximum and minimum

	for(size_t i = 0; i < sig_files.size(); ++i) {
	    //	double tmin_cos = sig_files.at(i)->tvertex->GetMinimum( (sig_files.at(i)->getBDTVariable(cosmic_focused_bdt).name + ">0").c_str()    );
	    double tmax_cos = sig_files.at(i)->tvertex->GetMaximum( sig_files.at(i)->getBDTVariable(cosmic_focused_bdt).name.c_str()    );
	    double tmax_bnb = sig_files.at(i)->tvertex->GetMaximum( sig_files.at(i)->getBDTVariable(bnb_focused_bdt).name.c_str()    );

	    //if( tmin_cos <= cut_min_cosmic) cut_min_cosmic=tmin_cos;
	    if( tmax_cos >= cut_max_cosmic) cut_max_cosmic=tmax_cos;
	    if( tmax_bnb >= cut_max_bnb) cut_max_bnb=tmax_bnb;

	}

/*--------------------------------------------------------------------------------------------------
*-------------------------------Set parameters below-----------------------------------------------
*--------------------------------------------------------------------------------------------------
*/
	cut_min_cosmic = cut_max_cosmic*0.75;
	cut_min_bnb = cut_max_bnb*0.75;

	//Zoomed in track
	//cut_min_cosmic = 0.58; cut_max_cosmic = 0.61;
	//cut_min_bnb = 0.53; cut_max_bnb = 0.55;

	//instead of steps, use resolution, i.e. the size of cuts step:
	double resolution = 0.01;   //initial resolution; decreases it after finishing one 
	double res_step = 0.5;	    //set of searches: resolution = resolution*resolution_step;
	int zoom_times = 1;	    //set up the minimum resolution 

	//the last set of searches has resolution:
	double final_resolution = resolution * pow (res_step, zoom_times);

/*-------------------------------Start significance calculation-----------------------------------*/

	std::cout<<"COSMIC sig scan from: "<<cut_min_cosmic<<" to "<<cut_max_cosmic<<std::endl;
	std::cout<<"BNB sig scan from: "<<cut_min_bnb<<" to "<<cut_max_bnb<<std::endl;

	std::vector<double> vec_sig_pre ;//prepare vectors for storing info. of significance and cuts.
	std::vector<double> vec_cut_pre ;	
	std::vector<double> *vec_sig; vec_sig = &vec_sig_pre;//some vectors to store TGraph info;
	std::vector<double> *vec_cut; vec_cut = &vec_cut_pre;

	//caclualte the amount of blocks for the TH2D plot, based on the final set of searches.
	double nsteps_cosmic = (cut_max_cosmic-cut_min_cosmic)/final_resolution + 1;
	double nsteps_bnb = (cut_max_bnb-cut_min_bnb)/final_resolution + 1;

	//std::cout<<"nsteps: "<<cut_max_cosmic<<","<<cut_min_cosmic<<","<<final_resolution<<","<<nsteps_cosmic<<" and "<<nsteps_bnb<<std::endl;

	TH2D * h2_sig_cut = new TH2D( "significance_2D", "significance_2D", nsteps_cosmic, cut_min_cosmic, 
		cut_max_cosmic + final_resolution, nsteps_bnb , cut_min_bnb , cut_max_bnb + final_resolution );

	//initialize cuts with no data ( distingushed from cuts with 0 background).
	for(int i = 1 ; i <= nsteps_cosmic ; i++ ){
	    for(int j = 1 ; j <= nsteps_bnb ; j++ ){
		h2_sig_cut->SetBinContent( i , j , 0 );
		//std::cout<<"add "<<i<<" and "<<j<<std::endl;
	    }
	}

	double d = cut_min_cosmic;//d for cosmic cut; d2 for bnb cut.
	double d2 = cut_min_bnb;

	bool is_cosmic_edge_value = false;//when reach the edge (maximum), it becomes true. 
	bool *is_cosmic_edge; is_cosmic_edge = &is_cosmic_edge_value;

	int sig_count_value = 0;	    //record how many cuts have been calcualted.
	int *sig_count; sig_count = &sig_count_value;
	// calculate the averaged significance. 
	// When the significance is lower than average, end the search (move on the smaller resolution).

	double ave_sig_value = 0;
	double *ave_sig; ave_sig = &ave_sig_value;

	int current_step_value = 0; 
	int *current_step; current_step = &current_step_value;

	while (1){//locate the maximum diagonally.
	    double int_test = (d-cut_min_cosmic)/resolution*res_step;//these are to verify if the current cuts are multipler of previous resolution,
	    double int_test2 = (d2-cut_min_bnb)/resolution*res_step;//i.e. if these fit a larger (previous) resolution, then avoid it.

	    double this_sig = 99;//large initial value for skipping the second next if statement
	    
	    //integer means these cuts have been done.
	   // std::cout<<int_test<<"second int_test: "<<int_test2<<" and step: "<<*current_step<<std::endl;
	   // std::cout<<std::floor(int_test)<<"second int_test: "<<std::floor(int_test2)<<" and step: "<<*current_step<<std::endl;
	   // if(std::floor(int_test)!=int_test) std::cout<<"true1"<<std::endl;
	   // if(std::floor(int_test2)!=int_test2) std::cout<<"true2"<<std::endl;
	   // if(int_test - std::floor(int_test)>=0.0000001) std::cout<<"true11"<<std::endl;
	   // if(std::floor(int_test2)!=int_test2) std::cout<<"true22"<<std::endl;
	   // if(*current_step == 0) std::cout<<"true3"<<std::endl;
	    if( (int_test - std::floor(int_test) >= 0.000001 || int_test2 - std::floor(int_test2) >= 0.000001) || *current_step == 0 ){
	    //== is bad for evaluating calculated value.

		this_sig = get_significance(sig_files, bkg_files, plot_pot, d , d2 , best_sig,
			best_cut, best_cut2, vec_sig, vec_cut);

		double ith_block = (d - cut_min_cosmic) / final_resolution + 1;
		double jth_block = (d2 - cut_min_bnb) / final_resolution + 1;

//		std::cout<<"fill in "<<ith_block<<"th block and "<<jth_block<<"th block."<<std::endl;

		h2_sig_cut->SetBinContent( ith_block , jth_block , this_sig );

		*ave_sig = ( *ave_sig*(*sig_count) + this_sig )/( ++*sig_count );//update average significance.

	    }

	    d  += resolution; d2 += resolution;

	    if ( d >= cut_max_cosmic || d2 >= cut_max_bnb || this_sig < *ave_sig ){

		d = *best_cut; d2 = *best_cut2;//take off from the best cuts.
		
		while ( 1 ){//go down, d2--; or left, d--

		    double next_sig = *ave_sig;

		    if(!*is_cosmic_edge){
			if(d2 - resolution <= cut_min_bnb)goto do_nothing;
			d2 -= resolution;
			//std::cout<<"going down>> "<< std::endl;
		    }else{
			if (d - resolution <= cut_min_cosmic)goto do_nothing;
			d -= resolution;
			//std::cout<<"<<going left "<< std::endl;
		    }

		    int_test = (d-cut_min_cosmic)/resolution*res_step;//these are to verify if the current cuts are multipler of previous resolution,
		    int_test2 = (d2-cut_min_bnb)/resolution*res_step;//i.e. if these fit a larger (previous) resolution, then avoid it.

		    if( (int_test - std::floor(int_test) >= 0.000001 || int_test2 - std::floor(int_test2) >= 0.000001) || *current_step == 0 ){

			next_sig = get_significance( sig_files , bkg_files , plot_pot , d , d2 , 
				best_sig, best_cut, best_cut2, vec_sig, vec_cut );

			h2_sig_cut->SetBinContent( (d - cut_min_cosmic) / final_resolution + 1 , 
				(d2 - cut_min_bnb ) / final_resolution + 1 , next_sig );//fill in the nth block.
		
//			std::cout<<"fill in "<<(d - cut_min_cosmic) / final_resolution + 1<<"th block and "<<(d2 - cut_min_bnb ) / final_resolution + 1<<"th block."<<std::endl;

			*ave_sig = ( *ave_sig*(*sig_count) + this_sig )/( ++*sig_count );//update average significance.
		    }
		    do_nothing:
		    if ( next_sig <= *ave_sig || d <= cut_min_cosmic || d2 < cut_min_bnb ) {
			d = *best_cut;
			d2 = *best_cut2;

			if(*is_cosmic_edge ){
			    break; 
			}else{
			    *is_cosmic_edge = true;//is_cosmic_edge true means finish searching d2--;
			}
		    }
		}
		*is_cosmic_edge = false;
		resolution *= res_step;
		std::cout<<"Zoom in to step: "<<resolution<<". Current zoom times: "<<++(*current_step)<<std::endl;
		if( *current_step > zoom_times ) break;
	    }
	}

	h2_sig_cut->SetStats(false);
	TCanvas * c_sig_cuts =  new TCanvas( "significance_cuts_colz", "significance_cuts_colz", 2000,1600 );
	c_sig_cuts->Divide(2,1);
	TPad *p1 = (TPad*)c_sig_cuts->cd(1);
	p1->SetRightMargin(0.13);
	h2_sig_cut->Draw("colz");
	h2_sig_cut->GetXaxis()->SetTitle("Cosmic Cut");
	h2_sig_cut->GetYaxis()->SetTitle("BNB Cut");//finish preparing the color blocks.

	double best_significance = *best_sig;
	double best_mva_cut = *best_cut;
	double best_mva_cut2 = *best_cut2;

	std::vector<double> vec_bf_cut1 = {best_mva_cut};
	std::vector<double> vec_bf_cut2 = {best_mva_cut2};

	TGraph *graph_bf = new TGraph(vec_bf_cut1.size(), &vec_bf_cut1[0], &vec_bf_cut2[0]);
	graph_bf->SetMarkerStyle(29);
	graph_bf->SetMarkerSize(2);
	graph_bf->SetMarkerColor(kBlack);
	graph_bf->Draw("same p");//finish preparing the star mark.


	TGraph * graph_cut = new TGraph( vec_sig->size() , &(*vec_cut)[0] , &(*vec_sig)[0] );
	graph_cut->SetTitle("1D slices");
	c_sig_cuts->cd(2);
	graph_cut->Draw("alp");

	h2_sig_cut->Write();
	c_sig_cuts->Write();

	//return std::vector<double>{best_mva_cut, best_mva_cut2, best_significance};
	std::cout<<"Clarification: unexplored area is assigned significance 0."<<std::endl;
	return std::vector<double>{*best_cut, *best_cut2, *best_sig};
    }
}

double get_significance(std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files, double plot_pot, double cosmic_cut, double bnb_cut, double *best_significance,
	double *best_mva_cut , double *best_mva_cut2, std::vector<double>* vec_sig,    std::vector<double>* vec_cut){	

    double signal = 0;
    double background = 0;
    std::vector<double> bkg;	

    for(size_t i = 0; i < sig_files.size(); ++i) {
	double pot_scale = (plot_pot/sig_files.at(i)->pot )*sig_files.at(i)->scale_data;

	std::string bnbcut = sig_files.at(i)->getStageCuts(3 , cosmic_cut , bnb_cut); 
	//std::string bnbcut = sig_files.at(i)->getStageCuts(3,d,d2); 
	signal += sig_files.at(i)->GetEntries(bnbcut.c_str())*pot_scale;

	if(signal == 0) return 0;
    }

    for(size_t i = 0; i < bkg_files.size(); ++i) {
	double pot_scale = (plot_pot/bkg_files.at(i)->pot)*bkg_files.at(i)->scale_data;

	std::string bnbcut = bkg_files.at(i)->getStageCuts(3,cosmic_cut , bnb_cut); 
	bkg.push_back(	bkg_files.at(i)->GetEntries(bnbcut.c_str())*pot_scale);			

	background += bkg.back();
    }

        double significance = 0;

    if( background != 0){
	significance = signal/sqrt(background);
	}
    //disable this section of print out if background is 0.
    /*if(signal == 0){
	significance = 0;
    }else if(background != 0){
	significance = signal/sqrt(background);
    }
    else{
	std::cout<<"method_best_significane_seperate || signal2+background2 == 0, so significance  = nan @ cut1: "<< cosmic_cut <<", cut2: "<< bnb_cut<<std::endl;
    }*/

    if(significance > *best_significance) {
	*best_significance = significance;
	*best_mva_cut = cosmic_cut;
	*best_mva_cut2 = bnb_cut;
	std::cout<<"	>>Find a maximum sigificance (see next line):"<< *best_significance << std::endl;
    }

    //Out put results:
    std::cout<<"ccut: ";
    std::cout<<std::left<<std::setw(8)<<cosmic_cut;
    std::cout<<", bcut: ";
    std::cout<<std::setw(8)<<bnb_cut;
    std::cout<<"; #signal: ";
    std::cout<<std::left<<std::setw(8)<<signal;
    std::cout<<", #bkg: ";
    std::cout<<std::left<<std::setw(8)<<background;
    std::cout<<"= ";
    std::cout<<std::left<<std::setw(8)<<bkg.at(0);
    std::cout<<"(BNB) + ";
    std::cout<<std::left<<std::setw(8)<<bkg.at(1);
    std::cout<<std::left<<"(cosmic) || "<<significance;

    if( background == 0 ){ 
	std::cout<<" (No background)"<<std::endl;
    }
    else{
	std::cout<<std::endl;
    }
    
    vec_sig->push_back( significance );

    vec_cut->push_back(cosmic_cut);

    //std::cout<<" push_bnb_cut sig "<<std::endl;
    vec_cut->push_back(bnb_cut);

    return significance;
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

