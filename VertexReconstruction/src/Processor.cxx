

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


void Processor::SetOutputFilterFileName(char const * name) {

  fstorage.SetOutputFilterFileName(name);

}


void Processor::RunEvent(int const entry) {

  fstorage.GetEvent(entry);
  
  bool pass = true;

  for(Analyzer * analyzer : fanalyzers) { 
    if(!analyzer->Run()) {
      pass = false;
      break;
    }
  }

  if(pass && fstorage.fevent_tree) {
    fstorage.fevent_tree->Fill();
  }

}


void Processor::Run(int const entry) {

  if(fofile) fofile->cd();

  for(Analyzer * analyzer : fanalyzers) { 
    analyzer->SetOutputFile(fofile);
    analyzer->Initialize();
  }  

  if(entry < 0) {
  
    for(int i = 0; i < fstorage.fnumber_of_events; ++i) {

      if(i % 100 == 0) std::cout << "Entry: " << i << "\n";      
      RunEvent(i);
      
    }

  }

  else {

    if(entry >= fstorage.fnumber_of_events) {
      std::cout << "ERROR: requested entry larger than total number of events\n";
      exit(1);
    }

    RunEvent(entry);

  }

  for(Analyzer * analyzer : fanalyzers) { 
    analyzer->Finalize();
  }  

  if(fstorage.fevent_tree) fstorage.Write();

}
