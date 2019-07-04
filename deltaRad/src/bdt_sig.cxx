#include "bdt_sig.h"
#include "TMath.h"
#include "TTreeIndex.h"//Use the TTreeIndex class
//#include "TMultiGraph.h"//draw markers
//#include "TF1.h"
#include "TF2.h"
#include "assert.h"

#include <fstream>//read and write txt file.
#include <unistd.h>//this is for exit(0)
#include <sys/stat.h>
#include <numeric> //to use iota

using namespace std;

contour_struct::contour_struct( string const & tree_name) {//Copy the struct from app.cxx
	tree = new TTree(tree_name.c_str(), "");
	tree->Branch("mva", &mva, "mva/D");
}
contour_struct::~contour_struct() {
	delete tree;
}


TH2D *fillin_grid (vector<bdt_file*> files, string hist_name, int step){
//Put things into histogram!

	double tmin_cos = 0 , tmax_cos = 1, tmin_bnb = 0, tmax_bnb = 1;
	if (access("sig_limits.txt",F_OK) == -1){//no file
		std::cout<<"Warning: No sig_limits.txt is found in the current directory. Need to do -o sig to generate it"<<endl;
		exit(EXIT_FAILURE);
	}
	else{
		std::cout<<"	Read off limits on axies from sig_limits.txt for cos & bnb responses. ";

		std::fstream best_responses("sig_limits.txt", std::ios_base::in);

		best_responses >> tmin_cos >> tmax_cos;
		best_responses >> tmin_bnb >> tmax_bnb;
	}


	TH2D *finished_grid = new TH2D((hist_name+to_string(files[0]->target_pot)).c_str(), hist_name.c_str(),
					step,tmin_cos, tmax_cos, step, tmin_bnb, tmax_bnb);//Book the grids, and fill it.
	cout<<"looping over 2D histogram with dimension: "<<step<<" x "<< step<<endl;

	for(int di=1; di<=step; di++) {
		for(int di2=1; di2<=step; di2++) {
			double events_rate = 0;


			for(size_t i = 0; i < files.size(); ++i) {
				double pot_scale = (files.at(i)->target_pot/files.at(i)->pot )*files.at(i)->scale_data;//target_pot is introduced in the bdt_file.h.
				double lower_cos_cut = (di-1.0)/(double)step*(tmax_cos-tmin_cos)+tmin_cos;
				double lower_bnb_cut = (di2-1.0)/(double)step*(tmax_bnb-tmin_bnb)+tmin_bnb;
				double upper_cos_cut = (di-0.0)/(double)step*(tmax_cos-tmin_cos)+tmin_cos;
				double upper_bnb_cut = (di2-0.0)/(double)step*(tmax_bnb-tmin_bnb)+tmin_bnb;

//				cout<<"CHECK!!"<<pot_scale<<". "<<lower_cos_cut<<" "<<upper_cos_cut<<"; "<<lower_bnb_cut<<" "<<upper_bnb_cut<<endl;
				string bottom_left = files.at(i)->getStageCuts(3,lower_cos_cut,lower_bnb_cut);//stage,cos,bnb
				string right_boundary = files.at(i)->getStageCuts(2,upper_cos_cut,0);//Get the opposite of this to make a box!
				string top_boundary = files.at(i)->getStageCuts(3,0,upper_bnb_cut);//Get the opposite of this to make a box!
				string selected_region = "("+bottom_left+")&&!("+right_boundary+")&&!("+top_boundary+")";
				events_rate += files.at(i)->GetEntries(selected_region.c_str())*pot_scale;
			}

			if(files.size()>1 && events_rate == 0){//bkg events;
				events_rate = 1e-26;//make up a small number
			}
			finished_grid->SetBinContent(di, di2, events_rate );//no need to do square root now.
			if(((di-1)*step+di2)%(step*step/100)==0){
			cerr.precision(3);
			cerr<<"\r Filling "<< step*step << " grid. "<<100*((di-1)*step+di2)/(step*step)<< "%";
			cerr.flush();
			}
//			cout<<" Finish "<<di<<" "<<di2<<" with "<<events_rate<<endl;
			//CHECK 
		}
	}
	if(false){	
	TCanvas* a_canvas = new TCanvas(files[0]->tag.c_str()," ",2000,1600);
	finished_grid->SetStats(false);
	finished_grid->Draw("colz");
	finished_grid->GetXaxis()->SetTitle("Cosmic BDT Responses");
	finished_grid->GetYaxis()->SetTitle("BNB BDT Responses");
//	a_canvas->Write();//I dont need to write~
//	a_canvas->Update();
	a_canvas->SaveAs((hist_name+".pdf").c_str(),"pdf");
	}
	
	cout<<"\nFinish producing 2D event grid for "<<hist_name<<endl;
	return finished_grid;
}

