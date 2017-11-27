#ifndef DATA_MC_H
#define DATA_MC_H

#include <vector>
#include <string>
#include <iostream>
/******** Our includes *****/

/******** Root includes *****/

#include "TTreeFormula.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TH1.h"
#include "TH1D.h"
#include "TLegend.h"

#include "TMVA/Types.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Reader.h"
#include "TFriendElement.h"


int data_mc();
int new_data_mc();

#endif
