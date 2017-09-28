

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

std::map<TTree *, TTree *> ttree_friend_map;

double pot_sp = -1;
double pot_sp_cosmic = -1;
double pot_bnb = -1;
double pot_bnb_cosmic = -1;
double ngenbnbcosmic = -1;
double ngencosmic = -1;

/*
double pot_sp = 2.23455e+22;
double pot_sp_cosmic = 1.98932e+22;
double pot_bnb = 1.0097e+19;
double pot_bnb_cosmic = 1.06793e+19;
double ngenbnbcosmic = 10450;
double ngencosmic = 10348;
//double const ngenbnbcosmic = 2150;
//double const ngencosmic = 1505;
*/

double const run_pot = 6.6e20;

std::string const signal_definition = "is_delta_rad == 1 && true_nu_vtx_fid_contained == 1";
//std::string const signal_definition = "is_delta_rad == 1 && true_nu_vtx_tpc_contained == 1";

int vpe_v_size = -1;


void reset() {

  vertex_tree_sp->Delete();  
  vertex_tree_bnb->Delete();  
  vertex_tree_bnb_cosmic->Delete();  
  vertex_tree_cosmic->Delete();  

  vertex_tree_sp = (TTree*)ifile_sp->Get("vertex_tree");  
  vertex_tree_bnb = (TTree*)ifile_bnb->Get("vertex_tree");
  vertex_tree_bnb_cosmic = (TTree*)ifile_bnb_cosmic->Get("vertex_tree");
  vertex_tree_cosmic = (TTree*)ifile_cosmic->Get("vertex_tree");

}



void plot(TTree * tree,
	  std::string const & cname,
	  std::string const & draw,
	  std::string const & binning,
	  std::string const & we,
	  std::string const & opt,
	  std::string const & title,
	  std::string const & xtitle,
	  std::string const & ytitle,
	  int const line_color = -1,
	  double const scaling = 1,
	  int const setstats = 0) {

  TCanvas * canvas = new TCanvas(cname.c_str());
  tree->Draw((draw+">>h"+binning).c_str(), we.c_str(), opt.c_str());
  TH1 * h = (TH1*)gDirectory->Get("h");
  h->Scale(scaling);
  h->SetStats(setstats);
  if(line_color != -1) h->SetLineColor(line_color);
  h->SetLineWidth(3);
  h->SetTitle(title.c_str());
  h->GetXaxis()->SetTitle(xtitle.c_str());
  h->GetXaxis()->CenterTitle(); 
  h->GetYaxis()->SetTitle(ytitle.c_str());
  h->GetYaxis()->CenterTitle(); 
  canvas->Write();
  delete canvas;
  delete h;

}



void plotdrawcut(TTree * tree,
		 std::string const & cname,
		 std::string const & draw,
		 std::string const & draw2,
		 std::string const & binning,
		 std::string const & we,
		 std::string const & opt,
		 std::string const & title,
		 std::string const & xtitle,
		 std::string const & ytitle,
		 int const line_color = kBlue,
		 double const scaling = 1,
		 int const setstats = 0) {
 
  TCanvas * canvas = new TCanvas(cname.c_str());
  tree->Draw((draw+">>h"+binning).c_str(), we.c_str(), opt.c_str());
  TH1 * h = (TH1*)gDirectory->Get("h");
  h->Scale(scaling);
  h->SetStats(setstats);
  h->SetLineColor(line_color);
  h->SetLineWidth(3);
  h->SetTitle(title.c_str());
  h->GetXaxis()->SetTitle(xtitle.c_str());
  h->GetXaxis()->CenterTitle(); 
  h->GetYaxis()->SetTitle(ytitle.c_str());
  h->GetYaxis()->CenterTitle();
  double ymax = h->GetBinContent(h->GetMaximumBin());

  TCanvas * canvas_temp = new TCanvas("temp");
  tree->Draw((draw2+">>h2"+binning).c_str(), we.c_str(), opt.c_str());
  TH1 * h2 = (TH1*)gDirectory->Get("h2");
  canvas->cd();
  h2->Draw("same");
  h2->Scale(scaling);
  h2->SetLineColor(line_color+3);
  h2->SetLineWidth(3);
  if(h2->GetBinContent(h2->GetMaximumBin()) > ymax)
    ymax = h2->GetBinContent(h2->GetMaximumBin());

  h->GetYaxis()->SetRangeUser(0, ymax*1.1);
  
  TLegend * leg = new TLegend(0.6, 0.9, 0.9, 0.6);
  leg->AddEntry(h, "Before Cut");
  leg->AddEntry(h2, "After Cut");
  leg->Draw();

  canvas->Write();
  delete canvas;
  delete canvas_temp;
  delete leg;
  delete h;
  delete h2;

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

  histe->Draw("same");

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
  hist_errors(h, 1./h->Integral(0,h->GetNbinsX()+1));
  double ymax = h->GetBinContent(h->GetMaximumBin());
  if(opt != "") opt += " && same";
  else opt = "same";
 
  /*
  if(vertex_tree) vertex_tree_bnb->Draw((draw+">>h2"+binning).c_str(), we2.c_str(), opt.c_str());
  else event_tree_bnb->Draw((draw+">>h2"+binning).c_str(), we2.c_str(), opt.c_str());
  TH1 * h2 = (TH1*)gDirectory->Get("h2");
  h2->SetLineColor(kRed+color_offset);
  h2->SetLineWidth(3);
  hist_errors(h2, 1./h2->Integral(0,h2->GetNbinsX()+1));
  if(h2->GetBinContent(h2->GetMaximumBin()) > ymax) ymax = h2->GetBinContent(h2->GetMaximumBin());
  */ 

  if(vertex_tree) vertex_tree_bnb_cosmic->Draw((draw+">>h3"+binning).c_str(), we2.c_str(), opt.c_str());
  else event_tree_bnb_cosmic->Draw((draw+">>h3"+binning).c_str(), we2.c_str(), opt.c_str());
  TH1 * h3 = (TH1*)gDirectory->Get("h3");
  h3->SetLineColor(kGreen+color_offset);
  h3->SetLineWidth(3);
  hist_errors(h3, 1./h3->Integral(0,h3->GetNbinsX()+1));
  if(h3->GetBinContent(h3->GetMaximumBin()) > ymax) ymax = h3->GetBinContent(h3->GetMaximumBin());  
 
  if(vertex_tree) vertex_tree_cosmic->Draw((draw+">>h4"+binning).c_str(), we2.c_str(), opt.c_str());
  else event_tree_cosmic->Draw((draw+">>h4"+binning).c_str(), we2.c_str(), opt.c_str()); 
  TH1 * h4 = (TH1*)gDirectory->Get("h4");
  h4->SetLineColor(kMagenta+color_offset);
  h4->SetLineWidth(3);
  hist_errors(h4, 1./h4->Integral(0,h4->GetNbinsX()+1));
  if(h4->GetBinContent(h4->GetMaximumBin()) > ymax) ymax = h4->GetBinContent(h4->GetMaximumBin());

  h->GetYaxis()->SetRangeUser(0, ymax*1.1);
  TLegend * l = new TLegend(0.6, 0.9, 0.9, 0.6);
  l->AddEntry(h, "NC #Delta Rad");
  //l->AddEntry(h2, "BNB Inclusive");
  l->AddEntry(h3, "BNB + Cosmic");
  l->AddEntry(h4, "Cosmic");
  l->Draw();

  canvas->Write();
  delete canvas;
  delete h;
  //delete h2;
  delete h3;
  delete h4;

}



template<typename T>
std::string to_string_with_precision(T const value, int const precision = 3) {
  std::ostringstream out;
  if(value!=int(value))
    out << std::setprecision(precision);
  out << value;
  return out.str();
}



TGraph * getgraphsig(TH1F * hist_sig,
		     TH1F * hist_back,
		     bool const forwards) {
  
  int const nbins = hist_sig->GetNbinsX();
  
  std::vector<double> xvalues;
  xvalues.reserve(nbins);
  std::vector<double> yvalues;
  yvalues.reserve(nbins);
  
  double sig_tot = hist_sig->Integral(0, hist_sig->GetNbinsX()+1);
  double back_tot = hist_back->Integral(0, hist_back->GetNbinsX()+1);

  cout << sig_tot << " " << back_tot << "\n";
  
  double largest_sig = 0;
  int largest_sig_bin = -1;
  
  if(!forwards) {
    for(int i = nbins; i >= 1; --i) {
      double sig = sig_tot/sqrt(back_tot);
      if(back_tot == 0) sig = 0;
      if(sig > largest_sig) {
	largest_sig = sig;
	largest_sig_bin = i;
      }
      xvalues.push_back(hist_sig->GetBinCenter(i));
      yvalues.push_back(sig);
      sig_tot -= hist_sig->GetBinContent(i);
      back_tot -= hist_back->GetBinContent(i);
    }
    std::reverse(xvalues.begin(), xvalues.end());
    std::reverse(yvalues.begin(), yvalues.end());
    
    std::cout << "largest siginficance: " << largest_sig << " at: " << hist_sig->GetBinLowEdge(largest_sig_bin)+hist_sig->GetBinWidth(largest_sig_bin) << "\n";
  }
  
  else {
    for(int i = 1; i <= nbins; ++i) {
      double sig = sig_tot/sqrt(back_tot);
      if(back_tot == 0) sig = 0;
      if(sig > largest_sig) {
	largest_sig = sig;
	largest_sig_bin = i;
      }
      cout << hist_sig->GetBinCenter(i) << " " << sig_tot << " " << back_tot << " " << sig << "\n";
      xvalues.push_back(hist_sig->GetBinCenter(i));
      yvalues.push_back(sig);
      sig_tot -= hist_sig->GetBinContent(i);
      back_tot -= hist_back->GetBinContent(i);
    }
    std::cout << "reverse largest siginficance: " << largest_sig << " at: " << hist_sig->GetBinLowEdge(largest_sig_bin) << "\n";
  }
  
  TGraph * graph = new TGraph(nbins,
			      &xvalues[0],
			      &yvalues[0]);
  
  graph->SetLineColor(0);
  graph->SetFillColor(0);
  graph->SetMarkerColor(kGreen+2);
  graph->SetMarkerStyle(5);   
  graph->SetMarkerSize(0.625);
  
  return graph;
}



TGraph * getgrapheff(TH1F * hist_sig,
		     bool const forwards) {
  
  int const nbins = hist_sig->GetNbinsX();
  
  std::vector<double> xvalues;
  xvalues.reserve(nbins);
  std::vector<double> yvalues;
  yvalues.reserve(nbins);
  
  double const sig_tot_const = hist_sig->Integral(0, hist_sig->GetNbinsX()+1);
  double sig_tot = sig_tot_const;
  
  if(!forwards) {
    for(int i = nbins; i >= 1; --i) {
      xvalues.push_back(hist_sig->GetBinCenter(i));
      yvalues.push_back(sig_tot/sig_tot_const);
      sig_tot -= hist_sig->GetBinContent(i);
    }
    std::reverse(xvalues.begin(), xvalues.end());
    std::reverse(yvalues.begin(), yvalues.end());
  }
  
  else {
    for(int i = 1; i <= nbins; ++i) {
      xvalues.push_back(hist_sig->GetBinCenter(i));
      yvalues.push_back(sig_tot/sig_tot_const);
      sig_tot -= hist_sig->GetBinContent(i);
    }
  }
  
  TGraph * graph = new TGraph(nbins,
			      &xvalues[0],
			      &yvalues[0]);
  
  graph->SetLineColor(0);
  graph->SetFillColor(0);
  graph->SetMarkerColor(kMagenta+2);
  graph->SetMarkerStyle(20);   
  graph->SetMarkerSize(0.625);
  
  return graph;
}



