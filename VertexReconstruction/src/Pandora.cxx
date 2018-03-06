

#include <iostream>
#include <time.h>

#include "Storage.h"
#include "Processor.h"
#include "VRAnalyzer.h"


int main(int const argc, char const * argv[]) {

  time_t start = time(0);

  Processor processor("FillLightEvent/pot_tree",
		      "FillLightEvent/meta_tree",
		      "FillLightEvent/event_tree",
		      {argv + 1, argv + argc});
  
  //VR
  VRAnalyzer * vrana = new VRAnalyzer("varana");
  //vrana->SetVBVariables(4, 20, 50, 13, 10);
  vrana->SetProducers("pandoraNu",
		      "pandoraNu",
		      "pandoraCosmicHitRemoval",
		      "simpleFlashBeam",
		      "swtrigger");
  vrana->RunPandora();
  vrana->RunFillTreeVariables();

  processor.AddAnalyzer(vrana);
  processor.SetOutputFileName("Pandora.root");
  processor.Run();

  std::cout << difftime(time(0), start) << "\n";
  
  return 0;

}
