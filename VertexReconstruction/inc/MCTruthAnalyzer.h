

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


class MCTruthAnalyzer : public Analyzer {

 public:

  MCTruthAnalyzer();

  void Initialize(){};
  void Run();
  void Finalize();

 private:
  
  TTree * ftree;

  int fstatus;
  int fpdg;

  double fnux_dist;
  double fnuy_dist;
  double fnuz_dist;
  
};


#endif