void draw_sigvsback(TTree * tree_sig,
		    TTree * tree_back1,
		    TTree * tree_back2,
		    std::string const & cname,
		    std::string const & dr,
		    std::string const & binning,
		    std::string const & we_sig,
		    std::string const & sig_label,
		    std::string const & we_back,		     
		    std::string const & back_label,		     
		    std::string const & op = "",
		    std::string const & title = "",
		    std::string const & xtitle = "",
		    std::string const & ytitle = "",
		    bool const significance = false,
		    bool const efficiency = false,
		    bool const forwards = false) {
    
  TString hname_sig = "h_sig";
  TString draw_str_sig = "";
  draw_str_sig += dr;
  draw_str_sig += ">>";
  draw_str_sig += hname_sig;
  draw_str_sig += binning;
  
  TString hname_back1 = "h_back1";
  TString draw_str_back1 = "";
  draw_str_back1 += dr;
  draw_str_back1 += ">>";
  draw_str_back1 += hname_back1;
  draw_str_back1 += binning;

  TString hname_back2 = "h_back2";
  TString draw_str_back2 = "";
  draw_str_back2 += dr;
  draw_str_back2 += ">>";
  draw_str_back2 += hname_back2;
  draw_str_back2 += binning;
  
  TCanvas * canvas = new TCanvas("temp");
  tree_sig->Draw(draw_str_sig.Data(),
		 we_sig.c_str(),
		 op.c_str());
  delete canvas;
  
  canvas = new TCanvas("temp");
  tree_back1->Draw(draw_str_back1.Data(),
		   we_back.c_str(),
		   op.c_str());
  delete canvas;

  canvas = new TCanvas("temp");
  tree_back2->Draw(draw_str_back2.Data(),
		   we_back.c_str(),
		   op.c_str());
  delete canvas;
  
  TH1F * hist_sig = (TH1F*)gDirectory->Get(hname_sig.Data());
  TH1F * hist_back1 = (TH1F*)gDirectory->Get(hname_back1.Data()); 
  TH1F * hist_back2 = (TH1F*)gDirectory->Get(hname_back2.Data()); 
  if(hist_sig->GetEntries() == 0) 
    std::cout << "draw_sigvsback: No signal for selection: \"" << we_sig << "\"\n";
  if(hist_back1->GetEntries() == 0) 
    std::cout << "draw_sigvsback: No background for selection: \"" << we_back << "\"\n";    
  if(hist_back2->GetEntries() == 0) 
    std::cout << "draw_sigvsback: No background for selection: \"" << we_back << "\"\n";    

  std::cout << "draw: " << draw_str_sig << "\n"
	    << draw_str_back1 << "\n"
	    << draw_str_back2 << "\n"
	    << "sig cut: " << we_sig << "\n"
	    << "back cut: " << we_back << "\n";

  std::cout << "ge - s: " << tree_sig->GetEntries(we_sig.c_str()) << " b1: " << tree_back1->GetEntries(we_back.c_str()) << " b2: " << tree_back2->GetEntries(we_back.c_str()) << "\n";

  std::cout << "signal: " << hist_sig->Integral(0, hist_sig->GetNbinsX()+1) << " "
	    << "back1: " << hist_back1->Integral(0, hist_back1->GetNbinsX()+1) << " "
	    << "back2: "<< hist_back2->Integral(0, hist_back2->GetNbinsX()+1) << "\n";

  hist_sig->Scale(run_pot / pot_sp);
  hist_back1->Scale(run_pot / pot_bnb_cosmic);
  hist_back2->Scale(ngenbnbcosmic/ngencosmic*10.729 / pot_bnb_cosmic * run_pot);  

  TH1F * hist_back = (TH1F*)hist_back1->Clone();
  hist_back->Add(hist_back2);

  hist_back->SetStats(0);
  hist_back->SetTitle(title.c_str());
  hist_back->GetXaxis()->SetTitle(xtitle.c_str());
  hist_back->GetXaxis()->CenterTitle(); 
  hist_back->GetYaxis()->SetTitle(ytitle.c_str());
  hist_back->GetYaxis()->CenterTitle();
  hist_sig->SetLineColor(kRed);
  hist_back->SetLineColor(kBlue);
  
  TLegend * legend = new TLegend(0.6, 0.9, 0.9, 0.6);
  legend->AddEntry(hist_sig, (sig_label+": "+to_string_with_precision(hist_sig->Integral(0,hist_sig->GetNbinsX()+1))+" events").c_str());
  legend->AddEntry(hist_back, (back_label+": "+to_string_with_precision(hist_back->Integral(0,hist_back->GetNbinsX()+1))+" events").c_str());

  canvas = new TCanvas(cname.c_str());   
  TPad * pad_base = new TPad();
  pad_base->SetName("pad_base");
  pad_base->Draw();
  pad_base->SetFillColor(0);
  double ymin_base = hist_sig->GetYaxis()->GetXmin();
  double ymax_base = hist_sig->GetYaxis()->GetXmax();
  double dy_base = (ymax_base-ymin_base)/0.8;
  double xmin_base = hist_sig->GetXaxis()->GetXmin();
  double xmax_base = hist_sig->GetXaxis()->GetXmax();
  double dx_base = (xmax_base-xmin_base)/0.8;
  pad_base->Range(xmin_base-0.1*dx_base,ymin_base-0.1*dy_base,xmax_base+0.1*dx_base,ymax_base+0.1*dy_base);
  pad_base->cd();
  hist_back->Draw();
  hist_sig->Draw();
  if(!significance && !efficiency) legend->Draw();

  TGraph * graph_sig = nullptr;
  TPad * pad_sig = nullptr;
  if(significance) {
    graph_sig = getgraphsig(hist_sig, hist_back, forwards);
    graph_sig->SetName((cname + "_sig").c_str());
    graph_sig->Write();
    /*
    canvas->cd();
    pad_sig = new TPad();
    pad_sig->SetName("pad_sig");
    pad_sig->SetFillStyle(4000);
    double ymin = graph_sig->GetYaxis()->GetXmin();
    double ymax = graph_sig->GetYaxis()->GetXmax();
    double dy = (ymax-ymin)/0.8;
    double xmax = 0;
    double ytemp = 0;
    graph_sig->GetPoint(graph_sig->GetN()-1, xmax, ytemp);
    pad_sig->Range(xmin_base-0.1*dx_base,ymin-0.1*dy,xmax_base+0.1*dx_base,ymax+0.1*dy);
    pad_sig->Draw();
    pad_sig->cd();
    graph_sig->Draw("samep");
    TGaxis * axis = new TGaxis(xmax_base,ymin,xmax_base,ymax,ymin,ymax, 510, "+L");
    axis->SetLabelColor(graph_sig->GetMarkerColor());
    axis->SetLabelSize(0.03);
    axis->Draw();
    legend->AddEntry(graph_sig, "Significance");
    if(!efficiency) legend->Draw();
    */
  }

  TGraph * graph_eff = nullptr;
  TPad * pad_eff = nullptr;
  if(efficiency) {
    graph_eff = getgrapheff(hist_sig, forwards);
    graph_eff->SetName((cname + "_eff").c_str());
    graph_eff->Write();
    /*
    canvas->cd();
    pad_eff = new TPad();
    pad_eff->SetFillStyle(4000);
    double ymin = graph_eff->GetYaxis()->GetXmin();
    double ymax = graph_eff->GetYaxis()->GetXmax();
    double dy = (ymax-ymin)/0.8;
    double xmax = 0;
    double ytemp = 0;
    graph_eff->GetPoint(graph_eff->GetN()-1, xmax, ytemp);
    pad_eff->Range(xmin_base-0.1*dx_base,ymin-0.1*dy,xmax_base+0.1*dx_base,ymax+0.1*dy);
    pad_eff->Draw();
    pad_eff->cd();
    graph_eff->Draw("samep");
    double axis_offset = 0;
    if(significance) axis_offset = 1./14*xmax;
    TGaxis * axis = new TGaxis(xmax_base+axis_offset,ymin,xmax_base+axis_offset,ymax,ymin,ymax, 510, "+L");   
    axis->SetLabelColor(graph_eff->GetMarkerColor());
    axis->SetLabelSize(0.03);
    axis->Draw();
    legend->AddEntry(graph_eff, "Efficiency");
    legend->Draw();
    */
  }
  
  hist_sig->Scale(1. / hist_sig->Integral(0, hist_sig->GetNbinsX()+1));
  hist_back->Scale(1. / hist_back->Integral(0, hist_back->GetNbinsX()+1)); 
  
  double const ymin_hist = 0;
  double ymax_hist = hist_sig->GetMaximum();
  if(hist_back->GetMaximum() > ymax_hist) ymax_hist = hist_back->GetMaximum();
  hist_back->GetYaxis()->SetRangeUser(ymin_hist, 1.1*ymax_hist);
  
  canvas->Write();
  delete canvas;
  delete hist_sig;
  delete hist_back;
  delete hist_back1;
  delete hist_back2;
  delete legend;
  if(graph_sig) delete graph_sig;
  if(graph_eff) delete graph_eff;
  
}



void divide_hist(TCanvas * canvas, TH1 * hspc, TH1 * hsp, double scale = 1, bool first = false) {

  std::vector<double> bin_errors;
  for(int i = 1; i <= hspc->GetNbinsX(); ++i) {
    double a = hspc->GetBinContent(i);
    double b = hsp->GetBinContent(i);
    double sa = hspc->GetBinError(i);
    double sb = hsp->GetBinError(i);
    if(b) bin_errors.push_back(sqrt(pow(sa/b, 2) + pow(sb*a/(b*b), 2)));
    else bin_errors.push_back(0);
  }

  hspc->Divide(hsp);
  TH1 * hspce = (TH1*)hspc->Clone();

  for(int i = 1; i <= hspc->GetNbinsX(); ++i) {
    if(bin_errors.at(i-1)) hspce->SetBinError(i, bin_errors.at(i-1)*scale);
  }

  hspc->Scale(scale);
  hspce->Scale(scale);
  hspce->SetLineColor(hspc->GetLineColor());
  hspce->SetLineWidth(hspc->GetLineWidth());

  canvas->cd();
  if(first) hspc->Draw();
  else hspc->Draw("same");
  hspce->Draw("sameep");

}



void cut_efficiency_plot(std::string const & cname,
			 std::string const & draw,
			 std::string const & binning,
			 std::string const & title = "",
			 std::string const & xtitle = "",
			 std::string const & ytitle = "") {

  TTree * vtf_sp = (TTree*)gDirectory->Get("tree_friend_sp");
  TTree * vtf_bnb = (TTree*)gDirectory->Get("tree_friend_bnb");
  TTree * vtf_bnb_cosmic = (TTree*)gDirectory->Get("tree_friend_bnb_cosmic");
  //TTree * vtf_cosmic = (TTree*)gDirectory->Get("tree_friend_cosmic");

  double sp_mean = 1;
  double bnb_mean = 1;
  double bnb_cosmic_mean = 1;
  //double cosmic_mean = 1;

  /*
  TH1 * vpesp = (TH1*)gDirectory->Get(vpe_sp_name.c_str());
  double sp_mean = vpesp->GetMean();
  delete vpesp;
  TH1 * vpebnb = (TH1*)gDirectory->Get(vpe_bnb_name.c_str());
  double bnb_mean = vpebnb->GetMean(); 
  delete vpebnb;
  TH1 * vpebnbcosmic = (TH1*)gDirectory->Get(vpe_bnbcosmic_name.c_str());
  double bnb_cosmic_mean = vpebnbcosmic->GetMean();
  delete vpebnbcosmic;
  TH1 * vpecosmic = (TH1*)gDirectory->Get(vpe_cosmic_name.c_str());
  double cosmic_mean = vpecosmic->GetMean();
  delete vpecosmic;
  */

  std::string cut = "passed_swtrigger == 1 && shower_dist_to_flashzcenter <= 38 && totalpe_ibg_sum > 139 && reco_nu_vtx_fid_contained == 1 && Sum$(associated_shower_info.fTotalEnergy) > 0.07 && longest_asso_track_displacement <= 32";

  int color_offset = 2;

  TCanvas * canvas_sp = new TCanvas(cname.c_str());
  vertex_tree_sp->Draw((draw+">>hsp"+binning).c_str(), signal_definition.c_str(), "");
  TH1 * hsp = (TH1*)gDirectory->Get("hsp");
  vertex_tree_sp->Draw((draw+">>hspc"+binning).c_str(), (signal_definition + " && " + cut).c_str(), "");
  TH1 * hspc = (TH1*)gDirectory->Get("hspc");
  hspc->SetStats(0);
  hspc->SetLineColor(kBlue+color_offset);
  hspc->SetMarkerColor(kBlue+color_offset);
  hspc->SetLineWidth(3);
  hspc->SetTitle(title.c_str());
  hspc->GetXaxis()->SetTitle(xtitle.c_str());
  hspc->GetXaxis()->CenterTitle(); 
  hspc->GetYaxis()->SetTitle(ytitle.c_str());
  hspc->GetYaxis()->CenterTitle();
  divide_hist(canvas_sp, hspc, hsp, 1./sp_mean, true);
  double ymax = hspc->GetBinContent(hspc->GetMaximumBin());

  TCanvas * canvas_bnb = new TCanvas("efficiency_bnb");
  vertex_tree_bnb->Draw((draw+">>hbnb"+binning).c_str());
  TH1 * hbnb = (TH1*)gDirectory->Get("hbnb");
  vertex_tree_bnb->Draw((draw+">>hbnbc"+binning).c_str(), (cut).c_str());
  TH1 * hbnbc = (TH1*)gDirectory->Get("hbnbc");
  hbnbc->SetStats(0);
  hbnbc->SetLineColor(kRed+color_offset);
  hbnbc->SetMarkerColor(kBlue+color_offset);
  hbnbc->SetLineWidth(3);
  divide_hist(canvas_sp, hbnbc, hbnb, 1./bnb_mean);

  TCanvas * canvas_bnb_cosmic = new TCanvas("efficiency_bnb_cosmic");
  vertex_tree_bnb_cosmic->Draw((draw+">>hbnb_cosmic"+binning).c_str());
  TH1 * hbnb_cosmic = (TH1*)gDirectory->Get("hbnb_cosmic");
  vertex_tree_bnb_cosmic->Draw((draw+">>hbnb_cosmicc"+binning).c_str(), (cut).c_str());
  TH1 * hbnb_cosmicc = (TH1*)gDirectory->Get("hbnb_cosmicc");
  hbnb_cosmicc->SetStats(0);
  hbnb_cosmicc->SetLineColor(kGreen+color_offset);
  hbnb_cosmicc->SetMarkerColor(kGreen+color_offset);
  hbnb_cosmicc->SetLineWidth(3);
  divide_hist(canvas_sp, hbnb_cosmicc, hbnb_cosmic, 1./bnb_cosmic_mean);

  /*
  std::string cosbinning = "(1, 0, 2.2)";
  TCanvas * canvas_cosmic = new TCanvas("efficiency_cosmic");
  vertex_tree_cosmic->Draw((draw+">>hcosmic"+cosbinning).c_str());
  TH1 * hcosmic = (TH1*)gDirectory->Get("hcosmic");
  vertex_tree_cosmic->Draw((draw+">>hcosmicc"+cosbinning).c_str(), (cut).c_str());
  TH1 * hcosmicc = (TH1*)gDirectory->Get("hcosmicc");
  hcosmicc->Divide(hcosmic);
  hcosmicc->SetStats(0);
  hcosmicc->SetLineColor(kMagenta+color_offset);
  hcosmicc->SetMarkerColor(kMagenta+color_offset);
  hcosmicc->SetLineWidth(3);
  hcosmicc->Scale(1./cosmic_mean);
  canvas_sp->cd();
  hcosmicc->Draw("same");
  */

  TLegend * leg = new TLegend(0.6, 0.9, 0.9, 0.6);
  leg->AddEntry(hspc, "NC #Delta Radiative");
  leg->AddEntry(hbnbc, "BNB Inclusive");
  leg->AddEntry(hbnb_cosmicc, "BNB + Cosmic");
  //leg->AddEntry(hcosmicc, "Intime Cosmic");
  leg->Draw();

  hspc->GetYaxis()->SetRangeUser(0, ymax*1.1);
  canvas_sp->Write();

  delete leg;
  //delete canvas_sp;
  //delete canvas_bnb;
  //delete canvas_bnb_cosmic;
  delete hspc;
  delete hbnbc;
  delete hbnb_cosmicc;

}



