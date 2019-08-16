#ifndef BDT_TRAIN_H
#define BDT_TRAIN_H

#include <iostream>
#include <fstream>

#include <string>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TCut.h"

#include "TMVA/Types.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Reader.h"

#include "method_struct.h"
#include "bdt_file.h"
#include "bdt_var.h"
#include "bdt_info.h"

#include <xgboost/c_api.h>

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
int plot_train(bdt_info info, bdt_file *signal_file, bdt_file *background_file);
int bdt_train(bdt_info info, bdt_file *signal_file, bdt_file *background_file);
int bdt_train(bdt_info, bdt_file*, bdt_file*, std::vector<bdt_variable>, std::vector<method_struct> & );
int bdt_train(bdt_info, bdt_file*, bdt_file*, std::vector<bdt_variable>, std::vector<bdt_variable>, std::vector<method_struct> & );

int convertToLibSVM(bdt_info info, bdt_file *file);
int convertToLibSVM(bdt_info info, bdt_file *signal_file, bdt_file *background_file);
int bdt_XGtrain(bdt_info info);

#endif
