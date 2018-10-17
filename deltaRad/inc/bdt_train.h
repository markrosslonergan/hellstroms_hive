#ifndef BDT_TRAIN_H
#define BDT_TRAIN_H


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



int bdt_train(bdt_info info, bdt_file *signal_file, bdt_file *background_file, bdt_file *signal_test, bdt_file *background_test, std::vector<bdt_variable> variables, std::vector<method_struct> & methods);
int bdt_train(bdt_info info, bdt_file *signal_file, bdt_file *background_file, std::vector<bdt_variable> variables, std::vector<method_struct> & methods);



#endif