void cut_efficiency_event_plot(std::string const & cname,
			       std::string const & draw,
			       std::string const & binning,
			       std::string const & title = "",
			       std::string const & xtitle = "",
			       std::string const & ytitle = "") {

  event_tree_sp->AddFriend("tree_friend_sp");
  event_tree_bnb->AddFriend("tree_friend_bnb");
  event_tree_bnb_cosmic->AddFriend("tree_friend_bnb_cosmic");
  event_tree_cosmic->AddFriend("tree_friend_cosmic");

  int color_offset = 2;

  double ymax = 0;
 
  TCanvas * canvas_sp = new TCanvas(cname.c_str());
  std::string cut_sp = "tree_friend_sp.vertices_per_event_cut" + std::to_string(vpe_v_size-1) + " > 0";
  event_tree_sp->Draw((draw+">>hsp"+binning).c_str(), signal_definition.c_str());
  TH1 * hsp = (TH1*)gDirectory->Get("hsp");
  event_tree_sp->Draw((draw+">>hspc"+binning).c_str(), (signal_definition + " && " + cut_sp).c_str(), "");
  TH1 * hspc = (TH1*)gDirectory->Get("hspc");
  hspc->SetStats(0);
  hspc->SetLineColor(kBlue+color_offset);
  hspc->SetMarkerColor(kBlue+color_offset);
  hspc->SetLineWidth(3);
  hspc->SetTitle(title.c_str());
  hspc->GetXaxis()->SetTitle(xtitle.c_str());
  hspc->GetXaxis()->CenterTitle(); 
  hspc->GetYaxis()->SetTitle(ytitle.c_str());
  hspc->GetYaxis()->CenterTitle();
  divide_hist(canvas_sp, hspc, hsp, 1, true);
  ymax = hspc->GetBinContent(hspc->GetMaximumBin());

  TCanvas * canvas_bnb = new TCanvas("efficiency_bnb");
  std::string cut_bnb = "tree_friend_bnb.vertices_per_event_cut" + std::to_string(vpe_v_size-1) + " > 0";
  event_tree_bnb->Draw((draw+">>hbnb"+binning).c_str());
  TH1 * hbnb = (TH1*)gDirectory->Get("hbnb");
  event_tree_bnb->Draw((draw+">>hbnbc"+binning).c_str(), cut_bnb.c_str());
  TH1 * hbnbc = (TH1*)gDirectory->Get("hbnbc");
  hbnbc->SetStats(0);
  hbnbc->SetLineColor(kRed+color_offset);
  hbnbc->SetMarkerColor(kBlue+color_offset);
  hbnbc->SetLineWidth(3);
  divide_hist(canvas_sp, hbnbc, hbnb, 1);

  TCanvas * canvas_bnb_cosmic = new TCanvas("efficiency_bnb_cosmic");
  std::string cut_bnb_cosmic = "tree_friend_bnb_cosmic.vertices_per_event_cut" + std::to_string(vpe_v_size-1) + " > 0";
  event_tree_bnb_cosmic->Draw((draw+">>hbnb_cosmic"+binning).c_str());
  TH1 * hbnb_cosmic = (TH1*)gDirectory->Get("hbnb_cosmic");
  event_tree_bnb_cosmic->Draw((draw+">>hbnb_cosmicc"+binning).c_str(), cut_bnb_cosmic.c_str());
  TH1 * hbnb_cosmicc = (TH1*)gDirectory->Get("hbnb_cosmicc");
  hbnb_cosmicc->SetStats(0);
  hbnb_cosmicc->SetLineColor(kGreen+color_offset);
  hbnb_cosmicc->SetMarkerColor(kGreen+color_offset);
  hbnb_cosmicc->SetLineWidth(3);
  divide_hist(canvas_sp, hbnb_cosmicc, hbnb_cosmic, 1);

  TCanvas * canvas_cosmic = new TCanvas("efficiency_cosmic");
  std::string cosbinning = "(1, 0.1, 2.4)";
  std::string cut_cosmic = "tree_friend_cosmic.vertices_per_event_cut" + std::to_string(vpe_v_size-1) + " > 0";
  event_tree_cosmic->Draw((draw+">>hcosmic"+cosbinning).c_str());
  TH1 * hcosmic = (TH1*)gDirectory->Get("hcosmic");
  event_tree_cosmic->Draw((draw+">>hcosmicc"+cosbinning).c_str(), cut_cosmic.c_str());
  TH1 * hcosmicc = (TH1*)gDirectory->Get("hcosmicc");
  hcosmicc->Divide(hcosmic);
  hcosmicc->SetStats(0);
  hcosmicc->SetLineColor(kMagenta+color_offset);
  hcosmicc->SetMarkerColor(kMagenta+color_offset);
  hcosmicc->SetLineWidth(3);
  canvas_sp->cd();
  hcosmicc->Draw("same");

  TLegend * leg = new TLegend(0.6, 0.9, 0.9, 0.6);
  leg->AddEntry(hspc, "NC #Delta Radiative");
  leg->AddEntry(hbnbc, "BNB Inclusive");
  leg->AddEntry(hbnb_cosmicc, "BNB + Cosmic");
  leg->AddEntry(hcosmicc, "In-time Cosmic");
  leg->Draw();

  hspc->GetYaxis()->SetRangeUser(0, ymax*1.1);
  canvas_sp->Write();

  delete leg;

  delete canvas_sp;
  delete canvas_bnb;
  delete canvas_bnb_cosmic;
  delete canvas_cosmic;

  delete hspc;
  delete hbnbc;
  delete hbnb_cosmicc;
  delete hcosmicc;

}



void signal_efficiency() {
  
  double total = event_tree_sp->GetEntries("is_delta_rad == 1");

  cout << total << " " << total/pot_sp*run_pot << "\n";

  double tpccont = event_tree_sp->GetEntries(signal_definition.c_str());
  cout << vertex_tree_sp->GetEntries(signal_definition.c_str()) << "\n";

  cout << tpccont << " " << tpccont/pot_sp*run_pot << " eff: " << tpccont/total*100<< "\n";

  double recoshower = event_tree_sp->GetEntries("is_delta_rad == 1 && true_nu_vtx_tpc_contained == 1 && showernumber > 0");

  cout << recoshower << " " << recoshower/pot_sp*run_pot << " rel: " << recoshower/tpccont*100 << " tot: " << recoshower/total*100 << "\n";

  double vr = event_tree_sp->GetEntries("is_delta_rad == 1 && true_nu_vtx_tpc_contained == 1 && showernumber > 0 && number_of_selected_vertices > 0");

  cout << vr << " " << vr/pot_sp*run_pot << " rel: " << vr/recoshower*100 << " tot: " << vr/total*100 << "\n";

}



void showermultiplicity(std::string const & acut = "") {

  plotsupimp("reco_asso_showers",
	     "reco_asso_showers",
	     "(9, 0, 9)",
	     acut,
	     "",
	     "",
	     "Number of Associated Reco Showers per Reco Vertex",
	     "Area Normalized");

  plotsupimp("reco_asso_showers_cut",
	     "Sum$(associated_shower_info.fTotalEnergy > 0.02 && associated_shower_info.fLength > 5)",
	     "(9, 0, 9)",
	     acut,
	     "",
	     "Showers with Energy > 20 MeV and Length > 5 cm",
	     "Number of Associated Reco Showers per Reco Vertex",
	     "Area Normalized");

  std::string asignal_cut = signal_definition;
  if(acut != "") asignal_cut += " && ";

  plotdrawcut(vertex_tree_sp,
	      "reco_asso_showers_sp",
	      "reco_asso_showers",
	      "Sum$(associated_shower_info.fTotalEnergy > 0.02 && associated_shower_info.fLength > 5)",
	      "(9, 0, 9)",
	      asignal_cut + acut,
	      "",
	      "NC #Delta Radiative",
	      "Showers with Energy > 20 MeV and Length > 5 cm",
	      "Events per 6.6e20 POT",
	      kBlue,
	      run_pot / pot_sp);

  plotdrawcut(vertex_tree_bnb,
	      "reco_asso_showers_bnb",
	      "reco_asso_showers",
	      "Sum$(associated_shower_info.fTotalEnergy > 0.02 && associated_shower_info.fLength > 5)",
	      "(9, 0, 9)",
	      acut,
	      "",
	      "BNB Inclusive",
	      "Showers with Energy > 20 MeV and Length > 5 cm",
	      "Events per 6.6e20 POT",
	      kRed,
	      run_pot / pot_bnb);

  plotdrawcut(vertex_tree_bnb_cosmic,
	      "reco_asso_showers_bnb_cosmic",
	      "reco_asso_showers",
	      "Sum$(associated_shower_info.fTotalEnergy > 0.02 && associated_shower_info.fLength > 5)",
	      "(9, 0, 9)",
	      acut,
	      "",
	      "BNB + Cosmic",
	      "Showers with Energy > 20 MeV and Length > 5 cm",
	      "Events per 6.6e20 POT",
	      kGreen,
	      run_pot / pot_bnb_cosmic);
  
  plotdrawcut(vertex_tree_cosmic,
	      "reco_asso_showers_cosmic",
	      "reco_asso_showers",
	      "Sum$(associated_shower_info.fTotalEnergy > 0.02 && associated_shower_info.fLength > 5)",
	      "(9, 0, 9)",
	      acut,
	      "",
	      "Open Cosmic",
	      "Showers with Energy > 20 MeV and Length > 5 cm",
	      "Events per 6.6e20 POT",
	      kMagenta,
	      ngenbnbcosmic/ngencosmic*10.729 / pot_bnb_cosmic * run_pot);
  
}



void showerrecoenergy(std::string cut = "") {

  if(cut != "") cut = " && " + cut;

  plotsupimp("most_energetic_shower_reco_energy_si",
	     "most_energetic_shower_reco_energy",
	     "(100, 0, 0.5)",
	     "most_energetic_shower_reco_energy > 0" + cut,
	     "",
	     "",
	     "Energy of Most Energetic Associated Reco Shower [Gev]",
	     "Area Normalized");

  plot(vertex_tree_sp,
       "most_energetic_shower_reco_energy_sp",
       "most_energetic_shower_reco_energy",
       "(100, 0, 0.5)",
       "most_energetic_shower_reco_energy > 0 && " + signal_definition + cut,
       "",
       "NC #Delta Radiative",
       "Energy of Most Energetic Associated Reco Shower [Gev]",
       "Events per 6.6e20 POT",
       kBlue,
       run_pot / pot_sp);

  plot(vertex_tree_bnb,
       "most_energetic_shower_reco_energy_bnb",
       "most_energetic_shower_reco_energy",
       "(100, 0, 0.5)",
       "most_energetic_shower_reco_energy > 0" + cut,
       "",
       "BNB Inclusive",
       "Energy of Most Energetic Associated Reco Shower [Gev]",
       "Events per 6.6e20 POT",
       kRed,
       run_pot / pot_bnb);

  plot(vertex_tree_bnb_cosmic,
       "most_energetic_shower_reco_energy_bnb_cosmic",
       "most_energetic_shower_reco_energy",
       "(100, 0, 0.5)",
       "most_energetic_shower_reco_energy > 0" + cut,
       "",
       "BNB + Cosmic",
       "Energy of Most Energetic Associated Reco Shower [Gev]",
       "Events per 6.6e20 POT",
       kGreen,
       run_pot / pot_bnb_cosmic);
   
  plot(vertex_tree_cosmic,
       "most_energetic_shower_reco_energy_cosmic",
       "most_energetic_shower_reco_energy",
       "(100, 0, 0.5)",
       "most_energetic_shower_reco_energy > 0" + cut,
       "",
       "Open Cosmic",
       "Energy of Most Energetic Associated Reco Shower [Gev]",
       "Events per 6.6e20 POT",
       kMagenta,
       ngenbnbcosmic/ngencosmic*10.729 / pot_bnb_cosmic * run_pot);
    
}



void tracklength(std::string const & cut = "") {

  std::string const bins = "25";
  std::string const min = "0";
  std::string const max = "150";
  
  std::string const binning = "("+bins+", "+min+", "+max+")";

  plotsupimp("longest_asso_track_displacement",
	     "longest_asso_track_displacement",
	     binning,
	     "reco_asso_tracks > 0 && " + cut,
	     "",
	     "",
	     "Longest Associated Track Length [cm]",
	     "Area Normalized");  

  std::string signal_cut = signal_definition;
  if(cut != "") signal_cut += " && ";

  plot(vertex_tree_sp,
       "longest_asso_track_displacement_sp",
       "longest_asso_track_displacement",
       binning,
       "reco_asso_tracks > 0 && " + signal_cut + cut,
       "",
       "NC #Delta Radiative",
       "Longest Associated Track Length [cm]",
       "Events per 6.6e20 POT",
       kBlue,
       run_pot / pot_sp);  
  
  plot(vertex_tree_bnb,
       "longest_asso_track_displacement_bnb",
       "longest_asso_track_displacement",
       binning,
       "reco_asso_tracks > 0 && " + cut,
       "",
       "BNB Inclusive",
       "Longest Associated Track Length [cm]",
       "Events per 6.6e20 POT",
       kRed,
       run_pot / pot_bnb); 
 
  plot(vertex_tree_bnb_cosmic,
       "longest_asso_track_displacement_bnb_cosmic",
       "longest_asso_track_displacement",
       binning,
       "reco_asso_tracks > 0 && " + cut,
       "",
       "BNB + Cosmic",
       "Longest Associated Track Length [cm]",
       "Events per 6.6e20 POT",
       kGreen,
       run_pot / pot_bnb_cosmic);  

  plot(vertex_tree_cosmic,
       "longest_asso_track_displacement_cosmic",
       "longest_asso_track_displacement",
       binning,
       "reco_asso_tracks > 0 && " + cut,
       "",
       "Open Cosmic",
       "Longest Associated Track Length [cm]",
       "Events per 6.6e20 POT",
       kMagenta,
       ngenbnbcosmic/ngencosmic*10.729 / pot_bnb_cosmic * run_pot);  
  
}



void recoassotracks(std::string const & cut) {

  std::string const bins = "9";
  std::string const min = "0";
  std::string const max = "9";
  
  std::string const binning = "("+bins+", "+min+", "+max+")";

  plotsupimp("reco_asso_tracks",
	     "reco_asso_tracks",
	     binning,
	     cut,
	     "",
	     "",
	     "Number of Associated Tracks",
	     "Area Normalized");  

  std::string signal_cut = signal_definition;
  if(cut != "") signal_cut += " && ";

  plot(vertex_tree_sp,
       "reco_asso_tracks_sp",
       "reco_asso_tracks",
       binning,
       signal_cut + cut,
       "",
       "NC #Delta Radiative",
       "Number of Associated Tracks",
       "Events per 6.6e20 POT",
       kBlue,
       run_pot / pot_sp);  
  
  plot(vertex_tree_bnb,
       "reco_asso_tracks_bnb",
       "reco_asso_tracks",
       binning,
       cut,
       "",
       "BNB Inclusive",
       "Number of Associated Tracks",
       "Events per 6.6e20 POT",
       kRed,
       run_pot / pot_bnb); 
 
  plot(vertex_tree_bnb_cosmic,
       "reco_asso_tracks_bnb_cosmic",
       "reco_asso_tracks",
       binning,
       cut,
       "",
       "BNB + Cosmic",
       "Number of Associated Tracks",
       "Events per 6.6e20 POT",
       kGreen,
       run_pot / pot_bnb_cosmic);  

  plot(vertex_tree_cosmic,
       "reco_asso_tracks_cosmic",
       "reco_asso_tracks",
       binning,
       cut,
       "",
       "Open Cosmic",
       "Number of Associated Tracks",
       "Events per 6.6e20 POT",
       kMagenta,
       ngenbnbcosmic/ngencosmic*10.729 / pot_bnb_cosmic * run_pot);  

}



void fid_volz(std::string const & cut) {

  std::string const offset = "10";

  std::string const xmin = "0+"+offset; 
  std::string const xmax = "256.35-"+offset;
  std::string const ymin = "-116.5+"+offset; 
  std::string const ymax = "116.5-"+offset;
  std::string const zmin = "0+"+offset;
  std::string const zmax = "1036.8";

  std::string const base = cut + " && reco_nuvertx > " + xmin + " && reco_nuvertx < " + xmax + " && reco_nuverty > " + ymin + " && reco_nuverty < " + ymax + " && reco_nuvertz > " + zmin + " && reco_nuvertz < ";

  int const total_sp = vertex_tree_sp->GetEntries((signal_definition + " && " + base + zmax).c_str());
  
  int const total_bnb_cosmic = vertex_tree_bnb_cosmic->GetEntries((base + zmax).c_str());
  int const total_cosmic = vertex_tree_cosmic->GetEntries((base + zmax).c_str());

  double const scale_sp = run_pot / pot_sp;
  double const scale_bnb_cosmic = run_pot / pot_bnb_cosmic;
  double const scale_cosmic = run_pot * ngenbnbcosmic/ngencosmic*10.729 / pot_bnb_cosmic;

  std::cout << "Total sig: " << total_sp * scale_sp / sqrt(total_bnb_cosmic * scale_bnb_cosmic + total_cosmic * scale_cosmic) << "\n";

  for(double d = 0; d < 100; d += 1) {

    int const var_sp = vertex_tree_sp->GetEntries((signal_definition + " && " + base + zmax + " - "  + std::to_string(d)).c_str());
    
    int const var_bnb_cosmic = vertex_tree_bnb_cosmic->GetEntries((base + zmax + " - "  + std::to_string(d)).c_str());
    int const var_cosmic = vertex_tree_cosmic->GetEntries((base + zmax + " - "  + std::to_string(d)).c_str());

    std::cout << "Var sig for " << d << ": " << var_sp * scale_sp / sqrt(var_bnb_cosmic * scale_bnb_cosmic + var_cosmic * scale_cosmic) << "\n";

  }

}



