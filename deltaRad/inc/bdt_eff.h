#ifndef BDT_EFF_H
#define BDT_EFF_H

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


class bdt_efficiency{
	public:
		bdt_file* file;
		std::string denominator;
		TEntryList *event_entry_list;
	
		std::vector<double> verticies;

		bdt_efficiency(bdt_file*, std::string denomin,double,double);

};


#endif
