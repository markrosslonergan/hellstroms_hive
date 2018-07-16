

#include "Pi0Filter.h"


Pi0Filter::Pi0Filter() {
    
}


bool Pi0Filter::Run() {

  std::vector<int> const & ccnc = *fstorage->fccnc;

  bool pass = true;

  if(ccnc.front() == 0) pass = false;
  
  return pass;

}


void Pi0Filter::Finalize() {

}
