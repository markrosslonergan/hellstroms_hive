#include "bdt_sig.h"
#include "TMath.h"
#include "TTreeIndex.h"//Use the TTreeIndex class
#include <fstream>//read and write txt file.
#include <unistd.h>//this is for exit(0)
#include <sys/stat.h>
#include <numeric> //to use iota

using namespace std;
/*
vector<double> file_reader( string str ){
		ifstream input(str);

		vector<double> content;
		if(input){
			double temp_content;
			while( input >> temp_content){

				content.push_back(temp_content);
			}
		}else{
			cerr<< str <<" is not found"<<endl;
			exit(0);
		}
		//cout<<responses[0][0]<<" and "<< responses[0][1]<<endl;
		//cout<<responses[1][0]<<" and "<< responses[1][1]<<endl;
		input.close();

		return content;
}*/

std::vector<double> scan_significance(TFile * fout, std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files, bdt_info cosmic_focused_bdt, bdt_info bnb_focused_bdt){

/*	if(false){//Record Entries and Produce BDT scatter plots here.
		cout<<"This process will extract entries from root files, and might take long.\n If you have done so, you may check ./contour_cut and stop it."<<endl;
		sleep (10);
		int number_of_files = sig_files.size()+bkg_files.size();
		string current_directory = "";

		cout<<"Working on "<< number_of_files<<" files."<<endl;
		vector<bdt_file*> MCfiles;
		MCfiles.reserve(number_of_files);
		MCfiles.insert( MCfiles.end(), sig_files.begin(), sig_files.end() );
		MCfiles.insert( MCfiles.end(), bkg_files.begin(), bkg_files.end() );

		vector< vector<string> > BDTfiles(number_of_files , vector<string> (2,current_directory));//app.root files with BDT responses.
		vector< vector<string> > new_var(number_of_files , vector<string> (2,""));//TTree in app.root.

		//	cout<<"I got the path in a 2d vector:"<<BDTfiles[0][0]<<endl;
		//	identifier+"_"+files[i]->tag+"_app"+".root"

		for(int index = 0; index<number_of_files; index++){//0 for cosmic, 1 for bnb!
			BDTfiles[index][0]+=cosmic_focused_bdt.identifier+"_"+MCfiles[index]->tag+"_app"+".root";
			cout<<BDTfiles[index][0]<<endl;//CLEAN
			BDTfiles[index][1]+=bnb_focused_bdt.identifier+"_"+MCfiles[index]->tag+"_app"+".root";

			new_var[index][0]+=MCfiles[index]->tag+"_"+cosmic_focused_bdt.identifier;
			cout<<new_var[index][0]<<endl;//CLEAN
			new_var[index][1]+=MCfiles[index]->tag+"_"+bnb_focused_bdt.identifier;
		}

		vector<string> new_tree = { "treecosmic", "treebnb"};
		//---------------- NAMES are ready!

		//Prepare Canvase for drawing:
		TCanvas * c = new TCanvas();
		//xaxis:Cosmic... yaxis:BNB	
		TH2D *dummyhist = new TH2D("dummyhist", "BDT Scores", 60,0.2,0.7,60,0.3,0.6);//TO be updated, CHECK
		dummyhist->GetXaxis()->SetTitle("Cosmic BDT Score");
		dummyhist->GetYaxis()->SetTitle("BNB BDT Score");
		dummyhist->Draw();


		vector<TGraph*> contents(number_of_files);
		TLegend *l = new TLegend(0.1,0.7,0.48,0.9);

		if (access("contour_cut",F_OK) == -1){
			mkdir("contour_cut",0777);//Create a folder for pdf.
		}
//		vector<int> candidate_entries[number_of_files];//create pointers for indices that track BDT responses.

		for (int index = number_of_files-1; index >= 0 ; index--){//Loop over bkg1,bkg2 ... signal
			MCfiles.at(index)->tvertex->AddFriend((new_tree.at(0)+" = "+ new_var[index][0]).c_str() , BDTfiles[index][0].c_str() );
			MCfiles.at(index)->tvertex->AddFriend((new_tree.at(1)+" = "+ new_var[index][1]).c_str() , BDTfiles[index][1].c_str() );
			//Friend Trees are ready! Variables (.mva) can be called in the tree named "new_tree"

			//Feed in TGraph object with values.
			Int_t num_events = MCfiles.at(index)->tvertex->GetEntries();
			vector<int> temp_indices(num_events);//all indices, initialized in next line
			iota (std::begin(temp_indices), std::end(temp_indices), 0);//indices from 1 to num_events;

			cout<<"Get number of events at "<< MCfiles.at(index)->tag <<": "<<num_events<<endl;

			double *cos = new double[num_events];//Dynamically memory allocation!!
			//Thanks https://www.learncpp.com/cpp-tutorial/69-dynamic-memory-allocation-with-new-and-delete/
			double *bnb = new double[num_events];

			Double_t weight=1;
			Double_t cosbdt,bnbbdt;//weight value to store and be used for checking lee_weights

			vector<int> temp_entries(num_events);
			Int_t count = 0;//for the scattering plot.

			iota (begin(temp_entries),end(temp_entries), 0);//fill vector with 0,1,2,...

			MCfiles.at(index)->tvertex->SetBranchAddress((new_tree.at(0)+".mva").c_str(),  &cosbdt);
			MCfiles.at(index)->tvertex->SetBranchAddress((new_tree.at(1)+".mva").c_str(),  &bnbbdt);

			if(index==0){//record target_bdt_cuts from the signal BDT responses

				//create files for recording BDT repsonses.
				std::ofstream mark_BDT_Response("./contour_cut/wheretocuts.txt");//create a new file
				mark_BDT_Response.close();
				mark_BDT_Response.open("./contour_cut/wheretocuts.txt",std::ios::app);

				ofstream entries_list("./contour_cut/"+MCfiles.at(index)->tag+".txt");//create a new file
				entries_list.close();
				entries_list.open("./contour_cut/"+MCfiles.at(index)->tag+".txt",std::ios::app);

				cout<<"Saving (cosmic && BNB) BDT responses on signal sample to ./contour_cut/wheretocuts.txt."<<endl;

				for( int i = num_events-1 ; i >= 0 ;i--){

					MCfiles.at(index)->tvertex->GetEntry(i);//this changes the value of weight, cosbdt, bnbbdt; local is the row# of the given index.

					if(cosbdt > -999 && bnbbdt > -999){
//						candidate_entries[index].push_back(i);
						cos[count] = cosbdt;
						bnb[count++] = bnbbdt;

						mark_BDT_Response<<cosbdt<<" "<<bnbbdt<<std::endl;//save good BDT responses to the txt file.
						entries_list<<i<<endl;
					}
					temp_entries.pop_back();//delete the last entrty index, no matter what.

					if(i%(num_events/100)==0){
						cout.precision(3);
						cout<<"\r"<<100*(num_events-1-i)/num_events+1<<"\% of "<< num_events << " entries complete.";
						cout.flush();
					}
				}

				mark_BDT_Response.close();

				entries_list.close();
				cout<<"(cosmic && BNB) BDT responses and entries list are recorded in wheretocuts.txt,"<<MCfiles.at(index)->tag<<".txt."<<endl;

			}else{//bkg samples

				ofstream entries_list("./contour_cut/"+MCfiles.at(index)->tag+".txt");//create a new file
				entries_list.close();
				entries_list.open("./contour_cut/"+MCfiles.at(index)->tag+".txt",std::ios::app);

				for( int i = num_events-1 ; i >= 0 ;i--){

					MCfiles.at(index)->tvertex->GetEntry(i);//this changes the value of weight, cosbdt, bnbbdt; local is the row# of the given index.

					if(cosbdt > -999 && bnbbdt > -999){
						cos[count] = cosbdt;
						bnb[count++] = bnbbdt;

						entries_list<<i<<endl;
					}
					temp_entries.pop_back();//delete the last entrty index, no matter what.

					if(i%(num_events/100)==0){
						cout.precision(3);
						cout<<"\r"<<100*(num_events-1-i)/num_events+1<<"\% of "<< num_events << " entries complete.";
						cout.flush();
					}
				}

				entries_list.close();
				cout<<"Candidate entries list is recorded in "<<MCfiles.at(index)->tag<<".txt."<<endl;

				cout<<"  Done, next!\n"<<endl;
												//(n, x, y)
				contents.at(index) = new TGraph (num_events, cos, bnb); //TGraph(Int_t , Double_t *, Double_t *);
				contents.at(index)->SetMarkerColor(index+2);//see https://root.cern.ch/doc/master/classTAttMarker.html#M2
				contents.at(index)->SetMarkerStyle((number_of_files-index)/number_of_files*24+1);//25 or 1.
				contents.at(index)->Draw("P same");

				l->AddEntry(contents.at(index),MCfiles.at(index)->tag.c_str(),"p");
			}
		}

		//Fill in dummyhist
		l->Draw();

		c->Write();
		c->SaveAs("BDTscattering.pdf","pdf");

		cout<<"The 2D histogram is created."<<endl;

		//std::vector<double> p;
		//return p;
	}

	if(true){//Calculate contour and produce root file.
		//Extract BDT responses in wheretocut.txt
		ifstream input("./contour_cut/wheretocuts.txt");
		vector<vector<double> > responses;
		if(input){
			vector<double> temp_responses(2);
			while( input >> temp_responses[0]>>temp_responses[1]){

				responses.push_back(temp_responses);
			}
		}else{
			cerr<<".txt for BDT reponses is not found"<<endl;
			exit(0);
		}
		cout<<responses[0][0]<<" and "<< responses[0][1]<<endl;
		cout<<responses[1][0]<<" and "<< responses[1][1]<<endl;
		input.close();
		
		//Prepare FILE_NAME.
		int number_of_files = sig_files.size()+bkg_files.size();
		string current_directory = "";

		cout<<"Working on "<< number_of_files<<" files."<<endl;
		vector<bdt_file*> MCfiles;
		MCfiles.reserve(number_of_files);
		MCfiles.insert( MCfiles.end(), sig_files.begin(), sig_files.end() );
		MCfiles.insert( MCfiles.end(), bkg_files.begin(), bkg_files.end() );

		vector< vector<string> > BDTfiles(number_of_files , vector<string> (2,current_directory));//app.root files with BDT responses.
		vector< vector<string> > new_var(number_of_files , vector<string> (2,""));//TTree in app.root.

		//	cout<<"I got the path in a 2d vector:"<<BDTfiles[0][0]<<endl;
		//	identifier+"_"+files[i]->tag+"_app"+".root"

		for(int index = 0; index<number_of_files; index++){//0 for cosmic, 1 for bnb!
			BDTfiles[index][0]+=cosmic_focused_bdt.identifier+"_"+MCfiles[index]->tag+"_app"+".root";
			cout<<BDTfiles[index][0]<<endl;//CLEAN
			BDTfiles[index][1]+=bnb_focused_bdt.identifier+"_"+MCfiles[index]->tag+"_app"+".root";

			new_var[index][0]+=MCfiles[index]->tag+"_"+cosmic_focused_bdt.identifier;
			cout<<new_var[index][0]<<endl;//CLEAN
			new_var[index][1]+=MCfiles[index]->tag+"_"+bnb_focused_bdt.identifier;
		}
		//FILE_NAMEs are ready.
	
		vector<double> test_vector= file_reader("./contour_cut/"+MCfiles.at(0)->tag+".txt");
		cout<<test_vector[0]<<endl;
		cout<<test_vector[1]<<endl;
		//2D array responses (cosmic BNB) is done!
	}


	exit(0);
*/
	bool quick_search = true;//use the original detail search via false value;
	//the if statement is the old program; change quick_search to enable it.
	if(!quick_search){//Scan significance steps by steps

		std::cout<<"Starting to Scan Significance"<<std::endl;
		double best_significance = 0;
		double best_mva_cut = DBL_MAX;
		double best_mva_cut2 = DBL_MAX;

		//double plot_pot = 6.6e20;
		double plot_pot=5e19;


		//for nice plots make the 50, 25 is quicker tho
		int nsteps_cosmic = 10;//50
		double cut_min_cosmic = 999;
		double cut_max_cosmic = -999;

		int nsteps_bnb = 10;//50
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
		cut_min_cosmic = 0.65; cut_max_cosmic = 0.7;
		cut_min_bnb = 0.55; cut_max_bnb = 0.63;
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


				//	std::cout<<"ccut: "<<d<<" bcut: "<<d2<<" "<<" #signal: "<<signal<<" #bkg: "<<background<<" || "<<" bnb: "<<bkg.at(0)<<" cos: "<<bkg.at(1)<<" || "<<significance<<std::endl;
				//OUTPUT
				std::cout<<"ccut: ";
				std::cout<<std::left<<std::setw(8)<<d;
				std::cout<<", bcut: ";
				std::cout<<std::setw(8)<<d2;
				std::cout<<"; #signal: ";
				std::cout<<std::left<<std::setw(8)<<signal;

				if(true){
					std::cout<<", #bkg: ";
					std::cout<<std::left<<std::setw(8)<<background;
					std::cout<<"= ";
					std::cout<<std::left<<std::setw(8)<<bkg.at(0);
					std::cout<<"(BNB) + ";
					std::cout<<std::left<<std::setw(8)<<bkg.at(1);
					std::cout<<std::left<<"(cosmic) || "<<to_string_prec(significance,2);

					std::cout<<std::left<<std::setw(8);
				}
				double ref_numbers = 100*signal*signal/background;
				std::cout<<std::left<<" >>Purity*signal#: "<<to_string_prec(ref_numbers,2)<<"%"<<endl;


				vec_sig.push_back(significance);
				vec_cut.push_back(d2);
				h2_sig_cut->SetBinContent(di,di2, significance);
			}

		}


		h2_sig_cut->SetStats(false);
		TCanvas * c_sig_cuts =  new TCanvas( "significance_cuts_colz", "significance_cuts_colz", 2000,1600 );
		//	c_sig_cuts->Divide(2,1);//disable the unnecessary plot.
		TPad *p1 = (TPad*)c_sig_cuts;//->cd(1);
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


		//	TGraph * graph_cut = new TGraph(vec_sig.size(), &vec_cut[0], &vec_sig[0]);
		//	graph_cut->SetTitle("1D slices");
		//	c_sig_cuts->cd(2);
		//	graph_cut->Draw("alp");

		h2_sig_cut->Write();
		c_sig_cuts->Write();

		return std::vector<double>{best_mva_cut, best_mva_cut2, best_significance};

	}else{//Start to scan significance quickly
		std::cout<<"Starting to Scan Significance quickly: "<<std::endl;
		double zero = 0 ;
		double max = DBL_MAX;
		double max2 = DBL_MAX;
		double *best_sig;   best_sig = &zero;
		double *best_cut;   best_cut = &max;
		double *best_cut2;  best_cut2 = &max2;//allocate memory for the best significance info.

		//	double plot_pot = 6.6e20;
		double plot_pot = 4.6e19;

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
		cut_min_cosmic = cut_max_cosmic*0.6;
		cut_min_bnb = cut_max_bnb*0.6;

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
		//	c_sig_cuts->Divide(2,1);
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


		//	TGraph * graph_cut = new TGraph( vec_sig->size() , &(*vec_cut)[0] , &(*vec_sig)[0] );
		//	graph_cut->SetTitle("1D slices");
		//	c_sig_cuts->cd(2);
		//	graph_cut->Draw("alp");

		std::cout<<"Writing pdf."<<std::endl;
		h2_sig_cut->Write();
		c_sig_cuts->Write();

		c_sig_cuts->SaveAs("significance.pdf","pdf");
		//return std::vector<double>{best_mva_cut, best_mva_cut2, best_significance};
		std::cout<<"Clarification: unexplored area is assigned significance 0."<<std::endl;

		//--------------------------- Make a 2D histogram for signal -------------------------
		//CHECK
		//	cout<<"Start to calculate 2D histogram for signal"<<endl;    
		//	TH2D * h2_cut = new TH2D( "signal_2D",  "signal_2D", 46, 0 , 1 , 46, 0, 1);
		//	h2_cut->SetStats(false);
		//
		//	TCanvas * c_cuts =  new TCanvas( "cuts_colz", "cuts_colz", 2000,1600 );
		//	TPad *p1 = (TPad*)c_cuts;
		//
		//	h2_cut->Draw("colz");
		//	h2_cut->GetXaxis()->SetTitle("Cosmic Cut");
		//	h2_cut->GetYaxis()->SetTitle("BNB Cut");
		//
		//	h2_cut->Write();
		//	c_cuts->SaveAs("./datamc/signalBDTVAR.pdf","pdf");
		//
		//----------------------- Finish Making a 2D histogram for signal ------------------------


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
		std::cout<<right<<setw(103)<<">>Find a maximum sigificance (see the next line): ";
		cout<< *best_significance << std::endl;
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
	std::cout<<std::left<<"(cosmic) || "<<to_string_prec(significance,2);

	std::cout<<std::left<<std::setw(8);
	double ref_numbers = 100*signal*signal/background;
	std::cout<<std::left<<" >>Purity*signal#: "<<to_string_prec(ref_numbers,2)<<"%";

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
	cout<<"GO AND CHECK sig.cxx\n\n\n\n\n"<<endl;
	/*	std::cout<<"Starting to Scan Significance"<<std::endl;
		double best_significance = 0;
		double best_mva_cut = DBL_MAX;
		double best_mva_cut2 = DBL_MAX;

	//	double plot_pot = 6.6e20;
	double plot_pot = 13.2e20;


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
	*/
}

