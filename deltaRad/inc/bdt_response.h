#ifndef BDT_RESPONSE_H
#define BDT_RESPONSE_H

#include <vector>
#include <string>
#include <iostream>
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


class bdt_response{
	public:
		
		bdt_info info;
		std::string bdt_type;
		bdt_file* bdt_sig;
		bdt_file* bdt_bkg;

		bdt_response(bdt_info infoin, bdt_file* sigin, bdt_file* bkgin) : info(infoin), bdt_sig(sigin), bdt_bkg(bkgin){	
			bdt_type = info.identifier;
		}


		int plot_bdt_response(TFile *fout);
};


#endif
