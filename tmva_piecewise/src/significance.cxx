
#include "significance.h"


std::pair<double, double> method_best_significance(std::string const & app_file_path, 
						   double const run_pot,
						   std::vector<std::pair<TTree *, std::string>> const & signal_trees, 
						   std::vector<std::string> const & signal_tree_cuts, 
						   std::vector<std::pair<int, double>> const & signal_pots, 
						   std::vector<std::pair<TTree *, std::string>> const & background_trees,
						   std::vector<std::string> const & background_tree_cuts, 
						   std::vector<std::pair<int, double>> const & background_pots,
						   std::string const & method) {
  
  for(auto const & p : signal_trees) {
    p.first->AddFriend((p.second + "_" + method).c_str(), app_file_path.c_str());
  }    
  for(auto const & p : background_trees) {
    p.first->AddFriend((p.second + "_" + method).c_str(), app_file_path.c_str());
  }    
  
  double best_significance = 0;
  double best_mva_cut = DBL_MAX;

  for(double d = -1.5; d < 1.5; d += 0.1) {
    
    double signal = 0;

    for(size_t i = 0; i < signal_trees.size(); ++i) {
      auto const & p = signal_trees.at(i);
      signal += p.first->GetEntries((signal_tree_cuts.at(i) + " && " + p.second + "_" + method + ".mva > " + std::to_string(d)).c_str()) * run_pot / signal_pots.at(i).second;
    }
    
    double background = 0;
    
    for(size_t i = 0; i < background_trees.size(); ++i) {
      auto const & p = background_trees.at(i);
      background += p.first->GetEntries((background_tree_cuts.at(i) + " && " + p.second + "_" + method + ".mva > " + std::to_string(d)).c_str()) * run_pot / background_pots.at(i).second;
    }

    
    if(signal/sqrt(signal + background) > best_significance) {
      best_significance = signal/sqrt(signal + background);
      best_mva_cut = d;
    }
    
  }

  return std::pair<double, double>(best_mva_cut, best_significance);
  
}


void cout_efficiency(TTree * tree, std::string const & name, std::string const & tree_cut, double const best_cut, 
		     std::string const & method, double const scale) {

  int const total = tree->GetEntries();
  int const pre_cut = tree->GetEntries(tree_cut.c_str());
  std::string cut;
  if(tree_cut != "") cut = tree_cut + " && ";
  int const mva_cut = tree->GetEntries((cut + name + "_" + method + ".mva > " + std::to_string(best_cut)).c_str());
  std::cout << name << ":\n"
	    << "Total Vertices: " << total;
  if(scale != 1)	    
    std::cout << " Scaled: " << total * scale;
  std::cout << "\nWith pre-cut: " << pre_cut;
  if(scale != 1)	    
    std::cout << " Scaled: " << pre_cut * scale;
  std::cout << " TE: " << double(pre_cut) / total * 100
	    << "\nWith MVA: " << mva_cut << " ";
  if(scale != 1)	    
    std::cout << " Scaled: " << mva_cut * scale;
  std::cout << " RE: " << double(mva_cut) / pre_cut * 100 << " TE: " << double(mva_cut) / total * 100 << "\n";

}


void cout_best_cut(std::pair<double, double> const & best_cut,
		   double const run_pot,
		   std::vector<std::pair<TTree *, std::string>> const & signal_trees, 
		   std::vector<std::string> const & signal_tree_cuts,
		   std::vector<std::pair<int, double>> const & signal_pots, 
		   std::vector<std::pair<TTree *, std::string>> const & background_trees, 
		   std::vector<std::string> const & background_tree_cuts,
		   std::vector<std::pair<int, double>> const & background_pots,
		   std::string const & method) {

  std::cout << "Method: " << method << "\n"
	    << "=============================================================\n"  
	    << "Cut: " << best_cut.first << " Significance: " << best_cut.second << " Scaled to: " << run_pot << "\n";
  for(size_t i = 0; i < signal_trees.size(); ++i) {
    cout_efficiency(signal_trees.at(i).first, signal_trees.at(i).second, signal_tree_cuts.at(i), best_cut.first, method, run_pot / signal_pots.at(i).second);
  }

  for(size_t i = 0; i < background_trees.size(); ++i) {
    cout_efficiency(background_trees.at(i).first, background_trees.at(i).second, background_tree_cuts.at(i), best_cut.first, method, run_pot / background_pots.at(i).second);
  }

  std::cout << "\n";

}