void flash() {
  
  plotsupimp("totalpe_ibg_sum",
	     "totalpe_ibg_sum",
	     "(200, 0, 2000)",
	     "",
	     "",
	     "",
	     "Summed PE",
	     "Area Normalized",
	     false);  

  draw_sigvsback(vertex_tree_sp,
		 vertex_tree_bnb_cosmic,
		 vertex_tree_cosmic,
		 "totalpe_ibg_sum_svb",
		 "totalpe_ibg_sum",
		 "(200, 0, 2000)",
		 signal_definition,
		 "sig",
		 "",
		 "back",
		 "",
		 "",
		 "Summed Total PE",
		 "Area Normalized",
		 true,
		 true,
		 true);
  
}



void cout_selection(std::string const & cut,
		    double & scaled_sp,
		    double & scaled_bnb,
		    double & scaled_bnb_cosmic,
		    double & scaled_cosmic,
		    double & scaled_back) {

  double const old_scaled_sp = scaled_sp;
  double const old_scaled_bnb = scaled_bnb;
  double const old_scaled_bnb_cosmic = scaled_bnb_cosmic;
  double const old_scaled_cosmic = scaled_cosmic;
  double const old_scaled_back = scaled_back;

  std::string is_delta_rad = signal_definition;
  if(cut != "") is_delta_rad += " && ";

  double const entries_sp =
    vertex_tree_sp->GetEntries((is_delta_rad + cut).c_str());
  double const entries_bnb =
    vertex_tree_bnb->GetEntries((cut).c_str());
  double const entries_bnb_cosmic =
    vertex_tree_bnb_cosmic->GetEntries((cut).c_str());
  double const entries_cosmic = 
    vertex_tree_cosmic->GetEntries((cut).c_str());
  
  scaled_sp = entries_sp / pot_sp * run_pot;
  scaled_bnb = entries_bnb / pot_bnb * run_pot;
  scaled_bnb_cosmic = entries_bnb_cosmic / pot_bnb_cosmic * run_pot;
  scaled_cosmic = entries_cosmic * ngenbnbcosmic/ngencosmic*10.729 / pot_bnb_cosmic * run_pot;

  double const total_scaled_sp =
    double(vertex_tree_sp->GetEntries(signal_definition.c_str())) / pot_sp * run_pot;
  double const total_scaled_bnb =
    double(vertex_tree_bnb->GetEntries()) / pot_bnb * run_pot;
  double const total_scaled_bnb_cosmic =
    double(vertex_tree_bnb_cosmic->GetEntries()) / pot_bnb_cosmic * run_pot;
  double const total_scaled_cosmic = 
    double(vertex_tree_cosmic->GetEntries()) * ngenbnbcosmic/ngencosmic*10.729 / pot_bnb_cosmic * run_pot;
  
  std::cout << cut << "\n"
	    << "sp: "<< entries_sp << " " << scaled_sp << " re: " << scaled_sp / old_scaled_sp * 100 << " te: " << scaled_sp / total_scaled_sp * 100 << "\n"
	    << "bnb: "<< entries_bnb << " " << scaled_bnb << " re: " << scaled_bnb / old_scaled_bnb * 100 << " te: " << scaled_bnb / total_scaled_bnb * 100 << "\n"
	    << "bnb_cosmic: "<< entries_bnb_cosmic << " " << scaled_bnb_cosmic << " re: " << scaled_bnb_cosmic / old_scaled_bnb_cosmic * 100 << " te: " << scaled_bnb_cosmic / total_scaled_bnb_cosmic * 100 << "\n"
	    << "cosmic: "<< entries_cosmic << " " << scaled_cosmic << " re: " << scaled_cosmic / old_scaled_cosmic * 100 << " te: " << scaled_cosmic / total_scaled_cosmic * 100 << "\n"
	    << "bkgr: " << scaled_bnb_cosmic + scaled_cosmic << " re: " << (scaled_bnb_cosmic + scaled_cosmic) / (old_scaled_bnb_cosmic + old_scaled_cosmic) * 100 << " te: " << (scaled_bnb_cosmic + scaled_cosmic) / (total_scaled_bnb_cosmic + total_scaled_cosmic) * 100 << "\n"
	    << "sig: " << scaled_sp / sqrt(scaled_bnb_cosmic + scaled_cosmic) << "\n"
	    << "----------------------------\n";
 
}



void cout_eff() {

  /*
  vertex_tree_sp->AddFriend((std::string(vertex_tree_sp->GetName()) + "_friend").c_str());
  vertex_tree_bnb->AddFriend((std::string(vertex_tree_sp->GetName()) + "_friend").c_str());
  vertex_tree_bnb_cosmic->AddFriend((std::string(vertex_tree_sp->GetName()) + "_friend").c_str());
  vertex_tree_cosmic->AddFriend((std::string(vertex_tree_sp->GetName()) + "_friend").c_str());
  */

  double scaled_sp =
    double(vertex_tree_sp->GetEntries(signal_definition.c_str())) / pot_sp * run_pot;
  double scaled_bnb =
    double(vertex_tree_bnb->GetEntries()) / pot_bnb * run_pot;
  double scaled_bnb_cosmic =
    double(vertex_tree_bnb_cosmic->GetEntries()) / pot_bnb_cosmic * run_pot;
  double scaled_cosmic = 
    double(vertex_tree_cosmic->GetEntries()) * ngenbnbcosmic/ngencosmic*10.729 / pot_bnb_cosmic * run_pot;
 
  double scaled_back = scaled_bnb_cosmic + scaled_cosmic;


  //MCC8
  std::string cut = "";
  cout_selection(cut, scaled_sp, scaled_bnb, scaled_bnb_cosmic, scaled_cosmic, scaled_back);
  cut += "passed_swtrigger == 1";
  cout_selection(cut, scaled_sp, scaled_bnb, scaled_bnb_cosmic, scaled_cosmic, scaled_back);
  cut += " && shower_dist_to_flashzcenter <= 38";
  cout_selection(cut, scaled_sp, scaled_bnb, scaled_bnb_cosmic, scaled_cosmic, scaled_back); 
  cut += " && totalpe_ibg_sum > 139";
  cout_selection(cut, scaled_sp, scaled_bnb, scaled_bnb_cosmic, scaled_cosmic, scaled_back);
  cut += " && reco_nu_vtx_fid_contained == 1";
  cout_selection(cut, scaled_sp, scaled_bnb, scaled_bnb_cosmic, scaled_cosmic, scaled_back);
  cut += " && summed_associated_reco_shower_energy > 0.07";
  cout_selection(cut, scaled_sp, scaled_bnb, scaled_bnb_cosmic, scaled_cosmic, scaled_back);
  cut += " && longest_asso_track_displacement <= 32";
  cout_selection(cut, scaled_sp, scaled_bnb, scaled_bnb_cosmic, scaled_cosmic, scaled_back);

}



void cout_test() {

  /*
  vertex_tree_sp->AddFriend((std::string(vertex_tree_sp->GetName()) + "_friend").c_str());
  vertex_tree_bnb->AddFriend((std::string(vertex_tree_sp->GetName()) + "_friend").c_str());
  vertex_tree_bnb_cosmic->AddFriend((std::string(vertex_tree_sp->GetName()) + "_friend").c_str());
  vertex_tree_cosmic->AddFriend((std::string(vertex_tree_sp->GetName()) + "_friend").c_str());
  */

  /*
  double scaled_sp =
    double(vertex_tree_sp->GetEntries(signal_definition.c_str())) / pot_sp * run_pot;
  double scaled_bnb =
    double(vertex_tree_bnb->GetEntries()) / pot_bnb * run_pot;
  double scaled_bnb_cosmic =
    double(vertex_tree_bnb_cosmic->GetEntries()) / pot_bnb_cosmic * run_pot;
  double scaled_cosmic = 
    double(vertex_tree_cosmic->GetEntries()) * ngenbnbcosmic/ngencosmic*10.729 / pot_bnb_cosmic * run_pot;
 
  double scaled_back = scaled_bnb_cosmic + scaled_cosmic;
  */

  double scaled_sp =
    double(vertex_tree_sp->GetEntries(signal_definition.c_str()));;
  double scaled_bnb =
    double(vertex_tree_bnb->GetEntries());
  double scaled_bnb_cosmic =
    double(vertex_tree_bnb_cosmic->GetEntries());
  double scaled_cosmic = 
    double(vertex_tree_cosmic->GetEntries());
 
  double scaled_back = scaled_bnb_cosmic + scaled_cosmic;


  /*
  std::string cut = "";
  cout_selection(cut, scaled_sp, scaled_bnb, scaled_bnb_cosmic, scaled_cosmic, scaled_back);
  cut += "passed_swtrigger == 1";
  //cout_selection(cut, scaled_sp, scaled_bnb, scaled_bnb_cosmic, scaled_cosmic, scaled_back);
  //cut += " && closest_asso_shower_dist_to_flashzcenter <= 279.754";
  cout_selection(cut, scaled_sp, scaled_bnb, scaled_bnb_cosmic, scaled_cosmic, scaled_back);
  cut += " && totalpe_ibg_sum > 236.937";
  cout_selection(cut, scaled_sp, scaled_bnb, scaled_bnb_cosmic, scaled_cosmic, scaled_back);
  cut += " && reco_nu_vtx_fid_contained == 1";
  cout_selection(cut, scaled_sp, scaled_bnb, scaled_bnb_cosmic, scaled_cosmic, scaled_back);
  cut += " && summed_associated_reco_shower_energy > 0.118569";
  cout_selection(cut, scaled_sp, scaled_bnb, scaled_bnb_cosmic, scaled_cosmic, scaled_back);
  //cut += " && longest_asso_track_displacement <= 147.11";
  //cout_selection(cut, scaled_sp, scaled_bnb, scaled_bnb_cosmic, scaled_cosmic, scaled_back);
  */

  std::string cut = "passed_swtrigger == 1 && reco_nu_vtx_fid_contained == 1";
  cout_selection(cut, scaled_sp, scaled_bnb, scaled_bnb_cosmic, scaled_cosmic, scaled_back);
  cut += " && closest_asso_shower_dist_to_flashzcenter <= 43.8007 && totalpe_ibg_sum <= 28190.4 && summed_associated_reco_shower_energy > 0.302242 && longest_asso_track_displacement <= 28.474";
  cout_selection(cut, scaled_sp, scaled_bnb, scaled_bnb_cosmic, scaled_cosmic, scaled_back);

}



void plot_vertices_per_event(TTree * tree,
			     std::string const & cname,
			     std::string const & binning,
			     std::string const & title = "",
			     std::string const & xtitle = "",
			     std::string const & ytitle = "",
			     double const scale = 1) {
	
  TObjArray * branches = tree->GetListOfBranches();
  TCanvas * canvas = new TCanvas(cname.c_str());
  TLegend * leg = new TLegend(0.6, 0.9, 0.9, 0.6);
  
  std::vector<std::string> leg_entries = 
    {"Total",
     "Passed Software Trigger",
     "Shower - Beam Spill Flash Distance <= 40 cm",
     "Total In Beam Spill PE > 140",
     "Reco Vertex Fiducial Volume Contained",
     "Summed Shower Energy > 70 MeV",
     "Longest Associated Track Length <= 30 cm"};

  int color_offset = 2;
  std::vector<int> cut_colors =
    {kGray+color_offset,
     kCyan+color_offset,
     kBlue+color_offset,
     kGreen+color_offset, 
     kRed+color_offset,
     kYellow+color_offset,
     kMagenta+color_offset};
  
  double largest_ymax = 0;
  for(int i = 0; i < branches->GetEntries(); ++i) {
    std::string const branch_name = branches->At(i)->GetName();
    std::string const hname = "h" + std::to_string(i);
    std::string const draw = branch_name + ">>" + hname + binning;
    if(i == 0) {
      tree->Draw(draw.c_str(), "");
      TH1F * hist = (TH1F*)gDirectory->Get(hname.c_str());
      hist->SetStats(0);
      hist->SetTitle(title.c_str());
      hist->GetXaxis()->SetTitle(xtitle.c_str());
      hist->GetXaxis()->CenterTitle();
      hist->GetYaxis()->SetTitle(ytitle.c_str());
      hist->GetYaxis()->CenterTitle();
    }
    else tree->Draw(draw.c_str(), "", "same");
    TH1F * hist = (TH1F*)gDirectory->Get(hname.c_str());
    hist->SetLineColor(cut_colors.at(i));
    hist->SetLineWidth(3);
    //hist_errors(hist, 1./hist->Integral(1, hist->GetNbinsX()+1));
    hist_errors(hist, scale);
    //TH1 * histe = (TH1*)gDirectory->Get((std::string(hist->GetName())+"e").c_str());
    //double ymax = histe->GetBinContent(histe->GetMaximumBin()) + histe->GetBinError(histe->GetMaximumBin());
    double ymax = hist->GetBinContent(hist->GetMaximumBin());
    if(ymax > largest_ymax) largest_ymax = ymax;
    leg->AddEntry(hname.c_str(), leg_entries.at(i).c_str());
  }
  
  TH1F * hist = (TH1F*)gDirectory->Get("h0");
  hist->GetYaxis()->SetRangeUser(0, 1.1*largest_ymax);

  leg->Draw();
  canvas->Write();
  
  delete canvas;
  delete leg;

}



