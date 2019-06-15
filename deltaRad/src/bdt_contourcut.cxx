#include "bdt_contourcut.h"

#include <fstream>//read and write txt file.
#include <unistd.h>//this is for exit(0)
#include <sys/stat.h>
#include <numeric> //to use iota

#include <assert.h>

using namespace std;

contour_struct::contour_struct( string const & tree_name) {
	tree = new TTree(tree_name.c_str(), "");
	tree->Branch("distance", &distance, "distance/D");
}
contour_struct::~contour_struct() {
	delete tree;
}

bool inside_contour( vector < double > bdt_responses , vector< vector<double> > boundary ){
	//This follows the solution for the problem "point in polygon":
	//1. It sends out a ray from a given "bdt_responses" point along the negative cosbdtresponse direction;
	//2. calculate how many times the ray penetrate the boundary;
	//2.5 with the consideration of the special case that the ray and the boundar is inline;
	//3. the number of penetration tells if the point inside the contour.

	double bdt_step = 999;
	for (int i = 0; i < boundary.size()-1; i ++){//determine the unit along x-axis; boundary[i][0];
		double temp_step = boundary[i+1][0]-boundary[i][0];
		if(temp_step > 0 && temp_step < bdt_step) bdt_step = temp_step;
	}

	//	cout<<"BDT Responses along the first coordinate has unit of "<<bdt_step<<endl;//bdt_step can be calculated.


	//find possible intersection point by drawing a ray along first coordiante negative direction.
	int intersection = 0;
	double temp_responses = bdt_responses[0];
	while(temp_responses>0){//count possible intersections
		temp_responses-=bdt_step;
		intersection++;

	}
//	cout<<"\nFind "<<intersection<<" possible intersecrtions."<<endl;

	int penetration = 0;//how many times a ray from bdt_responses penetrates the boundary.
	while(intersection > 0){//test if intersect; if it does, penetration++;

		double candidate_x = bdt_step*(intersection-1);
		vector<int> boundary_index;
//		cout<<"candidate: "<<candidate_x<<endl;
		for(int i = 0 ; i < boundary.size()-1; i++ ){//Find the possible lines (multiple points) on the boundary;
			if(abs(candidate_x - boundary[i][0])<0.001 && abs(candidate_x - boundary[i+1][0])<0.001){
//				cout<<"Push"<<endl;
				boundary_index.push_back(i);//record boundaries points that share the same x coordinate;
				boundary_index.push_back(i+1);

			}else if(boundary[i][1]==boundary[i+1][1] && bdt_responses[1] - boundary[i][1] < 1e-26
				&& (boundary[i][0] - bdt_responses[0])*(boundary[i+1][0] - bdt_responses[0]) < 0 ){//the point is on the boundary! Take it!
//				cout<<"The event is on the boundary, take it!"<<endl;
				return true;
			}
		}

//		cout<<"Looking at "<<candidate_x<<", "<<bdt_responses[1]<<"; Boundary grids "<<boundary_index.size()/2<<endl;
		for(int j = 0 ; 2*j < boundary_index.size(); j++){//test all possible lines; they share the same x coordinate;
//			cout<<"Testing point between ("<<boundary[boundary_index[2*j]][0]<<","<<boundary[boundary_index[2*j]][1];
//			cout<<") and ("<<boundary[boundary_index[2*j+1]][0]<<","<<boundary[boundary_index[2*j+1]][1]<<")"<<endl;

			double evaluation = (boundary[boundary_index[2*j] ][1] - bdt_responses[1])*(boundary[boundary_index[2*j+1] ][1] - bdt_responses[1]);

			if(evaluation < 1e-26 ){//when it is outside the interval btw two points, the value is positive.
//				cout<<"The ray penetrates the boundary. "<<evaluation<<endl;
				penetration++;
				
				if (evaluation > -1e-26 && boundary.size()>boundary_index[2*j]+3){//almost 0, then the ray is possibly inline
//					cout<<"The ray is possibly inline with the boundary!"<<endl;
					double grid1 = boundary[boundary_index[2*j]   ][1];
					double grid2 = boundary[boundary_index[2*j]+1 ][1];
					double grid3 = boundary[boundary_index[2*j]+2 ][1];
					double grid4 = boundary[boundary_index[2*j]+3 ][1];
//					cout<<"Grid: "<<grid1<<"Grid: "<<grid2<<"Grid: "<<grid3<<"Grid: "<<grid4<<endl;
					if(grid2 == grid3 && (grid1-grid2)*(grid3-grid4)>0){//such pattern goes like "n" with left/right half upside down
//					cout<<"The ray penetrates the boundary."<<endl;
					penetration++;
					}
				}
			}

		}

		intersection--;
	}

//	cout<<"Result: the number of penetration is "<<penetration<<endl;
	if( penetration%2==1 ){//odd inside the contour.
		return true;
	}else if( penetration%2==0 ){//even, outside the contour.
		return false;
	}
}



