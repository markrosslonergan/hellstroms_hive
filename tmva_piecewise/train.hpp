


#include <string>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TCut.h"

#include "tmva/Types.h"
#include "tmva/DataLoader.h"
#include "tmva/Reader.h"

#include "method_struct.hpp"



void train(std::string const & identifier, std::string const & all_cut, std::string const & signal_definition, std::string const & background_definition, std::vector<TTree *> const & signal_trees, std::vector<TTree *> const & background_trees, std::vector<std::pair<std::string, std::string>> const & variables, std::vector<method_struct> const & methods, bool const apply_weights = false) {

  std::string const name = identifier + "_training";
  
  TFile * runtmva_ofile = TFile::Open((name+".root").c_str(), "recreate");
  
  TMVA::Factory * factory = new TMVA::Factory(name.c_str(),
					      runtmva_ofile,
					      "!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification");
  TMVA::DataLoader * dataloader = new TMVA::DataLoader("dataset");

  TCut all_tcut = all_cut.c_str();
  TCut sig_tcut = all_tcut + TCut(signal_definition.c_str());
  TCut back_tcut = all_tcut + TCut(background_definition.c_str());
  
  double signal_entries = 0;
  for(TTree * tree : signal_trees) {
    dataloader->AddSignalTree(tree);
    signal_entries += tree->GetEntries(sig_tcut);
  }

  double background_entries = 0;
  for(TTree * tree : background_trees) {
    dataloader->AddBackgroundTree(tree);
    background_entries += tree->GetEntries(back_tcut);
  }

  for(std::pair<std::string, std::string> const & p : variables) dataloader->AddVariable(p.first.c_str());
  
  dataloader->PrepareTrainingAndTestTree(sig_tcut, back_tcut,
					 "nTrain_Signal="+std::to_string(int(signal_entries/2))+":nTrain_Background="+std::to_string(int(background_entries/2))+":SplitMode=Random:NormMode=NumEvents:!V");
  
  for(method_struct const & method : methods) factory->BookMethod(dataloader, method.type, method.str, method.option);
  
  factory->TrainAllMethods();
  factory->TestAllMethods();
  factory->EvaluateAllMethods();
  
  runtmva_ofile->Close();
  
  delete factory;
  delete dataloader;
  
}
