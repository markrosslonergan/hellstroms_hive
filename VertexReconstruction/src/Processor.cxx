

#include "Processor.h"


Processor::Processor(char const * pot_name,
		     char const * meta_name,
		     char const * event_name,
		     std::vector<char const *> const & files) :
  fstorage("FillLightEvent/pot_tree",
	   "FillLightEvent/meta_tree",
	   "FillLightEvent/event_tree",
	   files),
  fofile(nullptr){}


Processor::~Processor() {

  for(Analyzer * analyzer : fanalyzers) delete analyzer;
  if(fofile) fofile->Close();

}


void Processor::AddAnalyzer(Analyzer * analyzer) {

  analyzer->SetStorage(fstorage);
  fanalyzers.push_back(analyzer);

}


void Processor::SetOutputFileName(char const * name) {

  fofile = TFile::Open(name, "recreate");

}


void Processor::Run() {

  if(fofile) fofile->cd();
  for(Analyzer * analyzer : fanalyzers) { 
    analyzer->SetOutputFile(fofile);
    analyzer->Initialize();
  }  
  
  for(int i = 0; i < fstorage.fnumber_of_events; ++i) {
 
    fstorage.GetEvent(i);
    if(i % 100 == 0) std::cout << "Entry: " << i << "\n";

    for(Analyzer * analyzer : fanalyzers) { 
      analyzer->Run();
    }
 
  }

  for(Analyzer * analyzer : fanalyzers) { 
    analyzer->Finalize();
  }  

}
