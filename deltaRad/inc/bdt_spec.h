#ifndef BDT_SPEC_H
#define BDT_SPEC_H

#include <vector>
#include <string>
#include <iostream>
/******** Our includes *****/

#include  "bdt_file.h"
#include  "bdt_var.h"
#include  "bdt_cuts.h"

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

		bdt_stack(std::string in) : name(in){
			plot_pot=6.6e20;
		}

		int addToStack(bdt_file* in){
			stack.push_back(in);
			return 0;
		}

		THStack* getStack(bdt_variable var, int level,double c1, double c2);
		THStack* getBDTStack(std::string, int level,double c1, double c2);

		int plotStacks(TFile*f, bdt_variable var,double,double);
		int plotBDTStacks(TFile*f, std::string,double,double);

};


#endif
