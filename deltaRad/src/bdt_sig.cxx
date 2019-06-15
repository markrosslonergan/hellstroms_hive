#include "bdt_sig.h"
#include "TMath.h"
#include "TTreeIndex.h"//Use the TTreeIndex class
//#include "TMultiGraph.h"//draw markers
//#include "TF1.h"
#include "TF2.h"
//#include "assert.h"

#include <fstream>//read and write txt file.
#include <unistd.h>//this is for exit(0)
#include <sys/stat.h>
#include <numeric> //to use iota

using namespace std;

contour_struct_v2::contour_struct_v2( string const & tree_name) {
	tree = new TTree(tree_name.c_str(), "");
	tree->Branch("distance", &distance, "distance/D");
}
contour_struct_v2::~contour_struct_v2() {
	delete tree;
}

int shrinks_boundary(TH2D* sig_grid, int step, int fix_x, int fix_y, int const max_position ){
/*
	double temp=0;
	int best_mndex = (fix_x==0)? fix_y: fix_x;//this contrains the final spot along the diagonal.
	int mndex = 1;// track one of the coordinate in a coordinate pair 
	double reference;
	double fix_ref = sig_grid->GetBinContent( max_position, max_position );

	while(1){
		if(fix_x==0){//fix_x goes 0 means its a value determined through this loop.
			reference = sig_grid->GetBinContent( mndex , fix_y );
//			cout<<"with fixed y: searching "<<  mndex<<endl;
		}else if(fix_y == 0){
			reference =  sig_grid->GetBinContent( fix_x , mndex );
//			cout<<"with fixed x: searching "<<  mndex<<endl;
		}else{
			cout<<"Fail searching boundary. One of the coordinate needs to be fixed."<<endl;
			exit(EXIT_FAILURE);
		}

		int diagonal = (fix_x==0)? fix_y : fix_x;
		if( reference > fix_ref){//boundary is found, when a higher sig is found;
			return mndex;
		}else if( mndex >= diagonal ){//reach the edge!
			if(diagonal>best_mndex){
				return best_mndex;//best_mndex shrinks/expands as the searching proceeds;
			}else{
				return diagonal;//nothing can exceed the diagonal grids;
			}
		}

		if( temp < reference){//find a relative higher sig.? update it
			temp= reference;
			best_mndex = mndex;
		}
		mndex++;
	}

	return 0;
*/
}

void tailor_boundary (vector< vector <double> > & boundary){
	/*
	//Remove redundant points: those adjacent that are identical or inline by 3 points;
	vector< double > remove_index;
	for(int i = 0 ; i < boundary.size()-1; i++){//remove the identical
		if(boundary[i][0]==boundary[i+1][0] && boundary[i][1]==boundary[i+1][1]){
			remove_index.push_back(i+1);
//			cout<<"going to remove the "<<i+2<<"th element in the boundary;"<<endl;

		}else if(i+2<boundary.size()-1){
			if((boundary[i][0]==boundary[i+1][0] && boundary[i+1][0]==boundary[i+2][0])
					|| (boundary[i][1]==boundary[i+1][1] && boundary[i+1][1]==boundary[i+2][1])){
				//remove inline points;
				remove_index.push_back(i+1);
//				cout<<"going to remove the "<<i+2<<"th element in the boundary;"<<endl;
			}
		}
	}

	vector<vector<double>>::iterator remove_this;
	while(remove_index.size()>0){
		remove_this = boundary.begin()+remove_index.back();
		boundary.erase( remove_this, remove_this + 1 );//remove 1 element;
		remove_index.pop_back();
	}

	//now find the overlap points to determine the final contour;
	int temp_index = 0;
	int temp_indexb = 0;
	int temp_boundary_length = (boundary.size()-3)/2;
	for( int i = temp_boundary_length-1; i >=0; i--){
		for (int j = temp_boundary_length+3; j < boundary.size() ; j++){
			if(boundary[i][0] == boundary[j][0] && boundary[i][1] == boundary[j][1]){
				temp_index = i;
				temp_indexb = boundary.size()-1-j;
//				cout<<"Two identical elements: "<< i <<"th and the last "<<temp_indexb<<"th element;"<<endl;
				goto founded;
			}
		}
	}

founded:
	if(temp_index < 1 && temp_indexb < 1){
		cout<<"Not need to tailer the boundary, because the boundary is open." <<endl;
	}else{
		cout<<"Tailer the boundary by cutting "<<temp_index+temp_indexb<<" elements."<<endl;
		for(int i = 0; i<temp_indexb; i++){
			boundary.pop_back();
		}
		boundary.erase(boundary.begin(),boundary.begin()+temp_index);//clean up the first
	}

	if(boundary.size()<3){
		cout<<"WARNING: NO BOUNDARY is available! Required significance might be too high."<<endl;
		exit(EXIT_FAILURE);
	}
*/
}

