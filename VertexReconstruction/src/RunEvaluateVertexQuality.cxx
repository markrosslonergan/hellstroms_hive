

#include <iostream>
#include <time.h>

#include "EvaluateVertexQuality.h"


int main(int const argc, char const * argv[]) {

  //example: ./src/RunEvaluateVertexQuality ~/rootfiles/vertex_quality/bnb_cosmic_large/* ~/rootfiles/vertex_quality/RunVertexQualityPandoraBNB.root

  time_t start = time(0);

  if(argc < 2) {
    std::cout << "Input: vertex_quality_tree files, optional pandora file\n";
    return 1;
  }

  EvaluateVertexQuality evq("vertex_quality_tree_shower_match",
			    "permutation_tree",
			    "eval_tree",
			    {argv + 1, argv + argc});
  evq.SetOutputFile("RunEvaluateVertexQuality.root");

  std::vector<std::string> const method = {"max"};
  //std::vector<std::string> const metrics_to_draw = {"track_cleanliness", "track_completeness", "shower_completeness", "shower_cleanliness"};
  std::vector<std::string> const metrics_to_draw = {"completeness", "cleanliness", "combined"};
  std::vector<std::string> const parameters = {"start_prox", "shower_prox", "max_bp_dist", "cpoa_vert_prox", "cpoa_trackend_prox"};
  //std::vector<std::string> const parameters = {"shower_prox"};
  //std::vector<std::string> const performance_quantity = {"mean", "ratio_eq_1"};
  std::vector<std::string> const performance_quantity = {"ratio_eq_1"};

  /*
  evq.AddToDraw({"shower_completeness"},
		method,
		metrics_to_draw,
		parameters,
		performance_quantity);

  evq.AddToDraw({"shower_cleanliness"},
		method,
		metrics_to_draw,
		parameters,
		performance_quantity);

  evq.AddToDraw({"track_completeness"},
		method,
		metrics_to_draw,
		parameters,
		performance_quantity);

  evq.AddToDraw({"track_cleanliness"},
		method,
		metrics_to_draw,
		parameters,
		performance_quantity);

  evq.AddToDraw({"dist"},
		{"min"},
		{"dist"},
		parameters,
	        {"mean"});
  */

  evq.AddToDraw({"combined"},
		method,
		{"completeness", "cleanliness", "combined"},
		parameters,
		performance_quantity,
		"total");

  /*
  evq.AddToDraw({"combined"},
		method,
		{"track_completeness", "track_cleanliness", "track_combined"},
		parameters,
		performance_quantity,
		"track");

  evq.AddToDraw({"combined"},
		method,
		{"shower_completeness", "shower_cleanliness", "shower_combined"},
		parameters,
		performance_quantity,
		"shower");
  */

  /*  
  std::string const weight = "tpc_volume_contained == 1 && is_nc_delta_rad == 1 && nc_delta_rad_split_shower == 1";

  evq.AddPerformanceMetric({"correct_track_total / true_track_total", "", weight + " && true_track_total > 0",
	"track_completeness", "", "Track Completeness"});
  evq.AddPerformanceMetric({"correct_track_total / reco_track_total", "", weight + " && reco_track_total > 0",
	"track_cleanliness", "", "Track Cleanliness"});

  evq.AddPerformanceMetric({"correct_shower_total / true_shower_total", "", weight + " && true_shower_total > 0",
	"shower_completeness", "", "Shower Completeness"});
  evq.AddPerformanceMetric({"correct_shower_total / reco_shower_total", "", weight + " && reco_shower_total > 0",
	"shower_cleanliness", "", "Shower Cleanliness"});
  */

  //evq.Run({4, 40, 40, 12, 12}); //Run over specific permutation
  //evq.Run({4, 30, 100, 25, 5});
  evq.Run(); //Run over all permutations and find best one
   
  std::cout << "Wall time: " << difftime(time(0), start) << "\n";  

  return 0;

}