void significance(std::string const & app_file_path, 
		  double const run_pot,
		  std::vector<std::pair<TTree *, std::string>> const & signal_trees,
		  std::vector<std::string> const & signal_tree_cuts, 
		  std::vector<std::pair<int, double>> const & signal_pots, 
		  std::vector<std::pair<TTree *, std::string>> const & background_trees, 
		  std::vector<std::string> const & background_tree_cuts,
		  std::vector<std::pair<int, double>> const & background_pots,
		  std::vector<method_struct> const methods) {

  for(method_struct const & method : methods) {
    cout_best_cut(method_best_significance(app_file_path, run_pot, 
					   signal_trees, signal_tree_cuts, signal_pots, 
					   background_trees, background_tree_cuts, background_pots, 
					   method.str),
		  run_pot,
		  signal_trees, signal_tree_cuts, signal_pots, 
		  background_trees, background_tree_cuts, background_pots,
		  method.str);
  }
  
}


std::vector<double> method_best_significance_seperate(std::string const & app_file_path, 
						      double const run_pot,
						      std::vector<std::pair<TTree *, std::string>> const & signal_trees, 
						      std::vector<std::pair<std::string, std::string>> const & signal_tree_cuts, 
						      std::vector<std::pair<int, double>> const & signal_pots, 
						      std::vector<std::pair<TTree *, std::string>> const & background_trees,
						      std::vector<std::pair<std::string, std::string>> const & background_tree_cuts, 
						      std::vector<std::pair<int, double>> const & background_pots,
						      std::string const & method) {
    
  double best_significance = 0;
  double best_mva_cut = DBL_MAX;
  double best_mva_cut2 = DBL_MAX;

  for(double d = -1.5; d < 1.5; d += 0.1) {
    
    double signal = 0;

    for(size_t i = 0; i < signal_trees.size(); ++i) {
      auto const & p = signal_trees.at(i);
      signal += p.first->GetEntries((signal_tree_cuts.at(i).first + " && " + p.second + "_" + method + ".mva > " + std::to_string(d)).c_str()) * run_pot / signal_pots.at(i).second;
    }
    
    double background = 0;
    
    for(size_t i = 0; i < background_trees.size(); ++i) {
      auto const & p = background_trees.at(i);
      background += p.first->GetEntries((background_tree_cuts.at(i).first + " && " + p.second + "_" + method + ".mva > " + std::to_string(d)).c_str()) * run_pot / background_pots.at(i).second;
    }

    for(double d2 = -1.5; d2 < 1.5; d2 += 0.1) {
      
      double signal2 = signal;

      for(size_t i = 0; i < signal_trees.size(); ++i) {
	auto const & p = signal_trees.at(i);
	signal2 += p.first->GetEntries((signal_tree_cuts.at(i).second + " && " + p.second + "_" + method + ".mva > " + std::to_string(d2)).c_str()) * run_pot / signal_pots.at(i).second;
      }

      double background2 = background;

      for(size_t i = 0; i < background_trees.size(); ++i) {
	auto const & p = background_trees.at(i);
	background2 += p.first->GetEntries((background_tree_cuts.at(i).second + " && " + p.second + "_" + method + ".mva > " + std::to_string(d2)).c_str()) * run_pot / background_pots.at(i).second;
      }

      if(signal2/sqrt(signal2 + background2) > best_significance) {
	best_significance = signal2/sqrt(signal2 + background2);
	best_mva_cut = d;
	best_mva_cut2 = d2;
      }

    }
    
  }

  return std::vector<double>{best_mva_cut, best_mva_cut2, best_significance};
  
}