void define_boundary_v2 (TH2D * sig_grid, TH2D * bkg_grid, vector<double> strictness){
/*
	if (access("contour_cut",F_OK) == -1){
		mkdir("contour_cut",0777);//Create a folder for boundary.txt output.
	}
	int dimension = sig_grid->GetDimension();
	cout<<"Grids have dimension: "<<dimension<<endl;//seems not right

	//directly use distance;
	Int_t max_sig_bin = sig_grid->GetMaximumBin();
	Int_t x,y,z;
	vector<double> focus1(2); 
	sig_grid->GetBinXYZ(max_sig_bin, x,y,z);
	cout<<y<<endl;
	focus1[0] = (sig_grid->GetXaxis()->GetBinLowEdge(x)+sig_grid->GetXaxis()->GetBinLowEdge(x+1))/2;
	focus1[1] = (sig_grid->GetYaxis()->GetBinLowEdge(y)+sig_grid->GetYaxis()->GetBinLowEdge(y+1))/2;

	//I Get the focus now..
	cout<<focus1[0]<<endl;



	exit(0);





	parabola test({3,4},{-3,-4});//c1 on the right;
	
	double cos[] = {3,0.3,0.4,0.1,0.2};
	double bnb[] = {4,0.3,0.4,0.1,0.2};
	
	cout<<test.get_rotation()<<endl;
	cout<<test.get_focus()<<endl;
	vector< double* > something = {cos,bnb};
//	something = {cos, bnb};
	int count = sizeof(cos)/sizeof(double);
	test.search_directrix(something, count);
	cout<<"THIS directrix_std: "<<test.directrix_std<<endl;
	
	cout<<test.draw_std_conic()<<endl;
	cout<<test.draw_gen_conic()<<endl;
	cout<<test.draw_gen_line()<<endl;
	cout<<test.draw_std_line()<<endl;
	cout<<test.get_eccentricity({3,5})<<endl;
	
	//Draw
	if(false){
	TCanvas* a_canvas = new TCanvas("Signal/Bkg Ratio"," ",2000,1600);
	TF2 f1("f1", test.draw_std_conic().c_str(),0,10,0,10);
//	TF2 f1("f1", test.draw_gen_conic().c_str(),0,10,0,10);
	f1.Draw();
	a_canvas->SaveAs("testing_function.pdf","pdf");
	}

	exit(0);
	//Remember to weight events when calculating significance.
	//prepare Canvas;
*/
}