/*
void selected_vertices(TTree * vertex_tree, TTree * event_tree, bool const plot = false) {

  int is_delta_rad = -1;
  int true_nu_vtx_fid_contained = -1;
  
  int event_number = -1;
  int passed_swtrigger = -1;
  double shower_dist_to_flashzcenter = -1;
  double totalpe_ibg_sum = -1;
  int reco_nu_vtx_fid_contained = -1;
  std::vector<larlite::shower> * associated_shower_info = nullptr;
  //double most_energetic_shower_reco_energy = -1;
  double longest_asso_track_displacement = -1;

  vertex_tree->SetBranchAddress("is_delta_rad", &is_delta_rad);
  vertex_tree->SetBranchAddress("true_nu_vtx_fid_contained", &true_nu_vtx_fid_contained);

  vertex_tree->SetBranchAddress("event_number", &event_number);
  vertex_tree->SetBranchAddress("passed_swtrigger", &passed_swtrigger);
  vertex_tree->SetBranchAddress("shower_dist_to_flashzcenter", &shower_dist_to_flashzcenter);
  vertex_tree->SetBranchAddress("totalpe_ibg_sum", &totalpe_ibg_sum);
  vertex_tree->SetBranchAddress("reco_nu_vtx_fid_contained", &reco_nu_vtx_fid_contained);
  vertex_tree->SetBranchAddress("associated_shower_info", &associated_shower_info);
  //vertex_tree->SetBranchAddress("most_energetic_shower_reco_energy", &most_energetic_shower_reco_energy);
  vertex_tree->SetBranchAddress("longest_asso_track_displacement", &longest_asso_track_displacement);

  vpe_v_size = 7;
  std::vector< std::map<int, int> > vpe_v(vpe_v_size, std::map<int,int>());
  
  for(int i = 0; i < vertex_tree->GetEntries(); ++i) {
    
    vertex_tree->GetEntry(i);
    
    double summed_shower_energy = 0;
    for(larlite::shower const & s : *associated_shower_info) {
      summed_shower_energy += s.Energy();
    }

    if(vertex_tree == vertex_tree_sp && !(is_delta_rad == 1 && true_nu_vtx_fid_contained == 1)) continue;
    
    for(auto & vpe_map : vpe_v) {
      auto vpe_it = vpe_map.find(event_number);
      if(vpe_it == vpe_map.end()) {
	vpe_it = vpe_map.emplace(event_number, 0).first;
      }
    }

    int vpe_v_i = -1;
    ++vpe_v.at(++vpe_v_i).find(event_number)->second;
    if(passed_swtrigger == 1) {
      ++vpe_v.at(++vpe_v_i).find(event_number)->second;
      if(shower_dist_to_flashzcenter <= 38) {
	++vpe_v.at(++vpe_v_i).find(event_number)->second;
	if(totalpe_ibg_sum > 139) {
	  ++vpe_v.at(++vpe_v_i).find(event_number)->second;
	  if(reco_nu_vtx_fid_contained == 1) {
	    ++vpe_v.at(++vpe_v_i).find(event_number)->second;
	    if(summed_shower_energy > 0.07) {
	      ++vpe_v.at(++vpe_v_i).find(event_number)->second;
	      if(longest_asso_track_displacement <= 32) {
		++vpe_v.at(++vpe_v_i).find(event_number)->second;
	      }
	    }
	  }
	}
      }
    }
    
  }

  std::string tname;
  std::string cname;
  std::string title;
  double scaling = 1;
  if(vertex_tree == vertex_tree_sp) {
    tname = "tree_friend_sp";
    cname = "sp";
    title = "NC #Delta Radiative";
    scaling = run_pot / pot_sp;
  }
  else if(vertex_tree == vertex_tree_bnb) {
    tname = "tree_friend_bnb";
    cname = "bnb";
    title = "BNB Inclusive";
    scaling = run_pot / pot_bnb;
  }
  else if(vertex_tree == vertex_tree_bnb_cosmic) {
    tname = "tree_friend_bnb_cosmic";  
    cname = "bnb_cosmic";
    title = "BNB + Cosmic";
    scaling = run_pot / pot_bnb_cosmic;
  }
  else if(vertex_tree == vertex_tree_cosmic) {
    tname = "tree_friend_cosmic";
    cname = "cosmic";
    title = "Intime Cosmic";
    scaling = ngenbnbcosmic/ngencosmic*10.729 / pot_bnb_cosmic * run_pot;
  }

  TTree * tree_friend = new TTree(tname.c_str(), "");

  std::vector<int> vpe_cut(vpe_v_size, -1);
  for(int i = 0; i < vpe_v_size; ++i) {
    std::string const vpe_cut_str = "vertices_per_event_cut" + std::to_string(i);
    tree_friend->Branch(vpe_cut_str.c_str(), &vpe_cut.at(i), (vpe_cut_str + "/I").c_str());
  }

  TTree * event_tree_clone = event_tree->CloneTree();

  event_tree_clone->SetBranchAddress("is_delta_rad", &is_delta_rad);
  event_tree_clone->SetBranchAddress("true_nu_vtx_fid_contained", &true_nu_vtx_fid_contained);
  event_tree_clone->SetBranchAddress("event_number", &event_number);

  std::vector<int> vpe_test(vpe_v_size, 0);
  for(int i = 0; i < event_tree_clone->GetEntries(); ++i) {
    event_tree_clone->GetEntry(i);
    if(event_tree == event_tree_sp && !(is_delta_rad == 1 && true_nu_vtx_fid_contained == 1)) {
      for(int j = 0; j < vpe_v_size; ++j) vpe_cut.at(j) = -1;
      tree_friend->Fill();
      continue;
    }
    for(int j = 0; j < vpe_v_size; ++j) {
      auto & vpe_map = vpe_v.at(j);
      auto const vpe_it = vpe_map.find(event_number);
      if(vpe_it == vpe_map.end()) {
	vpe_cut.at(j) = 0;
	continue;
      }
      vpe_cut.at(j) = vpe_it->second;
      vpe_test.at(j) += vpe_it->second;
    }

    //if(vpe_cut.at(vpe_v_size-1) > 0) std::cout << i << " " << event_number << "\n";

    tree_friend->Fill();
  }

  event_tree_clone->SetDirectory(0);
  event_tree_clone->Delete();

  if(plot) {
    plot_vertices_per_event(tree_friend,
			    cname,
			    "(8, 1, 9)",
			    title,
			    "Selected Vertices per Event",
			    "Number of Events",
			    1);
  }

  reset();
  
  std::cout << "---------------------\n";
  if(event_tree == event_tree_sp) std::cout << "Number of events: " << event_tree->GetEntries(signal_definition.c_str()) << "\n";
  else std::cout << "Number of events: " << event_tree->GetEntries() << "\n";
  for(int const i : vpe_test) {
    std::cout << i << "\n";
  }
  std::cout << "\n";

}



void vertices_per_event(bool const plot = false) {

  selected_vertices(vertex_tree_sp, event_tree_sp, plot);
  selected_vertices(vertex_tree_bnb, event_tree_bnb, plot);
  selected_vertices(vertex_tree_bnb_cosmic, event_tree_bnb_cosmic, plot);
  selected_vertices(vertex_tree_cosmic, event_tree_cosmic, plot);

}
*/


std::string maximize(std::string const & var,
		     std::string const & binning,
		     std::string const & min,
		     std::string const & max,
		     std::string cut,
		     double & scaled_sig,
		     double & scaled_back,
		     std::string const & graph_name = "",
		     std::string const & graph_xtitle = "",
		     std::string const & graph_ytitle = "") {
  
  double const old_ss = scaled_sig;
  double const old_sb = scaled_back;

  TGraph * graph = nullptr;
  TGraph * graph_eff = nullptr;
  if(graph_name != "") {
    graph = new TGraph();
    graph->SetName(graph_name.c_str());
    graph_eff = new TGraph();
    graph_eff->SetName((graph_name + "_eff").c_str());
  }

  std::string is_delta_rad = signal_definition + " && ";
  if(cut != "") cut += " && ";
  
  double const dmin = std::stod(min);
  double const dmax = std::stod(max);
  double const interval = (dmax - dmin) / std::stoi(binning);

  double largest_sig = 0;
  double ls_interval = 0;
  std::string best_var_cut;

  double const scaled_sp =
    double(vertex_tree_sp->GetEntries(signal_definition.c_str())) / pot_sp * run_pot;
  double const scaled_bnb_cosmic =
    double(vertex_tree_bnb_cosmic->GetEntries()) / pot_bnb_cosmic * run_pot;
  double const scaled_cosmic = 
    double(vertex_tree_cosmic->GetEntries()) * ngenbnbcosmic/ngencosmic*10.729 / pot_bnb_cosmic * run_pot;
  
  int graph_index = -1;
  for(double d = dmin; d < dmax; d += interval) {
  
    std::string const var_cut =
      var + std::to_string(d);
      
    double const entries_sp =
      vertex_tree_sp->GetEntries((is_delta_rad + cut + var_cut).c_str());
    double const entries_bnb_cosmic =
      vertex_tree_bnb_cosmic->GetEntries((cut + var_cut).c_str());
    double const entries_cosmic = 
      vertex_tree_cosmic->GetEntries((cut + var_cut).c_str());
    
    double const ss = entries_sp / pot_sp * run_pot;
    double const sb = entries_bnb_cosmic / pot_bnb_cosmic * run_pot + entries_cosmic * ngenbnbcosmic/ngencosmic*10.729 / pot_bnb_cosmic * run_pot;

    double const sig = ss / sqrt(sb);
    
    if(sig > largest_sig) {
      largest_sig = sig;
      ls_interval = d;
      best_var_cut = var_cut;
      scaled_sig = ss;
      scaled_back = sb;
    }

    if(graph) {
      graph->SetPoint(++graph_index, d, sig);
      graph_eff->SetPoint(graph_index, d, ss/scaled_sp);
    }

  }

  if(graph) {
    TCanvas * canvas = new TCanvas(graph_name.c_str());
    graph->Draw();
    graph->GetXaxis()->SetTitle(graph_xtitle.c_str());
    graph->GetXaxis()->CenterTitle();
    graph->GetYaxis()->SetTitle(graph_ytitle.c_str());
    graph->GetYaxis()->CenterTitle();
    canvas->Write();
    TCanvas * canvas_eff = new TCanvas((graph_name + "_eff").c_str());
    graph_eff->Draw();
    graph_eff->GetXaxis()->SetTitle(graph_xtitle.c_str());
    graph_eff->GetXaxis()->CenterTitle();
    graph_eff->GetYaxis()->SetTitle("Signal Efficiency");
    graph_eff->GetYaxis()->CenterTitle();
    canvas_eff->Write();
    delete graph;
    delete graph_eff;
    delete canvas;
    delete canvas_eff;
  }
  
  std::cout << "------------------------------------------------------------------\n"
	    << "Best sig for: " << cut + var << ls_interval << ": " << largest_sig << "\n"
	    << "Scaled sig: " << scaled_sig << " Scaled back: " << scaled_back << "\n"
	    << "Relative efficiency - sig: " << scaled_sig / old_ss * 100 << " back: " << scaled_back / old_sb * 100 << "\n"
	    << "Total efficiency - sig: " << scaled_sig / scaled_sp * 100 << " back: " << scaled_back / (scaled_bnb_cosmic + scaled_cosmic)* 100 << "\n";

  return best_var_cut;

}



void maximize_all() {

  double const scaled_sp =
    double(vertex_tree_sp->GetEntries(signal_definition.c_str())) / pot_sp * run_pot;
  double const scaled_bnb_cosmic =
    double(vertex_tree_bnb_cosmic->GetEntries()) / pot_bnb_cosmic * run_pot;
  double const scaled_cosmic = 
    double(vertex_tree_cosmic->GetEntries()) * ngenbnbcosmic/ngencosmic*10.729 / pot_bnb_cosmic * run_pot;

  double scaled_sig = scaled_sp;
  double scaled_back = scaled_bnb_cosmic + scaled_cosmic;

  std::cout << "Scaled sig: " << scaled_sig << " Scaled back: " << scaled_back << "\n";
  std::string cut = maximize("passed_swtrigger == ", "2", "0", "2", "", scaled_sig, scaled_back);

  std::string signal_cut = signal_definition;
  if(cut != "") signal_cut += " && ";

  cut = cut + " && " + maximize("shower_dist_to_flashzcenter <= ", "500", "0", "500", cut, scaled_sig, scaled_back, "shower_dist_to_flashzcenter_sig", "Distance from Most Energetic Shower to FlashZCenter [cm]", "signal / sqrt(background)");

  cut = cut + " && " + maximize("totalpe_ibg_sum > ", "100", "0", "200", cut, scaled_sig, scaled_back, "totalpe_ibg_sum_sig", "Total PE (3.2 - 4.8 us)", "signal / sqrt(background)");

  cut = cut + " && " + maximize("reco_nu_vtx_fid_contained == ", "2", "0", "2", cut, scaled_sig, scaled_back);

  cut = cut + " && " + maximize("Sum$(associated_shower_info.fTotalEnergy) > ", "500", "0", "1", cut, scaled_sig, scaled_back, "most_energetic_shower_reco_energy_sig", "Most Energetic Shower Energy [GeV]", "signal / sqrt(background)");

  //////////////

  cut = cut + " && " + maximize("reco_asso_tracks <= ", "20", "0", "20", cut, scaled_sig, scaled_back, "reco_asso_tracks_sig", "Number of Associated Tracks", "signal / sqrt(background)");

  //////////////

  cut = cut + " && " + maximize("longest_asso_track_displacement <= ", "100", "0", "100", cut, scaled_sig, scaled_back, "longest_asso_track_displacement_sig", "Longest Associated Track Length [cm]", "signal / sqrt(background)");

  //////////////

  cut = cut + " && " + maximize("closest_asso_reco_shower_dist <= ", "200", "0", "200", cut, scaled_sig, scaled_back, "closest_asso_reco_shower_dist", "Distance Between Reco Vertex and Closest Shower Vertex [cm]", "signal / sqrt(background)");

  //////////////

}



