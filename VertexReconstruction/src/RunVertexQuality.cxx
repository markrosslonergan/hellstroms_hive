

#include <iostream>
#include <time.h>

#include "Storage.h"
#include "Processor.h"
#include "GetPermutations.h"
#include "VRAnalyzer.h"


int main(int const argc, char const * argv[]) {

  time_t start = time(0);

  /*
  Permutations permutations;
  permutations.AddParameter("start_prox", 4);
  permutations.AddParameter("shower_prox", 11, 4, 48);
  permutations.AddParameter("max_bp_dist", 11, 4, 48);
  permutations.AddParameter("cpoa_vert_prox", 13);
  permutations.AddParameter("cpoa_trackend_prox", 10);
  std::vector<std::vector<double>> const & permutation_v = permutations.Get();
  permutations.Check();
  */

  Permutations permutations;
  permutations.AddParameter("start_prox", 4);
  permutations.AddParameter("shower_prox", 20);
  permutations.AddParameter("max_bp_dist", 50);
  permutations.AddParameter("cpoa_vert_prox", 13);
  permutations.AddParameter("cpoa_trackend_prox", 10);
  std::vector<std::vector<double>> const & permutation_v = permutations.Get();

  Processor processor("FillLightEvent/pot_tree",
		      "FillLightEvent/meta_tree",
		      "FillLightEvent/event_tree",
		      {argv + 1, argv + argc});

  for(size_t i = 0; i < permutation_v.size(); ++i) {
    std::vector<double> const & permutation = permutation_v.at(i);
    VRAnalyzer * vrana = new VRAnalyzer("varana_" + std::to_string(i));
    vrana->SetVBVariables(permutation.at(0), permutation.at(1), permutation.at(2), permutation.at(3), permutation.at(4));
    vrana->SetProducers("pandoraNu",
			"pandoraNu",
			"pandoraCosmicHitRemoval",
			"simpleFlashBeam",
			"swtrigger");
    vrana->RunVertexQuality();
    processor.AddAnalyzer(vrana);
  }

  processor.SetOutputFileName("RunVertexQuality.root");
  processor.Run();

  std::cout << difftime(time(0), start) << "\n";  
  
  return 0;

}