//Start to find boundary here;
void define_boundary (TH2D * sig_grid, int step, vector<double> strictness){
/*	//strictness from 0 to 1 means easy to strict.
	double hard_cut_off = 10;//the target maximum significance cannot exceed this number.

		if (access("contour_cut",F_OK) == -1){
			mkdir("contour_cut",0777);//Create a folder for pdf.
		}

	TCanvas* a_canvas = new TCanvas("Signal/Bkg Ratio"," ",2000,1600);
	sig_grid->SetStats(false);
	sig_grid->Draw("colz");
	sig_grid->SetTitle("Regional Significance 2D Histogram");
	sig_grid->GetXaxis()->SetTitle("Cosmic BDT Score");
	sig_grid->GetYaxis()->SetTitle("BNB BDT Score");

	TLegend *legend = new TLegend();//0.1,0.7,0.48,0.9);
//	TMultiGraph * markers = new TMultiGraph();
	vector<TGraph*> temp_marker(strictness.size()) ;// = new TGraph(boundary.size(), px, py);
	for(int index = 0; index < strictness.size(); index++){//do this for different strictness;
		cout<<"\nDefining the boundary at tied "<<strictness[index]<<endl;
	//CHECK

		double max_sig = 0;
		int max_x, max_y;
		for(int jndex = 1; jndex <= step; jndex++){//each value fixed a column of the 2D histgram

			for(int jndex2 = 1; jndex2 <= step; jndex2++){//each value fixed a row

			cout.precision(3);
//			cout<<"\r Evaluating grids for max sig.: "<<jndex<<" "<<jndex2;
			cout.flush();

				double temp = sig_grid->GetBinContent(jndex, jndex2);
				if(max_sig<temp && temp<hard_cut_off){//centering reasonable max_sig; dont want pure signal grid
					max_sig = temp;
					max_x=jndex;
					max_y=jndex2;
					cout<<"\rMaximum regional significance is "<<max_sig<<" at "<<max_x<<" "<<max_y;
					cout.flush();
					}
			}
		}
		cout<<endl;
		//now find the boundary max_sig*strictness[i];

		vector< vector<double> > boundary;
		for(int jndex = 1; jndex <= step; jndex ++){//Search targeted significance diagonally until we locate it
			double temp2 = sig_grid->GetBinContent(jndex, jndex);

			cout.precision(3);
			cout<<"\r Evaluating grids for boundaries: "<<jndex<<" "<<jndex;
			cout.flush();
			cout<<"local significance "<< temp2<<", compared to the boundary requirement: " << max_sig*strictness[index];

			if(jndex == step){
				cout<<"Warning: CANT FIND target significance along the diagonal?? How about relax the cut?"<<endl;
				cout<<strictness[index]<<" of significance will results in a small reginal selection.\n"<<endl;
//				exit(EXIT_FAILURE);
//				CHECK, need a action to deal with this situation;
			}else if(temp2>max_sig*strictness[index]){//locate where the target bounrady starts.

			cout<< "\nLocate the first mark of boundary, with sig = "<<temp2<<" at "<<jndex<<" " <<jndex<<endl;

				//Prepare some tedious values
				double xmin = sig_grid->GetXaxis()->GetBinLowEdge(1);
				double ymin = sig_grid->GetYaxis()->GetBinLowEdge(1);
				double dx = sig_grid->GetXaxis()->GetBinWidth(1);
				double dy = sig_grid->GetYaxis()->GetBinWidth(1);
//				cout<<"CHECK"<<xmin<<" and dx "<<dx<<endl;
				
				int boundary_half_length=2*(step-jndex + 1);
				
				vector<vector<double> > default_boundary(boundary_half_length ,{xmin,ymin});
				boundary = default_boundary;//one side
				boundary.push_back({xmin+dx*(jndex-1),ymin+dy*(jndex)});//center to top
				boundary.push_back({xmin+dx*(jndex-1),ymin+dy*(jndex-1)});//center
				boundary.push_back({xmin+dx*(jndex),ymin+dy*(jndex-1)});//center to right
				//save the location of the best local significance in the sig.txt again;
				std::ofstream save_sig("sig_best_local.txt");
				save_sig<< xmin+dx*(jndex-1) <<" "<<ymin+dy*(jndex-1)<<std::endl;
				save_sig.close();

				boundary.insert(boundary.end(), default_boundary.begin(), default_boundary.end() );//another side

				//e.g.	0< 6 - 5 + 1 gives 0,1
				for (int kndex = 0; 2*kndex < boundary_half_length; kndex++){//work on the top most; boundaries.
					//e.g. (4,4) where max are (5,5), edge at (6,6)
					//do (n,6), n from 1 to 5; (n,5)...
					//do (n,5), n from 1 to 5; (n,5)...

					int temp_y = (step-kndex+1);
					int temp_x = shrinks_boundary(sig_grid, step, 0, temp_y , jndex);//fixing y, varying x.
//					cout<<"Shrink rows   "<<temp_y<<" landed on "<<temp_x<<" "<<temp_y<<"("<<xmin+dx*(temp_x-1)<<","<<ymin+dy*(temp_y - 1)<<endl;
					boundary[2*kndex+1] = { xmin+dx*(temp_x-1) , ymin+dy*(temp_y - 1)};//head of the line
					boundary[2*kndex] = { xmin+dx*(temp_x-1) , ymin+dy*(temp_y)};//add one to the top;
//					cout<<"CHECK THIS! "<<boundary[kndex][0]<<" "<<boundary[kndex][1]<<endl;

					//do (5,n), n from 1 to 5; (n,5)...
					//do (6,n), n from 1 to 5; (n,5)...

					temp_x = jndex+kndex+1;
					temp_y = shrinks_boundary(sig_grid, step, temp_x, 0 , jndex);//fixing x, varying y.
//					cout<<"Shrink columns "<<temp_x<<" landed on "<<temp_x<<" "<<temp_y<<endl;
					//+2 for 3 additional elements at the center.
					boundary[boundary_half_length+2 +2*kndex+1] = { xmin+dx*(temp_x-1), ymin+dy*(temp_y-1) };//tail of the line
					boundary[boundary_half_length+2 +2*kndex+2] = { xmin+dx*(temp_x), ymin+dy*(temp_y-1) };//add one to the right;

//					cout<<"Boundary has dimension: "<<boundary.size()<<endl;
					//move on next coordiante pair in the boundary vector
				}

//				boundary[0] = {boundary[1][0],boundary[1][1]+dy};//correct the edges of lines
//				boundary.back() = {boundary[boundary.size()-2][0]+dx,boundary[boundary.size()-2][1]};//correct the edges of lines

				break;
			}
		}
		//Add a piece of code to tailer boundaries
		tailor_boundary(boundary);


		//this shrinks horizontally;
//		cout<<"Boundary really has dimension: "<<boundary.size()<<endl;
		double px[boundary.size()], py[boundary.size()];
		for(int i = 0; i<boundary.size(); i++){
			px[i]=boundary[i][0];
			py[i]=boundary[i][1];
//			cout<<"Feed x :"<<px[i]<<endl;
		
		}
		temp_marker[index] = new TGraph(boundary.size(), px, py);
		temp_marker[index]->SetTitle((to_string(strictness[index])+"sig").c_str());
		temp_marker[index]->SetMarkerStyle(index+21);
		temp_marker[index]->SetMarkerSize(2);
		temp_marker[index]->SetMarkerColor(40+index);
		temp_marker[index]->SetLineColor(40+index);
		temp_marker[index]->Draw("same lp");

		//Save bdt_info.
		string txt_name = "./contour_cut/curve_selection"+to_string_prec(strictness.at(index),4)+".txt";
		ofstream curve_selection(txt_name);
		curve_selection.close();
		curve_selection.open(txt_name, ios::app);

		for( int kndex = 0 ; kndex < boundary.size(); kndex++){
			curve_selection<<boundary[kndex][0]<<" "<<boundary[kndex][1]<<endl;
//			cout<<"Recording line "<<kndex<<" with "<<boundary[kndex][0]<<endl;
		}

		curve_selection.close();
		cout<<"Curve info is saved to "<<txt_name<<"\n"<<endl;
		//Save bdt_info. Finish!

		legend->AddEntry(temp_marker[index],(to_string_prec(strictness[index],4)+" of the best significance").c_str(),"p");
	}
	legend->Draw();
	a_canvas->Update();
//	a_canvas->BuildLegend();
	a_canvas->SaveAs("Regional_Significance.pdf","pdf");
	*/
}


