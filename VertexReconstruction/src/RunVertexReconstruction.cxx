#include <iostream>
#include <time.h>

#include "Storage.h"
#include "Processor.h"
#include "VRAnalyzer.h"
#include <string>

int main(int const argc, char const * argv[]) {

  time_t start = time(0);
  std::string timestamp = std::to_string((int)start); 


  Processor processor("FillLightEvent/pot_tree",
		      "FillLightEvent/meta_tree",
		      "FillLightEvent/event_tree",
		      {argv + 1, argv + argc});
  
  //VR
  VRAnalyzer * vrana = new VRAnalyzer("varana");
  vrana->SetVBVariables(4, 30, 100, 25, 5);
  //vrana->SetVBVariables(4, 20, 50, 13, 10);
  vrana->SetProducers("pandoraNu",
		      "pandoraNu",
		      "pandoraCosmicHitRemoval",
		      "simpleFlashBeam",
		      //"swtrigger");
		      "daq");
  vrana->RunPandora();
  vrana->GetPOT();
  vrana->RunFillTreeVariables();
  vrana->RunVerticesPerEvent();
  

  processor.AddAnalyzer(vrana);
  processor.SetOutputFileName(("RunVertexReconstruction_"+timestamp+".root").c_str());
  processor.Run();

  std::cout << "Wall time: " << difftime(time(0), start) << "\n";
  
  return 0;

}
