

#include <iostream>
#include <time.h>

#include "TFile.h"

#include "Processor.h"
#include "VRAnalyzer.h"
#include "GetPermutations.h"


int main(int const argc, char const * argv[]) {

  time_t start = time(0);

  if(argc != 3) {
    std::cout << "Input: permutation_tree file, light_event file\n";
    return 1;
  }

  std::vector<std::vector<double>> permutation_v;
  Permutations permutations;
  permutations.ReadFile(argv[1], permutation_v);

  Processor processor("FillLightEvent/pot_tree",
		      "FillLightEvent/meta_tree",
		      "FillLightEvent/event_tree",
		      {argv + 2, argv + argc});
  processor.SetOutputFileName("RunVertexQuality.root");

  VertexQuality vq;

  for(size_t i = 0; i < permutation_v.size(); ++i) {
    std::vector<double> const & parameters = permutation_v.at(i);
    VRAnalyzer * vrana = new VRAnalyzer("varana_" + std::to_string(i), &vq);
    vrana->SetVBVariables(parameters.at(0), parameters.at(1), parameters.at(2), parameters.at(3), parameters.at(4));
    vrana->SetProducers("pandoraNu",
			"pandoraNu",
			"pandoraCosmicHitRemoval",
			"simpleFlashBeam",
			"swtrigger");
    processor.AddAnalyzer(vrana);
  }

  processor.Run();
  vq.Write();
  
  std::cout << difftime(time(0), start) << "\n";  

  return 0;

}
