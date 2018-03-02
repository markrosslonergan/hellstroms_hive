

#include <iostream>

#include "Storage.h"
#include "Processor.h"
#include "VRAnalyzer.h"


int main(int const argc, char const * argv[]) {

  Processor processor("FillLightEvent/pot_tree",
		      "FillLightEvent/meta_tree",
		      "FillLightEvent/event_tree",
		      {argv + 1, argv + argc});
  
  VRAnalyzer * vrana = new VRAnalyzer();
  vrana->SetVBVariables(4, 20, 50, 13, 10);
  vrana->SetProducers("pandoraNu",
		      "pandoraNu",
		      "pandoraCosmicHitRemoval",
		      "simpleFlashBeam",
		      "swtrigger");
  vrana->RunVertexQuality();

  processor.AddAnalyzer(vrana);
  processor.SetOutputFileName("VertexReconstruction.root");
  processor.Run();

  return 0;

}
