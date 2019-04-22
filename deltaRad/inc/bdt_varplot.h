#ifndef BDT_VARPLOT_H
#define BDT_VARPLOT_H

#include <vector>
#include <string>
#include <numeric>
#include <iostream>
#include <algorithm>
#include <numeric>
/******** Our includes *****/

#include  "bdt_file.h"
#include  "bdt_var.h"
#include  "bdt_info.h"
#include  "bdt_flow.h"
#include  "method_struct.h"

/******** Root includes *****/

#include "TTreeFormula.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TH1.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TLatex.h"
#include "THStack.h"
#include "TGraph.h"
#include "TLine.h"
#include "TMVA/Types.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Reader.h"
#include "TFriendElement.h"
#include "TText.h"

int plot_bdt_variables(bdt_file * signal_pure, bdt_file * background_pure, std::vector<bdt_variable> vars, bdt_info input_bdt_info, bool isSpectator);
int plot_bdt_variable(bdt_file * signal_pure, bdt_file * background_pure, bdt_variable v, bdt_info input_bdt_info, bool isSpectator);
int plot_bdt_variables(bdt_file * signal, bdt_file *background, std::vector<bdt_variable> vars, std::vector<double> bdt_cuts, int stage);

#endif