vector<int> select_events(bdt_file* file, int entries,  int * row, double * cos, double* bnb, vector<double> strictness ){
	
	//CHECK strictness, cut events with only one boundary.
	//if (strictness.size()>1){
	//	cerr<<"Please select one strictness to define the boundaries, which can be found at the begining of the main code as  'vector< double > strictness'."<<endl;
	//	exit(EXIT_FAILURE);
	//}

	vector<int> sum_in_contour;
	
	TFile * contour_cut;
	contour_cut = TFile::Open((file->tag+"_contour_v2.root").c_str(), "recreate");
	for(int index = 0; index < strictness.size(); index++){
		string strict_name =to_string_prec(strictness.at(index),4);
		string txt_name = "./contour_cut/curve_selection" + strict_name +".txt";
		cout<<"\n	Loading boundary info. from file: "<<txt_name<<" for the sample "<< file->tag <<endl;


		ifstream input(txt_name);

		vector< vector<double> > boundary;

		if(input){
			vector<double> temp_responses(2);
			while( input >> temp_responses[0]>>temp_responses[1]){

				boundary.push_back(temp_responses);//responses storage dual BDT info.
			}
		}else{
			//CHECK boundary infomation;
			cerr<<txt_name<<" does not exist; boundary information is not found. ";
			cerr<<"Please run -o sig to define the boundary."<<endl;
			exit(EXIT_FAILURE);
		}
		input.close();
		//boundary is loaded;

		double cos_max = 0;
		double bnb_max = 0;

		if (access("sig_best_local.txt",F_OK) == -1){//no file
			std::cout<<"Warning: No sig_best_local.txt is found in the current directory. Need to do -o sig first."<<endl; 
			exit(EXIT_FAILURE);
		}
		else{
			std::cout<<"	Read off significance from sig.txt for coscut & bnbcut. ";

			std::fstream best_responses("sig_best_local.txt", std::ios_base::in);

			best_responses >> cos_max >> bnb_max;//these are what we want
			std::cout<<"The best sig is found at resposes : (cos , bnb)= "<<cos_max <<", "<<bnb_max<<std::endl;
		}
		//best respones are loaded;

		int index_tracker = 0;
		Int_t num_events = file->tvertex->GetEntries();
		
		//make more trees!!
		string friend_tree_name = file->tag+strict_name;
		contour_struct contour_info(friend_tree_name);
		int in_contour=0;
		int out_contour=0;
		cout<<"	Evaluating each entries in file "<<file->tag<<endl;
		for( int jndex = 0; jndex < num_events ; jndex++){
			if(jndex%(num_events/100)==0){//indicator
				cout.precision(3);
				cout<<"\r	"<<100*jndex/num_events+1<<"\% of "<< num_events << " entries complete. ";
				cout<<"	Report "<<in_contour<<" inside the contour; "<<out_contour<<" outside the contour.";
				cout.flush();
			}			
			//assert(file->tvertex->GetEntry(jndex));
			contour_info.distance = 999;
			//				assert(row[index_tracker]);//fails at 1205?

			if(row[index_tracker] == jndex){//an entry that we want to use, which has positive responses
//				cout<<"\n locate the entry: "<<index_tracker << "cos: "<<cos[index_tracker]<<" bnb: "<<bnb[index_tracker]<<endl;

				if(inside_contour( {cos[index_tracker], bnb[index_tracker]} , boundary)){//INSIDE THE CNOTOUR!
					//Calculate the distance.
					contour_info.distance = sqrt(pow(cos[index_tracker]-cos_max,2)+pow(bnb[index_tracker]-bnb_max,2));
//					cout<<"\nDistance is updated as "<<contour_info.distance<<" for ("<<cos[index_tracker]<<","<<bnb[index_tracker]<<")"<<endl;
					in_contour++;
				}else{
					out_contour++;
				}
				index_tracker++;
			}
			contour_info.tree->Fill();
		}
		//summary
		if(false){
			cout<<"	List of events selected: with strictness "<<strictness[index]<<" in file "<<file->tag<<endl;
			for(int i = 0; i<index_tracker; i++){
				cout<<row[i] << "cos: "<<cos[i]<<" bnb: "<<bnb[i]<<endl;
			}
		}


		contour_info.tree->Write();
		cout<<"\n	Finish adding a root file for strictness: "<<strictness[index]<<endl;
		sum_in_contour.push_back(in_contour);

	
	}//cant Open and Close TFile inside this scope, idk why~ but solved by taking them out;

	contour_cut->Close();
	cout<<"	Finish recording contour_info for the file "<<file->tag<<endl;
	
	return sum_in_contour;
}