//This is new, use this to evaluate the "cut_info", possible cuts for signal and associated eff.;
//when the out_put is true, print out efficiency and corresponding cuts in cut_eff.txt
vector<double> evaluate_events(bdt_file* file, bdt_info contour_bdt, int entries,  int * row, double * cos, double* bnb, vector<double> focus1, vector<double> focus2, bool out_put){

	TFile * contour_cut = TFile::Open((file->tag+"_contour.root").c_str(), "recreate");

	int index_tracker = 0;
	Int_t num_events = file->tvertex->GetEntries();

	//make more trees!!
	string friend_tree_name = file->tag+"_"+contour_bdt.identifier;//+strict_name;
	contour_struct contour_info(friend_tree_name);
	cout<<"	Evaluating distance of each entries in file "<<file->tag<<endl;
	vector<double> signal_distances;
	for( int index = 0; index < num_events ; index++){
					
		contour_info.mva = -999;

		if(row[index_tracker] == index){//an entry that we want to use, which has positive responses
			
			 double temp_dis = sqrt(pow(cos[index_tracker]-focus2[0],2)+pow(bnb[index_tracker]-focus2[1],2))
				-sqrt(pow(cos[index_tracker]-focus1[0],2)+pow(bnb[index_tracker]-focus1[1],2));
			contour_info.mva = temp_dis;
			if(out_put) signal_distances.push_back(temp_dis);
//			cout<<"Ding "<<contour_info.distance<<endl;
			index_tracker++;
		}
		contour_info.tree->Fill();
	}
	contour_info.tree->Write();


//	contour_cut->Close();
	cout<<"	Finish recording contour_info for the file "<<file->tag<<endl;
	if(out_put){
		sort(signal_distances.begin(),signal_distances.end());

		//output cuts along with efficiency for reference;
		int iterator = 0;
		int how_long = signal_distances.size();
		cout<<"Save cuts and efficiency reference."<<endl;
		std::ofstream save_cuteff("cut_eff.txt");//recored limits of boundary;
		save_cuteff<<"Cuts Efficiency"<<endl;
		while(iterator < how_long ){
			save_cuteff<< signal_distances[iterator] <<" "<<to_string_prec(1-iterator/(double)how_long,3)<<endl;
			iterator++;
		}
		save_cuteff.close();
	}

//	contour_cut->Close();
	return signal_distances;
}


vector< double> get_peaks (TH2D* grid){
	Int_t max_sig_bin = grid->GetMaximumBin();
	Int_t x0,y0,z;
	grid->GetBinXYZ(max_sig_bin, x0,y0,z);
	Int_t dimension = grid->GetSize();
//	cout<<dimension<<endl;
//	Int_t ybinmax = grid->GetNBinY();
	int extand = 10;
	if(x0< extand + 1 || y0< extand + 1 || x0 > sqrt(dimension)-2-extand || y0 > sqrt(dimension)-2-extand ){
		cout<<"Dimension is too small, the foci might be determined incorrectly."<<endl;
		double x_coordinate =(grid->GetXaxis()->GetBinLowEdge(x0)
						+ grid->GetXaxis()->GetBinLowEdge(x0+1))/2;//x coor.
		double y_coordinate =(grid->GetYaxis()->GetBinLowEdge(y0)
						+ grid->GetYaxis()->GetBinLowEdge(y0+1))/2;//y coor.
		return {x_coordinate,y_coordinate};
	}
	if(true){//new
		int slots = pow(2*extand+1,2);
		vector< vector<double> > info_slots(slots, vector<double> (3,0));//9x2 location & weight;
		int x = x0 - extand;
		int y = y0 - extand;
		double all_events = grid->Integral(x,x+2*extand,y,y+2*extand);
		cout<<"all events "<<all_events<<" in "<<slots<<" slots around the peak: "<<x0<<","<<y0<<endl;

		for(int index = 0; index <= 2*extand; index++ ){
			for(int jndex = 0; jndex <= 2*extand; jndex++ ){
				info_slots[index*(2*extand+1)+jndex][0]=(grid->GetXaxis()->GetBinLowEdge(x+index)
						+ grid->GetXaxis()->GetBinLowEdge(x+index+1))/2;//x coor.
				info_slots[index*(2*extand+1)+jndex][1]=(grid->GetYaxis()->GetBinLowEdge(y+jndex)
						+ grid->GetYaxis()->GetBinLowEdge(y+jndex+1))/2;//y coor.
				info_slots[index*(2*extand+1)+jndex][2] = grid->GetBinContent(x+index,y+jndex)/all_events;
				cout<<"\rCalculating ("<<x+index<<"["<<index*(2*extand)+jndex<<"],"<<y+jndex<<")"<<grid->GetBinContent(x+index,y+jndex)/all_events;
				cout.flush();
			}
		}
		vector < double > result = {0,0};

		for(int index = 0; index <slots; index++){
			result[0]+=info_slots[index][0]*info_slots[index][2];
//			cout<<result[0]<<"+="<<info_slots[index][0]<<"*"<<info_slots[index][2]<<endl;
			result[1]+=info_slots[index][1]*info_slots[index][2];
		}
//		cout<<"\nRESULT "<<result[0]<<","<<result[1]<<endl;
		return result;
	}
	//(x,y) are the maximum;
	if(false){//old
	vector< vector<double> > info_slots(9, vector<double> (3,0));//9x2 location & weight;
	vector< vector<double> > helper = {	{-1,+1},{0,+1},{+1,+1},
										{-1, 0},{0, 0},{+1, 0},
										{-1,-1},{0,-1},{+1,-1}	};
	int x = x0;
	int y = y0;
	double all_events = grid->Integral(x-1,x+1,y-1,y+1);
	cout<<"all events "<<all_events<<" in 9 slots around the peak: "<<x0<<","<<y0<<endl;
	for(int index = 0; index < 9; index++){
		x = x0+helper[index][0];
		y = y0+helper[index][1];

		info_slots[index][0]=(grid->GetXaxis()->GetBinLowEdge(x)
						+ grid->GetXaxis()->GetBinLowEdge(x+1))/2;//x coor.
		info_slots[index][1]=(grid->GetYaxis()->GetBinLowEdge(y)
						+ grid->GetYaxis()->GetBinLowEdge(y+1))/2;//y coor.
		info_slots[index][2] = grid->GetBinContent(x,y)/all_events;
		cout<<"\rCalculating ("<<x<<"["<<info_slots[index][0]<<"],"<<y<<")"<<grid->GetBinContent(x,y)/all_events<<endl;
	}
	vector < double > result = {0,0};

	for(int index = 0; index < 9; index++){
		result[0]+=info_slots[index][0]*info_slots[index][2];
		cout<<result[0]<<endl;
		result[1]+=info_slots[index][1]*info_slots[index][2];
	}
		cout<<"\nRESULT "<<result[0]<<","<<result[1]<<endl;
	return result;
	}
}

