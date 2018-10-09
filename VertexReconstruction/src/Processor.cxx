

#include "Processor.h"


Processor::Processor(char const * pot_name,
		     char const * meta_name,
		     char const * event_name,
		     std::vector<char const *> const & files) :
  fstorage("FillLightEvent/pot_tree",
	   "FillLightEvent/meta_tree",
	   "FillLightEvent/event_tree",
	   files),
  fofile(nullptr),
  ffilter_dir(nullptr),
  fevent_tree(nullptr),
  fworkdir(nullptr){}

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
  ffilter_dir = fstorage.ffilter_dir;
  fevent_tree = fstorage.fevent_tree;

}


void Processor::RunEvent(int const entry) {

  fstorage.GetEvent(entry);
  
  bool pass = true;

  for(Analyzer * analyzer : fanalyzers) { 
    if(!analyzer->Run() && ffilter_dir) {
      pass = false;
      break;
    }
  }

  if(pass && ffilter_dir) {
    ffilter_dir->cd();
    fevent_tree->Fill();
    fworkdir->cd();
  }

}


void Processor::Run(int const entry) {

  if(fofile) fofile->cd();
  fworkdir = gDirectory;

  for(Analyzer * analyzer : fanalyzers) { 
    analyzer->SetOutputFile(fofile);
    analyzer->Initialize();
  }  

  if(entry < 0) {
  
    for(int i = 0; i < fstorage.fnumber_of_events; ++i) {

      if(i % 1 == 0) std::cout << "Entry: " << i << "\n";      
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

  if(ffilter_dir) fstorage.Write();

}