vector<int> index_reader( string str){//read of a single column (double) txt file.
	ifstream input(str);

	vector<int> content,reverse_content;
	int temp_content;//put the first column into trash
	double trash;
	if(input){
		while(input>>temp_content>>trash>>trash ){
			content.push_back(temp_content);
		}
	}else{
		cerr<< str <<" is not found"<<endl;
		exit(EXIT_FAILURE);
	}
	int repeating = content.size();

	for (int i=0 ; i < repeating; i++){
		reverse_content.push_back(content.back());
		content.pop_back();
	}
	input.close();

	return reverse_content;
}

//Main code;
void contour_selection(std::vector<bdt_file*> files, bdt_info cosmic_focused_bdt, bdt_info bnb_focused_bdt, double c1, double c2, bool make_cut, vector<double> strictness){//c1 is cos BDT cut; c2 is BNB BDT cut.
	//Setting for scattering plot:
	double min_cos, max_cos, min_bnb, max_bnb;

	if (access("sig_limits.txt",F_OK) == -1){//no file
		std::cout<<"Warning: No sig_limits.txt is found in the current directory. Need to do -o sig first.Or the limits are taken from 0 to 1;"<<endl;
		min_cos = 0;
		max_cos = 1;
		min_bnb = 0;
		max_bnb = 1;
	}
	else{
		std::cout<<"	Read off limits on axies from sig_limits.txt for cos & bnb responses. ";

		std::fstream best_responses("sig_limits.txt", std::ios_base::in);

		best_responses >> min_cos >> max_cos;
		best_responses >> min_bnb >> max_bnb;
	}

	if(true){//set it true for maximum efficiency, i.e. allow more events
		c1 = -999;
		c2 = -999;
	}

	int number_of_files = files.size();

	cout<<"Working on "<< number_of_files<<" files."<<endl;

	vector< vector<string> > BDTfiles(number_of_files , vector<string> (2,""));//app.root files with BDT responses.
	vector< vector<string> > new_var(number_of_files , vector<string> (2,""));//TTree in app.root.

	for(int index = 0; index < number_of_files; index++){//0 for cosmic, 1 for bnb!
		BDTfiles[index][0]+=cosmic_focused_bdt.identifier+"_"+files[index]->tag+"_app"+".root";
		cout<<BDTfiles[index][0]<<endl;//CLEAN
		BDTfiles[index][1]+=bnb_focused_bdt.identifier+"_"+files[index]->tag+"_app"+".root";

		new_var[index][0]+=files[index]->tag+"_"+cosmic_focused_bdt.identifier;
		cout<<new_var[index][0]<<endl;//CLEAN
		new_var[index][1]+=files[index]->tag+"_"+bnb_focused_bdt.identifier;
	}

	vector<string> new_tree = { "treecosmic", "treebnb"};

	if(true){//Record Entries and Produce BDT scatter plots here.
		if(!make_cut)cout<<"Doing a Scattering Plot Only! No cuts are made."<<endl;
		//Prepare Canvase for drawing:
		TCanvas * canvas = new TCanvas();
		//xaxis:Cosmic... yaxis:BNB
		TH2D *responses_scattering = new TH2D("dummyhist", "BDT Scores", 60, min_cos, max_cos, 60, min_bnb, max_bnb);//To be updated, CHECK

		responses_scattering->SetStats(false);
		responses_scattering->GetXaxis()->SetTitle("Cosmic BDT Score");
		responses_scattering->GetYaxis()->SetTitle("BNB BDT Score");
		responses_scattering->Draw();


		vector<TGraph*> contents(number_of_files);
		TLegend *legend = new TLegend(0.1,0.7,0.48,0.9);

		if (access("contour_cut",F_OK) == -1){
			mkdir("contour_cut",0777);//Create a folder for pdf.
		}
		//		vector<int> candidate_entries[number_of_files];//create pointers for indices that track BDT responses.
		vector< string > file_tag;//keep track of which file provides events;
		vector< vector< double >> signal_bkg_num(number_of_files, vector<double> (strictness.size(), 0 ));//normalized events number of signal or background;
		vector< vector< double >> efficiency(number_of_files, vector<double> (strictness.size(), 0 ));
		vector< vector<int>> raw_number(number_of_files, vector<int> (strictness.size(), 0 ));//{ file1, file2,..,} ; inside file={strictness1, strictness2};

//		for (int index = number_of_files-1; index >= 0 ; index--){//Loop over bkg1,bkg2 ... signal; such that signal are on top of everything;
		for (int index = 0; index < number_of_files; index++){//Loop over bkg1,bkg2 ... signal; such that signal are on top of everything;
			files.at(index)->tvertex->AddFriend((new_tree.at(0)+" = "+ new_var[index][0]).c_str() , BDTfiles[index][0].c_str() );
			files.at(index)->tvertex->AddFriend((new_tree.at(1)+" = "+ new_var[index][1]).c_str() , BDTfiles[index][1].c_str() );
			//Friend Trees are ready! Variables (.mva) can be called in the tree named "new_tree"

			//Feed in TGraph object with values.
			Int_t num_events = files.at(index)->tvertex->GetEntries();
			vector<int> temp_indices(num_events);//all indices, initialized in next line
			iota (std::begin(temp_indices), std::end(temp_indices), 0);//indices from 1 to num_events;

			cout<<"Get number of events at "<< files.at(index)->tag <<": "<<num_events<<endl;

			double *cos = new double[num_events];//Dynamically memory allocation!!
			//Thanks https://www.learncpp.com/cpp-tutorial/69-dynamic-memory-allocation-with-new-and-delete/
			double *bnb = new double[num_events];

			int *row = new int[num_events];

			Double_t weight=1;
			Double_t cosbdt,bnbbdt;//weight value to store and be used for checking lee_weights

			vector<int> temp_entries(num_events);
			Int_t count = 0;//for the scattering plot.

			iota (begin(temp_entries),end(temp_entries), 0);//fill vector with 0,1,2,...

			files.at(index)->tvertex->SetBranchAddress((new_tree.at(0)+".mva").c_str(),  &cosbdt);
			files.at(index)->tvertex->SetBranchAddress((new_tree.at(1)+".mva").c_str(),  &bnbbdt);

			for( int i = 0; i < num_events ;i++){//Selected good entries, i.e. with positive BDT responses.

				files.at(index)->tvertex->GetEntry(i);//this changes the value of weight, cosbdt, bnbbdt; local is the row# of the given index.

				if(cosbdt > c1 && bnbbdt > c2){
					if(make_cut) row[count] = i;

					cos[count] = cosbdt;
					bnb[count++] = bnbbdt;
				
				}
				temp_entries.pop_back();//delete the last entrty index, no matter what.
				if(i%(num_events/100)==0){
					cout.precision(3);
					cout<<"\r"<<100*i/num_events+1<<"\% of "<< num_events << " entries complete.";
					cout.flush();
				}
			}

			if(make_cut){//going through all strictness in one sample
				file_tag.push_back(files[index]->tag);
				double pot_scale = (files.at(index)->target_pot/files.at(index)->pot )*files.at(index)->scale_data;//target_pot is introduced in the bdt_file.h.
				//The following line relies on the boundary from -o sig;
				raw_number[index] = select_events(files.at(index), count, row, cos, bnb, strictness);//calculate raw_number with different strictness;
				//CHECK
				for(int i = 0; i < raw_number[index].size(); i++){
				cout<<"In file "<<files[index]->tag<<", number of selected events: "<<raw_number[index][i]*pot_scale<<endl;
				signal_bkg_num[index][i] = raw_number[index][i]*pot_scale;//works on all numbers;
				efficiency[index][i] = raw_number[index][i]/files[index]->GetEntries();
				}
			}

			contents.at(index) = new TGraph (num_events, cos, bnb); //TGraph(Int_t , Double_t *, Double_t *);
			contents.at(index)->SetMarkerColor(index+2);//see https://root.cern.ch/doc/master/classTAttMarker.html#M2
			contents.at(index)->SetMarkerStyle(1);//(number_of_files-index)/number_of_files+1);//2 or 1.
			contents.at(index)->Draw("P same");

			legend->AddEntry(contents.at(index),files.at(index)->tag.c_str(),"p");

			cout<<"Done, next!\n"<<endl;

			delete cos,bnb,row;//clear memory and assign them to NULL again.
			cos = 0, bnb = 0, row = 0;
		}

		if(make_cut){//make a report if contour cuts are made;
			//Calculate the real significance;
			//calculate events by different efficiencies in different samples
			vector<double> sig_num(strictness.size());
			vector<double> bkg_num(strictness.size());

			for(int i = 0; i < strictness.size(); i++){
			cout<<"------ Strictness: "<<strictness[i]<<endl;
				sig_num[i] = signal_bkg_num[0][i];//The last file calculated is signal file;
				for (int j = 0; j<signal_bkg_num.size() ; j++){
					if(j>0){
					bkg_num[i] += signal_bkg_num[j][i];
					}
					cout<<file_tag[j]<<" has normalized events: "<<signal_bkg_num[j][i];
					cout<<" with efficiency: "<<efficiency[j][i]<<endl;
				}
			cout<<"Signl file "<<file_tag[0]<<" has normalized events: "<<sig_num[i]<<endl;

			cout<<"=== Significance inside the contour is: "<<sig_num[i]/sqrt(bkg_num[i])<<endl;
			cout<<"===== Efficiency inside the contour is: "<<efficiency[0][i]<<endl;
			cout<<endl;
			}
			//make a summary statement.
		}

		//Fill in responses_scattering
		legend->Draw();

		canvas->Write();
		canvas->SaveAs("BDTscattering.pdf","pdf");//Do a scattering plot in the mid way~

		cout<<"The 2D histogram is created."<<endl;

	}



	// TO BE DUMPED!
	// TO BE DUMPED!
	// TO BE DUMPED!
	// TO BE DUMPED!
	if(false){//read out entries from txt files and record contour. calculating radius;

		cout<<"Loading contour info."<<endl;

		ifstream input("./contour_cut/"+files.at(0)->tag+".txt");

		vector< vector<double> > responses;
		double trash;

		if(input){
			vector<double> temp_responses(2);
			while( input >> trash >> temp_responses[0]>>temp_responses[1]){

				responses.push_back(temp_responses);//responses storage dual BDT info.
			}
		}else{
			cerr<<".txt for BDT reponses is not found"<<endl;
			exit(EXIT_FAILURE);
		}
		cout<<responses[0][0]<<" and "<< responses[0][1]<<endl;
		cout<<responses[1][0]<<" and "<< responses[1][1]<<endl;
		input.close();


		//Load up indices in sample_indices[?];
		vector< vector<int> > sample_indices(number_of_files);
		for (int index = number_of_files-1; index >= 0 ; index--){//Loop over bkg1,bkg2 ... signal
			sample_indices[index]= index_reader( ("./contour_cut/"+files.at(index)->tag+".txt").c_str());

			//Create root file to store info.
			cout<<"On file: "<<files.at(index)->tag<<endl;
			TFile * contour_cut = TFile::Open((files.at(index)->tag+"_contour.root").c_str(), "recreate");
			files.at(index)->tvertex->ResetBranchAddresses();

			string friend_tree_name = files.at(index)->tag;

			contour_struct contour_info(friend_tree_name);

			//equivalent to bdt_app_tree_struct;

			cout<< "Calculating contour radius for "<<files.at(index)->tag<<endl;

			files.at(index)->tvertex->AddFriend((new_tree.at(0)+" = "+ new_var[index][0]).c_str() , BDTfiles[index][0].c_str() );
			files.at(index)->tvertex->AddFriend((new_tree.at(1)+" = "+ new_var[index][1]).c_str() , BDTfiles[index][1].c_str() );

			Int_t num_events = files.at(index)->tvertex->GetEntries();

			Double_t cosbdt,bnbbdt;

			files.at(index)->tvertex->SetBranchAddress((new_tree.at(0)+".mva").c_str(),  &cosbdt);
			files.at(index)->tvertex->SetBranchAddress((new_tree.at(1)+".mva").c_str(),  &bnbbdt);

			int index_tracker = 0;
			double temp_radius;

			cout<<"# indices: "<<sample_indices[index].size()<<endl;

			if(sample_indices[index].size()>0){
				for( int i = 0; i < num_events ; i++){
					if(i%(num_events/100)==0){
						cout.precision(3);
						cout<<"\r"<<100*i/num_events+1<<"\% of "<< num_events << " entries complete.";
						cout.flush();
					}			
					files.at(index)->tvertex->GetEntry(i);

					contour_info.distance = 999;
					if(sample_indices[index][index_tracker] == i){//entry that we want to use
						index_tracker++;
						for (int jndex = 0; jndex < responses.size(); jndex ++){
							temp_radius = sqrt(pow(cosbdt-responses[jndex][0],2)+pow(bnbbdt-responses[jndex][1],2));
							if(contour_info.distance>temp_radius){
								contour_info.distance = temp_radius;
							}

						}
					}
					contour_info.tree->Fill();
				}
				contour_info.tree->Write();
				cout<<"Finish adding one root file!"<<endl;
			}
			contour_cut->Close();
		}
		cout<<"Done"<<endl;
		//2D array responses (cosmic BNB) is done!
	}

}
