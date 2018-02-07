#ifndef BDT_VAR_H
#define BDT_VAR_H

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
#include "TLine.h"
#include "TMVA/Types.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Reader.h"
#include "TFriendElement.h"


struct bdt_variable{

	public:
		std::string name;
		std::string binning;
		std::string unit;
		bool is_track;
		std::string type;

		bdt_variable(std::string inname, std::string inbin, std::string inunit,bool intrack, std::string intype) : 
			name(inname), 
			binning(inbin),
			is_track(intrack),
			unit(inunit),
			type(intype)
		{};

		bdt_variable(){};

		bdt_variable(std::string inname, std::string inbin, std::string inunit,bool intrack) : 
			name(inname), 
			binning(inbin),
			is_track(intrack),
			unit(inunit)
	{};

};





#endif
