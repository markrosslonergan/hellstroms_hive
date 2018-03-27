

#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "Analyzer.h"
#include "TFile.h"


class Processor {

 public:

  Processor(char const * pot_name,
	    char const * meta_name,
	    char const * event_name,
	    std::vector<char const *> const & files = {});
  ~Processor();

  void AddAnalyzer(Analyzer * analyzer);
  void SetOutputFileName(char const * name);

  void RunEvent(int const entry);
  void Run(int const entry = -1);

 private:

  Storage fstorage;
  std::vector<Analyzer *> fanalyzers;
  TFile * fofile;

};


#endif
