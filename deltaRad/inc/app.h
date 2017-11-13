#ifndef APP_H
#define APP_H

/******** Our includes *****/
#include "method_struct.h"
#include "void_vec.h"

/******** Root includes *****/

#include "TTreeFormula.h"
#include "TFile.h"

#include "TMVA/Types.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Reader.h"



struct app_tree_struct {

	TTree * tree;
	int is_delta_rad;
	int true_nu_vtx_fid_contained;
	int reco_asso_tracks;
	double mva;
	double summed_associated_reco_shower_energy;
	app_tree_struct(std::string const & tree_name, bool const extra = true);
	~app_tree_struct();

};



void update(void_vec const & tvv, std::vector<float *> & rvv);

void app_tree(std::string const & identifier,
		TTree * tree,
		std::string const & cut,
		std::string const & otree_name,
		std::vector<std::pair<std::string, std::string>> const & variables,
		std::vector<method_struct> const & methods);

void app(std::string const & identifier, 
		std::vector<std::pair<TTree *, std::string>> const & trees, 
		std::vector<std::string> const & tree_cuts,
		std::string const & additional_cut,
		std::vector<std::pair<std::string, std::string>> const & variables,
		std::vector<method_struct> const & methods);


#endif
