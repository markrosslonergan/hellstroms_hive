


TFile * ifile_sp = nullptr;
TFile * ifile_bnb = nullptr;
TFile * ifile_bnb_cosmic = nullptr;
TFile * ifile_cosmic = nullptr;
TFile * ifile_sp_cosmic = nullptr;

TFile * ofile = nullptr;

TTree * vertex_tree_sp = nullptr;
TTree * vertex_tree_bnb = nullptr;
TTree * vertex_tree_bnb_cosmic = nullptr;
TTree * vertex_tree_cosmic = nullptr;
TTree * vertex_tree_sp_cosmic = nullptr;

TTree * event_tree_sp = nullptr;
TTree * event_tree_bnb = nullptr;
TTree * event_tree_bnb_cosmic = nullptr;
TTree * event_tree_cosmic = nullptr;
TTree * event_tree_sp_cosmic = nullptr;

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

  /*  
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
  */

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

  /*
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
  */

  ifile_sp_cosmic = TFile::Open((dir+"runmv_sp_cosmic.root").c_str());
  if(!ifile_sp_cosmic) {
    std::cout << "Could not find file\n";
    exit(1);
  }
  event_tree_sp_cosmic = (TTree*)ifile_sp_cosmic->Get((gdir+"event_tree").c_str());
  vertex_tree_sp_cosmic = (TTree*)ifile_sp_cosmic->Get((gdir+"vertex_tree").c_str());  
  TTree * pot_tree_sp_cosmic = (TTree*)ifile_sp_cosmic->Get((gdir+"get_pot").c_str());
  pot_tree_sp_cosmic->SetBranchAddress("pot", &pot_sp_cosmic);
  pot_tree_sp_cosmic->GetEntry(0);

  ofile = TFile::Open("plotsupimp.root", "recreate");
  
}



void hist_errors(TH1 * hist, double scaling) {

  std::vector<double> errors;
  for(int i = 1; i <= hist->GetNbinsX(); ++i) {
    errors.push_back(hist->GetBinError(i)*scaling);
  }

  hist->Scale(scaling);
  TH1 * histe = (TH1*)hist->Clone();
  histe->SetName((std::string(hist->GetName()) + "e").c_str());
  for(int i = 1; i <= histe->GetNbinsX(); ++i) {
    histe->SetBinError(i, errors.at(i-1));
  }
  histe->SetLineColor(hist->GetLineColor());
  histe->SetLineWidth(hist->GetLineWidth());
  histe->SetMarkerColor(hist->GetLineColor());

  histe->Draw("hist same");

}



