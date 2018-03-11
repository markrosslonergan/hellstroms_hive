#ifndef BDT_RECOMC_H
#define BDT_RECOMC_H

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


class bdt_recomc{
	public:
		
		bdt_info info;
		std::string bdt_type;
		bdt_file* bdt_sig;
		bdt_file* bdt_bkg;

//		bdt_recomc(bdt_infos cutin, bdt_file* sigin, bdt_file* bkgin) : cut(cutin), bdt_sig(sigin), bdt_bkg(bkgin){	
//			bdt_type = cut.identifier;
//		
//		}
		bdt_recomc(){};

		int plot_recomc(TFile *fout, bdt_file* file, bdt_variable var, double cut_cosmic_val, double cut_bnb_val);



};


#endif