//efficiencies: a set of boundaries that yield to target efficiencies (relative to pre-cut) will be drawn in the plot.
//step: the dimension of the plot would be (step x step);
//3 plots will be produced: signal-bkg, signal-bkg_enhanced, BDTResponsesScatter.
void select_events (vector<bdt_file*> sig_files, vector<bdt_file*> bkg_files, bdt_info cosmic_focused_bdt, bdt_info bnb_focused_bdt, bdt_info contour_bdt, vector<double> efficiencies, int step, vector<double> adjustment){
	double tmin_cos = 99 , tmin_bnb = 99 , tmax_cos = 0, tmax_bnb = 0;
	double temp_tmin_cos = 0 , temp_tmin_bnb = 0 , temp_tmax_cos = 0, temp_tmax_bnb = 0;
	double more = 0.05;//apply this value to push the limit of responses shown on axies.
	bool quick_plot = false;//turn this on, the scattering plot will produced quickly, but the BDT responses for bkg are not prepared;

	vector< string > saved_name = {"signal_bkg_events.root","dimension", "signal_events_rate", "background_events_rate_SquareRoot"};
	vector<bdt_file*> files;//Concate signal file and bkg files into a vector.

	files.reserve(sig_files.size()+bkg_files.size());
	files.insert( files.end(), sig_files.begin(), sig_files.end() );
	files.insert( files.end(), bkg_files.begin(), bkg_files.end() );

	if (access("sig_limits.txt",F_OK) == -1){//file does not exist, then figure it out here.
		cout<<"Finding the extremum of responses: "<<endl;

		for(int index = 0; index<files.size(); index++){//get the extremum BDT info by creating a histogram and extract the extremum from the histogram;
			cout<<"\rFinish examining "<<index+1<<" of "<< files.size() << " files.";
			cout.flush();
			bdt_file* temp_file = files[index];
			temp_file->calcBNBBDTEntryList(0,0);
			temp_file->setStageEntryList(3);//apply cos and BNB BDT cuts on 0;

			temp_tmin_cos = temp_file->tvertex->GetMinimum( temp_file->getBDTVariable(cosmic_focused_bdt).name.c_str());
			temp_tmax_cos = temp_file->tvertex->GetMaximum( temp_file->getBDTVariable(cosmic_focused_bdt).name.c_str());
			temp_tmin_bnb = temp_file->tvertex->GetMinimum( temp_file->getBDTVariable(bnb_focused_bdt).name.c_str() );
			temp_tmax_bnb = temp_file->tvertex->GetMaximum( temp_file->getBDTVariable(bnb_focused_bdt).name.c_str() );

			temp_file = NULL;

			if (temp_tmin_cos<tmin_cos) tmin_cos = temp_tmin_cos;
			if (temp_tmin_bnb<tmin_bnb) tmin_bnb = temp_tmin_bnb;
			if (temp_tmax_cos>tmax_cos) tmax_cos = temp_tmax_cos;
			if (temp_tmax_bnb>tmax_bnb) tmax_bnb = temp_tmax_bnb;
		}

		tmin_cos -= more;
		tmax_cos += more;
		tmin_bnb -= more;
		tmax_bnb += more;

		std::ofstream save_siglimits("sig_limits.txt");//recored limits of boundary;
		save_siglimits<< tmin_cos <<" "<<tmax_cos<<std::endl;
		save_siglimits<< tmin_bnb <<" "<<tmax_bnb<<std::endl;
		save_siglimits.close();
	} else{
		std::cout<<"	Read off limits on axies from sig_limits.txt for cos & bnb responses. ";

		std::fstream best_responses("sig_limits.txt", std::ios_base::in);

		best_responses >> tmin_cos >> tmax_cos;
		best_responses >> tmin_bnb >> tmax_bnb;
	}
	cout<<"\nRange of Responses: Cosmic:"<<tmin_cos<<" "<<tmax_cos<<" BNB:"<<tmin_bnb<<" "<<tmax_bnb<<endl;
	if(tmin_cos<0 || tmin_bnb<0){
		cout<<"Something wrong with the range of respones"<<endl;
		exit(EXIT_FAILURE);
	}
	//OK, limit is determined, for plotting;


	TH2D* signal_grid;
	TH2D* bkg_grid;
	TFile* signal_bkg_events;//Open files in the if,else section; and close it at the end.

	if (access(saved_name[0].c_str(),F_OK) == 0){//a root file for events grid exists, maybe no need to write it?

		cout<<"Event grids were previously generated; I might just load them directly;"<<endl;

		signal_bkg_events = TFile::Open(saved_name[0].c_str(), "READ");

		//load the file!
		TObject * temp_pointer;
		signal_bkg_events->GetObject(saved_name[1].c_str(),temp_pointer);

		UInt_t test_step = temp_pointer->GetUniqueID();//check the step that previously used.
		
		if( test_step != step){
			cout<<"The step has been changed, ";
			cout<<"from "<<test_step<<" to "<<step<<"."<<endl;
			goto writeit;//since the step is different from the previous root file, then make a new one.
		}

		//Looks good, load the histograms directly from the root file.
		signal_grid = (TH2D*) signal_bkg_events->Get(saved_name[2].c_str());
		bkg_grid = (TH2D*) signal_bkg_events->Get(saved_name[3].c_str());

		cout<<"Finish loading two event grids"<<endl;

	} else{//it does not exist, write it!
writeit:
		cout<<"Generate event grids"<<endl;

		//Note that sig_files[1] is ommitted, which is the data file.
		signal_grid = fillin_grid ({sig_files[0]}, saved_name[2], step);//this function maps events into the 2d grid.
		bkg_grid    = fillin_grid (bkg_files, saved_name[3], step);//this function maps events into the 2d grid.

		//save histograms to the file
		signal_bkg_events = TFile::Open(saved_name[0].c_str(), "recreate");
		

		TObject dimension;
		//Record the step as a check for whether or not we need to remake the root file.
		dimension.SetUniqueID(step);//Reference: https://root.cern.ch/root/roottalk/roottalk00/2311.html
		dimension.Write(saved_name[1].c_str());

		signal_grid->Write(saved_name[2].c_str());
		bkg_grid->Write(saved_name[3].c_str());

		cout<<"Write 2 histograms: "<<saved_name[2]<<" and "<< saved_name[3]<<" into the root file "<<saved_name[0]<<"."<<endl;
	}

	//ABOVE IS OK AND GOOD TO BE USED, THE FOLLOWING IS NEW


	//the following aims to create a root file that summarize bnb and cos responses into one.
	int efficiencies_choices = efficiencies.size();
	vector <TF2*> boundary(efficiencies_choices);
	TLegend *boundary_legend = new TLegend(0.1,0.7,0.4,0.9);//0.1,0.7,0.48,0.9);

	TLegend *scatter_legend = new TLegend(0.75,0.11,0.99,0.56);//SCATTER
										//x1,y1,x2,y2
	assert(files.size());	
	vector<TGraph*> contents(files.size());//SCATTER

	vector<double> focus1(2), focus2(2);
	if(true){//figuring out the boundary;
		
		focus1 = get_peaks( signal_grid);
		cout<<"Got signal peak above; see the following around bkg peak;"<<endl;
		focus2 = get_peaks( bkg_grid);
		focus1[0]+=adjustment[0];//use adjustment to change the foci
		focus1[1]+=adjustment[1];
		focus2[0]+=adjustment[2];
		focus2[1]-=adjustment[3];
		//CHECK!!
		cout<<"\nTwo FOCI:"<<endl;
		cout<<"For signal: "<<focus1[0]<<","<<focus1[1]<<endl;
		cout<<"For bkg   : "<<focus2[0]<<","<<focus2[1]<<endl;

		//prepare TF2 objects for showing boundary;
		for(int index = 0; index<files.size(); index++){//Construct root file for each sample.

			cout<<"\rPreparing root files: "<<index+1<<" of "<< files.size() << " finished."<<endl;
			//use a fake list to capture entry numbers
			string fake = "a_fake_entry_list";
			files[index]->tvertex->Draw((">>"+fake).c_str(), files[index]->getStageCuts_v2(3,0,0).c_str() , "entrylist");
			TEntryList * fake_list = (TEntryList*)gDirectory->Get(fake.c_str());
			int num_events = fake_list->GetN();
//			cout<<"EVENTS NUMBER: "<< num_events<<endl;

//			cout<<"Get number of events at "<< files[index]->tag <<": "<<num_events1<<endl;

			double *cos = new double[num_events];//Dynamically memory allocation!!
			//Thanks https://www.learncpp.com/cpp-tutorial/69-dynamic-memory-allocation-with-new-and-delete/
			double *bnb = new double[num_events];
			int *row = new int[num_events];
			Double_t cosbdt,bnbbdt;//store values:

			vector<string> new_tree = {files[index]->tag +"_"+cosmic_focused_bdt.identifier,
				files[index]->tag +"_"+bnb_focused_bdt.identifier};

			files[index]->tvertex->SetBranchAddress((new_tree.at(0)+".mva").c_str(),  &cosbdt);
			files[index]->tvertex->SetBranchAddress((new_tree.at(1)+".mva").c_str(),  &bnbbdt);

			for( int count = 0; count < num_events ;count++){//Selected good entries, i.e. with positive BDT responses.
				files[index]->tvertex->GetEntry(fake_list->GetEntry(count));//this changes the value of weight, cosbdt, bnbbdt; local is the row# of the given index.

					row[count] = fake_list->GetEntry(count);
					cos[count] = cosbdt;
					bnb[count] = bnbbdt;

//				temp_entries.pop_back();//delete the last entrty index, no matter what.
				if(count%(num_events/100)==0){
					cout.precision(3);
					cout<<"\r"<<100*count/num_events+1<<"\% of "<< num_events << " entries complete.";
					cout.flush();
				}
			}

			//SCATTER
			contents.at(index) = new TGraph (num_events, cos, bnb); //TGraph(Int_t , Double_t *, Double_t *);
			//Add transparency with "Alpha";
			contents.at(index)->SetMarkerColorAlpha(files[index]->col,0.6);//see https://root.cern.ch/doc/master/classTAttMarker.html#M2
			contents.at(index)->SetFillColorAlpha(files[index]->col,0.6);//see https://root.cern.ch/doc/master/classTAttMarker.html#M2
			contents.at(index)->SetMarkerStyle((files.size()-index)/files.size()*5+1);//2 or 1.
			//contents.at(index)->Draw("P same");

			scatter_legend->AddEntry(contents.at(index),files.at(index)->tag.c_str(),"f");
			//SCATTER

			double pot_scale = (files.at(index)->target_pot/files.at(index)->pot )*files.at(index)->scale_data;//target_pot is introduced in the bdt_file.h.
			
			vector< double > cut_info = evaluate_events(files.at(index), contour_bdt, num_events, row, cos, bnb, focus1, focus2, index == 0);//calculate raw_number with different efficiencies;
			int jndex = 0;

			while(index == 0 && jndex < efficiencies_choices){
				//CHECK to correct the possible efficiency;
				int eff_index = floor(cut_info.size()*(1-efficiencies[jndex]));//I DONT THINKS THIS USUALLY HAPPENS.. forget it;
				while(cut_info[eff_index]==cut_info[eff_index+1]){
					eff_index++;	
				}
				if(eff_index != floor(cut_info.size()*(1-efficiencies[jndex]))) cout<<"Due to repeated points; the efficiency is rounded to "<<1-eff_index/cut_info.size()<<endl;

				Double_t chosen_value[1] = {cut_info[eff_index]};//use this as contour level; take everything above it

				string dis1 = "(x-"+ to_string(focus2[0])+")^2+(y-"+to_string(focus2[1])+")^2";
				string dis2 = "(x-"+ to_string(focus1[0])+")^2+(y-"+to_string(focus1[1])+")^2";
				string expression = "sqrt("+dis1+")-sqrt("+dis2+")";
				cout<<"Obtain boundary: "<<expression<<" at level "<<chosen_value[0]<<endl;
				boundary[jndex] = new TF2("cut_info", expression.c_str(), tmin_cos, tmax_cos, tmin_bnb, tmax_bnb);
				boundary[jndex]->SetContour(1,chosen_value);//second argument is a poitner... ridiculous right?
				//see https://root.cern.ch/root/roottalk/roottalk03/1706.html
				boundary[jndex]->SetLineColor(30+jndex);
				boundary_legend->AddEntry(boundary[jndex],("after pre-cut, "+to_string_prec(efficiencies[jndex]*100,1)+"\% eff.").c_str(),"l");
				jndex++;
			}

			cout<<"Finish one file, next!\n"<<endl;

			delete cos,bnb,row;//clear memory and assign them to NULL again.
			cos = 0, bnb = 0, row = 0;

			if(quick_plot)		break;//This break helps to make quick plot;
		}
	}
//try a likelihood grids;
	if(false){
	TH2D* likelihood_norm = (TH2D*) signal_grid->Clone();
	TH2D* likelihood_deno = (TH2D*) signal_grid->Clone();
	likelihood_deno->Add(bkg_grid);
	likelihood_norm->Divide(likelihood_deno);
	TCanvas* e_canvas = new TCanvas(files[0]->tag.c_str()," ",2000,1600);
	likelihood_norm->SetTitle("Signal Likelihood (13.2E20 POT)");
	likelihood_norm->SetStats(false);
	likelihood_norm->Draw("colz");
	likelihood_norm->GetXaxis()->SetTitle("Cosmic BDT Responses");
	likelihood_norm->GetYaxis()->SetTitle("BNB BDT Responses");
	e_canvas->SaveAs("signal_likelihood.pdf","pdf");
	}


//draw a normal one;
	TH2D* compared_grid = (TH2D*) signal_grid->Clone();
	compared_grid->Add(signal_grid,bkg_grid,1,-1);//signal over background!
	TCanvas* a_canvas = new TCanvas(files[0]->tag.c_str()," ",2000,1600);
	compared_grid->SetTitle("Signal minus Background (13.2E20 POT)");
	compared_grid->SetStats(false);
	compared_grid->Draw("colz");
	compared_grid->GetXaxis()->SetTitle("Cosmic BDT Responses");
	compared_grid->GetYaxis()->SetTitle("BNB BDT Responses");
	for (int i = 0; i < efficiencies_choices; i++){
		boundary[i]->Draw("same");
	}
	boundary_legend->Draw();
	a_canvas->SaveAs("signal_minus_bkg.pdf","pdf");

//draw a enhanced_signal one;
	TH2D* compared2_grid = (TH2D*) signal_grid->Clone();
	compared2_grid->Add(signal_grid,bkg_grid,800,-1);//signal over background!
	TCanvas* a_canvas2 = new TCanvas(files[0]->tag.c_str()," ",2000,1600);
	compared2_grid->SetTitle("Signal (enhanced 800 times) minus Background (13.2E20 POT)");
	compared2_grid->SetStats(false);
	compared2_grid->Draw("colz");
	compared2_grid->GetXaxis()->SetTitle("Cosmic BDT Responses");
	compared2_grid->GetYaxis()->SetTitle("BNB BDT Responses");
	for (int i = 0; i < efficiencies_choices; i++){
		boundary[i]->Draw("same");
	}
	boundary_legend->Draw();
	//add foci to the plot
	double px[2],py[2];
	px[0]=focus1[0];
	py[0]=focus1[1];
	px[1]=focus2[0];
	py[1]=focus2[1];
	TGraph *foci = new TGraph(2, px, py);
	foci->SetMarkerStyle(29);
	foci->SetMarkerSize(2);
	foci->SetMarkerColor(kBlack);
	foci->Draw("same p");//finish preparing the star mark.

	a_canvas2->SaveAs("signal_minus_bkg_enhanced_signal.pdf","pdf");

//draw BDTscater_plot
	if(!quick_plot){
	TCanvas* a_canvas3 = new TCanvas();
	TH2D *responses_scattering = new TH2D("dummyhist", "BDT Responses", 60, tmin_cos, tmax_cos, 60, tmin_bnb, tmax_bnb);//To be updated, CHECK
	responses_scattering->SetStats(false);
	responses_scattering->GetXaxis()->SetTitle("Cosmic BDT Responses");
	responses_scattering->GetYaxis()->SetTitle("BNB BDT Responses");
	responses_scattering->Draw();
	for (int i =0 ; i < files.size(); i++){
		contents[i]->Draw("P same");
	}
	scatter_legend->Draw("p");
	for (int i = 0; i < efficiencies_choices; i++){
		boundary[i]->Draw("same");
	}
	boundary_legend->Draw();
	a_canvas3->Write();//we might not need this
	a_canvas3->SaveAs("BDTResponsescatter.pdf","pdf");
	}else{
		cout<<"THIS DOES NOT CHANGE THE ROOT FILES!"<<endl;
//		exit(EXIT_FAILURE);
	}
	signal_bkg_events->Close();

}

