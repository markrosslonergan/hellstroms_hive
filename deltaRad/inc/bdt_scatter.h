#ifndef BDT_SCATTER_H
#define BDT_SCATTER_H

#include <vector>
#include <string>
#include <iostream>
#include <algorithm> 
#include "stdlib.h"

/******** Our includes *****/

#include "bdt_file.h"
#include "bdt_var.h"
#include "bdt_info.h"

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
#include "TObject.h"
#include "TStyle.h"
#include "TString.h"

int getBDTScatter(TFile* fout, std::vector<bdt_file*> files, bdt_info cosmic_info, bdt_info bnb_info);

#endif
