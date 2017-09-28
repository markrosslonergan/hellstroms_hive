TFile * ifile_sp = nullptr;
TFile * ifile_bnb = nullptr;
TFile * ifile_bnb_cosmic = nullptr;
TFile * ifile_cosmic = nullptr;

TFile * ofile = nullptr;

TTree * vertex_tree_sp = nullptr;
TTree * vertex_tree_bnb = nullptr;
TTree * vertex_tree_bnb_cosmic = nullptr;
TTree * vertex_tree_cosmic = nullptr;

TTree * event_tree_sp = nullptr;
TTree * event_tree_bnb = nullptr;
TTree * event_tree_bnb_cosmic = nullptr;
TTree * event_tree_cosmic = nullptr;

double pot_sp = -1;
double pot_sp_cosmic = -1;
double pot_bnb = -1;
double pot_bnb_cosmic = -1;
double ngenbnbcosmic = -1;
double ngencosmic = -1;

std::string const signal_definition = "is_delta_rad == 1 && true_nu_vtx_fid_contained == 1";


void init(std::string dir) {

  if(dir != "") dir += "/";
  std::string gdir;
  if(dir.find("old") == std::string::npos) gdir = "LEEPhoton/";

  ifile_sp = TFile::Open((dir+"runmv_sp.root").c_str());
  if(!ifile_sp) {
    std::cout << "Could not find file\n";
    exit(1);
  }
  event_tree_sp = (TTree*)ifile_sp->Get((gdir+"event_tree").c_str());
  vertex_tree_sp = (TTree*)ifile_sp->Get((gdir+"vertex_tree").c_str());  
  TTree * pot_tree_sp = (TTree*)ifile_sp->Get((gdir+"get_pot").c_str());
  pot_tree_sp->SetBranchAddress("pot", &pot_sp);
  pot_tree_sp->GetEntry(0);
  
  ifile_bnb = TFile::Open((dir+"runmv_bnb.root").c_str());
  if(!ifile_bnb) {
    std::cout << "Could not find file\n";
    exit(1);
  }
  event_tree_bnb = (TTree*)ifile_bnb->Get((gdir+"event_tree").c_str());
  vertex_tree_bnb = (TTree*)ifile_bnb->Get((gdir+"vertex_tree").c_str());
  TTree * pot_tree_bnb = (TTree*)ifile_bnb->Get((gdir+"get_pot").c_str());
  pot_tree_bnb->SetBranchAddress("pot", &pot_bnb);
  pot_tree_bnb->GetEntry(0);

  ifile_bnb_cosmic = TFile::Open((dir+"runmv_bnb_cosmic.root").c_str());
  if(!ifile_bnb_cosmic) {
    std::cout << "Could not find file\n";
    exit(1);
  }
  event_tree_bnb_cosmic = (TTree*)ifile_bnb_cosmic->Get((gdir+"event_tree").c_str());
  vertex_tree_bnb_cosmic = (TTree*)ifile_bnb_cosmic->Get((gdir+"vertex_tree").c_str());
  TTree * pot_tree_bnb_cosmic = (TTree*)ifile_bnb_cosmic->Get((gdir+"get_pot").c_str());
  pot_tree_bnb_cosmic->SetBranchAddress("pot", &pot_bnb_cosmic);
  int temp_ngenbnbcosmic;
  pot_tree_bnb_cosmic->SetBranchAddress("number_of_events", &temp_ngenbnbcosmic);
  pot_tree_bnb_cosmic->GetEntry(0);
  ngenbnbcosmic = temp_ngenbnbcosmic;

  ifile_cosmic = TFile::Open((dir+"runmv_cosmic.root").c_str());
  if(!ifile_cosmic) {
    std::cout << "Could not find file\n";
    exit(1);
  }
  event_tree_cosmic = (TTree*)ifile_cosmic->Get((gdir+"event_tree").c_str());
  vertex_tree_cosmic = (TTree*)ifile_cosmic->Get((gdir+"vertex_tree").c_str());
  TTree * pot_tree_cosmic = (TTree*)ifile_cosmic->Get((gdir+"get_pot").c_str());
  int temp_ngencosmic;
  pot_tree_cosmic->SetBranchAddress("number_of_events", &temp_ngencosmic);
  pot_tree_cosmic->GetEntry(0);
  ngencosmic = temp_ngencosmic;

  std::cout << pot_sp << "\n"
	    << pot_bnb << "\n"
	    << pot_bnb_cosmic << "\n"
	    << ngenbnbcosmic << "\n"
	    << ngencosmic << "\n";

  ofile = TFile::Open("plot_dedx.root", "recreate");
  
}



