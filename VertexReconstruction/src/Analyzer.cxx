

#include "Analyzer.h"  


Analyzer::Analyzer() :
  fofile(nullptr) {}


void Analyzer::SetStorage(Storage const & storage) {

  fstorage = &storage;

}


void Analyzer::SetOutputFile(TFile * file) {

  fofile = file;

}
