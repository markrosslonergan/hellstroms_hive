

#include <iostream>
#include <time.h>

#include "TFile.h"

#include "Processor.h"
#include "VRAnalyzer.h"
#include "GetPermutations.h"


int main(int const argc, char const * argv[]) {

  time_t start = time(0);

  if(argc != 3 && argc != 4) {
    std::cout << "Input: permutation_tree file, light_event file, optional pandora flag\n";
    return 1;
  }

  int pandora = 0;
  if(!strcmp(argv[argc-1], "pandora")) pandora = 1;

  Processor processor("FillLightEvent/pot_tree",
		      "FillLightEvent/meta_tree",
		      "FillLightEvent/event_tree",
		      {argv + 2, argv + argc - pandora});
  if(pandora) processor.SetOutputFileName("RunVertexQualityPandora.root");
  else processor.SetOutputFileName("RunVertexQuality.root");

  std::string const track_producer = "pandoraNu";
  std::string const shower_producer = "pandoraNu";

  VertexQuality vq;
  vq.RunSig();
  vq.SetProducers(track_producer, shower_producer);

  std::string const weight = "tpc_volume_contained == 1 && is_nc_delta_rad == 1 && nc_delta_rad_split_shower == 1";
  std::string const ratio_binning = "(101, 0, 1.01)";

  vq.AddPerformanceMetric({"correct_track_total / true_track_total", ratio_binning, weight + " && true_track_total > 0",
	"track_completeness", "", "Track Completeness"});
  vq.AddPerformanceMetric({"correct_track_total / reco_track_total", ratio_binning, weight + " && reco_track_total > 0",
	"track_cleanliness", "", "Track Cleanliness"});
  
  vq.AddPerformanceMetric({"correct_shower_total / true_shower_total", ratio_binning, weight + " && true_shower_total > 0",
	"shower_completeness", "", "Shower Completeness"});
  vq.AddPerformanceMetric({"correct_shower_total / reco_shower_total", ratio_binning, weight + " && reco_shower_total > 0",
	"shower_cleanliness", "", "Shower Cleanliness"});
  
  vq.AddPerformanceMetric({"(correct_track_total + correct_shower_total) / (true_track_total + true_shower_total)", 
	ratio_binning, 
	weight + " && (true_track_total + true_shower_total) > 0",
	"completeness",
	"",
	"Completeness"});
  vq.AddPerformanceMetric({"(correct_track_total + correct_shower_total) / (reco_track_total + reco_shower_total)",
	ratio_binning,
	weight + " && (reco_track_total + reco_shower_total) > 0",
	"cleanliness",
	"",
	"Cleanliness"});
  
  vq.AddPerformanceMetric({"dist", "", weight,
	"dist", "", "Distance"});  

  if(pandora) {

    vq.AddPermutations({{0, 0, 0, 0, 0}});
    VRAnalyzer * vrana = new VRAnalyzer("varana_pandora", &vq);
    vrana->SetVBVariables(0, 0, 0, 0, 0);
    vrana->SetProducers(track_producer,
			shower_producer,
			"pandoraCosmicHitRemoval",
			"simpleFlashBeam",
			"swtrigger");
    vrana->RunPandora();
    processor.AddAnalyzer(vrana);
    
  }

  else {

    Permutations permutations;
    std::vector<std::vector<double>> const & permutation_v = permutations.ReadFile(argv[1]);
    if(permutation_v.front().size() != 5) {
      std::cout << "ERROR: requires exactly 5 parameters per permutation\n";
      return 1;
    }

    permutations.WritePermutationTreeToFile();
    vq.AddPermutations(permutation_v);

    for(size_t i = 0; i < permutation_v.size(); ++i) {
      std::vector<double> const & parameters = permutation_v.at(i);
      VRAnalyzer * vrana = new VRAnalyzer("varana_" + std::to_string(i), &vq);
      vrana->SetVBVariables(parameters.at(0), parameters.at(1), parameters.at(2), parameters.at(3), parameters.at(4));
      vrana->SetProducers(track_producer,
			  shower_producer,
			  "pandoraCosmicHitRemoval",
			  "simpleFlashBeam",
			  "swtrigger");
      processor.AddAnalyzer(vrana);
    }

  }    

  processor.Run();
  vq.Evaluate();
  vq.Write();
  
  std::cout << "Wall time: " << difftime(time(0), start) << "\n";  

  return 0;

}
