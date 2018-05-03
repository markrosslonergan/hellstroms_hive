

#ifndef PANDORAANALYZER_H
#define PANDORAANALYZER_H

#include "Analyzer.h"
#include "Storage.h"
#include "TTree.h"
#include "ParticleAssociations.h"
#include "VertexBuilder.h"
#include "VertexQuality.h"
#include "FillTreeVariables.h"
#include "GeoVector.h"

#include "TTree.h"


class PandoraAnalyzer : public Analyzer {

 public:

  PandoraAnalyzer();

  void Initialize(){};
  void Run();
  void Finalize();

 private:

  TTree * ftree;

  double dist;
  double distx;
  double disty;
  double distz;
  
};


#endif
