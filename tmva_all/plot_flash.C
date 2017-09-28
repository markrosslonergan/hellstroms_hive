

TFile * file_sp = nullptr;
TFile * file_sp_cosmic = nullptr;
TFile * file_bnb_cosmic = nullptr;
TFile * ofile = nullptr;


TTree * tree_sp = nullptr;
TTree * tree_sp_cosmic = nullptr;
TTree * tree_bnb_cosmic = nullptr;


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


void plot(std::string const & name,
	  std::string const & draw,
	  std::string const & binning,
	  std::string const & cut,
	  std::string const & title = "",
	  std::string const & xtitle = "",
	  std::string const & ytitle = "") {

  int color_offset = 2;
  TCanvas * canvas = new TCanvas(name.c_str());

  tree_sp->Draw((draw+">>hsp"+binning).c_str(), cut.c_str());
  TH1 * hsp = (TH1*)gDirectory->Get("hsp");
  hsp->SetStats(0);
  hsp->SetLineColor(kBlue+color_offset);
  hsp->SetTitle(title.c_str());
  hsp->GetXaxis()->SetTitle(xtitle.c_str());
  hsp->GetXaxis()->CenterTitle();
  hsp->GetYaxis()->SetTitle(ytitle.c_str());
  hsp->GetYaxis()->CenterTitle();
  hist_errors(hsp, 1./hsp->Integral(1, hsp->GetNbinsX()));

  tree_sp_cosmic->Draw((draw+">>hspc"+binning).c_str(), cut.c_str(), "same");
  TH1 * hspc = (TH1*)gDirectory->Get("hspc");
  hspc->SetStats(0);
  hspc->SetLineColor(kCyan+color_offset);
  hist_errors(hspc, 1./hspc->Integral(1, hspc->GetNbinsX()));

  tree_bnb_cosmic->Draw((draw+">>hbnbc"+binning).c_str(), cut.c_str(), "same");
  TH1 * hbnbc = (TH1*)gDirectory->Get("hbnbc");
  hbnbc->SetStats(0);
  hbnbc->SetLineColor(kGreen+color_offset);
  hist_errors(hbnbc, 1./hbnbc->Integral(1, hbnbc->GetNbinsX()));

  TLegend * leg = new TLegend(0.6, 0.9, 0.9, 0.6);
  leg->AddEntry(hsp, "NC #Delta Radiative");
  leg->AddEntry(hspc, "NC #Delta Radiative + Cosmic");
  leg->AddEntry(hbnbc, "BNB + Cosmic");
  leg->Draw();

  canvas->Write();

  delete canvas;
  delete leg;
  delete hsp;
  delete hspc;
  delete hbnbc;

}


void plot2d(std::string const & name,
	    TTree * tree,
	    std::string const & draw,
	    std::string const & binning,
	    std::string const & cut,
	    std::string const & title = "",
	    std::string const & xtitle = "",
	    std::string const & ytitle = "") {

  int color_offset = 2;
  TCanvas * canvas = new TCanvas(name.c_str());

  tree->Draw((draw+">>h"+binning).c_str(), cut.c_str(), "colz");
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


void plot_flash(std::string const dir) {

  file_sp = TFile::Open((dir+"/runmv_sp.root").c_str());
  file_sp_cosmic = TFile::Open((dir+"/runmv_sp_cosmic.root").c_str());
  file_bnb_cosmic = TFile::Open((dir+"/runmv_bnb_cosmic.root").c_str());
  ofile = TFile::Open("plot_flash.root", "recreate");
  
  tree_sp = (TTree*)file_sp->Get("flashana/flash_tree");
  tree_sp_cosmic = (TTree*)file_sp_cosmic->Get("flashana/flash_tree");
  tree_bnb_cosmic = (TTree*)file_bnb_cosmic->Get("flashana/flash_tree");

  plot("time",
       "time",
       "(100, 0, 25)",
       "",
       "",
       "Time (#mus)",
       "Area Normalized");

  plot("pe",
       "pe",
       "(100, 0, 5000)",
       "",
       "",
       "PE",
       "Area Normalized");

  plot2d("pe_vs_time_sp",
	 tree_sp,
	 "pe:time",
	 "(100, 0, 25, 100, 0, 5000)",
	 "",
	 "NC #Delta Radiative",
	 "Time (#mus)",
	 "PE");
  
  plot2d("pe_vs_time_sp_cosmic",
	 tree_sp_cosmic,
	 "pe:time",
	 "(100, 0, 25, 100, 0, 5000)",
	 "",
	 "NC #Delta Radiative + Cosmic",
	 "Time (#mus)",
	 "PE");

  plot2d("pe_vs_time_bnb_cosmic",
	 tree_bnb_cosmic,
	 "pe:time",
	 "(100, 0, 25, 100, 0, 5000)",
	 "",
	 "BNB + Cosmic",
	 "Time (#mus)",
	 "PE");

}
