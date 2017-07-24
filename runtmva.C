

#include "tmva.C"
#include "tmva_app.C"
#include "getsig.C"



void runtmva(std::string dir) {

  tmva(dir);
  tmva_app(dir);
  getsig("tmva_app.root");

}
