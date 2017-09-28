


TFile * ofile = nullptr;



void draw_app(std::string const & name, std::string const & file_path, std::string const & cut, std::string const & method) {
  
  TFile * sig_file = TFile::Open("../samples/mcc82/runmv_sp.root");
  TTree * sig_tree = (TTree*)sig_file->Get("LEEPhoton/vertex_tree");
  
  sig_tree->AddFriend(("signal_"+method).c_str(), file_path.c_str());
  
  ofile->cd();

  TCanvas * canvasa = new TCanvas((name+"_canvas").c_str());
  sig_tree->Draw(("signal_"+method+".mva>>"+name).c_str(), cut.c_str());
  TH1 * h = (TH1*)gDirectory->Get(name.c_str());
  h->SetTitle(name.c_str());

  delete sig_tree;
  sig_file->Close();

}



void test_app() {

  ofile = TFile::Open("test_app.root", "recreate");

  std::string const all_cut = "closest_asso_shower_dist_to_flashzcenter <= 40 && totalpe_ibg_sum > 140 && reco_asso_showers == 1";
  std::string const all_cut_notrack = all_cut + " && reco_asso_tracks == 0";
  std::string const all_cut_trackonly = all_cut + " && reco_asso_tracks > 0";

  draw_app("all_notrack", "runtmva_app.root", all_cut_notrack, "BDTG");
  draw_app("notrack", "runtmva_notrack_app.root", all_cut_notrack, "BDTG");
  draw_app("all_trackonly", "runtmva_app.root", all_cut_trackonly, "BDTG");
  draw_app("trackonly", "runtmva_trackonly_app.root", all_cut_trackonly, "BDTG");

}