void maximize_all_edit() {

  double const scaled_sp =
    double(vertex_tree_sp->GetEntries(signal_definition.c_str())) / pot_sp * run_pot;
  double const scaled_bnb_cosmic =
    double(vertex_tree_bnb_cosmic->GetEntries()) / pot_bnb_cosmic * run_pot;
  double const scaled_cosmic = 
    double(vertex_tree_cosmic->GetEntries()) * ngenbnbcosmic/ngencosmic*10.729 / pot_bnb_cosmic * run_pot;

  double scaled_sig = scaled_sp;
  double scaled_back = scaled_bnb_cosmic + scaled_cosmic;

  std::string const passed_swtrigger = "passed_swtrigger == 1";
  std::string const shower_dist_to_flashzcenter = "shower_dist_to_flashzcenter <= 38";
  std::string const totalpe_ibg_sum = "totalpe_ibg_sum > 139";
  std::string const reco_nu_vtx_fid_contained = "reco_nu_vtx_fid_contained == 1";
  std::string const sum_reco_shower_energy = "Sum$(associated_shower_info.fTotalEnergy) > 0.07";
  std::string const longest_asso_track_displacement = "longest_asso_track_displacement <= 32";
  std::string const shower_dist_to_vert = "";  

  /*

  //////////////////////////////

  std::cout << "Scaled sig: " << scaled_sig << " Scaled back: " << scaled_back << "\n";

  std::string const shower_dist_to_flashzcenter_cut = passed_swtrigger + " && " + totalpe_ibg_sum + " && " + reco_nu_vtx_fid_contained + " && " + sum_reco_shower_energy + " && " + longest_asso_track_displacement;
  maximize("shower_dist_to_flashzcenter <= ", "500", "0", "100", shower_dist_to_flashzcenter_cut, scaled_sig, scaled_back, "shower_dist_to_flashzcenter_sig", "Distance from Most Energetic Shower to FlashZCenter [cm]", "signal / sqrt(background)");

  //////////////////////////////

  scaled_sig = scaled_sp;
  scaled_back = scaled_bnb_cosmic + scaled_cosmic;
  
  std::string const totalpe_ibg_sum_cut = passed_swtrigger + " && " + shower_dist_to_flashzcenter + " && " + reco_nu_vtx_fid_contained + " && " + sum_reco_shower_energy + " && " + longest_asso_track_displacement;
  maximize("totalpe_ibg_sum > ", "200", "0", "200", totalpe_ibg_sum_cut, scaled_sig, scaled_back, "totalpe_ibg_sum_sig", "Total PE (3.2 - 4.8 us)", "signal / sqrt(background)");

  //////////////////////////////

  scaled_sig = scaled_sp;
  scaled_back = scaled_bnb_cosmic + scaled_cosmic;

  std::string const summed_reco_energy_cut = passed_swtrigger + " && " + shower_dist_to_flashzcenter + " && " + totalpe_ibg_sum + " && " + reco_nu_vtx_fid_contained + " && " + longest_asso_track_displacement;
  maximize("Sum$(associated_shower_info.fTotalEnergy) > ", "200", "0", "1", summed_reco_energy_cut, scaled_sig, scaled_back, "summed_reco_energy_cut_sig", "Associated Shower Energy Sum [GeV]", "signal / sqrt(background)");

  //////////////////////////////

  scaled_sig = scaled_sp;
  scaled_back = scaled_bnb_cosmic + scaled_cosmic;

  std::string const longest_asso_track_displacement_cut = passed_swtrigger + " && " + shower_dist_to_flashzcenter + " && " + totalpe_ibg_sum + " && " + reco_nu_vtx_fid_contained + " && " + sum_reco_shower_energy;
  maximize("longest_asso_track_displacement <= ", "100", "0", "100", longest_asso_track_displacement_cut, scaled_sig, scaled_back, "longest_asso_track_displacement_sig", "Longest Associated Track Length [cm]", "signal / sqrt(background)");

  //////////////////////////////

  scaled_sig = scaled_sp;
  scaled_back = scaled_bnb_cosmic + scaled_cosmic;

  std::string const reco_asso_tracks_cut = passed_swtrigger + " && " + shower_dist_to_flashzcenter + " && " + totalpe_ibg_sum + " && " + reco_nu_vtx_fid_contained + " && " + sum_reco_shower_energy + " && " + longest_asso_track_displacement;
  maximize("reco_asso_tracks <= ", "10", "0", "10", reco_asso_tracks_cut, scaled_sig, scaled_back, "reco_asso_tracks", "Number of Associated Tracks", "signal / sqrt(background)");

  */

  //////////////////////////////

  scaled_sig = scaled_sp;
  scaled_back = scaled_bnb_cosmic + scaled_cosmic;

  //std::string const closest_asso_reco_shower_dist_cut = passed_swtrigger + " && " + shower_dist_to_flashzcenter + " && " + totalpe_ibg_sum + " && " + reco_nu_vtx_fid_contained + " && " + sum_reco_shower_energy + " && " + longest_asso_track_displacement;
  std::string const closest_asso_reco_shower_dist_cut =  "";
  maximize("closest_asso_reco_shower_dist > ", "100", "0", "100", closest_asso_reco_shower_dist_cut, scaled_sig, scaled_back, "closest_asso_reco_shower_dist", "Distance Between Shower Start and Reco Vertex [cm]", "signal / sqrt(background)");

  plotsupimp("closest_asso_reco_shower_dist",
	     "closest_asso_reco_shower_dist",
	     "(100, 0, 10)",
	     closest_asso_reco_shower_dist_cut,
	     "",
	     "",
	     "Distance Between Shower Start and Reco Vertex [cm]"
	     "Area Normalized");

  //////////////////////////////

}



void maximize2d(std::string const & xvar,
		std::string const & xbinning,
		std::string const & xmin,
		std::string const & xmax,
		std::string const & yvar,
		std::string const & ybinning,
		std::string const & ymin,
		std::string const & ymax,
		std::string cut) {

  std::string is_delta_rad = signal_definition + " && ";
  if(cut != "") cut += " && ";

  double const dxmin = std::stod(xmin);
  double const dxmax = std::stod(xmax);
  double const xinterval = (dxmax - dxmin) / std::stoi(xbinning);
  
  double const dymin = std::stod(ymin);
  double const dymax = std::stod(ymax);
  double const yinterval = (dymax - dymin) / std::stoi(ybinning);

  double largest_sig = 0;
  double ls_xinterval = 0;
  double ls_yinterval = 0;
  std::string best_xvar_cut;
  std::string best_yvar_cut;

  for(double dx = dxmin; dx < dxmax; dx += xinterval) {

    cout << dx << "\n";

    std::string const xvar_cut =
      xvar + std::to_string(dx) + " && ";

    for(double dy = dymin; dy < dymax; dy += yinterval) {
      
      std::string const yvar_cut =
	yvar + std::to_string(dy);      

      double const entries_sp =
	vertex_tree_sp->GetEntries((is_delta_rad + cut + xvar_cut + yvar_cut).c_str());
      double const entries_bnb_cosmic =
	vertex_tree_bnb_cosmic->GetEntries((cut + xvar_cut + yvar_cut).c_str());
      double const entries_cosmic = 
	vertex_tree_cosmic->GetEntries((cut + xvar_cut + yvar_cut).c_str());
     
      double const ss = entries_sp / pot_sp * run_pot;
      double const sb = (entries_bnb_cosmic / pot_bnb_cosmic * run_pot) + (entries_cosmic * ngenbnbcosmic/ngencosmic*10.729 / pot_bnb_cosmic * run_pot);

      double const sig = ss / sqrt(sb);

      if(sig > largest_sig) {
	largest_sig = sig;
	ls_xinterval = dx;
	ls_yinterval = dy;
	best_xvar_cut = xvar_cut;
	best_yvar_cut = yvar_cut;
      }

    }
    
  }

  std::cout << "------------------------------------------------------------------\n"
	    << "Best sig for: " << cut << best_xvar_cut << best_yvar_cut << " dx: " << ls_xinterval << " dy: " << ls_yinterval << ": " << largest_sig << "\n";

}



void maximize_all2d() {

  /*
  std::string const cut = "passed_swtrigger == 1 && totalpe_ibg_sum > 66 && reco_nu_vtx_fid_contained == 1";

  maximize2d("longest_asso_track_displacement <= ",
	     "100",
	     "0",
	     "100",
	     "most_energetic_shower_reco_energy > ",
	     "200",
	     "0",
	     "1",
	     cut);
  */

  std::string const cut = "passed_swtrigger == 1 && reco_nu_vtx_fid_contained == 1 && longest_asso_track_displacement <= 80";

  maximize2d("totalpe_ibg_sum > ",
	     "100",
	     "0",
	     "100",
	     "most_energetic_shower_reco_energy > ",
	     "200",
	     "0",
	     "1",
	     cut);

}



void marginal_dist() {

  double const scaled_sp =
    double(vertex_tree_sp->GetEntries(signal_definition.c_str())) / pot_sp * run_pot;
  double const scaled_bnb_cosmic =
    double(vertex_tree_bnb_cosmic->GetEntries()) / pot_bnb_cosmic * run_pot;
  double const scaled_cosmic = 
    double(vertex_tree_cosmic->GetEntries()) * ngenbnbcosmic/ngencosmic*10.729 / pot_bnb_cosmic * run_pot;
  
  std::cout << "\n\nlongest_asso_track_displacement\n";
  std::cout << "==================================================================================================\n";

  double scaled_sig = scaled_sp;
  double scaled_back = scaled_bnb_cosmic + scaled_cosmic;

  std::cout << "Scaled sig: " << scaled_sig << " Scaled back: " << scaled_back << "\n";
  std::string cut = maximize("passed_swtrigger == ", "2", "0", "2", "", scaled_sig, scaled_back);
  cut = cut + " && " + maximize("totalpe_ibg_sum > ", "100", "0", "200", cut, scaled_sig, scaled_back);
  cut = cut + " && " + maximize("shower_dist_to_flashzcenter <= ", "500", "0", "500", cut, scaled_sig, scaled_back);
  cut = cut + " && " + maximize("reco_nu_vtx_fid_contained == ", "2", "0", "2", cut, scaled_sig, scaled_back);
  cut = cut + " && " + maximize("most_energetic_shower_reco_energy > ", "200", "0", "1", cut, scaled_sig, scaled_back);
  cut = cut + " && " + maximize("longest_asso_track_displacement <= ", "100", "0", "100", cut, scaled_sig, scaled_back, "longest_asso_track_displacement_sig", "Longest Associated Track Length [cm]", "signal / sqrt(background)");

  std::cout << "\n\npassed_swtrigger\n";
  std::cout << "==================================================================================================\n";

  scaled_sig = scaled_sp;
  scaled_back = scaled_bnb_cosmic + scaled_cosmic;

  std::cout << "Scaled sig: " << scaled_sig << " Scaled back: " << scaled_back << "\n";
  cut = maximize("totalpe_ibg_sum > ", "100", "0", "200", "", scaled_sig, scaled_back);
  cut = cut + " && " + maximize("shower_dist_to_flashzcenter <= ", "500", "0", "500", cut, scaled_sig, scaled_back);
  cut = cut + " && " + maximize("reco_nu_vtx_fid_contained == ", "2", "0", "2", cut, scaled_sig, scaled_back);
  cut = cut + " && " + maximize("most_energetic_shower_reco_energy > ", "200", "0", "1", cut, scaled_sig, scaled_back);
  cut = cut + " && " + maximize("longest_asso_track_displacement <= ", "100", "0", "100", cut, scaled_sig, scaled_back);
  cut = cut + " && " + maximize("passed_swtrigger == ", "2", "0", "2", cut, scaled_sig, scaled_back);

  std::cout << "\n\ntotalpe_ibg_sum\n";
  std::cout << "==================================================================================================\n";
  
  scaled_sig = scaled_sp;
  scaled_back = scaled_bnb_cosmic + scaled_cosmic;

  std::cout << "Scaled sig: " << scaled_sig << " Scaled back: " << scaled_back << "\n";
  cut = maximize("passed_swtrigger == ", "2", "0", "2", "", scaled_sig, scaled_back);
  cut = cut + " && " + maximize("shower_dist_to_flashzcenter <= ", "500", "0", "500", cut, scaled_sig, scaled_back);
  cut = cut + " && " + maximize("reco_nu_vtx_fid_contained == ", "2", "0", "2", cut, scaled_sig, scaled_back);
  cut = cut + " && " + maximize("most_energetic_shower_reco_energy > ", "200", "0", "1", cut, scaled_sig, scaled_back);
  cut = cut + " && " + maximize("longest_asso_track_displacement <= ", "100", "0", "100", cut, scaled_sig, scaled_back);
  cut = cut + " && " + maximize("totalpe_ibg_sum > ", "100", "0", "200", cut, scaled_sig, scaled_back, "totalpe_ibg_sum_sig", "Total PE (3.2 - 4.8 us)", "signal / sqrt(background)");

  std::cout << "\n\nshower_dist_to_flashzcenter\n";
  std::cout << "==================================================================================================\n";
  
  scaled_sig = scaled_sp;
  scaled_back = scaled_bnb_cosmic + scaled_cosmic;

  std::cout << "Scaled sig: " << scaled_sig << " Scaled back: " << scaled_back << "\n";
  cut = maximize("passed_swtrigger == ", "2", "0", "2", "", scaled_sig, scaled_back);
  cut = cut + " && " + maximize("totalpe_ibg_sum > ", "100", "0", "200", cut, scaled_sig, scaled_back);
  cut = cut + " && " + maximize("reco_nu_vtx_fid_contained == ", "2", "0", "2", cut, scaled_sig, scaled_back);
  cut = cut + " && " + maximize("longest_asso_track_displacement <= ", "100", "0", "100", cut, scaled_sig, scaled_back);
  cut = cut + " && " + maximize("most_energetic_shower_reco_energy > ", "200", "0", "1", cut, scaled_sig, scaled_back);
  cut = cut + " && " + maximize("shower_dist_to_flashzcenter <= ", "500", "0", "500", cut, scaled_sig, scaled_back, "shower_dist_to_flashzcenter_sig", "Distance from Most Energetic Shower to FlashZCenter [cm]", "signal / sqrt(background)");

  std::cout << "\n\nreco_nu_vtx_fid_contained\n";
  std::cout << "==================================================================================================\n";
  
  scaled_sig = scaled_sp;
  scaled_back = scaled_bnb_cosmic + scaled_cosmic;

  std::cout << "Scaled sig: " << scaled_sig << " Scaled back: " << scaled_back << "\n";
  cut = maximize("passed_swtrigger == ", "2", "0", "2", "", scaled_sig, scaled_back);
  cut = cut + " && " + maximize("totalpe_ibg_sum > ", "100", "0", "200", cut, scaled_sig, scaled_back);
  cut = cut + " && " + maximize("shower_dist_to_flashzcenter <= ", "500", "0", "500", cut, scaled_sig, scaled_back);
  cut = cut + " && " + maximize("most_energetic_shower_reco_energy > ", "200", "0", "1", cut, scaled_sig, scaled_back);
  cut = cut + " && " + maximize("longest_asso_track_displacement <= ", "100", "0", "100", cut, scaled_sig, scaled_back);
  cut = cut + " && " + maximize("reco_nu_vtx_fid_contained == ", "2", "0", "2", cut, scaled_sig, scaled_back);

  std::cout << "\n\nmost_energetic_reco_shower_energy\n";
  std::cout << "==================================================================================================\n";
  
  scaled_sig = scaled_sp;
  scaled_back = scaled_bnb_cosmic + scaled_cosmic;

  std::cout << "Scaled sig: " << scaled_sig << " Scaled back: " << scaled_back << "\n";
  cut = maximize("passed_swtrigger == ", "2", "0", "2", "", scaled_sig, scaled_back);
  cut = cut + " && " + maximize("totalpe_ibg_sum > ", "100", "0", "200", cut, scaled_sig, scaled_back);
  cut = cut + " && " + maximize("shower_dist_to_flashzcenter <= ", "500", "0", "500", cut, scaled_sig, scaled_back);
  cut = cut + " && " + maximize("reco_nu_vtx_fid_contained == ", "2", "0", "2", cut, scaled_sig, scaled_back);
  cut = cut + " && " + maximize("longest_asso_track_displacement <= ", "100", "0", "100", cut, scaled_sig, scaled_back);
  cut = cut + " && " + maximize("most_energetic_shower_reco_energy > ", "200", "0", "1", cut, scaled_sig, scaled_back, "most_energetic_reco_shower_energy", "Most Energetic Shower Energy [GeV]", "signal / sqrt(background)");

  std::cout << "\n\nreco_asso_tracks\n";
  std::cout << "==================================================================================================\n";
  
  scaled_sig = scaled_sp;
  scaled_back = scaled_bnb_cosmic + scaled_cosmic;

  std::cout << "Scaled sig: " << scaled_sig << " Scaled back: " << scaled_back << "\n";
  cut = maximize("passed_swtrigger == ", "2", "0", "2", "", scaled_sig, scaled_back);
  cut = cut + " && " + maximize("totalpe_ibg_sum > ", "100", "0", "200", cut, scaled_sig, scaled_back);
  cut = cut + " && " + maximize("shower_dist_to_flashzcenter <= ", "500", "0", "500", cut, scaled_sig, scaled_back);
  cut = cut + " && " + maximize("reco_nu_vtx_fid_contained == ", "2", "0", "2", cut, scaled_sig, scaled_back);
  cut = cut + " && " + maximize("longest_asso_track_displacement <= ", "100", "0", "100", cut, scaled_sig, scaled_back);
  cut = cut + " && " + maximize("most_energetic_shower_reco_energy > ", "200", "0", "1", cut, scaled_sig, scaled_back, "most_energetic_reco_shower_energy", "Most Energetic Shower Energy [GeV]", "signal / sqrt(background)");
  cut = cut + " && " + maximize("reco_asso_tracks <= ", "20", "0", "20", cut, scaled_sig, scaled_back);

}



