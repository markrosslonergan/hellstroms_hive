

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

  int delta_pdg;
  int proton_pdg;
  int photon_pdg;

  std::vector<int> exiting_particles;
  int number_of_exiting_particles;

  double mp;
  double Ep;
  double Pp;
  double Eg;
  
  double Pxp;
  double Pyp;
  double Pzp;
  
  double Pxg;
  double Pyg;
  double Pzg;
  
  double costerm;

  double inv_delta_mass;
     
};


#endif
