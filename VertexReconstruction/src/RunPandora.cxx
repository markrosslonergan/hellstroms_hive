

#include <iostream>
#include <time.h>

#include "TFile.h"

#include "Processor.h"
#include "PandoraAnalyzer.h"


int main(int const argc, char const * argv[]) {

  time_t start = time(0);

  if(argc < 2) {
    std::cout << "Input: light_event file\n";
    return 1;
  }

  Processor processor("FillLightEvent/pot_tree",
		      "FillLightEvent/meta_tree",
		      "FillLightEvent/event_tree",
		      {argv + 1, argv + argc});
  processor.SetOutputFileName("RunPandora.root");

  PandoraAnalyzer * PAna = new PandoraAnalyzer;

  processor.AddAnalyzer(PAna);

  processor.Run();
  
  std::cout << "Wall time: " << difftime(time(0), start) << "\n";  

  return 0;

}
