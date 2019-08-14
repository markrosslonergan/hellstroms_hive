#ifndef BDT_VERTEFF_H
#define BDT_VERTEFF_H

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
#include "TStyle.h"


class bdt_vertex_eff{
	public:
        bdt_file * file;
		bdt_variable plot_var;
        std::string denominator_cut;
        std::vector<std::string> cut_vector;
        
        bdt_vertex_eff(bdt_file * infile , bdt_variable invar, std::string in_denominator_cut, std::vector<std::string> incut_vector) : file(infile), plot_var(invar), denominator_cut(in_denominator_cut), cut_vector(incut_vector) {};


        int plotEfficiencies(std::string outputname);


};


#endif
