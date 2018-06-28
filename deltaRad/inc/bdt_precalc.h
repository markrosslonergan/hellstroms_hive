#ifndef BDT_PRECALC_H
#define BDT_PRECALC_H

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <fstream>
/******** Our includes *****/

#include  "bdt_file.h"
#include  "bdt_var.h"
#include  "bdt_info.h"

#include "TruncMean.h"
/******** Root includes *****/
#include "TFitResult.h"
#include "TF1.h"
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

int addPreFriends(bdt_file* filein,std::string which);

class bdt_precalc{
	public:
	bdt_file* file;
	TFile *friend_file_out;
	//pass in a bdt_file and it calculates stuff and add's it as friends!
		bdt_precalc(bdt_file* in) : file(in) {
		friend_file_out = new TFile((file->tag+"_friends.root").c_str(),"update");	
		};
		//This will actually
		~bdt_precalc(){ friend_file_out->Close();}

int gen(std::string which );
int genTrackInfo();
int genNewTrackInfo();
int genPi0Info();
int genBNBcorrectionInfo();




};


#endif
