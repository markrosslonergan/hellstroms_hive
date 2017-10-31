


#include "TFriendElement.h"



double get_vertices_per_event(TTree * vertex_tree, std::string const & cut1 = "", std::string const & cut2 = "", TTree * mva_tree = nullptr, double const mva1 = -DBL_MAX, double const mva2 = -DBL_MAX) {

  TTreeFormula * vertex_tf1 = nullptr;
  TTreeFormula * vertex_tf2 = nullptr;
    
  if(cut1 != "") vertex_tf1 = new TTreeFormula("vertex_tf1", cut1.c_str(), vertex_tree);
  if(cut2 != "") vertex_tf2 = new TTreeFormula("vertex_tf2", cut2.c_str(), vertex_tree);
 
  TTreeFormula * mva_tf1 = nullptr;
  TTreeFormula * mva_tf2 = nullptr;

  if(mva_tree) {
    mva_tf1 = new TTreeFormula("mva_tf1", ("mva > " + std::to_string(mva1)).c_str(), mva_tree);
    mva_tf2 = new TTreeFormula("mva_tf2", ("mva > " + std::to_string(mva2)).c_str(), mva_tree);    
  }  

  int number_of_events = 0;
  int number_of_vertices = 0;
  
  int event;
  vertex_tree->SetBranchAddress("event_number", &event);
  int previous_event = -1;
  
  for(int i = 0; i < vertex_tree->GetEntries(); ++i) {
    
    vertex_tree->GetEntry(i);
    
    if(mva_tree) {
      if(vertex_tf1->EvalInstance()) {
	mva_tree->GetEntry(i);
	if(mva_tf1->EvalInstance()) {
	  ++number_of_vertices;
	  if(event != previous_event) {
	    ++number_of_events;
	    previous_event = event;
	  }
	}
      }
	
      else if(vertex_tf2->EvalInstance()) {
	mva_tree->GetEntry(i);
	if(mva_tf2->EvalInstance()) {
	  ++number_of_vertices;
	  if(event != previous_event) {
	    ++number_of_events;
	    previous_event = event;
	  }
	}
      }
    }
    
    else {
      if(cut1 != "") {
	if(vertex_tf1->EvalInstance()) {
	  ++number_of_vertices;
	  if(event != previous_event) {
	    ++number_of_events;
	    previous_event = event;
	  }
	}
      }
      else {
	++number_of_vertices;
	if(event != previous_event) {
	  ++number_of_events;
	  previous_event = event;
	}
      }
    }

  }
    
  if(vertex_tf1) delete vertex_tf1;
  if(vertex_tf1) delete vertex_tf2;
  
  delete mva_tf1;
  delete mva_tf2;
  
  return double(number_of_vertices) / number_of_events;

}