void plotdist() {
  
  std::string cut = "passed_swtrigger == 1";
  std::string signal_cut = signal_definition;
  if(cut != "") signal_cut += " && ";

  std::string binning = "(100, 0, 10)";

  plot(vertex_tree_sp,
       "totalpe_ibg_sum_sp",
       "totalpe_ibg_sum",
       binning,
       signal_cut + cut,
       "",
       "NC #Delta Radiative",
       "Total PE (3.2 - 4.8 us)",
       "Events per 6.6e20 POT",
       kBlue,
       run_pot / pot_sp);  
  
  plot(vertex_tree_bnb,
       "totalpe_ibg_sum_bnb",
       "totalpe_ibg_sum",
       binning,
       cut,
       "",
       "BNB Inclusive",
       "Total PE (3.2 - 4.8 us)",
       "Events per 6.6e20 POT",
       kRed,
       run_pot / pot_bnb);  

  plot(vertex_tree_bnb_cosmic,
       "totalpe_ibg_sum_bnb_cosmic",
       "totalpe_ibg_sum",
       binning,    
       cut,
       "",
       "BNB + Cosmic",
       "Total PE (3.2 - 4.8 us)",
       "Events per 6.6e20 POT",
       kGreen,
       run_pot / pot_bnb_cosmic);  

  plot(vertex_tree_cosmic,
       "totalpe_ibg_sum_cosmic",
       "totalpe_ibg_sum",
       binning,
       cut,
       "",
       "Intime Cosmic",
       "Total PE (3.2 - 4.8 us)",
       "Events per 6.6e20 POT",
       kMagenta,
       ngenbnbcosmic/ngencosmic*10.729 / pot_bnb_cosmic * run_pot);  

  /*
  showermultiplicity("");
  showerrecoenergy("Sum$(associated_shower_info.fTotalEnergy > 0.02 && associated_shower_info.fLength > 5)");
  recoassotracks("Sum$(associated_shower_info.fTotalEnergy > 0.02 && associated_shower_info.fLength > 5) && most_energetic_shower_reco_energy > 0.085 && reco_nu_vtx_fid_contained == 1");
  tracklength("Sum$(associated_shower_info.fTotalEnergy > 0.02 && associated_shower_info.fLength > 5) && most_energetic_shower_reco_energy > 0.085 && reco_nu_vtx_fid_contained == 1");
  */

}



void plot2d() {

  std::string cut = "";

  std::string signal_cut = signal_definition;
  if(cut != "") signal_cut += " && ";

  std::string binning = "(50, 0, 100, 50, 0, 1)";

  /////////////////////////////////////////////////////////////////////////////////////

  plot(vertex_tree_sp,
       "most_energetic_shower_reco_energy_v_totalpe_ibg_sum_sp",
       "most_energetic_shower_reco_energy:totalpe_ibg_sum",
       binning,
       signal_cut + cut,
       "colz",
       "NC #Delta Radiative",
       "Total PE (3.2 - 4.8 us)",
       "Most Energetic Shower Energy [GeV]",
       -1,
       run_pot / pot_sp);  

  plot(vertex_tree_bnb_cosmic,
       "most_energetic_shower_reco_energy_v_totalpe_ibg_sum_bnb_cosmic",
       "most_energetic_shower_reco_energy:totalpe_ibg_sum",
       binning,
       cut,
       "colz",
       "BNB + Cosmic",
       "Total PE (3.2 - 4.8 us)",
       "Most Energetic Shower Energy [GeV]",
       -1,
       run_pot / pot_bnb_cosmic);  
  
  plot(vertex_tree_cosmic,
       "most_energetic_shower_reco_energy_v_totalpe_ibg_sum_cosmic",
       "most_energetic_shower_reco_energy:totalpe_ibg_sum",
       binning,
       cut,
       "colz",
       "Cosmic",
       "Total PE (3.2 - 4.8 us)",
       "Most Energetic Shower Energy [GeV]",
       -1,
       ngenbnbcosmic/ngencosmic*10.729 / pot_bnb_cosmic * run_pot);  

  /////////////////////////////////////////////////////////////////////////////////////
  
  binning = "(50, 0, 500, 50, 0, 1)";

  plot(vertex_tree_sp,
       "most_energetic_shower_reco_energy_v_longest_asso_track_displacement_sp",
       "most_energetic_shower_reco_energy:longest_asso_track_displacement",
       binning,
       signal_cut + cut,
       "colz",
       "NC #Delta Radiative",
       "Longest Associated Track Length [cm]",
       "Most Energetic Shower Energy [GeV]",
       -1,
       run_pot / pot_sp);  

  plot(vertex_tree_bnb_cosmic,
       "most_energetic_shower_reco_energy_v_longest_asso_track_displacement_bnb_cosmic",
       "most_energetic_shower_reco_energy:longest_asso_track_displacement",
       binning,
       cut,
       "colz",
       "BNB + Cosmic",
       "Longest Associated Track Length [cm]",
       "Most Energetic Shower Energy [GeV]",
       -1,
       run_pot / pot_bnb_cosmic);  
  
  plot(vertex_tree_cosmic,
       "most_energetic_shower_reco_energy_v_longest_asso_track_displacement_cosmic",
       "most_energetic_shower_reco_energy:longest_asso_track_displacement",
       binning,
       cut,
       "colz",
       "Cosmic",
       "Longest Associated Track Length [cm]",
       "Most Energetic Shower Energy [GeV]",
       -1,
       ngenbnbcosmic/ngencosmic*10.729 / pot_bnb_cosmic * run_pot);  

  /////////////////////////////////////////////////////////////////////////////////////
  
}



/*
std::vector<int> get_type_hist(TTree * tree,
			       std::string const & type,
			       std::string const & we) {
  
  int type_var;
  TBranch * branch = tree->GetBranch(type.c_str());  
  tree->SetBranchAddress(type.c_str(), &type_var);
  
  std::vector<int> ret_v;
  for(int i = 1; i <= tree->GetEntries(); ++i) {
    branch->GetEntry(i);
    std::cout << type_var << "\n";
    if(type_var) {
      auto rv_it = std::find(ret_v.begin(), ret_v.end(), type_var);
      if(rv_it == ret_v.end()) ret_v.push_back(type_var);
    }
  }

  return ret_v;

}
*/



std::vector<int> get_type_hist(TTree * tree,
			       std::string const & type,
			       std::string const & we) {
  
  TCanvas * c = new TCanvas("temp");

  std::string htype_name = "h";
  tree->Draw((type+">>"+htype_name).c_str(), we.c_str());
  TH1 * h = (TH1*)gDirectory->Get(htype_name.c_str());
  double xmin = h->GetBinLowEdge(1);
  double xmax = h->GetBinLowEdge(h->GetNbinsX()+1);
  delete h;

  std::cout << "type_v min: " << xmin << " max: " << xmax << "\n";

  std::string htype2_name = "h2";
  std::string const binning = "("+std::to_string(xmax-xmin)+","+std::to_string(xmin)+","+std::to_string(xmax)+")"; 
  tree->Draw((type+">>"+htype2_name+binning).c_str(), we.c_str());
  h = (TH1*)gDirectory->Get(htype2_name.c_str());

  std::vector<int> ret_v;
  for(int i = 1; i <= h->GetNbinsX(); ++i) {
    if(h->GetBinContent(i)) ret_v.push_back(h->GetBinLowEdge(i));
  }

  delete c;
  delete h;

  return ret_v;

}



struct type_group {

  type_group(std::string type_name, std::vector<int> type_number, int color) :
    _type_name(type_name),
    _type_number(type_number),
    _color(color),
    _hist(nullptr) {}

  std::string _type_name;
  std::vector<int> _type_number;
  int _color;
  TH1 * _hist;

};



std::map<int, int> fill_type_map(std::vector<type_group> const & tg_v) {

  std::map<int, int> tg_m;

  for(int i = 0; i < tg_v.size(); ++i) {
    std::vector<int> const & nc = tg_v.at(i)._type_number;
    for(int const j : nc) {
      tg_m.emplace(j, i);
    }
  }

  return tg_m;

}



void background_by_type(std::string const & cname,
			std::string const & type,
			std::string const & draw,
			std::string const & binning,
			std::string const & weight,
			std::string const & opt,
			std::string const & title = "",
			std::string const & xtitle = "",
			std::string const & ytitle = "") {

  double const bnb_scaling = run_pot / pot_bnb;

  std::vector<int> const type_v = get_type_hist(vertex_tree_bnb, type, weight);

  std::string we = weight;
  if(we != "") we += " && ";

  int color_offset = 2;
  /*
  std::vector<type_group> tg_v =
    {type_group("Other", {1000, 1013, 1015, 1016, 1096, 1097}, kCyan+color_offset),
     type_group("CCQE", {1001}, kBlue+color_offset),
     type_group("NCQE", {1002}, kGreen+color_offset), 
     type_group("CCPi+", {1003, 1005}, kRed+color_offset),
     type_group("CCPi0", {1004}, kYellow+color_offset),
     type_group("NCPi0", {1006, 1008}, kMagenta+color_offset),
     type_group("NCPi+", {1007, 1014}, 1),
     type_group("NCPi-", {1009}, 2),
     type_group("CCDIS", {1091}, 3),
     type_group("NCDIS", {1092}, 4)};
  */
  std::vector<type_group> tg_v =
    {type_group("Other", {1000, 1002, 1007, 1009, 1013, 1014, 1015, 1016, 1096, 1097}, kCyan+color_offset),
     type_group("CCQE", {1001}, kBlue+color_offset),
     type_group("CCPi+", {1003, 1005}, kRed+color_offset),
     type_group("Pi0", {1004, 1006, 1008}, kYellow+color_offset),
     type_group("DIS", {1091, 1092}, 3)};
  std::map<int, int> tg_m = fill_type_map(tg_v);

  TCanvas * temp = new TCanvas("temp");
  for(int const m : type_v) {

    auto const tg_it = tg_m.find(m);
    if(tg_it == tg_m.end()) {
      std::cout << "No nuance channel: " << m << "\n";
      continue;
    }

    std::string const hname = "hm" + std::to_string(m);
    std::string const dr = draw + ">>" + hname + binning;
    
    vertex_tree_bnb->Draw(dr.c_str(), (we + type + " == " + std::to_string(m)).c_str(), opt.c_str());
    TH1 * hist = (TH1*)gDirectory->Get(hname.c_str());
    hist->Scale(bnb_scaling);

    type_group & tg = tg_v.at(tg_it->second);
    if(tg._hist == nullptr) {
      tg._hist = hist;
    }
    else {
      tg._hist->Add(hist);
    }

  }
  delete temp;

  THStack * stack = new THStack();
  TLegend * leg = new TLegend(0.6, 0.9, 0.9, 0.6);

  for(type_group const & tg : tg_v) {

    if(tg._hist == nullptr || tg._hist->GetEntries() == 0) continue;
    TH1 * hist = tg._hist;
    hist->SetLineColor(1);
    hist->SetFillColor(tg._color);
    stack->Add(hist);
    leg->AddEntry(hist, tg._type_name.c_str());
  
  }

  TCanvas * canvas = new TCanvas(cname.c_str());
  stack->Draw();
  stack->SetTitle(title.c_str());
  stack->GetXaxis()->SetTitle(xtitle.c_str());
  stack->GetXaxis()->CenterTitle();
  stack->GetYaxis()->SetTitle(ytitle.c_str());
  stack->GetYaxis()->CenterTitle();

  /*
  std::string const hename = "he";
  std::string const dr = draw + ">>" + hename + binning;
  vertex_tree_bnb->Draw(dr.c_str(), weight.c_str(), "samepe");
  TH1 * he = (TH1*)gDirectory->Get(hename.c_str());
  he->SetLineColor(1);
  */

  leg->Draw();

  canvas->Write();
  delete canvas;
  delete stack;
  delete leg;

}



void dist_by_closest_mcshower_pdg(TTree * vertex_tree,
				  std::string const & cname,
				  std::string const & type,
				  std::string const & type_title,
				  std::string const & draw,
				  std::string const & binning,
				  std::string const & we,
				  std::string const & opt,
				  std::string const & title = "",
				  std::string const & xtitle = "",
				  std::string const & ytitle = "",
				  double const scaling = 1) {

  std::vector<int> const type_v = get_type_hist(vertex_tree, type, we);

  std::vector<type_group> tg_v =
    {type_group("#gamma", {22}, kGreen+1),
     type_group("e^{-}", {11}, kYellow+1),
     type_group("e^{+}", {-11}, kYellow+2),
     type_group("#pi^{+}", {211}, kMagenta+1),
     type_group("#pi^{-}", {-211}, kMagenta+2),
     type_group("#mu^{-}", {13}, kBlue+1),
     type_group("#mu^{+}", {-13}, kBlue+2),
     type_group("#pi^{0}", {111}, kCyan+1),
     type_group("p", {2212}, kGreen-1),
     type_group("n", {2112}, kOrange+1),
     type_group("Cosmic producer", {2}, kViolet-9),
     type_group("No MCShower", {0}, kGray+1)};
  std::map<int, int> tg_m = fill_type_map(tg_v);

  for(int const i : type_v) {
    auto const tg_it = tg_m.find(i);
    if(tg_it == tg_m.end()) {
      std::cout << "No pdg: " << i << "\n";
      return;
    }
  }

  std::string iwe = we;
  if(iwe != "") iwe += " && ";

  THStack * stack = new THStack();
  TLegend * leg = new TLegend(0.6, 0.9, 0.9, 0.4);
  TCanvas * temp = new TCanvas("temp");
  for(type_group & tg : tg_v) {

    for(int const m : tg._type_number) {
    
      auto const type_v_it = std::find(type_v.begin(), type_v.end(), m);
      if(type_v_it == type_v.end()) continue;

      std::string const hname = "hm" + std::to_string(m);
      std::string const dr = draw + ">>" + hname + binning;
      
      vertex_tree->Draw(dr.c_str(), (iwe + type + " == " + std::to_string(m)).c_str(), opt.c_str());
      TH1 * hist = (TH1*)gDirectory->Get(hname.c_str());
      if(!tg._hist) {
	hist->Scale(scaling);
	hist->SetFillColor(tg._color);
	hist->SetLineColor(1);
	tg._hist = hist;
      }
      else tg._hist->Add(hist);
      
    }

    if(tg._hist) {
      stack->Add(tg._hist);
      leg->AddEntry(tg._hist, tg._type_name.c_str());
    }      

  }
  delete temp;

  TCanvas * canvas = new TCanvas(cname.c_str());
  stack->Draw();
  stack->SetTitle(title.c_str());
  stack->GetXaxis()->SetTitle(xtitle.c_str());
  stack->GetXaxis()->CenterTitle();
  stack->GetYaxis()->SetTitle(ytitle.c_str());
  stack->GetYaxis()->CenterTitle();
  stack->GetYaxis()->SetTitleOffset(1.4);

  std::string const hename = "he";
  std::string const dr = draw + ">>" + hename + binning;
  vertex_tree->Draw(dr.c_str(), we.c_str(), "samepe");
  TH1 * he = (TH1*)gDirectory->Get(hename.c_str());
  he->SetLineColor(1);
  he->Scale(scaling);

  stack->SetMinimum(0);
  stack->SetMaximum((he->GetBinContent(he->GetMaximumBin()) + he->GetBinError(he->GetMaximumBin())) * 1.1);

  leg->SetHeader(type_title.c_str());
  leg->Draw();

  canvas->Write();
  delete canvas;
  delete stack;
  delete leg;  

}



