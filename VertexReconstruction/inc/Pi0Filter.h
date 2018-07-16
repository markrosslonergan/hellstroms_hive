

#ifndef PI0FILTER_H
#define PI0FILTER_H

#include "Analyzer.h"
#include "Storage.h"

#include <unordered_map>


class Pi0Filter : public Analyzer {

 public:

  Pi0Filter();

  void Initialize(){};
  bool Run();
  void Finalize();
  
};


#endif
