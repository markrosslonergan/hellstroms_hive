#ifndef BDT_DATAMC_H
#define BDT_DATAMC_H

#include <vector>
#include <string>
#include <iostream>
/******** Our includes *****/

#include  "bdt_file.h"
#include  "bdt_var.h"
#include  "bdt_info.h"
#include  "bdt_spec.h"

/******** Root includes *****/

#include "TTreeFormula.h"
#include "TText.h"
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


class bdt_datamc{
	public:
	

	
		bdt_file* data_file;
		bdt_stack *mc_stack;
		std::string tag;

		bool is_bdt_variable;
        bool do_subtraction; 
        std::vector<bool> subtraction_vec;

		bdt_datamc(bdt_file* datafilein, bdt_stack* stackin) : data_file(datafilein), mc_stack(stackin) {tag = "null";is_bdt_variable=false; do_subtraction=false;};
		bdt_datamc(bdt_file* datafilein, bdt_stack* stackin, std::string tagin) : data_file(datafilein), mc_stack(stackin), tag(tagin) {is_bdt_variable = false; do_subtraction=false;};
		bdt_datamc(bdt_file* datafilein, bdt_stack* stackin, std::string tagin, bdt_info infoin) : data_file(datafilein), mc_stack(stackin), tag(tagin) {do_subtraction=false;};


        int setSubtractionVector(std::vector<bool> trac){
            subtraction_vec = trac;
            do_subtraction=true;
            return 0;
        }

		int plotStacks(TFile *ftest, std::vector<bdt_variable> vars, double c1, double c2);
		int plotStacks(TFile *ftest, bdt_variable var,double c1, double c2, bdt_info whichbdt);
		int plotStacks(TFile*f, bdt_variable var,double,double);
		int plotBDTStacks(TFile*f, bdt_info,double,double);


};


#endif
