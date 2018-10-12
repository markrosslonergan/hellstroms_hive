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
std::vector<std::pair<int,int> > leadingProjBins() {
    std::vector<std::pair<int,int> > ranges;
    ranges.push_back(std::make_pair(5 , 10) );
    ranges.push_back(std::make_pair(10, 15) );
    ranges.push_back(std::make_pair(15, 20) );
    ranges.push_back(std::make_pair(20, 25) );
    ranges.push_back(std::make_pair(25, 30) );
    ranges.push_back(std::make_pair(30, 40) );
    ranges.push_back(std::make_pair(40, 50) );
    return ranges;
}

std::vector<std::pair<int,int> > subleadingProjBins() {
    std::vector<std::pair<int,int> > subranges;
    subranges.push_back(std::make_pair(3 , 6) );
    subranges.push_back(std::make_pair(6, 9) );
    subranges.push_back(std::make_pair(9, 12) );
    subranges.push_back(std::make_pair(12, 15) );
    return subranges;
}
