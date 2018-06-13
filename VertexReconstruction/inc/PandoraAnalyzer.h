

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

  void FillVertexTree();
  void FillTrackTree();
  void FillShowerTree();

  TTree * fvertex_tree;
  double fdist;
  double fdistx;
  double fdisty;
  double fdistz;

  TTree * ftrack_tree;
  double fstart_dist;
  double fstart_distx;
  double fstart_disty;
  double fstart_distz;
  double fend_dist;
  double fend_distx;
  double fend_disty;
  double fend_distz;
  
  TTree * fshower_tree;
  double fstartx;
  double fstarty;
  double fstartz;
  double ftrue_pdg;
  double ftrue_startx;
  double ftrue_starty;
  double ftrue_startz;

};


#endif