void plot_2d(TTree * tree,
	     std::string const & cname,
	     std::string const & draw,
	     std::string const & binning,
	     std::string const & we,
	     std::string const & title = "",
	     std::string const & xtitle = "",
	     std::string const & ytitle = "") {

  TCanvas * canvas = new TCanvas(cname.c_str());
  tree->Draw((draw+">>h"+binning).c_str(), we.c_str(), "colz");
  TH2 * h = (TH2*)gDirectory->Get("h");
  h->SetStats(0);
  h->SetTitle(title.c_str());
  h->GetXaxis()->SetTitle(xtitle.c_str());
  h->GetXaxis()->CenterTitle();
  h->GetYaxis()->SetTitle(ytitle.c_str());
  h->GetYaxis()->CenterTitle();

  canvas->Write();

  delete canvas;
  delete h;

}



void plot_dedx_supimp(TTree * tree,
		      std::string const & cname,
		      std::string const & binning,
		      std::string const & title = "",
		      std::string const & xtitle = "",
		      std::string const & ytitle = "") {

  TCanvas * canvas = new TCanvas(cname.c_str());
  tree->Draw(("reco_shower_dedx_plane2>>h"+binning).c_str(), "", "hist");
  TH1 * h = (TH1*)gDirectory->Get("h");
  h->SetStats(0);
  h->SetTitle(title.c_str());
  h->GetXaxis()->SetTitle(xtitle.c_str());
  h->GetXaxis()->CenterTitle();
  h->GetYaxis()->SetTitle(ytitle.c_str());
  h->GetYaxis()->CenterTitle();
  h->Scale(1./h->Integral());
  double ymax = h->GetBinContent(h->GetMaximumBin());

  std::string const ecut = "0.2";
  tree->Draw(("reco_shower_dedx_plane2>>h2"+binning).c_str(), ("most_energetic_shower_reco_energy > " + ecut).c_str(), "hist same");
  TH1 * h2 = (TH1*)gDirectory->Get("h2");
  h2->SetLineColor(kRed);
  h2->Scale(1./h2->Integral());
  if(h2->GetBinContent(h2->GetMaximumBin()) > ymax) ymax = h2->GetBinContent(h2->GetMaximumBin());

  h->GetYaxis()->SetRangeUser(0, ymax*1.2);

  TLegend * legend = new TLegend(0.6, 0.9, 0.9, 0.6);
  legend->AddEntry(h, "All");
  legend->AddEntry(h2, ("Shower Energy > " + ecut + " GeV").c_str());
  legend->Draw();

  canvas->Write();

  delete canvas;
  delete h;

}



void plot_dedx(std::string const dir = "") {

  init(dir);

  plot_2d(vertex_tree_sp,
	  "sp2d",
	  "most_energetic_shower_reco_energy:reco_shower_dedx_plane2",
	  "(20, 0, 5, 20, 0, 0.5)",
	  "reco_shower_dedx_plane2 > -1",
	  "NC #Delta Radiative",
	  "Shower dE/dx [MeV/cm]",
	  "Shower Energy [GeV]");

  plot_2d(vertex_tree_bnb,
	  "bnb2d",
	  "most_energetic_shower_reco_energy:reco_shower_dedx_plane2",
	  "(20, 0, 5, 20, 0, 0.5)",
	  "reco_shower_dedx_plane2 > -1",
	  "BNB",
	  "Shower dE/dx [MeV/cm]",
	  "Shower Energy [GeV]");

  plot_2d(vertex_tree_bnb_cosmic,
	  "bnb_cosmic2d",
	  "most_energetic_shower_reco_energy:reco_shower_dedx_plane2",
	  "(20, 0, 5, 20, 0, 0.5)",
	  "reco_shower_dedx_plane2 > -1",
	  "BNB + Cosmic",
	  "Shower dE/dx [MeV/cm]",
	  "Shower Energy [GeV]");

  plot_2d(vertex_tree_cosmic,
	  "cosmic2d",
	  "most_energetic_shower_reco_energy:reco_shower_dedx_plane2",
	  "(20, 0, 5, 20, 0, 0.5)",
	  "reco_shower_dedx_plane2 > -1",
	  "In-Time Cosmic",
	  "Shower dE/dx [MeV/cm]",
	  "Shower Energy [GeV]");

  plot_dedx_supimp(vertex_tree_sp,
		   "dedx_supimp_sp",
		   "(40, 0, 5)",
		   "NC #Delta Radiative",
		   "Shower dE/dx [MeV/cm]",
		   "Area Normalized");

  plot_dedx_supimp(vertex_tree_bnb,
		   "dedx_supimp_bnb",
		   "(40, 0, 5)",
		   "BNB",
		   "Shower dE/dx [MeV/cm]",
		   "Area Normalized");

  plot_dedx_supimp(vertex_tree_bnb_cosmic,
		   "dedx_supimp_bnb_cosmic",
		   "(40, 0, 5)",
		   "BNB + Cosmic",
		   "Shower dE/dx [MeV/cm]",
		   "Area Normalized");
  
  plot_dedx_supimp(vertex_tree_cosmic,
		   "dedx_supimp_cosmic",
		   "(40, 0, 5)",
		   "In-Time Cosmic",
		   "Shower dE/dx [MeV/cm]",
		   "Area Normalized");

}
