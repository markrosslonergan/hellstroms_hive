

#include "load_trees.hpp"


void cout_efficiency(TTree * tree, std::string const & cut_str, double const scaling = 1, std::string const & pre_cut_str = "") {

  int total = tree->GetEntries(pre_cut_str.c_str());
  std::string andstr;
  if(pre_cut_str != "") andstr = " && ";
  int cut = tree->GetEntries((cut_str + andstr + pre_cut_str).c_str());

  //std::cout << "Total: " << cut * scaling << " Cut: " << cut_str << ": " << cut << " " << double(cut) / total * 100 << " %\n";
  std::cout << cut * scaling << " " << double(cut) / total * 100 << " %\n";

}


void cout_cut(load_trees const & lt, std::string const & cut) {

  double const run_pot = 6.6e20;

  std::string const sd = "is_delta_rad == 1 && true_nu_vtx_fid_contained == 1";

  std::cout << "sp: ";
  cout_efficiency(lt.vertex_tree_sp, cut, run_pot / lt.pot_sp, sd);
  std::cout << "sp_cosmic: ";
  cout_efficiency(lt.vertex_tree_sp_cosmic, cut, run_pot / lt.pot_sp_cosmic, sd);
  std::cout << "bnb_cosmic: ";
  cout_efficiency(lt.vertex_tree_bnb_cosmic, cut, run_pot / lt.pot_bnb_cosmic);
  std::cout << "\n";

}



void cout_mva(load_trees const & lt, std::string const & cut) {
  
  double const run_pot = 6.6e20;

  std::string const sd = "is_delta_rad == 1 && true_nu_vtx_fid_contained == 1";

  std::cout << "sp: ";
  cout_efficiency(lt.vertex_tree_sp, cut + " && ((reco_asso_tracks == 0 && ncdelta_BDTD.mva > "+std::to_string(0.1)+") || (reco_asso_tracks > 0 && ncdelta_BDTD.mva > "+std::to_string(0.1)+"))", run_pot / lt.pot_sp, sd);
  std::cout << "sp_cosmic: ";
  cout_efficiency(lt.vertex_tree_sp_cosmic, cut + " && ((reco_asso_tracks == 0 && ncdelta_cosmic_BDTD.mva > "+std::to_string(0.1)+") || (reco_asso_tracks > 0 && ncdelta_cosmic_BDTD.mva > "+std::to_string(0.1)+"))", run_pot / lt.pot_sp_cosmic, sd);
  std::cout << "bnb_cosmic: ";
  cout_efficiency(lt.vertex_tree_bnb_cosmic, cut + " && ((reco_asso_tracks == 0 && bnb_cosmic_BDTD.mva > "+std::to_string(0.1)+") || (reco_asso_tracks > 0 && bnb_cosmic_BDTD.mva > "+std::to_string(0.1)+"))", run_pot / lt.pot_bnb_cosmic);
  std::cout << "\n";

}



void get_efficiency(std::string const dir) {

  load_trees lt(dir, "runtmva_app.root");

  std::string cut = "passed_swtrigger ==1 && reco_asso_showers == 1";
  cout_cut(lt, cut);

  cut += " && totalpe_ibg_sum > 140";
  cout_cut(lt, cut);

  cut += " && closest_asso_shower_dist_to_flashzcenter <= 40";
  cout_cut(lt, cut);

  cut += " && reco_asso_tracks > 0";
  cout_cut(lt, cut);  

  cout_mva(lt, cut);

}
