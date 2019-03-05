#ifndef BDT_SPEC_H
#define BDT_SPEC_H

#include <vector>
#include <string>
#include <iostream>
#include <algorithm> //for sorter

/******** Our includes *****/

#include  "bdt_file.h"
#include  "bdt_var.h"
#include  "bdt_info.h"

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
#include "TObject.h"

class bdt_stack{
	public:
		std::string name;
		double plot_pot;
		std::vector<bdt_file*> stack;
        std::vector<TH1*> vec_hists;

        bool do_subtraction; 
        std::vector<bool> subtraction_vec;



		bdt_stack(std::string in) : name(in){
			plot_pot=6.6e20;
            do_subtraction = false;
		}

		int addToStack(bdt_file* in){
			stack.push_back(in);
			return 0;
		}
		THStack* getEntryStack(bdt_variable var);
		THStack* getEntryStack(bdt_variable var,int);
		THStack* getStack(bdt_variable var, int level,double c1, double c2);
		THStack* getBDTStack(bdt_info, int level,double c1, double c2);
		THStack* getBDTStack(bdt_info,std::string bin, int level,double c1, double c2);
		
		TH1* getSum(bdt_variable, int level, double c1, double c2);
		TH1* getEntrySum(bdt_variable);
		TH1* getEntrySum(bdt_variable,int);
		TH1* getBDTSum(bdt_info which, int level, double c1, double c2);
		TH1* getBDTSum(bdt_info which,std::string bin, int level, double c1, double c2);

		int plotStacks(TFile*ftest, std::vector<bdt_variable> vars, double c1, double c2);
		int plotStacks(TFile*f, bdt_variable var,double,double);
		int plotBDTStacks(TFile*f, bdt_info,double,double);

        int setSubtractionVector(std::vector<bool> trac){
            subtraction_vec = trac;
            do_subtraction=true;
            return 0;
        }

		int makeSBNspec(std::string tag, bdt_variable var, double, double, std::vector<std::string> hist_names);
};


#endif
