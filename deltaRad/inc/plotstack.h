#ifndef PLOTSTACK_H
#define PLOTSTACK_H

#include <string>
#include <vector>

#include <sstream>
#include <iomanip>

#include "TFile.h"
#include "TTree.h"
#include "TCut.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TLegend.h"
#include "THStack.h"


#include "TMVA/Types.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Reader.h"

#include "method_struct.h"



#include "load_trees.h"



void plot(load_trees const & lt,
		TTree * tree,
		double const sample_pot,
		std::string const & name,
		std::string const & draw,
		std::string const & binning,
		std::string const & weight = "",
		std::string const & option = "",
		std::string const & title = "",
		std::string const & xtitle = "",
		std::string const & ytitle = "",
		bool const logy = false,
		std::string const & method = "",
		double const mva1 = DBL_MAX,
		double const mva2 = DBL_MAX);




void plotstack(std::string const & ifile_path, std::string const & friend_tree_path);


#endif

