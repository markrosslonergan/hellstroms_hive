#ifndef BDT_APP_H
#define BDT_APP_H

/******** Our includes *****/
#include "method_struct.h"
#include "void_vec.h"

#include "bdt_var.h"
#include "bdt_flow.h"
#include "bdt_file.h"
#include "bdt_info.h"
#include "bdt_flow.h"
#include "bdt_train.h"
/******** Root includes *****/

#include "TTreeFormula.h"
#include "TFile.h"

#include "TMVA/Types.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Reader.h"

struct bdt_app_tree_struct {

	TTree * tree;
	int is_delta_rad;
	int true_nu_vtx_fid_contained;
	int reco_asso_tracks;
	double mva;
	double summed_associated_reco_shower_energy;
	bdt_app_tree_struct(std::string const & tree_name, bool const extra = true);
	~bdt_app_tree_struct();

};



void bdt_app_update(void_vec const & tvv, std::vector<float *> & rvv);

int bdt_app_tree(std::string identifier, TTree * tree, bdt_flow flow, std::string otree_name, std::vector<bdt_variable> vars, std::vector<method_struct> const & methods);
int bdt_app_tree(std::string identifier, TTree * tree, bdt_flow flow, std::string otree_name, std::vector<bdt_variable> train_vars, std::vector<bdt_variable> plot_vars, std::vector<method_struct> const & methods);


int bdt_app(bdt_info info, bdt_file* file);
int bdt_app(bdt_info info, bdt_file* file, std::vector<bdt_variable> vars, std::vector<method_struct> & method);

/* Predict xgboost training result on input bdt_file, and save bdt scores into corresponding **_app.root file */
int bdt_XGapp(bdt_info info, bdt_file* file);

void super_bdt_app(std::string &analysis_tag, const std::vector<bdt_info> & bdt_infos, const std::vector<bdt_file*> & files);

#endif
