#ifndef TRAIN_H
#define TRAIN_H


#include <string>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TCut.h"

#include "TMVA/Types.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Reader.h"

#include "method_struct.h"




void train(std::string const & identifier, std::string const & all_cut, std::string const & signal_definition, std::string const & background_definition, std::vector<TTree *> const & signal_trees, std::vector<TTree *> const & background_trees, std::vector<std::pair<std::string, std::string>> const & variables, std::vector<method_struct> const & methods, bool const apply_weights=false );




#endif