void plotsupimp(std::string const & cname,
		std::string const & draw,
		std::string const & binning = "",
		std::string we = "",
		std::string opt = "",
		std::string const & title = "",
		std::string const & xtitle = "",
		std::string const & ytitle = "",
		bool const log = false,
		bool const vertex_tree = true,
		int const setstats = 0) {
  
  std::string we2 = we;
  if(we != "") we += " && ";

  int const color_offset = 2;

  TCanvas * canvas = new TCanvas(cname.c_str());

  if(vertex_tree) vertex_tree_sp->Draw((draw+">>h"+binning).c_str(), (we + signal_definition).c_str(), opt.c_str());
  else event_tree_sp->Draw((draw+">>h"+binning).c_str(), (we + signal_definition).c_str(), opt.c_str());
  TH1 * h = (TH1*)gDirectory->Get("h");
  h->SetStats(setstats);
  h->SetLineColor(kBlue+color_offset);
  h->SetLineWidth(3);
  h->SetTitle(title.c_str());
  h->GetXaxis()->SetTitle(xtitle.c_str());
  h->GetXaxis()->CenterTitle(); 
  h->GetYaxis()->SetTitle(ytitle.c_str());
  h->GetYaxis()->CenterTitle();
  hist_errors(h, 1./h->Integral(1,h->GetNbinsX()));
  double ymax = h->GetBinContent(h->GetMaximumBin());
  if(opt != "") opt += " && same";
  else opt = "same";
 
  /*
  if(vertex_tree) vertex_tree_bnb->Draw((draw+">>h2"+binning).c_str(), we2.c_str(), opt.c_str());
  else event_tree_bnb->Draw((draw+">>h2"+binning).c_str(), we2.c_str(), opt.c_str());
  TH1 * h2 = (TH1*)gDirectory->Get("h2");
  h2->SetLineColor(kRed+color_offset);
  h2->SetLineWidth(3);
  hist_errors(h2, 1./h2->Integral(1,h2->GetNbinsX()+1));
  if(h2->GetBinContent(h2->GetMaximumBin()) > ymax) ymax = h2->GetBinContent(h2->GetMaximumBin());
  */ 

  if(vertex_tree) vertex_tree_bnb_cosmic->Draw((draw+">>h3"+binning).c_str(), we2.c_str(), opt.c_str());
  else event_tree_bnb_cosmic->Draw((draw+">>h3"+binning).c_str(), we2.c_str(), opt.c_str());
  TH1 * h3 = (TH1*)gDirectory->Get("h3");
  h3->SetLineColor(kGreen+color_offset);
  h3->SetLineWidth(3);
  hist_errors(h3, 1./h3->Integral(1,h3->GetNbinsX()));
  if(h3->GetBinContent(h3->GetMaximumBin()) > ymax) ymax = h3->GetBinContent(h3->GetMaximumBin());  
 
  /*
  if(vertex_tree) vertex_tree_cosmic->Draw((draw+">>h4"+binning).c_str(), we2.c_str(), opt.c_str());
  else event_tree_cosmic->Draw((draw+">>h4"+binning).c_str(), we2.c_str(), opt.c_str()); 
  TH1 * h4 = (TH1*)gDirectory->Get("h4");
  h4->SetLineColor(kMagenta+color_offset);
  h4->SetLineWidth(3);
  hist_errors(h4, 1./h4->Integral(1,h4->GetNbinsX()));
  if(h4->GetBinContent(h4->GetMaximumBin()) > ymax) ymax = h4->GetBinContent(h4->GetMaximumBin());
  */

  if(vertex_tree) vertex_tree_sp_cosmic->Draw((draw+">>h5"+binning).c_str(), we2.c_str(), opt.c_str());
  else event_tree_sp_cosmic->Draw((draw+">>h5"+binning).c_str(), we2.c_str(), opt.c_str());
  TH1 * h5 = (TH1*)gDirectory->Get("h5");
  h5->SetLineColor(kCyan+color_offset);
  h5->SetLineWidth(3);
  hist_errors(h5, 1./h5->Integral(1,h5->GetNbinsX()));
  if(h5->GetBinContent(h5->GetMaximumBin()) > ymax) ymax = h5->GetBinContent(h5->GetMaximumBin());  
  

  h->GetYaxis()->SetRangeUser(0, ymax*1.6);
  TLegend * l = new TLegend(0.6, 0.9, 0.9, 0.6);
  l->AddEntry(h, "NC #Delta Rad");
  l->AddEntry(h5, "NC #Delta Rad + Cosmic");
  //l->AddEntry(h2, "BNB Inclusive");
  l->AddEntry(h3, "BNB + Cosmic");
  //l->AddEntry(h4, "Cosmic");
  l->Draw();

  canvas->Write();
  delete canvas;
  delete h;
  //delete h2;
  delete h3;
  //delete h4;
  delete h5;

}



