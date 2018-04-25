#include <iostream>
#include <time.h>
#include <vector>
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
  
  //Bit of code to extract the jobsub id's from the path and append to the output filename
  std::vector<char const*>  files_input = {argv +1, argv+argc}; 
  std::string first_file = (files_input.at(0));
  std::vector<std::string> broken_down_first_file;
  std::string delimiter = "/";

  size_t pos = 0;
  std::string token;
  while ((pos = first_file.find(delimiter)) != std::string::npos) {
  	token = first_file.substr(0, pos);
	broken_down_first_file.push_back(token);
    	first_file.erase(0, pos + delimiter.length());
  }


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
  processor.SetOutputFileName(("RunVertexReconstruction_"+timestamp+"_"+broken_down_first_file.at(broken_down_first_file.size()-2)+"_"+broken_down_first_file.back()+".root").c_str());
  processor.Run();

  std::cout << "Wall time: " << difftime(time(0), start) << "\n";
  
  return 0;

}