void plot_truth_background() {

  background_by_type("true_nu_E",
		     "interaction_type",
		     "true_nu_E",
		     "(10, 0, 2.5)",
		     "passed_swtrigger == 1 && shower_dist_to_flashzcenter <= 38 && totalpe_ibg_sum > 139 && reco_nu_vtx_fid_contained == 1 && Sum$(associated_shower_info.fTotalEnergy) > 0.07 && longest_asso_track_displacement <= 32",
		     "",
		     "BNB Inclusive",
		     "True Neutrino Energy [GeV]",
		     "Number of Vertices per 6.6e20 POT");

  dist_by_closest_mcshower_pdg(vertex_tree_sp,
			       "summed_shower_energy_pdg_sp",
			       "closest_mcshower_pdg",
			       "Shower Particle",
			       "Sum$(associated_shower_info.fTotalEnergy)",
			       "(10, 0.07, 1.07)",
			       "passed_swtrigger == 1 && shower_dist_to_flashzcenter <= 38 && totalpe_ibg_sum > 139 && reco_nu_vtx_fid_contained == 1 && Sum$(associated_shower_info.fTotalEnergy) > 0.07 && longest_asso_track_displacement <= 32",
			       "",
			       "NC #Delta Radiative",
			       "Summed Reco Shower Energy [GeV]",
			       "Number of Vertices per 6.6e20 POT",
			       run_pot / pot_sp);
  
  dist_by_closest_mcshower_pdg(vertex_tree_sp,
			       "summed_shower_energy_mother_pdg_sp",
			       "closest_mcshower_mother_pdg",
			       "Shower Parent",
			       "Sum$(associated_shower_info.fTotalEnergy)",
			       "(10, 0.07, 1.07)",
			       "passed_swtrigger == 1 && shower_dist_to_flashzcenter <= 38 && totalpe_ibg_sum > 139 && reco_nu_vtx_fid_contained == 1 && Sum$(associated_shower_info.fTotalEnergy) > 0.07 && longest_asso_track_displacement <= 32",
			       "",
			       "NC #Delta Radiative",
			       "Summed Reco Shower Energy [GeV]",
			       "Number of Vertices per 6.6e20 POT",
			       run_pot / pot_sp);
  
  dist_by_closest_mcshower_pdg(vertex_tree_bnb,
			       "Summed_shower_energy_pdg_bnb",
			       "closest_mcshower_pdg",
			       "Shower Particle",
			       "Sum$(associated_shower_info.fTotalEnergy)",
			       "(10, 0.07, 1.07)",
			       "passed_swtrigger == 1 && shower_dist_to_flashzcenter <= 38 && totalpe_ibg_sum > 139 && reco_nu_vtx_fid_contained == 1 && Sum$(associated_shower_info.fTotalEnergy) > 0.07 && longest_asso_track_displacement <= 32",
			       "",
			       "BNB Inclusive",
			       "Summed Reco Shower Energy [GeV]",
			       "Number of Vertices per 6.6e20 POT",
			       run_pot / pot_bnb);

  dist_by_closest_mcshower_pdg(vertex_tree_bnb,
			       "Summed_shower_energy_mother_pdg_bnb",
			       "closest_mcshower_mother_pdg",
			       "Shower Parent",
			       "Sum$(associated_shower_info.fTotalEnergy)",
			       "(10, 0.07, 1.07)",
			       "passed_swtrigger == 1 && shower_dist_to_flashzcenter <= 38 && totalpe_ibg_sum > 139 && reco_nu_vtx_fid_contained == 1 && Sum$(associated_shower_info.fTotalEnergy) > 0.07 && longest_asso_track_displacement <= 32",
			       "",
			       "BNB Inclusive",
			       "Summed Reco Shower Energy [GeV]",
			       "Number of Vertices per 6.6e20 POT",
			       run_pot / pot_bnb);

  dist_by_closest_mcshower_pdg(vertex_tree_cosmic,
			       "summed_shower_energy_pdg_cosmic",
			       "closest_mcshower_pdg",
			       "Shower Particle",
			       "Sum$(associated_shower_info.fTotalEnergy)",
			       "(10, 0.07, 1.07)",
			       "passed_swtrigger == 1 && shower_dist_to_flashzcenter <= 38 && totalpe_ibg_sum > 139 && reco_nu_vtx_fid_contained == 1 && Sum$(associated_shower_info.fTotalEnergy) > 0.07 && longest_asso_track_displacement <= 32",
			       "",
			       "Intime Cosmic",
			       "Summed Reco Shower Energy [GeV]",
			       "Number of Vertices per 6.6e20 POT",
			       ngenbnbcosmic/ngencosmic*10.729 / pot_bnb_cosmic * run_pot);

  dist_by_closest_mcshower_pdg(vertex_tree_cosmic,
			       "summed_shower_energy_mother_pdg_cosmic",
			       "closest_mcshower_mother_pdg",
			       "Shower Parent",			       
			       "Sum$(associated_shower_info.fTotalEnergy)",
			       "(10, 0.07, 1.07)",
			       "passed_swtrigger == 1 && shower_dist_to_flashzcenter <= 38 && totalpe_ibg_sum > 139 && reco_nu_vtx_fid_contained == 1 && Sum$(associated_shower_info.fTotalEnergy) > 0.07 && longest_asso_track_displacement <= 32",
			       "",
			       "Intime Cosmic",
			       "Summed Reco Shower Energy [GeV]",
			       "Number of Vertices per 6.6e20 POT",
			       ngenbnbcosmic/ngencosmic*10.729 / pot_bnb_cosmic * run_pot);

  dist_by_closest_mcshower_pdg(vertex_tree_cosmic,
			       "summed_shower_energy_ancestor_pdg_cosmic",
			       "closest_mcshower_ancestor_pdg",
			       "Shower Ancestor",			       
			       "Sum$(associated_shower_info.fTotalEnergy)",
			       "(10, 0.07, 1.07)",
			       "passed_swtrigger == 1 && shower_dist_to_flashzcenter <= 38 && totalpe_ibg_sum > 139 && reco_nu_vtx_fid_contained == 1 && Sum$(associated_shower_info.fTotalEnergy) > 0.07 && longest_asso_track_displacement <= 32",
			       "",
			       "Intime Cosmic",
			       "Summed Reco Shower Energy [GeV]",
			       "Number of Vertices per 6.6e20 POT",
			       ngenbnbcosmic/ngencosmic*10.729 / pot_bnb_cosmic * run_pot);

}



void sig_back_stacked(std::string const & cname,
		      std::string const & draw,
		      std::string const & binning,
		      std::string const & we,
		      std::string const & opt,
		      std::string const & title,
		      std::string const & xtitle,
		      std::string const & ytitle,
		      bool const log = false) {

  std::string dr_we = signal_definition;
  if(we != "") dr_we = dr_we + " && " + we;

  TCanvas * canvas = new TCanvas(cname.c_str());

  std::string const hspname = "hsp";
  std::string const drawsp = draw + ">>" + hspname + binning;
  vertex_tree_sp->Draw(drawsp.c_str(), dr_we.c_str(), opt.c_str());
  TH1F * hsp = (TH1F*)gDirectory->Get(hspname.c_str());
  hsp->SetFillColor(kBlue+2);
  hsp->SetLineColor(1);
  hsp->Scale(run_pot / pot_sp);

  std::string const hbnbcname = "hbnbcosmic";
  std::string const drawbnbc = draw + ">>" + hbnbcname + binning;
  vertex_tree_bnb_cosmic->Draw(drawbnbc.c_str(), we.c_str(), opt.c_str());
  TH1F * hbnbc = (TH1F*)gDirectory->Get(hbnbcname.c_str());
  hbnbc->SetFillColor(kGreen+2);
  hbnbc->SetLineColor(1);
  hbnbc->Scale(run_pot / pot_bnb_cosmic);  

  std::string const hcosmicname = "hcosmic";
  std::string const drawcosmic = draw + ">>" + hcosmicname + binning;
  vertex_tree_cosmic->Draw(drawcosmic.c_str(), we.c_str(), opt.c_str());
  TH1F * hcosmic = (TH1F*)gDirectory->Get(hcosmicname.c_str());
  hcosmic->SetFillColor(kMagenta+2);
  hcosmic->SetLineColor(1);
  hcosmic->Scale(ngenbnbcosmic/ngencosmic*10.729 / pot_bnb_cosmic * run_pot);

  THStack * stack = new THStack();
  stack->Add(hsp);  
  stack->Add(hbnbc);
  stack->Add(hcosmic);

  TLegend * leg = new TLegend(0.6, 0.9, 0.9, 0.6);
  leg->AddEntry(hcosmic, "In-time Cosmic");
  leg->AddEntry(hbnbc, "BNB + Cosmic");
  leg->AddEntry(hsp, "NC #Delta Radiative");  

  stack->Draw();
  stack->GetXaxis()->SetTitle(xtitle.c_str());
  stack->GetXaxis()->CenterTitle();
  stack->GetYaxis()->SetTitle(ytitle.c_str());
  stack->GetYaxis()->CenterTitle();

  if(log) {
    stack->SetMinimum(0.1);
    stack->SetMaximum(1e6);
    canvas->SetLogy(); 
  }

  leg->Draw();

  canvas->Write();

  delete canvas;
  delete stack;
  delete leg;
  
}



void sig_back_all() {

  /*
  sig_back_stacked("sig_back_stacked",
		   "Sum$(associated_shower_info.fTotalEnergy)",
		   "(10, 0.07, 1.07)",
		   "passed_swtrigger == 1 && shower_dist_to_flashzcenter <= 38 && totalpe_ibg_sum > 139 && reco_nu_vtx_fid_contained == 1 && Sum$(associated_shower_info.fTotalEnergy) > 0.07 && longest_asso_track_displacement <= 32",
		   "",
		   "",
		   "Summed Reco Shower Energy [GeV]",
		   "Number of Vertices per 6.6e20 POT");
  */

  sig_back_stacked("sig_back_stacked_log",
		   "Sum$(associated_shower_info.fTotalEnergy)",
		   "(10, 0.07, 1.07)",
		   "passed_swtrigger == 1 && shower_dist_to_flashzcenter <= 38 && totalpe_ibg_sum > 139 && reco_nu_vtx_fid_contained == 1 && Sum$(associated_shower_info.fTotalEnergy) > 0.07 && longest_asso_track_displacement <= 32",
		   "",
		   "",
		   "Summed Reco Shower Energy [GeV]",
		   "Number of Vertices per 6.6e20 POT",
		   true);
  
}



void cut_efficiency() {

  cut_efficiency_plot("cut_efficiency_vert",
		      "true_nu_E",
		      "(23, 0.1, 2.4)",
		      "",
		      "True Neutrino Energy [GeV]",
		      "Vertex Selection Efficiency");

  /*
  cut_efficiency_event_plot("cut_efficiency_event",
			    "true_nu_E",
			    "(23, 0.1, 2.4)",
			    "",
			    "True Neutrino Energy [GeV]",
			    "Event Selection Efficiency");
  */  

}



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

  ofile = TFile::Open("plotMV4.root", "recreate");
  
}



void plotMV4(std::string const dir = "") {

  init(dir);

  //signal_efficiency();
  //vertices_per_event(true);
  //cut_efficiency();
  //plotdist();
  //fid_volz();  
  //flash();
  //maximize_all();
  //maximize_all_edit();
  //maximize_all2d();
  //marginal_dist();
  //cout_test();
  cout_eff();
  //plot2d();

  //sig_back_all();

  //plot_truth_background();

  /*
  std::string const flashz_cut = "passed_swtrigger == 1";
  plotsupimp("shower_dist_to_flashzcenter",
	     "shower_dist_to_flashzcenter",
	     "(15, 0, 300)",
	     flashz_cut,
	     "",
	     "",
	     "Shower Distance to Closest in-Beam Flash [cm]",
	     "Area Normalized");

  std::string const totalpe_ibg_sum_cut = "passed_swtrigger == 1 && shower_dist_to_flashzcenter <= 38";
  plotsupimp("totalpe_ibg_sum",
	     "totalpe_ibg_sum",
	     "(20, 0, 2000)",
	     totalpe_ibg_sum_cut,
	     "",
	     "",
	     "Total in-Beam PE",
	     "Area Normalized");
  */

  /*
  std::string const summed_shower_energy_cut = "passed_swtrigger == 1 && shower_dist_to_flashzcenter <= 38 && totalpe_ibg_sum > 139 && reco_nu_vtx_fid_contained == 1";
  plotsupimp("summed_shower_energy",
	     "Sum$(associated_shower_info.fTotalEnergy)",
	     "(25, 0, 0.5)",
	     summed_shower_energy_cut,
	     "",
	     "",
	     "Summed Shower Energy [GeV]",
	     "Area Normalized");
  */

  /*
  std::string const longest_asso_track_displacement_cut = "passed_swtrigger == 1 && shower_dist_to_flashzcenter <= 38 && totalpe_ibg_sum > 139 && reco_nu_vtx_fid_contained == 1 && Sum$(associated_shower_info.fTotalEnergy) > 0.07";
  plotsupimp("longest_asso_track_displacement",
	     "longest_asso_track_displacement",
	     "(15, 0, 300)",
	     longest_asso_track_displacement_cut,
	     "",
	     "",
	     "Longest Associated Track Length [cm]",
	     "Area Normalized");
  */

  ofile->Close();

}
