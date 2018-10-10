// C++ includes
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <exception>
#include <tuple>
#include <stdexcept>

// Root includes
#include "TInterpreter.h"
#include "TROOT.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "TBranch.h"
#include "TStreamer.h"
#include "TFile.h"
#include "TNtuple.h"
#include "TClonesArray.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TLine.h"
#include "TLegend.h"
#include "TStyle.h"
#include "THStack.h"
#include "TVector3.h"
#include "TMath.h"
#include "TString.h"
#include "TF1.h"
#include "TProfile.h"

// Define bin ranges for projection histograms
// Each bin is 10 MeV, so the energy range will be bin_num/10
// Ex: To project between 100 and 150 MeV, use (10, 15) as bin numbers
std::vector<std::pair<int,int> > projBins() {
    std::vector<std::pair<int,int> > ranges;
    //ranges.push_back(std::make_pair(3, 5) );
    ranges.push_back(std::make_pair(3, 4) );
    ranges.push_back(std::make_pair(5, 6) );
    ranges.push_back(std::make_pair(7, 9) );
    ranges.push_back(std::make_pair(10, 14) );
    ranges.push_back(std::make_pair(15, 19) );
    ranges.push_back(std::make_pair(20, 24) );
    ranges.push_back(std::make_pair(25, 29) );
    ranges.push_back(std::make_pair(30, 39) );
    ranges.push_back(std::make_pair(40, 50) );
    return ranges;
}

