#ifndef BDT_TRAIN_H
#define BDT_TRAIN_H

#include <iostream>
#include <fstream>

#include <utility>

#include <string>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TCut.h"
#include "TChain.h"

#include "TMVA/Types.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Reader.h"

#include "method_struct.h"
#include "bdt_file.h"
#include "bdt_var.h"
#include "bdt_info.h"

#include <xgboost/c_api.h>


// safe guard for all calls to XGBoost’s C API functions 
#define safe_xgboost(call) {                                            \
int err = (call);                                                       \
if (err != 0) {                                                         \
  fprintf(stderr, "%s:%d: error in %s: %s\n", __FILE__, __LINE__, #call, XGBGetLastError()); \
  exit(1);                                                              \
}                                                                       \
}

/*
int bdt_train(bdt_info info, bdt_file *signal_file, bdt_file *background_file, std::vector<bdt_variable> variables, std::vector<method_struct> & methods);

// If training and plotting variables are separate
int bdt_train(bdt_info info, bdt_file *signal_file, bdt_file *background_file, std::vector<bdt_variable> train_variables, std::vector<bdt_variable> plot_variables, std::vector<method_struct> & methods);
*/

int plot_train(bdt_info & info, bdt_file *signal_train_file, bdt_file *signal_test_file,   bdt_file *background_train_file,  bdt_file *background_test_file);
int bdt_train(bdt_info & info, bdt_file *signal_train_file, bdt_file * signal_test_file, bdt_file * background_train_file, bdt_file *background_test_file);
int bdt_train(bdt_info & info, bdt_file *signal_train_file, bdt_file * signal_test_file, bdt_file * background_train_file, bdt_file *background_test_file, std::vector<bdt_variable>& variables, std::vector<method_struct> & methods);

/* For given bdt_file, save event info (after precut) into libsvm format */ 
int convertToLibSVM(bdt_info &info, bdt_file *file);
int convertToLibSVM(bdt_info &info, bdt_file *signal_file, bdt_file *background_file);
int convertToLibSVM(bdt_info &info, bdt_file *signal_file_train, bdt_file *signal_file_test, std::string signal_test_cut, bdt_file *background_file_train, bdt_file *background_file_test, std::string background_test_cut);//Depreciaed! 

/* For all training/test events, write weight and values of training variables into .dat files separately for training and testing files
 * Note: trainig/test events are these after preselection
 * Note: the run-to-run configuration is considered
 */
int convertToLibSVMTT(bdt_info &info, bdt_file *signal_file_train, bdt_file *signal_file_test, std::string signal_test_cut, bdt_file *background_file_train, bdt_file *background_file_test, std::string background_test_cut);//Modern, use this

/* Perform BDT training with XGboost, and save the training result in .mod file */
int bdt_XGtrain(bdt_info &info);

/* Dump information from the xgboost training, and print out information such as total use, total gain, average gain for all variables */ 
int bdt_XGBoost_importance(bdt_info &info);
int bdt_XGBoost_importance(bdt_info &info, BoosterHandle &booster);
 

int super_bdt_train(std::string &analysis_tag, const std::vector<bdt_info> & bdt_infos, const std::vector<std::string> & s_tags, const std::vector<std::string> & b_tags, const std::string & additonal_sig_cuts, const std::string & additional_bkg_cuts);



#endif
