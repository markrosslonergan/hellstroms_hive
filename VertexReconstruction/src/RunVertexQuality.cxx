

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

  Permutations permutations;
  std::vector<std::vector<double>> const & permutation_v = permutations.ReadFile(argv[1]);
  if(permutation_v.front().size() != 5) {
    std::cout << "ERROR: requires exactly 5 parameters per permutation\n";
    return 1;
  }

  Processor processor("FillLightEvent/pot_tree",
		      "FillLightEvent/meta_tree",
		      "FillLightEvent/event_tree",
		      {argv + 2, argv + argc});
  processor.SetOutputFileName("RunVertexQuality.root");
  permutations.WritePermutationTreeToFile();

  std::string const track_producer = "pandoraNu";
  std::string const shower_producer = "pandoraNu";

  VertexQuality vq;
  vq.RunSig();
  vq.SetProducers(track_producer, shower_producer);

  vq.AddParameterToDraw({"correct_shower_total / true_shower_total", "", "tpc_volume_contained == 1",
	"completeness", "", "Completeness"});
  vq.AddParameterToDraw({"correct_shower_total / reco_shower_total", "", "tpc_volume_contained == 1",
	"cleanliness", "", "Cleanliness"});
  vq.AddParameterToDraw({"(correct_shower_total / reco_shower_total) * (correct_shower_total / true_shower_total)", "", "tpc_volume_contained == 1",
	"combined", "", "Combined"});
  
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

  processor.Run();
  vq.Evaluate();
  vq.Write();
  
  std::cout << "Wall time: " << difftime(time(0), start) << "\n";  

  return 0;

}
