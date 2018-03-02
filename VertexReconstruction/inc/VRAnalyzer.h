

#ifndef VRANALYZER_H
#define VRANALYZER_H

#include "Analyzer.h"
#include "Storage.h"
#include "TTree.h"
#include "ParticleAssociations.h"
#include "VertexBuilder.h"
#include "VertexQuality.h"
#include "FillTreeVariables.h"


class VRAnalyzer : public Analyzer {

 public:

  VRAnalyzer();

  void SetVerbose(bool const verbose = true);
  void SetProducers(std::string const & track_producer,
		    std::string const & shower_producer,
		    std::string const & hit_producer,
		    std::string const & opflash_producer,
		    std::string const & trigger_product);
  void SetVBVariables(double const start_prox,
		      double const shower_prox,
		      double const max_bp_dist,
		      double const cpoa_vert_prox,
		      double const cpoa_trackend_prox);
  void RunVertexQuality(bool const run_vertex_quality = true);

  void Initialize();
  void AddTracks(DetectorObjects & detos,
		 std::string const & producer,
		 bool const track_original_indices = false);
  void AddShowers(DetectorObjects & detos,
		  std::string const & producer,
		  bool const track_original_indices = false);
  void Run();
  void Finalize();

 private:

  bool fverbose;

  bool fmcordata;
  std::string ftrack_producer;
  std::string fswtrigger_product;
  std::string fshower_producer;
  std::string fhit_producer;
  std::string fopflash_producer;

  double fstart_prox;
  double fshower_prox;
  double fmax_bp_dist;
  double fcpoa_vert_prox;
  double fcpoa_trackend_prox;

  bool frun_vertex_quality;
  VertexQuality fvq;

  FillTreeVariables fftv;

};


#endif
