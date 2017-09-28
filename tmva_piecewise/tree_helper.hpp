


#include <iostream>

#include "TFile.h"
#include "TTree.h"



class tree_helper {



  std::vector<TFile *> open_files;



public:



  ~tree_helper() {
    for(TFile * file : open_files) file->Close();
  }



  TTree * GetTree(std::string const & file_path, std::string const & tree_path) {
    
    TFile * file = TFile::Open(file_path.c_str());
    if(!file) {
      std::cout << "ERROR - could not find file:\n" << file_path << "\n";
      exit(1);
    }
    TTree * tree = dynamic_cast<TTree *>(file->Get(tree_path.c_str()));
    if(!tree) {
      std::cout << "ERROR - could not find tree:\n" << tree_path << "\n";
      exit(1);
    }
    open_files.push_back(file);
    return tree;
    
  }



};