void cout_efficiency_seperate(TTree * tree, std::string const & name, TTree * tree_friend,
			      std::string const & tree_cut1, std::string const & tree_cut2, double const best_cut1, double const best_cut2, 
			      std::string const & method, double const scale ) {

  int const total = tree->GetEntries();
  int const pre_cut = tree->GetEntries(tree_cut1.c_str()) + tree->GetEntries(tree_cut2.c_str());
  std::string cut1;
  if(tree_cut1 != "") cut1 = tree_cut1 + " && ";
  std::string cut2;
  if(tree_cut2 != "") cut2 = tree_cut2 + " && ";
  int const mva_cut = tree->GetEntries(("(" + cut1 + name + "_" + method + ".mva > " + std::to_string(best_cut1) + ") || (" + cut2 + name + "_" + method + ".mva > " + std::to_string(best_cut2) + ")").c_str());
  std::cout << name << ":\n"
	    << "Total Vertices: " << total;
  if(scale != 1)	    
    std::cout << " Scaled: " << total * scale;
  std::cout << " Vtx/Evts: " << get_vertices_per_event(tree) << "\n"
	    << "With pre-cut: " << pre_cut;
  if(scale != 1)	    
    std::cout << " Scaled: " << pre_cut * scale;
  std::cout << " TE: " << double(pre_cut) / total * 100 << " Vtx/Evts: " << get_vertices_per_event(tree, tree_cut1) << "\n"
	    << "With MVA: " << mva_cut << " ";
  if(scale != 1)	    
    std::cout << " Scaled: " << mva_cut * scale;
  std::cout << " RE: " << double(mva_cut) / pre_cut * 100 << " TE: " << double(mva_cut) / total * 100 << " Vtx/Evts: " << get_vertices_per_event(tree, tree_cut1, tree_cut2, tree_friend, best_cut1, best_cut2) << "\n";

}


void cout_best_cut_seperate(std::vector<double> const & best_cut,
			    double const run_pot,
			    std::vector<std::pair<TTree *, std::string>> const & signal_trees, 
			    std::vector<std::pair<std::string, std::string>> const & signal_tree_cuts,
			    std::vector<std::pair<int, double>> const & signal_pots,
			    std::vector<TTree*> const & signal_tree_friends, 
			    std::vector<std::pair<TTree *, std::string>> const & background_trees, 
			    std::vector<std::pair<std::string, std::string>> const & background_tree_cuts,
			    std::vector<std::pair<int, double>> const & background_pots,
			    std::vector<TTree*> const & background_tree_friends,
			    std::string const & method) {

  std::cout << "Method: " << method << "\n"
	    << "=============================================================\n"  
	    << "Cut1: " << best_cut.at(0) << " Cut2: " << best_cut.at(1) << " Significance: " << best_cut.at(2) << " Scaled to: " << run_pot << "\n";
  for(size_t i = 0; i < signal_trees.size(); ++i) {
    cout_efficiency_seperate(signal_trees.at(i).first, signal_trees.at(i).second, signal_tree_friends.at(i), signal_tree_cuts.at(i).first, signal_tree_cuts.at(i).second, best_cut.at(0), best_cut.at(1), method, run_pot / signal_pots.at(i).second);
  }

  for(size_t i = 0; i < background_trees.size(); ++i) {
    cout_efficiency_seperate(background_trees.at(i).first, background_trees.at(i).second, background_tree_friends.at(i), background_tree_cuts.at(i).first, background_tree_cuts.at(i).second, best_cut.at(0), best_cut.at(1), method, run_pot / background_pots.at(i).second);
  }

  std::cout << "\n";

}


void significance_seperate(std::string const & app_file_path, 
			   double const run_pot,
			   std::vector<std::pair<TTree *, std::string>> const & signal_trees,
			   std::vector<std::pair<std::string, std::string>> const & signal_tree_cuts, 
			   std::vector<std::pair<int, double>> const & signal_pots, 
			   std::vector<std::pair<TTree *, std::string>> const & background_trees, 
			   std::vector<std::pair<std::string, std::string>> const & background_tree_cuts,
			   std::vector<std::pair<int, double>> const & background_pots,
			   std::vector<method_struct> const methods) {

  for(method_struct const & method : methods) {

    std::vector<TTree*> signal_tree_friends;
    for(auto const & p : signal_trees) {
      signal_tree_friends.push_back(p.first->AddFriend((p.second + "_" + method.str).c_str(), app_file_path.c_str())->GetTree());
    }    
    std::vector<TTree*> background_tree_friends;
    for(auto const & p : background_trees) {
      background_tree_friends.push_back(p.first->AddFriend((p.second + "_" + method.str).c_str(), app_file_path.c_str())->GetTree());
    }    
    
    cout_best_cut_seperate(method_best_significance_seperate(app_file_path, run_pot, 
							     signal_trees, signal_tree_cuts, signal_pots,
							     background_trees, background_tree_cuts, background_pots,
							     method.str),
			   run_pot,
			   signal_trees, signal_tree_cuts, signal_pots, signal_tree_friends,
			   background_trees, background_tree_cuts, background_pots, background_tree_friends,
			   method.str);
  }
  
}
