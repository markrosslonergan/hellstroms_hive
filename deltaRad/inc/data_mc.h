#ifndef DATA_MC_H
#define DATA_MC_H

#include <vector>
#include <string>
#include <iostream>
/******** Our includes *****/

/******** Root includes *****/

#include "TTreeFormula.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TH1.h"
#include "TH1D.h"
#include "TLegend.h"
#include "THStack.h"
#include "TGraph.h"
#include "TLine.h"
#include "TMVA/Types.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Reader.h"
#include "TFriendElement.h"

	template <typename T>
std::string to_string_prec(const T a_value, const int n = 6)
{
	std::ostringstream out;
	out <<std::fixed<< std::setprecision(n) << a_value;
	return out.str();
}

struct bdt_variable{

	public:
		std::string name;
		std::string binning;
		std::string unit;
		bool is_track;

		bdt_variable(std::string inname, std::string inbin, std::string inunit,bool intrack) : 
			name(inname), 
			binning(inbin),
			is_track(intrack),
			unit(inunit)
	{};

};

struct bdt_file{
	public:
		std::string dir;
		std::string name;
		std::string tag;
		std::string plot_ops;
		std::string root_dir;
		std::string friend_tree_file;

		int col;
		bool is_data;
		bool is_mc;

		std::string leg;

		std::string friend_tree_name;

		double pot;

		TFile *f;
		TTree *tvertex;
		TTree *tpot;

		double scale_data;

		int scale(double scalein){
			scale_data = scalein;
			return 0;
		}
		int setPOT(double inpot){
			pot = inpot;
			return 0;
		}

		TH1* getTH1(bdt_variable var, std::string cuts, std::string nam, double plot_POT){

				TCanvas *ctmp = new TCanvas();
				this->tvertex->Draw((var.name+">>"+nam+ var.binning).c_str() ,cuts.c_str(),"goff");
				std::cout<<"Done with Draw for "<<(var.name+">>"+nam+ var.binning).c_str()<<std::endl;

				TH1* th1 = (TH1*)gDirectory->Get(nam.c_str()) ;
				th1->Scale(this->scale_data*plot_POT/this->pot);
			return th1;
		}

		bdt_file(std::string indir,std::string inname, std::string intag, std::string inops, std::string inrootdir, std::string infriend, std::string infriendtree, int incol, bool indata) :
			dir(indir),
			name(inname),
			tag(intag),
			root_dir(inrootdir),
			friend_tree_file(infriend),
			friend_tree_name(infriendtree),
			plot_ops(inops),
			col(incol),
			is_data(indata),
			is_mc(!indata)
	{

		scale_data =1.0;
		std::cout<<"Loading : "<<name<<std::endl;
		f = new TFile((dir+"/"+name).c_str(), "read");	


		if(!f->IsOpen() || !f){
			std::cout<<"ERROR: didnt open file right: "<<dir<<"/"<<name<<std::endl;
			exit(EXIT_FAILURE);
		}

		std::string tnam = root_dir+"/"+"vertex_tree";
		std::string tnam_pot = root_dir+"/"+"get_pot";

		std::cout<<"Getting vertex tree"<<std::endl;
		tvertex = (TTree*)f->Get(tnam.c_str());
		std::cout<<"Got vertex tree"<<std::endl;

		double potbranch = 0;

		if(is_mc){
			leg = "l";
			std::cout<<"Getting POT tree: "<<tnam_pot<<std::endl;
			tpot = (TTree*)f->Get(tnam_pot.c_str());
			tpot->SetBranchAddress("pot",&potbranch);

			std::cout<<"Set the POT branch"<<std::endl;
			double tmppot=0;
			for(int i=0; i<tpot->GetEntries(); i++){
				tpot->GetEntry(i);
				tmppot += potbranch;
			}
			pot=tmppot;
			std::cout<<"--> POT is MC: ";
			std::cout<<"--> value: "<<pot<<std::endl;
		}else{
			leg = "lp";
			std::cout<<"--> POT is data: ";
			pot = 4.95e19*7131/13671;// 376954.0/382718.0;//7131/13671;
			std::cout<<"--> value: "<<pot<<std::endl;
		}

		std::cout<<"---> VERTEXCOUNT: "<<tag<<" "<<tvertex->GetEntries()*5e19/pot<<std::endl;

		std::cout<<"Getting friend trees!"<<std::endl;

		std::string method = "BDT";
		if(is_mc){
			friend_tree_name =  std::string(tvertex->AddFriend((friend_tree_name+"_"+method).c_str(), friend_tree_file.c_str())->GetTree()->GetName());
		}else{
			friend_tree_name =  std::string(tvertex->AddFriend((friend_tree_name+"_"+method).c_str(), friend_tree_file.c_str())->GetTree()->GetName());
		}

		std::cout<<"Done!"<<std::endl;

	};

		~bdt_file(){
			f->Close();
		}





};

int run_efficiency();

int data_mc();
int new_data_mc();

#endif
