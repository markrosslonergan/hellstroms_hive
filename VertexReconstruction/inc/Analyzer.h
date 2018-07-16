

#ifndef ANALYZER_H
#define ANALYZER_H

#include "Storage.h"


class Analyzer {

 public:

  Analyzer();

  void SetStorage(Storage const & storage);
  void SetOutputFile(TFile * file);

  virtual void Initialize(){}
  virtual bool Run(){return true;}
  virtual void Finalize(){}

 protected:

  Storage const * fstorage;
  TFile * fofile;

};


#endif
