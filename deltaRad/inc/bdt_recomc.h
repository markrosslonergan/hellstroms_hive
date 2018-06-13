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

		std::vector<std::string> recomc_cuts;
		std::vector<std::string> recomc_names;
		std::vector<int> recomc_cols;

		std::string tag;
		bool do_rebin;
		bool is_log;
//		bdt_recomc(bdt_infos cutin, bdt_file* sigin, bdt_file* bkgin) : cut(cutin), bdt_sig(sigin), bdt_bkg(bkgin){	
//			bdt_type = cut.identifier;
//		
//		}
		bdt_recomc(){};
		bdt_recomc(std::vector<std::string> namein, std::vector<std::string> cutsin, std::vector<int> incols, std::string tagin) : recomc_cuts(cutsin), recomc_names(namein), recomc_cols(incols) , tag(tagin){ do_rebin = true; is_log = false;};

		int plot_recomc(TFile *fout, bdt_file* file, bdt_variable var, double cut_cosmic_val, double cut_bnb_val);
		int setRebin(bool in){ do_rebin = in;};


};


#endif
