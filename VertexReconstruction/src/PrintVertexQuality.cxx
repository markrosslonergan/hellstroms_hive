

#include <iostream>
#include <time.h>

#include "TFile.h"
#include "TTree.h"

int main(int const argc, char const * argv[]) {

  time_t start = time(0);

  if(argc != 2) {
    std::cout << "Input: vertex file\n";
    return 1;
  }

  std::string const weight = "tpc_volume_contained == 1";
  std::string const ratio_binning = "(101, 0, 1.01)";

  TFile * file = TFile::Open(argv[1]);
  TTree * vertex_tree = (TTree*)file->Get("vertex_quality_tree_shower_match");
  TTree * event_tree = (TTree*)file->Get("event_tree_shower_match");

  int true_nu_vtx_number;
  int reco_vtx_number;
  int tpc_volume_contained;
  int true_associated_shower;

  event_tree->SetBranchAddress("true_nu_vtx_number", &true_nu_vtx_number);
  event_tree->SetBranchAddress("reco_vtx_number", &reco_vtx_number);
  event_tree->SetBranchAddress("tpc_volume_contained", &tpc_volume_contained);
  event_tree->SetBranchAddress("true_associated_shower", &true_associated_shower);

  int sum = 0;

  for(int i = 0; i < event_tree->GetEntries(); ++i) {

    event_tree->GetEntry(i);

    sum += abs(true_nu_vtx_number - reco_vtx_number);
    
  }

  std::cout << sum << "\n";

  std::cout << "Wall time: " << difftime(time(0), start) << "\n";  

  return 0;

}