void significance_eff(vector<bdt_file*> sig_files, vector<bdt_file*> bkg_files, bdt_info contour_bdt_info){
	//Load the limits

	int num_bins = 800;//divide the x-axis into bins;
	double min_distance = 0;
	double max_distance = sig_files[0]->tvertex->GetMaximum( sig_files[0]->getBDTVariable(contour_bdt_info).name.c_str());
	double factor = 0.4;//Max efficiency;
	min_distance = max_distance * factor;

	//the limits are loaded
	string hist_title =  "significance versus signal efficiency (0 means not calculated)";
	TH1F* sig_hist = new TH1F("sig_eff", hist_title.c_str(), num_bins, min_distance, max_distance);
	
	double cuts;//the value for performing the contour selection;
	double gap = (max_distance-min_distance)/num_bins;
	vector< double > best_sig = {0,0,0};
	vector< int > best_at={3,2,1};
	double best_data = 0;
	int best_data_at = 0;

	double temp_sig = 0;
	int temp_index = 0;
	vector< int > temp_list(num_bins);
	iota (std::begin(temp_list), std::end(temp_list), 1);//indices from 1 to num_bins-1 for looping significance;
	int jump = num_bins/4;//initial jump for checking each significance;

	if(true){//new method;
		int run = 0;
		while(1){
		cout<<"The following efficiency indice are to be investigated:"<<endl;
		for(int i=0; i < temp_list.size(); i++){
			cout<<temp_list[i]<<" ";
		}
		cout<<endl;


		//calculate and fill in significance
//		cout<<"\r"<<"Filling "<<temp_index<<" of "<< num_bins<<" entries for the sig_eff.pdf.";
//		cout.flush();

		int current_index = temp_list[temp_index];
		cuts = min_distance + (current_index-1)* gap;
		double signal = 0, data = 0, background = 0;
		cout<<"Looking at "<<current_index<<endl;
//		for(int jndex = 0; jndex < sig_files.size(); jndex ++){
		string contourcut = sig_files[0]->getStageCuts_v2(5, cuts, 0);//get signal;
		signal = sig_files[0]->GetEntries(contourcut.c_str())*sig_files[0]->target_pot/sig_files[0]->pot;//13.2e20 POT is set in bdt_file.h
//			cout<<"signal: "<<signal<<endl;
//		}
		contourcut = sig_files[1]->getStageCuts_v2(5, cuts, 0);//get datamc;
		data = sig_files[1]->GetEntries(contourcut.c_str())*sig_files[1]->target_pot/sig_files[1]->pot;//13.2e20 POT is set in bdt_file.h

		for(int jndex = 0; jndex < bkg_files.size(); jndex ++){
			string contourcut = bkg_files[jndex]->getStageCuts_v2(5, cuts, 0);
			background += bkg_files[jndex]->GetEntries(contourcut.c_str())*bkg_files[jndex]->target_pot/bkg_files[jndex]->pot;//13.2e20 POT is set in bdt_file.h
//			cout<<"bkg: "<<background<<endl;
		}
		
		temp_sig = (background == 0)? 0:signal/sqrt(background);
		cout<<"Sig: "<<temp_sig<<" at cuts "<<cuts<<endl;
		sig_hist->SetBinContent(current_index, temp_sig);

		//update best info;
		if(temp_sig>best_sig[0]){
		cout<<"Compare agreement "<<signal+background<<" and "<<data-signal-background<<endl;
			if(0.5*(signal+background)>abs(data-signal-background)){
				best_data = temp_sig;
				best_data_at = current_index;
				cout<<"Find a Data-MC agreement cut"<<endl;
			}

			best_sig = {temp_sig, best_sig[0], best_sig[1]};
			best_at = {current_index, best_at[0], best_at[1]};
			cout<<"Update the best sig;"<<endl;
		}else if(temp_sig>best_sig[1]){
			best_sig = {best_sig[0],temp_sig, best_sig[1]};
			best_at = {best_at[0], current_index, best_at[1]};
			cout<<"Update the 2nd best sig;"<<endl;
		}else if(temp_sig>best_sig[2]){
			best_sig[2] = temp_sig;
			best_at[2] = current_index;
			cout<<"Update the 3rd best sig."<<endl;
		}
		cout<<"The best sig  (2nd, 3rd to the best) are found at indices: "<<best_at[0]<<","<<best_at[1]<<","<<best_at[2]<<" ";
		if((best_at[1]+best_at[2]-2*best_at[0]==0&& best_at[0]>best_at[1]&&best_at[0]>best_at[2]) || temp_list.size()<2){//best is at the middle and the 2nd,3rd best are next to it;
			break;
		}

		int dangerous_index = 0;//find the index that gives the index to be dealed with;
		while(temp_list[dangerous_index]!=best_at[2]&&temp_list[dangerous_index]!=current_index){
			dangerous_index ++;
		}
//		if(dangerous_index>temp_list.size()){
//			dangerous_index = 0;
//		}
			
		//modify vector for searching significance;
		temp_list.erase( temp_list.begin()+dangerous_index, temp_list.begin()+dangerous_index+1);
		//		if(dangerous_index !=0){
		if(best_at[0]>best_at[1] && best_at[1]>best_at[2]){
			cout<<" cut the front; ";
			temp_list.erase( temp_list.begin(), temp_list.begin()+dangerous_index );//monotonic increasing, then remove elements before best_at[0];

		}else if( best_at[0]<best_at[1] && best_at[1]<best_at[2]){
			cout<<" cut the tail; ";
			//	temp_list.erase( temp_list.begin()+best_at[2]+2, temp_list.end() );//monotonic decreasing, then remove elements after best_at[2];+2 is due to the removal of temp_index.
			for(int i = 0; i < dangerous_index; i++){
				temp_list.pop_back();
			}
		}
		//		}

		if(temp_index-1+jump<temp_list.size()){
			temp_index+=jump-1;
		}else if (jump == 1){
			break;
		}else{
			jump /= 2;
			temp_index = 0;
		}
		cout<<" with step: "<<jump<<endl;
		cout<<"Finish number of runs "<<++run<<endl;
		cout<<"-----"<<endl;
		}

	}		
	std::ofstream save_sig("sig_ref.txt");//recored limits of boundary;
	save_sig<< best_data <<" "<<best_sig[0]<<std::endl;
	save_sig.close();
	cout<<"Saved the aboved two cuts for selection."<<endl;


	if(false){
	for(int index = 1; index <= num_bins; index++ ){

		cout<<"\r"<<"Filling "<<index<<" of "<< num_bins<<" entries for the sig_eff.pdf.";
		cout.flush();

		cuts = min_distance + (index-1)* gap;
		double signal = 0, background = 0;
		for(int jndex = 0; jndex < sig_files.size(); jndex ++){
			string contourcut = sig_files[jndex]->getStageCuts_v2(5, cuts, 0);
			signal += sig_files[jndex]->GetEntries(contourcut.c_str())*sig_files[jndex]->target_pot/sig_files[jndex]->pot;//13.2e20 POT is set in bdt_file.h
//			cout<<"signal: "<<signal<<endl;
		}

		for(int jndex = 0; jndex < bkg_files.size(); jndex ++){
			string contourcut = bkg_files[jndex]->getStageCuts_v2(5, cuts, 0);
			background += bkg_files[jndex]->GetEntries(contourcut.c_str())*bkg_files[jndex]->target_pot/bkg_files[jndex]->pot;//13.2e20 POT is set in bdt_file.h
//			cout<<"bkg: "<<background<<endl;
		}
		
		double significance = (background == 0)? 0:signal/sqrt(background);
		cout<<"Sig: "<<significance<<" at cuts "<<cuts<<endl;
		sig_hist->SetBinContent(index, significance);
	}
	}
	cout<<"\n Filling finished! The best sig "<<best_sig[0];
	cout<<" is found at "<< min_distance + (best_at[0]-1)* gap<<endl;

	cout<<"With Data-MC aggrement, the best sig "<<best_data;
	cout<<" is found at "<< min_distance + (best_data_at-1)* gap<<endl;

	TCanvas* a_canvas = new TCanvas();
	sig_hist->SetStats(false);
	string plot_name = "Contour Response (links to efficiencies from" + to_string_prec( factor, 2) + " to 0) ";
	sig_hist->GetXaxis()->SetTitle(plot_name.c_str());
	sig_hist->GetYaxis()->SetTitle("Significance [13.2e20 POT]");
	sig_hist->Draw("L");

	a_canvas->SaveAs("significance_reference.pdf","pdf");
	
}








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
			//	double tmin_cos = sig_files.at(i)->tvertex->GetMinimum( (sig_files.at(i)->getBDTVariable(cosmic_focused_bdt).name + ">0").c_str()    );//THIS DOES NOT WORK;
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

