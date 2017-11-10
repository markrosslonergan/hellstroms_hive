#ifndef LOADTREES_H
#define LOADTREES_H

/******** Standard includes *****/

/******** Our includes *****/

/******** Root includes *****/
#include "TFriendElement.h"

#include "TFile.h"
#include "TTree.h"


struct load_trees {


	TFile * ifile_sp;
	TFile * ifile_bnb;
	TFile * ifile_bnb_cosmic;
	TFile * ifile_cosmic;
	TFile * ifile_sp_cosmic;

	TTree * vertex_tree_sp;
	TTree * vertex_tree_bnb;
	TTree * vertex_tree_bnb_cosmic;
	TTree * vertex_tree_cosmic;
	TTree * vertex_tree_sp_cosmic;

	TTree * event_tree_sp;
	TTree * event_tree_bnb;
	TTree * event_tree_bnb_cosmic;
	TTree * event_tree_cosmic;
	TTree * event_tree_sp_cosmic;

	double pot_sp = -1;
	double pot_sp_cosmic = -1;
	double pot_bnb = -1;
	double pot_bnb_cosmic = -1;
	double ngenbnbcosmic = -1;
	double ngencosmic = -1;

	std::string const signal_definition;

	load_trees(std::string dir, std::string const & friend_tree_file = "");

	std::vector<std::string> friend_tree_names;

	void init(std::string dir, std::string const & friend_tree_file = "");

};


#endif