TH2D *event_grid (vector<bdt_file*> files, vector<bdt_info> bdt_infos, string hist_name, int step){
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
			cout.precision(3);
			cout<<"\r Filling grids: "<<(di-1)*step+di2<<" of "<< step*step << " complete.";
			cout.flush();
//			cout<<" Finish "<<di<<" "<<di2<<" with "<<events_rate<<endl;
			//CHECK 
		}
	}
	
	TCanvas* a_canvas = new TCanvas(files[0]->tag.c_str()," ",2000,1600);
	finished_grid->SetStats(false);
	finished_grid->Draw("colz");
	finished_grid->GetXaxis()->SetTitle("Cosmic BDT Responses");
	finished_grid->GetYaxis()->SetTitle("BNB BDT Responses");
//	a_canvas->Write();//I dont need to write~
//	a_canvas->Update();
	a_canvas->SaveAs((hist_name+".pdf").c_str(),"pdf");

	cout<<"\nFinish producing 2D event grid for "<<hist_name<<endl;
	return finished_grid;
}

//THIs is new, use this to evaluate the "cut_info", possible cuts for signal and associated eff.;
vector<double> evaluate_events(bdt_file* file, int entries,  int * row, double * cos, double* bnb, vector<double> focus1, vector<double> focus2, bool draw){

	TFile * contour_cut = TFile::Open((file->tag+"_contour_v3.root").c_str(), "recreate");

	int index_tracker = 0;
	Int_t num_events = file->tvertex->GetEntries();

	//make more trees!!
	string friend_tree_name = file->tag;//+strict_name;
	contour_struct_v2 contour_info(friend_tree_name);
	cout<<"	Evaluating distance of each entries in file "<<file->tag<<endl;
	vector<double> signal_distances;
	for( int index = 0; index < num_events ; index++){
					
		contour_info.distance = -999;

		if(row[index_tracker] == index){//an entry that we want to use, which has positive responses
			
			 double temp_dis = sqrt(pow(cos[index_tracker]-focus2[0],2)+pow(bnb[index_tracker]-focus2[1],2))
				-sqrt(pow(cos[index_tracker]-focus1[0],2)+pow(bnb[index_tracker]-focus1[1],2));
			contour_info.distance = temp_dis;
			if(draw) signal_distances.push_back(temp_dis);
//			cout<<"Ding "<<contour_info.distance<<endl;
			index_tracker++;
		}
		contour_info.tree->Fill();
	}
	contour_info.tree->Write();


//	contour_cut->Close();
	cout<<"	Finish recording contour_info for the file "<<file->tag<<endl;
	if(draw){
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

	return signal_distances;
}
//
//TGraph* BDT_scatter ( TLegend* legend, int index, int num_events, double* bnb, double* cos, bool draw){
//
//	TGraph* temp = new TGraph (num_events, cos, bnb); //TGraph(Int_t , Double_t *, Double_t *);
//	//add some transparency
//	temp->SetMarkerColorAlpha(index+2,0.5);//see https://root.cern.ch/doc/master/classTAttMarker.html#M2
//	temp->SetMarkerStyle(1);//(number_of_files-index)/number_of_files+1);//2 or 1.
//
//	legend->AddEntry(contents.at(index),files.at(index)->tag.c_str(),"p");
//	return temp;
//}

vector< double> get_max (TH2D* grid){
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

//OK,, this is an experimental function
void select_events_v2 (vector<bdt_file*> sig_files, vector<bdt_file*> bkg_files, bdt_info cosmic_focused_bdt, bdt_info bnb_focused_bdt, vector<double> efficiencies, int step){
	
	double tmin_cos = 99 , tmin_bnb = 99 , tmax_cos = 0, tmax_bnb = 0;
	double temp_tmin_cos = 0 , temp_tmin_bnb = 0 , temp_tmax_cos = 0, temp_tmax_bnb = 0;
	double more = 0.05;//apply this value to push the limit of responses shown on axies.

	vector< string > saved_name = {"signal_bkg_events.root","dimension", "Signal_events_rate", "Background_events_rate_SquareRoot"};

	cout<<"Finding the extremum of responses: ";

	vector<bdt_file*> files;//Concate signal file and bkg files into a vector.
	files.reserve(sig_files.size()+bkg_files.size());
	files.insert( files.end(), sig_files.begin(), sig_files.end() );
	files.insert( files.end(), bkg_files.begin(), bkg_files.end() );
	

	for(int index = 0; index<files.size(); index++){//get the extremum BDT info by creating a histogram and extract the extremum from the histogram;
		bdt_file* temp_file = files[index];
		temp_file->calcBNBBDTEntryList(0,0);
		temp_file->setStageEntryList(3);//apply cos and BNB BDT cuts on 0;

//		bdt_variable cos_var = 	temp_file->getBDTVariable(cosmic_focused_bdt);
//		TH1* temp_hist = temp_file->getTH1(cos_var, "1", cos_var.safe_name,1.0); 
//
//		int temp_bin = temp_hist->GetMinimumBin();
		temp_tmin_cos = temp_file->tvertex->GetMinimum( temp_file->getBDTVariable(cosmic_focused_bdt).name.c_str());
//		temp_bin = temp_hist->GetMaximumBin()+1;
		temp_tmax_cos = temp_file->tvertex->GetMaximum( temp_file->getBDTVariable(cosmic_focused_bdt).name.c_str());

//
//		bdt_variable bnb_var = 	temp_file->getBDTVariable(bnb_focused_bdt);
//		temp_hist = temp_file->getTH1(bnb_var, "1", bnb_var.safe_name,1.0); 
//
//		temp_bin = temp_hist->GetMinimumBin();
		temp_tmin_bnb = temp_file->tvertex->GetMinimum( temp_file->getBDTVariable(bnb_focused_bdt).name.c_str() );
		temp_tmax_bnb = temp_file->tvertex->GetMaximum( temp_file->getBDTVariable(bnb_focused_bdt).name.c_str() );
//		temp_bin = temp_hist->GetMaximumBin()+1;
//
//		delete temp_hist;
//		delete temp_file;
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

	cout<<"Range of Responses: Cosmic:"<<tmin_cos<<" "<<tmax_cos<<" BNB:"<<tmin_bnb<<" "<<tmax_bnb<<endl;
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
		signal_grid = event_grid ({sig_files[0]}, {cosmic_focused_bdt, bnb_focused_bdt}, saved_name[2], step);//this function maps events into the 2d grid.
		bkg_grid    = event_grid (bkg_files, {cosmic_focused_bdt, bnb_focused_bdt}, saved_name[3], step);//this function maps events into the 2d grid.

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
	vector<TGraph*> contents(files.size());//SCATTER

	bool quick_plot = false;
	vector<double> focus1(2), focus2(2);
	if(true){//figuring out the boundary;
		
		focus1 = get_max( signal_grid);
		cout<<"Got signal peak above; see the following around bkg peak;"<<endl;
		focus2 = get_max( bkg_grid);
		focus1[0]+=0.028;
		focus1[1]+=0.015;
		focus2[0]+=0.142;
		focus2[1]-=0.031;
		//CHECK!!
		cout<<"\nTwo FOCI:"<<endl;
		cout<<"For signal: "<<focus1[0]<<","<<focus1[1]<<endl;
		cout<<"For bkg   : "<<focus2[0]<<","<<focus2[1]<<endl;


		//I Get the foci now..
		//cout<<focus1[0]<<endl;
		
		//prepare TF2 objects for showing boundary;
		for(int index = 0; index<files.size(); index++){//Construct root file for each sample.

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
			
			vector< double > cut_info = evaluate_events(files.at(index), num_events, row, cos, bnb, focus1, focus2, index == 0);//calculate raw_number with different efficiencies;
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
	TH2D *responses_scattering = new TH2D("dummyhist", "BDT Scores", 60, tmin_cos, tmax_cos, 60, tmin_bnb, tmax_bnb);//To be updated, CHECK
	responses_scattering->SetStats(false);
	responses_scattering->GetXaxis()->SetTitle("Cosmic BDT Score");
	responses_scattering->GetYaxis()->SetTitle("BNB BDT Score");
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
	a_canvas3->SaveAs("BDTscatter.pdf","pdf");
	}else{
		cout<<"THIS DOES NOT CHANGE THE ROOT FILES!"<<endl;
//		exit(EXIT_FAILURE);
	}
	signal_bkg_events->Close();

}

//the main function
void select_events (vector<bdt_file*> sig_files, vector<bdt_file*> bkg_files, bdt_info cosmic_focused_bdt, bdt_info bnb_focused_bdt, vector<double> percent_sig, int step){
/*
//Associated with define_boundary and shrinks_boundary functions; but not used,
	double tmin_cos = 99 , tmin_bnb = 99 , tmax_cos = 0, tmax_bnb = 0;
	double temp_tmin_cos = 0 , temp_tmin_bnb = 0 , temp_tmax_cos = 0, temp_tmax_bnb = 0;
	double more = 0.05;//apply this value to push the limit of responses shown on axies.

	vector< string > saved_name = {"signal_bkg_events.root","dimension", "Signal_events_rate", "Background_events_rate_SquareRoot"};

	cout<<"Finding the extremum of responses: ";

	vector<bdt_file*> files;//Concate signal file and bkg files into a vector.
	files.reserve(sig_files.size()+bkg_files.size());
	files.insert( files.end(), sig_files.begin(), sig_files.end() );
	files.insert( files.end(), bkg_files.begin(), bkg_files.end() );
	

	for(int index = 0; index<files.size(); index++){//get the extremum BDT info by creating a histogram and extract the extremum from the histogram;
		bdt_variable cos_var = 	files.at(index)->getBDTVariable(cosmic_focused_bdt);
		TH1* temp_hist = files[index]->getTH1(cos_var, "1", cos_var.safe_name,1.0); 

		int temp_bin = temp_hist->GetMinimumBin();
		temp_tmin_cos = temp_hist->GetXaxis()->GetBinLowEdge(temp_bin);
		temp_bin = temp_hist->GetMaximumBin()+1;
		temp_tmax_cos = temp_hist->GetXaxis()->GetBinLowEdge(temp_bin);

		delete temp_hist;

		bdt_variable bnb_var = 	files.at(index)->getBDTVariable(bnb_focused_bdt);
		temp_hist = files[index]->getTH1(bnb_var, "1", bnb_var.safe_name,1.0); 

		temp_bin = temp_hist->GetMinimumBin();
		temp_tmin_bnb = temp_hist->GetXaxis()->GetBinLowEdge(temp_bin);
		temp_bin = temp_hist->GetMaximumBin()+1;
		temp_tmax_bnb = temp_hist->GetXaxis()->GetBinLowEdge(temp_bin);

		delete temp_hist;

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

	cout<<"Range of Responses: Cosmic:"<<tmin_cos<<" "<<tmax_cos<<" BNB:"<<tmin_bnb<<" "<<tmax_bnb<<endl;


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

		signal_grid = event_grid (sig_files, {cosmic_focused_bdt, bnb_focused_bdt}, saved_name[2], step);//this function maps events into the 2d grid.
		bkg_grid    = event_grid (bkg_files, {cosmic_focused_bdt, bnb_focused_bdt}, saved_name[3], step);//this function maps events into the 2d grid.

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

	TH2D* significance_grid(signal_grid);
	significance_grid->Add(signal_grid,bkg_grid,40,-1);//signal over background!

//	define_boundary (significance_grid, step, percent_sig);//Find appropriate boundries
	TCanvas* a_canvas = new TCanvas(files[0]->tag.c_str()," ",2000,1600);
	significance_grid->SetStats(false);
	significance_grid->Draw("colz");
	significance_grid->GetXaxis()->SetTitle("Cosmic BDT Responses");
	significance_grid->GetYaxis()->SetTitle("BNB BDT Responses");

	a_canvas->SaveAs("testing.pdf","pdf");

	signal_bkg_events->Close();
*/
}


void significance_eff(vector<bdt_file*> sig_files, vector<bdt_file*> bkg_files, bdt_info contour_bdt_info){
	//Load the limits
	//CHECK could add a function for searching the maximum sig. here; or a PDF.
	

	double min_distance = 0;
////code for reading the min_distance;	
//	if (access("cut_eff.txt",F_OK) == -1){//no file
//		std::cout<<"Warning: No cut_eff.txt is found in the current directory. Please run select_events_v2() under -o sig."<<std::endl;
//	}
//	else{
//		std::cout<<"Read lower limit value of the 'distance' variable in signal_contour.root file"<<std::endl;
//
//		std::fstream dis_file("cut_eff.txt", std::ios_base::in);
//		string trash, trash1;
//		dis_file >> trash >> trash1; 
//		dis_file >> min_distance;
//	}//min_distance is read

	double max_distance = sig_files[0]->tvertex->GetMaximum( sig_files[0]->getBDTVariable_contour(contour_bdt_info).name.c_str());
	 min_distance = max_distance * 0.8;

	//the limits are loaded
	string hist_title =  "significance versus signal efficiency";
	int num_bins = 20;
	TH1F* sig_hist = new TH1F("sig_eff", hist_title.c_str(), num_bins, min_distance, max_distance);
	
	double cuts;
	double gap = (max_distance-min_distance)/num_bins;
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
	cout<<"\n Filling finished!"<<endl;

	TCanvas* a_canvas = new TCanvas();
	sig_hist->SetStats(false);
	sig_hist->GetXaxis()->SetTitle("Cut Value (Efficiency from 0.2 to 0) ");
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

/*std::vector<double> lin_scan(std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files, bdt_info cosmic_focused_bdt, bdt_info bnb_focused_bdt, double c1, double c2){
	cout<<"GO AND CHECK sig.cxx\n\n\n\n\n"<<endl;
		std::cout<<"Starting to Scan Significance"<<std::endl;
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
}
	*/

