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
#include "TLorentzVector.h"

// Define bin ranges for projection histograms
// Each bin is 10 MeV, so the energy range will be bin_num/10
// Ex: To project between 100 and 150 MeV, use (10, 15) as bin numbers
std::vector<std::pair<int,int> > leadingProjBins() {
    std::vector<std::pair<int,int> > ranges;
    //ranges.push_back(std::make_pair(2, 3) );
    ranges.push_back(std::make_pair(3, 4) );
    ranges.push_back(std::make_pair(4, 5) );
    ranges.push_back(std::make_pair(5, 6) );
    ranges.push_back(std::make_pair(6, 7) );
    ranges.push_back(std::make_pair(7, 8) );
    ranges.push_back(std::make_pair(8, 9) );
    ranges.push_back(std::make_pair(9, 10) );
    ranges.push_back(std::make_pair(10, 11) );
    ranges.push_back(std::make_pair(11, 12) );
    ranges.push_back(std::make_pair(12, 13) );
    ranges.push_back(std::make_pair(13, 14) );
    ranges.push_back(std::make_pair(14, 15) );
    ranges.push_back(std::make_pair(15, 16) );
    ranges.push_back(std::make_pair(16, 17) );
    ranges.push_back(std::make_pair(17, 18) );
    ranges.push_back(std::make_pair(18, 20) );
    ranges.push_back(std::make_pair(20, 22) );
    ranges.push_back(std::make_pair(22, 25) );
    ranges.push_back(std::make_pair(25, 30) );
    ranges.push_back(std::make_pair(30, 35) );
    ranges.push_back(std::make_pair(35, 40) );
    ranges.push_back(std::make_pair(40, 45) );
    /*
    ranges.push_back(std::make_pair(20, 30) );
    ranges.push_back(std::make_pair(30, 40) );
    ranges.push_back(std::make_pair(40, 50) );
    ranges.push_back(std::make_pair(50, 60) );
    ranges.push_back(std::make_pair(60, 70) );
    ranges.push_back(std::make_pair(70, 80) );
    ranges.push_back(std::make_pair(80, 90) );
    ranges.push_back(std::make_pair(90, 100) );
    ranges.push_back(std::make_pair(100, 120) );
    ranges.push_back(std::make_pair(120, 140) );
    ranges.push_back(std::make_pair(140, 180) );
    ranges.push_back(std::make_pair(180, 220) );
    ranges.push_back(std::make_pair(220, 300) );
    ranges.push_back(std::make_pair(300, 500) );
    */
    return ranges;
}
