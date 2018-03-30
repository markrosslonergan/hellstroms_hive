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

		bdt_datamc(bdt_file* datafilein, bdt_stack* stackin) : data_file(datafilein), mc_stack(stackin) {};


		int plotStacks(TFile*f, bdt_variable var,double,double);
		int plotBDTStacks(TFile*f, std::string,double,double);



};


#endif
