
template <typename T>
void compare_vb_template(char const * oldname, char const * newname, char const * asso) {

  TFile * oldfile = TFile::Open(oldname);
  TFile * newfile = TFile::Open(newname);

  TTree * oldtree = (TTree*)oldfile->Get("ERAlgoVertexBuilder");
  TTree * newtree = (TTree*)newfile->Get("LEEPhoton/VertexBuilder");

  int event_number;
  T var;
  
  oldtree->SetBranchAddress("event_id", &event_number);
  if(oldtree->SetBranchAddress(asso, &var) != 0) {
    std::cout << "Could not find " << asso << " in " << oldtree->GetName() << "\n";
    return;
  }
  
  newtree->SetBranchAddress("event_number", &event_number);
  if(newtree->SetBranchAddress(asso, &var) != 0) {
    std::cout << "Could not find " << asso << " in " << newtree->GetName() << "\n";
    return;
  }

  std::map<int, T> old_asso_map;
  std::map<int, T> new_asso_map;

  for(int i = 0; i < oldtree->GetEntries(); ++i) {
    oldtree->GetEntry(i);
    auto om_it = old_asso_map.find(event_number);
    if(om_it != old_asso_map.end()) cout << "Multiple same events old\n";
    old_asso_map.emplace(event_number, var);
  }

  for(int i = 0; i < newtree->GetEntries(); ++i) {
    newtree->GetEntry(i);
    auto nm_it = new_asso_map.find(event_number);
    if(nm_it != new_asso_map.end()) cout << "Multiple same events new\n";
    new_asso_map.emplace(event_number, var);
  }

  
  int mismatch = 0;
  for(auto const & op : old_asso_map) {
    auto const nm_it = new_asso_map.find(op.first);
    if(nm_it == new_asso_map.end()) cout << "Could not find event\n";
    if(op.second != nm_it->second) {
      cout << op.first << " " << op.second << " " << nm_it->second << "\n";
      ++mismatch;
    }
  }

  cout << mismatch << "\n";

}


template <typename T>
void compare_template(char const * oldname, char const * newname, char const * asso) {

  TFile * oldfile = TFile::Open(oldname);
  TFile * newfile = TFile::Open(newname);

  TTree * oldtree = (TTree*)oldfile->Get("vertex_tree");
  TTree * newtree = (TTree*)newfile->Get("LEEPhoton/vertex_tree");

  int event_number;
  T var;
  
  oldtree->SetBranchAddress("event_number", &event_number);
  if(oldtree->SetBranchAddress(asso, &var) != 0) {
    std::cout << "Could not find " << asso << " in " << oldtree->GetName() << "\n";
    return;
  }
  
  newtree->SetBranchAddress("event_number", &event_number);
  if(newtree->SetBranchAddress(asso, &var) != 0) {
    std::cout << "Could not find " << asso << " in " << newtree->GetName() << "\n";
    return;
  }

  std::vector<int> old_asso_key_v;
  std::vector<std::vector<T>> old_asso_var_v;
  std::vector<int> new_asso_key_v;
  std::vector<std::vector<T>> new_asso_var_v;

  for(int i = 0; i < oldtree->GetEntries(); ++i) {
    oldtree->GetEntry(i);
    auto om_it = std::find(old_asso_key_v.begin(), old_asso_key_v.end(), event_number);
    if(om_it == old_asso_key_v.end()) {
      old_asso_key_v.push_back(event_number);
      old_asso_var_v.push_back(std::vector<T>(1, var));
    }
    else {
      old_asso_var_v.at(om_it-old_asso_key_v.begin()).push_back(var);
    }
  }
  if(old_asso_key_v.size() != old_asso_var_v.size()) std::cout << "Old vectors different size\n";

  for(int i = 0; i < newtree->GetEntries(); ++i) {
    newtree->GetEntry(i);
    auto nm_it = std::find(new_asso_key_v.begin(), new_asso_key_v.end(), event_number);
    if(nm_it == new_asso_key_v.end()) {
      new_asso_key_v.push_back(event_number);
      new_asso_var_v.push_back(std::vector<T>(1, var));
    }
    else {
      new_asso_var_v.at(nm_it-new_asso_key_v.begin()).push_back(var);
    }
  }  
  if(new_asso_key_v.size() != new_asso_var_v.size()) std::cout << "New vectors different size\n";

  for(int i = 0; i < old_asso_key_v.size(); ++i) {
    auto nm_it = std::find(new_asso_key_v.begin(), new_asso_key_v.end(), old_asso_key_v.at(i));
    if(nm_it == new_asso_key_v.end()) continue;
    std::vector<T> & old_var_v = old_asso_var_v.at(i);
    std::vector<T> & new_var_v = new_asso_var_v.at(nm_it-new_asso_key_v.begin());
    if(old_var_v.size() != new_var_v.size()) {
      std::cout << "Old and new var vectors different size\n";
      continue;
    }
    for(T const var : old_var_v) {
      auto nmv_it = std::find(new_var_v.begin(), new_var_v.end(), var);
      if(nmv_it == new_var_v.end()) {
	sort(old_var_v.begin(), old_var_v.end());
	sort(new_var_v.begin(), new_var_v.end());
       int large_diff_index = -1;
	for(int j = 0; j < old_var_v.size(); ++j) {
	  if(old_var_v.at(j) - new_var_v.at(j) > 1e-10)
	    large_diff_index = j;
	}
	if(large_diff_index != -1) {
	  std::cout << "Event: " << old_asso_key_v.at(i) << "\n";
	  for(int j = 0; j < old_var_v.size(); ++j) {
	    std::cout << "Old: " << old_var_v.at(j) << " " << "New: " << new_var_v.at(j);
	    if(j == large_diff_index) std::cout << " <---";
	    std::cout << "\n";
	  }
	}
      }
    }
  }

}


void compare(char const * oldname, char const * newname, std::string const type, char const * asso) {

  if(type == "int")
    compare_template<int>(oldname, newname, asso);
  else if(type == "double")
    compare_template<double>(oldname, newname, asso);
  else
    std::cout << "No function for that type\n";

}
