


#include "get_pot_info.h"



void get_pot_info(TTree * pot_tree, double & pot_total) {
  
  int i;
  get_pot_info(pot_tree, pot_total, i);
  
}

	

void get_pot_info(TTree * pot_tree, double & pot_total, int & event_total) {
  
  double pot;
  int events;
  
  pot_tree->SetBranchAddress("pot", &pot);
  pot_tree->SetBranchAddress("number_of_events", &events);
  
  pot_total = 0;
  event_total = 0;
  for(int i = 0; i < pot_tree->GetEntries(); ++i) {
    pot_tree->GetEntry(i);
    pot_total += pot;
    event_total += events;
  }
  
}
