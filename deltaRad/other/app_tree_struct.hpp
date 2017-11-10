


struct app_tree_struct {

  TTree * tree;
  int is_delta_rad;
  int true_nu_vtx_fid_contained;
  int reco_asso_tracks;
  double mva;

  app_tree_struct(std::string const & tree_name, bool const extra = true) {
    tree = new TTree(tree_name.c_str(), "");
    if(extra) {
      tree->Branch("is_delta_rad", &is_delta_rad, "is_delta_rad/I");
      tree->Branch("true_nu_vtx_fid_contained", &true_nu_vtx_fid_contained, "true_nu_vtx_fid_contained/I");
      tree->Branch("reco_asso_tracks", &reco_asso_tracks, "reco_asso_tracks/I");
    }
    tree->Branch("mva", &mva, "mva/D");
  }

  ~app_tree_struct() {
    delete tree;
  }

};
