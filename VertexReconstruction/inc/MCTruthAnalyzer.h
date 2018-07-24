

#ifndef MCTRUTHANALYZER_H
#define MCTRUTHANALYZER_H

#include "Analyzer.h"
#include "Storage.h"
#include "TTree.h"
#include "ParticleAssociations.h"
#include "VertexBuilder.h"
#include "VertexQuality.h"
#include "FillTreeVariables.h"

#include "TTree.h"

#include <unordered_map>


class MCTruthAnalyzer : public Analyzer {

 public:

  MCTruthAnalyzer();

  void Initialize(){};
  bool Run();
  void Finalize();

 private:
  
  TTree * ftree;

  int fpi0_statuscode;
     
};


#endif
