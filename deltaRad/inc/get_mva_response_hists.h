#ifndef GETMVARESPONSEHISTS_H
#define GETMVARESPONSEHISTS_H


/******** Standard includes *****/
#include <string>
#include <iostream>

/******** Our includes *****/
#include "method_struct.h"

/******** Root includes *****/
#include "TCanvas.h"
#include "TH1F.h"
#include "TFriendElement.h"
#include "TTree.h"
#include "TDirectory.h"
#include "TFile.h"

TH1 *  get_hist(std::string const & hname,
		TTree * tree,
		std::string const & mva_tree_name,
		std::string const & draw,
		std::string const & bins,
		std::string const & cut);


void draw_mva_response_hist(std::string const & name,
		TTree * tree,
		std::string const & mva_tree_name,
		std::string const & draw,
		std::string const & bins,
		std::string const & all_cut,
		std::string const & cut1,
		std::string const & cut2); 

void get_mva_response_hists(std::string const & ofile_path,
		std::string const & ifile_path,
		std::vector<std::pair<TTree *, std::string>> const & trees,
		std::vector<method_struct> const & methods,
		std::vector<std::pair<std::string, std::string>> const & branches,
		std::string const & bins,
		std::vector<std::string> const & tree_cuts,
		std::string const & cut1,
		std::string const & cut2); 

#endif
