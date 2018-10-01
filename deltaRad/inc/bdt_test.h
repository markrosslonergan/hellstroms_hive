#ifndef BDT_TEST_H
#define BDT_TEST_H

#include <vector>
#include <string>
#include <iostream>
/******** Our includes *****/

#include  "bdt_file.h"
#include  "bdt_var.h"
#include  "bdt_info.h"
#include  "bdt_spec.h"
#include  "variable_list.h"

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


class bdt_test{
	public:
		bdt_file* file;
        std::vector<bdt_variable> vars;
		std::string tag;

		bdt_test(bdt_file* filein, std::vector<bdt_variable> varin, std::string tagin) : file(filein), vars(varin), tag(tagin) {};
	
		int RunTests();
};


#endif
