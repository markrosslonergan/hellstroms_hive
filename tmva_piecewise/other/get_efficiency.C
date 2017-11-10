

#include "load_trees.hpp"


void cout_efficiency(TTree * tree, std::string const cut_str) {

  int total = tree->GetEntries();
  int cut = tree->GetEntries(cut_str.c_str());

  std::cout << "Total: " << total << " Cut - " << cut_str << ": " << cut << " Efficiency: " << double(cut) / total * 100 << " %\n";

}


void get_efficiency(std::string const dir) {

  load_trees lt(dir);

  std::cout << "NC Delta Radiative: ";
  cout_efficiency(lt.vertex_tree_sp, "reco_asso_showers == 1");
  std::cout << "NC Delta Radiative + Cosmics: ";
  cout_efficiency(lt.vertex_tree_sp_cosmic, "reco_asso_showers == 1");
  std::cout << "BNB + Cosmics: ";
  cout_efficiency(lt.vertex_tree_bnb_cosmic, "reco_asso_showers == 1");

}
