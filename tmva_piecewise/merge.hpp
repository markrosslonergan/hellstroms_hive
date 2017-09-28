


#include "TTreeFormula.h"



template <class T>
void set_up_branch(std::string const & branch_name,
		   std::string const & type,
		   void_vec & branches1,
		   void_vec & branches2,
		   void_vec & branches_merged,
		   TTree * tree1,
		   TTree * tree2,
		   TTree * tree_merged) {
  
  branches1.push_back(new T(-1));
  branches2.push_back(new T(-1));
  branches_merged.push_back(new T(-1));
  tree1->SetBranchAddress(branch_name.c_str(), branches1.back<T>());
  tree2->SetBranchAddress(branch_name.c_str(), branches2.back<T>());
  tree_merged->Branch(branch_name.c_str(), branches_merged.back<T>(), (branch_name+"/"+type).c_str());    
  
}



TTree * merge_trees(TTree * tree1,
		    TTree * tree2,
		    std::string const & tree_merged_name,
		    std::vector<std::pair<std::string, std::string>> const & branches,
		    TTree * cut_tree,
		    std::string const & all_cut,
		    std::string const & cut1, 
		    std::string const & cut2) {
  
  if(cut_tree->GetEntries() != tree1->GetEntries() || tree1->GetEntries() != tree2->GetEntries()) {
    std::cout << "ERROR: " << __LINE__ << " " << __PRETTY_FUNCTION__ << "\n"
	      << "trees have different entries:\n"
	      << cut_tree->GetName() << ": " << cut_tree->GetEntries() << "\n"
	      << tree1->GetName() << ": " << tree1->GetEntries() << "\n"
	      << tree2->GetName() << ": " << tree2->GetEntries() << "\n";
    exit(1);
  }

  void_vec branches1;
  void_vec branches2;
  void_vec branches_merged;
  TTree * tree_merged = new TTree(tree_merged_name.c_str(), "");

  for(std::pair<std::string, std::string> const & p : branches) {
    if(p.second == "d") {
      set_up_branch<double>(p.first, "D", branches1, branches2, branches_merged, tree1, tree2, tree_merged);
    }
    else if(p.second == "i") {
      set_up_branch<int>(p.first, "I", branches1, branches2, branches_merged, tree1, tree2, tree_merged);
    }
    else {
      std::cout << __LINE__ << " " << __PRETTY_FUNCTION__ <<"\n"
		<< "ERROR: invalid type: " << p.second << "\n";
    }
  }

  TTreeFormula * tf = new TTreeFormula("tf", all_cut.c_str(), cut_tree);
  TTreeFormula * tf1 = new TTreeFormula("tf1", (all_cut + " && " + cut1).c_str(), cut_tree);
  TTreeFormula * tf2 = new TTreeFormula("tf2", (all_cut + " && " + cut2).c_str(), cut_tree);

  for(int i = 0; i < cut_tree->GetEntries(); ++i) {

    cut_tree->GetEntry(i);
    tree1->GetEntry(i);
    tree2->GetEntry(i);

    for(size_t j = 0; j < branches.size(); ++j) {
      
      if(tf->EvalInstance()) {
	
	if(tf1->EvalInstance()) {
	  if(branches.at(j).second == "d")
	    *branches_merged.at<double>(j) = *branches1.at<double>(j);
	  else if(branches.at(j).second == "i")
	    *branches_merged.at<int>(j) = *branches1.at<int>(j);
	}
	
	else if(tf2->EvalInstance()) {
	  if(branches.at(j).second == "d")
	  *branches_merged.at<double>(j) = *branches2.at<double>(j);
	  else if(branches.at(j).second == "i")
	    *branches_merged.at<int>(j) = *branches2.at<int>(j);
	}
	
	else {
	  std::cout << "WARINING:\n" << __LINE__ << " " << __PRETTY_FUNCTION__ << "\nNeither cut evaluted true\n";
	}
	
      }

      tree_merged->Fill();

    }

  }

  delete tf;
  delete tf1;
  delete tf2;

  return tree_merged;

}



void merge(std::string const & ofile_path,
	   std::string const & file_path1,
	   std::string const & file_path2,
	   std::vector<std::pair<TTree *, std::string>> const & trees,
	   std::vector<method_struct> const & methods,
	   std::vector<std::pair<std::string, std::string>> const & branches,
	   std::string const & all_cut,
	   std::string const & cut1,
	   std::string const & cut2) {

  object_helper oh;
  TFile * ofile = TFile::Open(ofile_path.c_str(), "recreate");

  for(method_struct const & method : methods) {

    for(auto const & p : trees) {

      TTree * tree1 = oh.GetObject<TTree>(file_path1, p.second+"_"+method.str);
      TTree * tree2 = oh.GetObject<TTree>(file_path2, p.second+"_"+method.str);

      ofile->cd();

      TTree * tree_merged = merge_trees(tree1, tree2, p.second+"_"+method.str, branches, p.first, all_cut, cut1, cut2);
      tree_merged->Write();
      delete tree_merged;

    }

  }

  ofile->Close();

}
