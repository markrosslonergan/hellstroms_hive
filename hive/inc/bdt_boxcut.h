#ifndef BDT_BOXCUT_H
#define BDT_BOXCUT_H

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


class bdt_boxcut{
	public:
		std::string core_cuts;	
		std::string tag;	
		bdt_stack* mc_stack;
		std::vector<std::string> boxcuts;
		std::vector<bdt_variable> vars;	

		bdt_boxcut(std::string intag, bdt_stack * stackin, std::vector<std::string> inbox, std::vector<bdt_variable> invars) : tag(intag), mc_stack(stackin), boxcuts(inbox), vars(invars){ core_cuts = "1";};

		int doCuts(TFile *ftest);

        int setUpFolders();

};


#endif
