

struct vinfo {

  void * ptr;
  std::string type;
  char const * branch_name;

};


void setup_new_tree(TTree * tree,
		    std::vector<vinfo> const & branch_v) {

  for(vinfo const & v : branch_v) {
    std::string const & type = v.type;
    if(type == "d") tree->SetBranchAddress(v.branch_name, (double*)v.ptr);
    else if(type == "i") tree->SetBranchAddress(v.branch_name, (int*)v.ptr);
  }

}


void test_variables_loop(char const * name1, char const * name2) {

  TFile * file1 = TFile::Open(name1);
  TFile * file2 = TFile::Open(name2);

  TTree * tree1 = (TTree*)file1->Get("vertex_tree");
  TTree * tree2 = (TTree*)file2->Get("LEEPhoton/vertex_tree");

  if(tree1->GetEntries() != tree2->GetEntries()) {
    std::cout << "TTrees have different number of entries\n";
    return;
  }

  double const prec = 1e-14;
  cout << setprecision(14);

  setup_new_tree(tree1);

  for(int i = 0; i < tree1->GetEntries(); ++i) {
    
    tree1->GetEntry(i);
    tree2->GetEntry(i);


  }

}