void plotsupimp(std::string const dir = "") {

  init(dir);
  
  std::string acut = "passed_swtrigger == 1";

  plotsupimp("closest_asso_shower_dist_to_flashzcenter",
	     "closest_asso_shower_dist_to_flashzcenter",
	     "(25, 0, 200)",
	     acut,
	     "",
	     "",
	     "Distance Between Associated Shower and Closest In-Time Flash Z [cm]",
	     "Area Normalized");

  plotsupimp("totalpe_ibg_sum",
	     "totalpe_ibg_sum",
	     "(25, 0, 2000)",
	     acut,
	     "",
	     "",
	     "Total Reconstructed In-Time PE",
	     "Area Normalized");

  plotsupimp("totalpe_bbg_sum",
	     "totalpe_bbg_sum",
	     "(25, 0, 200)",
	     acut,
	     "",
	     "",
	     "Total Reconstructed Before-Beam PE",
	     "Area Normalized");

  plotsupimp("summed_associated_reco_shower_energy",
	     "summed_associated_reco_shower_energy",
	     "(25, 0, 0.5)",
	     acut,
	     "",
	     "",
	     "Summed Associated Shower Reco Energy [GeV]",
	     "Area Normalized");

  plotsupimp("summed_associated_helper_shower_energy",
	     "summed_associated_helper_shower_energy",
	     "(25, 0, 0.5)",
	     acut,
	     "",
	     "",
	     "Summed Associated Shower Helper Energy [GeV]",
	     "Area Normalized");

  plotsupimp("reco_nu_vtx_dist_to_closest_tpc_wall",
             "reco_nu_vtx_dist_to_closest_tpc_wall",
             "(25, 0, 125)",
             acut,
             "",
             "",
             "Reco Vtx Distance to Closest TPC Wall [cm]",
             "Area Normalized");

  plotsupimp("most_energetic_shower_reco_thetaxz",
             "most_energetic_shower_reco_thetaxz",
             "(25, -1.7, 1.7)",
             acut,
             "",
             "",
             "Most Energetic Shower #theta_{xz} [radians]",
             "Area Normalized");  

  plotsupimp("most_energetic_shower_reco_thetayz",
             "most_energetic_shower_reco_thetayz",
             "(25, -1.7, 1.7)",
             acut,
             "",
             "",
             "Most Energetic Shower #theta_{yz} [radians]",
             "Area Normalized");  

  plotsupimp("most_energetic_shower_bp_dist_to_tpc",
             "most_energetic_shower_bp_dist_to_tpc",
             "(25, 0, 1000)",
             acut,
             "",
             "",
             "Backwards Procted Shower Distance to TPC Wall [cm]",
             "Area Normalized");

  plotsupimp("reco_shower_dedx_plane0",
             "reco_shower_dedx_plane0",
             "(48, 0, 10)",
             acut,
             "",
             "Plane 0",
             "Most Energetic Shower dE/dx [MeV/cm]",
             "Area Normalized");  

  plotsupimp("reco_shower_dedx_plane1",
             "reco_shower_dedx_plane1",
             "(48, 0, 10)",
             acut,
             "",
             "Plane 1",
             "Most Energetic Shower dE/dx [MeV/cm]",
             "Area Normalized");  

  plotsupimp("reco_shower_dedx_plane2",
             "reco_shower_dedx_plane2",
             "(48, 0, 10)",
             acut,
             "",
             "Plane 2",
             "Most Energetic Shower dE/dx [MeV/cm]",
             "Area Normalized");  

  plotsupimp("shortest_asso_shower_to_vert_dist",
             "shortest_asso_shower_to_vert_dist",
             "(25, 0, 10)",
             acut + " && reco_asso_tracks == 1",
             "",
             "",
             "Distance Between Closest Associated Shower Start and Reco Vtx [cm]",
             "Area Normalized");

  plotsupimp("longest_asso_track_thetaxz",
	     "longest_asso_track_thetaxz",
             "(25, -1.7, 1.7)",
             acut,
             "",
             "",
             "Longest Associated Track #theta_{xz} [radians]",
             "Area Normalized");  

  plotsupimp("longest_asso_track_thetayz",
             "longest_asso_track_thetayz",
             "(25, -1.7, 1.7)",
             acut,
             "",
             "",
             "Longest Associated Track #theta_{yz} [radians]",
             "Area Normalized");  

  plotsupimp("reco_asso_tracks",
             "reco_asso_tracks",
             "(5, 0, 5)",
             acut,
             "",
             "",
             "Number of Associated Tracks",
             "Area Normalized");  

  plotsupimp("longest_asso_track_displacement",
	     "longest_asso_track_displacement",
	     "(25, 0, 300)",
	     acut,
	     "",
	     "",
	     "Longest Associated Track Length [cm]",
	     "Area Normalized");

}
