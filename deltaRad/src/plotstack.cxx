#include "plotstack.h"
#include <iomanip>
#include "TTreeFormula.h"



std::string double_to_string(double const d, double const precision) {

  std::stringstream sstr;
  sstr << std::setprecision(precision);
  sstr << d;

  return sstr.str();

}



double get_nu_weight(double const true_nu_E) {

  std::vector<double> const weights { 3.14581, 2.98233, 2.87886,2.60848,2.78501,2.38658,2.58249,2.54745,2.84427,3.1148,2.90477};
  std::vector<double> bins{400,600,700,800,900,1000,1200,1400,1600,1800,2000,2500}; for(double & d : bins) d *= 1e-3;

  if(true_nu_E < bins.front() || true_nu_E > bins.back()) return 1;

  for(size_t i = 0; i < weights.size(); ++i) {
    if(true_nu_E < bins.at(i+1)) return weights.at(i);
  }

  std::cout << "something broke\n";
  return -1;

}



double get_gamma_weight(double const true_gamma_E) {

  std::vector<double> const weights{3.62113, 3.40032, 3.21182, 3.01981,2.67115, 2.72306, 1.14906, 1.21308};
  std::vector<double> bins{50,150,200,250,300,350,450,550,700}; for(double & d : bins) d *= 1e-3;

  if(true_gamma_E < bins.front() || true_gamma_E > bins.back()) return 1;

  for(size_t i = 0; i < weights.size(); ++i) {
    if(true_gamma_E < bins.at(i+1)) return weights.at(i);
  }

  std::cout << "something broke\n";
  return -1;

}



void plotsig(load_trees const & lt,
	     double const sample_pot,
	     std::string const & name,
	     std::string const & draw,
	     std::string const & binning,
	     std::string const & weight,
	     std::string const & option,
	     std::string const & title,
	     std::string const & xtitle,
	     std::string const & ytitle,
	     bool const logy,
	     std::string const & method,
	     double const mva1,
	     double const mva2) {

  double const run_pot = 6.6e20;
  std::string const signal_definition = "(is_delta_rad == 1 && true_nu_vtx_fid_contained == 1)";

  TCanvas * canvas = new TCanvas(name.c_str());

  std::string andstr;
  if(weight != "") andstr = " && ";

  std::string sig_mvastr;
  std::string sig_mva_andstr;
  std::string const sig_friend_tree_name = "ncdelta_cosmic_" + method;
  if(method != "" && (mva1 != DBL_MAX || mva2 != DBL_MAX)) {
    sig_mvastr = "((reco_asso_tracks == 0 && " + sig_friend_tree_name + ".mva > "+std::to_string(mva1)+") || (reco_asso_tracks > 0 && " + sig_friend_tree_name + ".mva > "+std::to_string(mva2)+"))";
    sig_mva_andstr = " && ";
  }

  lt.vertex_tree_sp_cosmic->Draw((draw+">>h_signal"+binning).c_str(), ("(" + weight + andstr + sig_mvastr + sig_mva_andstr + signal_definition + ")").c_str(), "hist");
  TH1 * hist_signal = (TH1*)gDirectory->Get("h_signal");
  hist_signal->SetLineColor(1);
  hist_signal->SetFillColor(1);
  hist_signal->SetFillStyle(3004);
  hist_signal->Scale(run_pot / lt.pot_sp_cosmic * 500);

  hist_signal->SetTitle(title.c_str());
  hist_signal->GetXaxis()->SetTitle(xtitle.c_str());
  hist_signal->GetXaxis()->CenterTitle();
  hist_signal->GetYaxis()->SetTitle(ytitle.c_str());
  hist_signal->GetYaxis()->CenterTitle();

  canvas->Write();

}



void plotmode(load_trees const & lt,
	      TTree * tree,
	      double const sample_pot,
	      std::string const & name,
	      std::string const & draw,
	      std::string const & binning,
	      std::string const & weight,
	      std::string const & option,
	      std::string const & title,
	      std::string const & xtitle,
	      std::string const & ytitle,
	      bool const logy,
	      std::string const & method,
	      double const mva1,
	      double const mva2,
	      bool const lee) {

  std::map<int, std::pair<std::string, int>> mode_map = {
    {0, std::make_pair("QE", kRed)},
    {1, std::make_pair("RES", kYellow)},
    {2, std::make_pair("DIS", kGreen)},
    {3, std::make_pair("COH", kBlue)},
    {5, std::make_pair("Electron Scattering", kMagenta)},
    {10, std::make_pair("MEC", kCyan)}
  };
  
  double const run_pot = 6.6e20;
  std::string const signal_definition = "(is_delta_rad == 1 && true_nu_vtx_fid_contained == 1)";

  std::string andstr;
  if(weight != "") andstr = " && ";
  std::string mvastr;
  std::string mva_andstr;
  std::string const friend_tree_name = "bnb_cosmic_" + method;
  if(method != "" && (mva1 != DBL_MAX || mva2 != DBL_MAX)) {
    mvastr = "((reco_asso_tracks == 0 && " + friend_tree_name + ".mva > "+std::to_string(mva1)+") || (reco_asso_tracks > 0 && " + friend_tree_name + ".mva > "+std::to_string(mva2)+"))";
    mva_andstr = " && ";
  }

  TH1 * other_hist = nullptr;

  TCanvas * canvas = new TCanvas(name.c_str());
  THStack * stack = new THStack((name + "_stack").c_str(), "");
  TLegend * leg = new TLegend(0.6, 0.9, 0.9, 0.6);

  std::string sig_mvastr;
  std::string sig_mva_andstr;
  std::string const sig_friend_tree_name = "ncdelta_cosmic_" + method;
  if(method != "" && (mva1 != DBL_MAX || mva2 != DBL_MAX)) {
    sig_mvastr = "((reco_asso_tracks == 0 && " + sig_friend_tree_name + ".mva > "+std::to_string(mva1)+") || (reco_asso_tracks > 0 && " + sig_friend_tree_name + ".mva > "+std::to_string(mva2)+"))";
    sig_mva_andstr = " && ";
  }
  std::string lee_str;
  if(lee) lee_str = " * get_nu_weight(true_nu_E)";
  lt.vertex_tree_sp_cosmic->Draw((draw+">>h_signal"+binning).c_str(), ("(" + weight + andstr + sig_mvastr + sig_mva_andstr + signal_definition + ")" + lee_str).c_str());
  TH1 * hist_signal = (TH1*)gDirectory->Get("h_signal");
  if(hist_signal->Integral() > 0) {
    hist_signal->SetLineColor(1);
    hist_signal->SetFillColor(1);
    hist_signal->SetFillStyle(3004);
    hist_signal->Scale(run_pot / lt.pot_sp_cosmic);
    stack->Add(hist_signal);
    if(lee) leg->AddEntry(hist_signal, ("NC #Delta Radiative LEE Scaled: " + double_to_string(hist_signal->Integral(0, hist_signal->GetNbinsX()+1))).c_str());
    else leg->AddEntry(hist_signal, ("NC #Delta Radiative: " + double_to_string(hist_signal->Integral(0, hist_signal->GetNbinsX()+1))).c_str());
  }
  tree->Draw((draw+">>he"+binning).c_str(), (weight + mva_andstr + mvastr).c_str());
  TH1 * he = (TH1*)gDirectory->Get("he");  
  he->Scale(run_pot / sample_pot);
  he->Add(hist_signal);
  he->SetLineColor(1);

  for(int mode = 0; mode <= 11; ++mode) {
    for(int ccnc = 0; ccnc <= 1; ++ccnc) {
      std::string ccnc_str = "CC";
      if(ccnc == 1) ccnc_str = "NC";
      tree->Draw((draw+">>h_"+ccnc_str+std::to_string(mode)+binning).c_str(), (weight + andstr + "ccnc == " + std::to_string(ccnc) + " && mode == " + std::to_string(mode) + " && " + mvastr + mva_andstr + "!" + signal_definition).c_str(), "hist");
      TH1 * hist = (TH1*)gDirectory->Get(("h_"+ccnc_str+std::to_string(mode)).c_str());
      hist->Scale(run_pot / sample_pot);
      auto im_it = mode_map.find(mode);
      if(im_it == mode_map.end()) {
	if(!other_hist) {
	  if(hist->Integral() > 0) other_hist = hist;
	}
	else other_hist->Add(hist);
      }
      else if(hist->Integral() > 0) {
	hist->SetLineColor(1);
	hist->SetFillColor(im_it->second.second + ccnc);
	stack->Add(hist);
	leg->AddEntry(hist, (ccnc_str + " " + im_it->second.first + ": "  + double_to_string(hist->Integral(0, hist->GetNbinsX()+1))).c_str());
      }
    }
  }
  if(other_hist) {
    other_hist->SetLineColor(1);
    stack->Add(other_hist);
    leg->AddEntry(other_hist, ("Other: " + double_to_string(other_hist->Integral(0, other_hist->GetNbinsX()+1))).c_str());
  }

  stack->Draw("hist");
  stack->SetTitle(title.c_str());
  stack->GetXaxis()->SetTitle(xtitle.c_str());
  stack->GetXaxis()->CenterTitle();
  stack->GetYaxis()->SetTitle(ytitle.c_str());
  stack->GetYaxis()->CenterTitle();
  stack->GetYaxis()->SetTitleOffset(1.4);

  if(logy) {
    stack->SetMinimum(0.5);
    stack->SetMaximum(1e3 * he->GetBinContent(he->GetMaximumBin()));
    canvas->SetLogy();
  }
  else {
    stack->SetMaximum(1.1 * he->GetBinContent(he->GetMaximumBin()));
    he->Draw("same");
    leg->SetHeader(("Total Number of Reco-Vertices: " + double_to_string(he->Integral(0, he->GetNbinsX()+1))).c_str(), "C");
    leg->Draw();
  }

  canvas->Write();
  delete stack;
  delete canvas;

}



void plotnu(load_trees const & lt,
	    TTree * tree,
	    double const sample_pot,
	    std::string const & name,
	    std::string const & draw,
	    std::string const & binning,
	    std::string const & weight,
	    std::string const & option,
	    std::string const & title,
	    std::string const & xtitle,
	    std::string const & ytitle,
	    bool const logy,
	    std::string const & method,
	    double const mva1,
	    double const mva2,
	    bool const lee) {

  std::map<int, std::pair<std::string, int>> mode_map = {
    {0, std::make_pair("QE", kRed)},
    {1, std::make_pair("RES", kYellow)},
    {2, std::make_pair("DIS", kGreen)},
    {3, std::make_pair("COH", kBlue)},
    {5, std::make_pair("Electron Scattering", kMagenta)},
    {10, std::make_pair("MEC", kCyan)}
  };

  double const run_pot = 6.6e20;
  std::string const signal_definition = "(is_delta_rad == 1 && true_nu_vtx_fid_contained == 1)";

  std::string andstr;
  if(weight != "") andstr = " && ";
  std::string mvastr;
  std::string mva_andstr;
  std::string const friend_tree_name = "bnb_cosmic_" + method;
  if(method != "" && (mva1 != DBL_MAX || mva2 != DBL_MAX)) {
    mvastr = "((reco_asso_tracks == 0 && " + friend_tree_name + ".mva > "+std::to_string(mva1)+") || (reco_asso_tracks > 0 && " + friend_tree_name + ".mva > "+std::to_string(mva2)+"))";
    mva_andstr = " && ";
  }

  TH1 * other_hist = nullptr;

  TCanvas * canvas = new TCanvas(name.c_str());
  THStack * stack = new THStack((name + "_stack").c_str(), "");
  TLegend * leg = new TLegend(0.6, 0.9, 0.9, 0.6);

  std::string sig_mvastr;
  std::string sig_mva_andstr;
  std::string const sig_friend_tree_name = "ncdelta_cosmic_" + method;
  if(method != "" && (mva1 != DBL_MAX || mva2 != DBL_MAX)) {
    sig_mvastr = "((reco_asso_tracks == 0 && " + sig_friend_tree_name + ".mva > "+std::to_string(mva1)+") || (reco_asso_tracks > 0 && " + sig_friend_tree_name + ".mva > "+std::to_string(mva2)+"))";
    sig_mva_andstr = " && ";
  }
  std::string lee_str;
  if(lee) lee_str = " * get_nu_weight(true_nu_E)";
  lt.vertex_tree_sp_cosmic->Draw((draw+">>h_signal"+binning).c_str(), ("(" + weight + andstr + sig_mvastr + sig_mva_andstr + signal_definition + ")" + lee_str).c_str());
  TH1 * hist_signal = (TH1*)gDirectory->Get("h_signal");
  if(hist_signal->Integral() > 0) {
    hist_signal->SetLineColor(1);
    hist_signal->SetFillColor(1);
    hist_signal->SetFillStyle(3004);
    hist_signal->Scale(run_pot / lt.pot_sp_cosmic);
    stack->Add(hist_signal);
    if(lee) leg->AddEntry(hist_signal, ("NC #Delta Radiative LEE Scaled: " + double_to_string(hist_signal->Integral(0, hist_signal->GetNbinsX()+1))).c_str());
    else leg->AddEntry(hist_signal, ("NC #Delta Radiative: " + double_to_string(hist_signal->Integral(0, hist_signal->GetNbinsX()+1))).c_str());
  }
  tree->Draw((draw+">>he"+binning).c_str(), (weight + mva_andstr + mvastr).c_str());
  TH1 * he = (TH1*)gDirectory->Get("he");  
  he->Scale(run_pot / sample_pot);
  he->Add(hist_signal);
  he->SetLineColor(1);

  for(int mode = 0; mode <= 11; ++mode) {
    for(int ccnc = 12; ccnc <= 14; ccnc += 2) {
      std::string ccnc_str = "#nu_{e}";
      if(ccnc == 14) ccnc_str = "#nu_{#mu}";
      tree->Draw((draw+">>h_"+ccnc_str+std::to_string(mode)+binning).c_str(), (weight + andstr + " abs(nu_pdg) == " + std::to_string(ccnc) + " && mode == " + std::to_string(mode) + " && " + mvastr + mva_andstr + "!" + signal_definition).c_str(), "hist");
      TH1 * hist = (TH1*)gDirectory->Get(("h_"+ccnc_str+std::to_string(mode)).c_str());
      hist->Scale(run_pot / sample_pot);
      auto im_it = mode_map.find(mode);
      if(im_it == mode_map.end()) {
	if(!other_hist) {
	  if(hist->Integral() > 0) other_hist = hist;
	}
	else other_hist->Add(hist);
      }
      else if(hist->Integral() > 0) {
	hist->SetLineColor(1);
	int offset = 0;
	if(ccnc == 14) offset = 1;
	hist->SetFillColor(im_it->second.second + offset);
	stack->Add(hist);
	leg->AddEntry(hist, (ccnc_str + " " + im_it->second.first + ": " + double_to_string(hist->Integral(0, hist->GetNbinsX()+1))).c_str());
      }
    }
  }
  if(other_hist) {
    other_hist->SetLineColor(1);
    stack->Add(other_hist);
    leg->AddEntry(other_hist, ("Other: " + double_to_string(other_hist->Integral(0, other_hist->GetNbinsX()+1))).c_str());
  }

  stack->Draw("hist");
  stack->SetTitle(title.c_str());
  stack->GetXaxis()->SetTitle(xtitle.c_str());
  stack->GetXaxis()->CenterTitle();
  stack->GetYaxis()->SetTitle(ytitle.c_str());
  stack->GetYaxis()->CenterTitle();
  stack->GetYaxis()->SetTitleOffset(1.4);

  if(logy) {
    stack->SetMinimum(0.5);
    stack->SetMaximum(1e3 * he->GetBinContent(he->GetMaximumBin()));
    canvas->SetLogy();
  }
  else {
    stack->SetMaximum(1.1 * he->GetBinContent(he->GetMaximumBin()));
    he->Draw("same");
    leg->SetHeader(("Total Number of Reco-Vertices: " + double_to_string(he->Integral(0, he->GetNbinsX()+1))).c_str(), "C");
    leg->Draw();
  }

  canvas->Write();
  delete stack;
  delete canvas;

}



void plotpi0(load_trees const & lt,
	     TTree * tree,
	     double const sample_pot,
	     std::string const & name,
	     std::string const & draw,
	     std::string const & binning,
	     std::string const & weight,
	     std::string const & option,
	     std::string const & title,
	     std::string const & xtitle,
	     std::string const & ytitle,
	     bool const logy,
	     std::string const & method,
	     double const mva1,
	     double const mva2,
	     bool const lee) {  

  std::map<int, std::pair<std::string, int>> mode_map = {
    {0, std::make_pair("QE", kRed)},
    {1, std::make_pair("RES", kYellow)},
    {2, std::make_pair("DIS", kGreen)},
    {3, std::make_pair("COH", kBlue)},
    {5, std::make_pair("Electron Scattering", kMagenta)},
    {10, std::make_pair("MEC", kCyan)}
  };

  double const run_pot = 6.6e20;
  std::string const signal_definition = "(is_delta_rad == 1 && true_nu_vtx_fid_contained == 1)";

  std::string andstr;
  if(weight != "") andstr = " && ";
  std::string mvastr;
  std::string mva_andstr;
  std::string const friend_tree_name = "bnb_cosmic_" + method;
  if(method != "" && (mva1 != DBL_MAX || mva2 != DBL_MAX)) {
    mvastr = "((reco_asso_tracks == 0 && " + friend_tree_name + ".mva > "+std::to_string(mva1)+") || (reco_asso_tracks > 0 && " + friend_tree_name + ".mva > "+std::to_string(mva2)+"))";
    mva_andstr = " && ";
  }

  TH1 * other_hist = nullptr;

  TCanvas * canvas = new TCanvas(name.c_str());
  THStack * stack = new THStack((name + "_stack").c_str(), "");
  TLegend * leg = new TLegend(0.6, 0.9, 0.9, 0.6);

  std::string sig_mvastr;
  std::string sig_mva_andstr;
  std::string const sig_friend_tree_name = "ncdelta_cosmic_" + method;
  if(method != "" && (mva1 != DBL_MAX || mva2 != DBL_MAX)) {
    sig_mvastr = "((reco_asso_tracks == 0 && " + sig_friend_tree_name + ".mva > "+std::to_string(mva1)+") || (reco_asso_tracks > 0 && " + sig_friend_tree_name + ".mva > "+std::to_string(mva2)+"))";
    sig_mva_andstr = " && ";
  }
  std::string lee_str;
  if(lee) lee_str = " * get_nu_weight(true_nu_E)";
  lt.vertex_tree_sp_cosmic->Draw((draw+">>h_signal"+binning).c_str(), ("(" + weight + andstr + sig_mvastr + sig_mva_andstr + signal_definition + ")" + lee_str).c_str());
  TH1 * hist_signal = (TH1*)gDirectory->Get("h_signal");
  if(hist_signal->Integral() > 0) {
    hist_signal->SetLineColor(1);
    hist_signal->SetFillColor(1);
    hist_signal->SetFillStyle(3004);
    hist_signal->Scale(run_pot / lt.pot_sp_cosmic);
    stack->Add(hist_signal);
    if(lee) leg->AddEntry(hist_signal, ("NC #Delta Radiative LEE Scaled: " + double_to_string(hist_signal->Integral(0, hist_signal->GetNbinsX()+1))).c_str());
    else leg->AddEntry(hist_signal, ("NC #Delta Radiative: " + double_to_string(hist_signal->Integral(0, hist_signal->GetNbinsX()+1))).c_str());
  }
  tree->Draw((draw+">>he"+binning).c_str(), (weight + mva_andstr + mvastr).c_str());
  TH1 * he = (TH1*)gDirectory->Get("he");  
  he->Scale(run_pot / sample_pot);
  he->Add(hist_signal);
  he->SetLineColor(1);

  for(int mode = 0; mode <= 11; ++mode) {
    for(int ccnc = 0; ccnc <= 1; ++ccnc) {
      std::string ccnc_str = "No exiting #pi^{0}";
      std::string cut_str = "exiting_pi0_number == 0";
      if(ccnc == 1) {
	ccnc_str = "Exiting #pi^{0}";
	cut_str = "exiting_pi0_number > 0";
      }
      tree->Draw((draw+">>h_"+ccnc_str+std::to_string(mode)+binning).c_str(), (weight + andstr + cut_str + " && mode == " + std::to_string(mode) + " && " + mvastr + mva_andstr + "!" + signal_definition).c_str(), "hist");
      TH1 * hist = (TH1*)gDirectory->Get(("h_"+ccnc_str+std::to_string(mode)).c_str());
      hist->Scale(run_pot / sample_pot);
      auto im_it = mode_map.find(mode);
      if(im_it == mode_map.end()) {
	if(!other_hist) {
	  if(hist->Integral() > 0) other_hist = hist;
	}
	else other_hist->Add(hist);
      }
      else if(hist->Integral() > 0) {
	hist->SetLineColor(1);
	hist->SetFillColor(im_it->second.second + ccnc);
	stack->Add(hist);
	leg->AddEntry(hist, (ccnc_str + " " + im_it->second.first + ": "  + double_to_string(hist->Integral(0, hist->GetNbinsX()+1))).c_str());
      }
    }
  }
  if(other_hist) {
    other_hist->SetLineColor(1);
    stack->Add(other_hist);
    leg->AddEntry(other_hist, ("Other: " + double_to_string(other_hist->Integral(0, other_hist->GetNbinsX()+1))).c_str());
  }

  stack->Draw("hist");
  stack->SetTitle(title.c_str());
  stack->GetXaxis()->SetTitle(xtitle.c_str());
  stack->GetXaxis()->CenterTitle();
  stack->GetYaxis()->SetTitle(ytitle.c_str());
  stack->GetYaxis()->CenterTitle();
  stack->GetYaxis()->SetTitleOffset(1.4);

  if(logy) {
    stack->SetMinimum(0.5);
    stack->SetMaximum(1e3 * he->GetBinContent(he->GetMaximumBin()));
    canvas->SetLogy();
  }
  else {
    stack->SetMaximum(1.1 * he->GetBinContent(he->GetMaximumBin()));
    he->Draw("same");
    leg->SetHeader(("Total Number of Reco-Vertices: " + double_to_string(he->Integral(0, he->GetNbinsX()+1))).c_str(), "C");
    leg->Draw();
  }

  canvas->Write();
  delete stack;
  delete canvas;

}



std::vector<int> get_pdg_v(TTree * tree,
			   std::string const & pdg_str,
			   std::string const & cut_str) {

  std::vector<int> pdg_v;
  int pdg;
  tree->SetBranchAddress(pdg_str.c_str(), &pdg);

  TTreeFormula * tf = new TTreeFormula("tf", cut_str.c_str(), tree);

  bool add_0 = false;
  for(int i = 0; i < tree->GetEntries(); ++i) {
    tree->GetEntry(i);
    if(!tf->EvalInstance()) continue;
    if(pdg == 0) {
      add_0 = true;
      continue;
    }
    auto const pv_it = std::find(pdg_v.begin(), pdg_v.end(), pdg);
    if(pv_it == pdg_v.end()) pdg_v.push_back(pdg);
  }

  if(add_0) pdg_v.push_back(0);

  delete tf;

  return pdg_v;

}



void plotpdg(TTree * tree,
	     double const scaling,
	     std::string const & name,
	     std::string const & pdg_str,
	     std::string const & draw,
	     std::string const & binning,
	     std::string const & weight,
	     std::string const & title,
	     std::string const & xtitle,
	     std::string const & ytitle,
	     bool const logy,
	     std::string const & method,
	     double const mva1,
	     double const mva2) {

  std::map<int, std::pair<std::string, int>> pdg_map = {
    {22, std::make_pair("#gamma", kBlue)}
    ,{2212, std::make_pair("p", kCyan)}
    ,{2112, std::make_pair("n", kGreen)}
    ,{0, std::make_pair("No match", kGray+2)}
  };

  std::string andstr;
  if(weight != "") andstr = " && ";
  std::string mvastr;
  std::string mva_andstr;
  std::string const friend_tree_name = "bnb_cosmic_" + method;
  if(method != "" && (mva1 != DBL_MAX || mva2 != DBL_MAX)) {
    mvastr = "((reco_asso_tracks == 0 && " + friend_tree_name + ".mva > "+std::to_string(mva1)+") || (reco_asso_tracks > 0 && " + friend_tree_name + ".mva > "+std::to_string(mva2)+"))";
    mva_andstr = " && ";
  }

  TH1 * other_hist = nullptr;

  TCanvas * canvas = new TCanvas(name.c_str());
  THStack * stack = new THStack((name + "_stack").c_str(), "");
  TLegend * leg = new TLegend(0.6, 0.9, 0.9, 0.6);

  tree->Draw((draw+">>he"+binning).c_str(), (weight + mva_andstr + mvastr).c_str());
  TH1 * he = (TH1*)gDirectory->Get("he");  
  he->Scale(scaling);
  he->SetLineColor(1);

  std::vector<int> const pdg_v = get_pdg_v(tree, pdg_str, weight + mva_andstr + mvastr);

  for(int const i : pdg_v) {
    std::string const hist_name = "h_"+std::to_string(i);
    tree->Draw((draw + ">>" + hist_name + binning).c_str(), (weight + andstr + mvastr + mva_andstr + pdg_str + " == " + std::to_string(i)).c_str(), "hist");
    TH1 * hist = (TH1*)gDirectory->Get(hist_name.c_str());
    hist->Scale(scaling);
    auto pm_it = pdg_map.find(i);
    if(pm_it == pdg_map.end()) {
      if(!other_hist) {
	if(hist->Integral() > 0) other_hist = hist;
      }
      else other_hist->Add(hist);
    }
    else if(hist->Integral() > 0) {
      hist->SetLineColor(1);
      hist->SetFillColor(pm_it->second.second);
      stack->Add(hist);
      leg->AddEntry(hist, (pm_it->second.first + ": "  + double_to_string(hist->Integral(0, hist->GetNbinsX()+1))).c_str());
    }
  }
  if(other_hist) {
    other_hist->SetLineColor(1);
    stack->Add(other_hist);
    leg->AddEntry(other_hist, ("Other: " + double_to_string(other_hist->Integral(0, other_hist->GetNbinsX()+1))).c_str());
  }

  stack->Draw("hist");
  stack->SetTitle(title.c_str());
  stack->GetXaxis()->SetTitle(xtitle.c_str());
  stack->GetXaxis()->CenterTitle();
  stack->GetYaxis()->SetTitle(ytitle.c_str());
  stack->GetYaxis()->CenterTitle();
  stack->GetYaxis()->SetTitleOffset(1.4);

  if(logy) {
    stack->SetMinimum(0.5);
    stack->SetMaximum(1e3 * he->GetBinContent(he->GetMaximumBin()));
    canvas->SetLogy();
  }
  else {
    stack->SetMaximum(1.1 * he->GetBinContent(he->GetMaximumBin()));
    he->Draw("same");
    leg->SetHeader(("Total Number of Reco-Vertices: " + double_to_string(he->Integral(0, he->GetNbinsX()+1))).c_str(), "C");
    leg->Draw();
  }

  canvas->Write();
  delete stack;
  delete canvas;

}



void plotmva(load_trees const & lt,
	     TTree * tree,
	     double const sample_pot,
	     std::string const & name,
	     std::string const & draw,
	     std::string const & binning,
	     std::string const & weight,
	     std::string const & option,
	     std::string const & title,
	     std::string const & xtitle,
	     std::string const & ytitle,
	     bool const logy,
	     std::string const & method) {

  std::map<int, std::pair<std::string, int>> mode_map = {
    {0, std::make_pair("QE", kRed)},
    {1, std::make_pair("RES", kYellow)},
    {2, std::make_pair("DIS", kGreen)},
    {3, std::make_pair("COH", kBlue)},
    {5, std::make_pair("Electron Scattering", kMagenta)},
    {10, std::make_pair("MEC", kCyan)}
  };
  
  double const run_pot = 6.6e20;
  std::string const signal_definition = "(is_delta_rad == 1 && true_nu_vtx_fid_contained == 1)";

  std::string andstr;
  if(weight != "") andstr = " && ";
  std::string const friend_tree_name = "bnb_cosmic_" + method;

  TH1 * other_hist = nullptr;

  TCanvas * canvas = new TCanvas(name.c_str());
  THStack * stack = new THStack((name + "_stack").c_str(), "");
  TLegend * leg = new TLegend(0.6, 0.9, 0.9, 0.6);

  std::string const sig_friend_tree_name = "ncdelta_" + method;

  lt.vertex_tree_sp->Draw((sig_friend_tree_name + ".mva>>h_signal"+binning).c_str(), ("(" + weight + andstr + signal_definition + ")").c_str());
  TH1 * hist_signal = (TH1*)gDirectory->Get("h_signal");
  if(hist_signal->Integral() > 0) {
    hist_signal->SetLineColor(1);
    hist_signal->SetFillColor(1);
    hist_signal->SetFillStyle(3004);
    leg->AddEntry(hist_signal, "NC #Delta Radiative");
  }
  tree->Draw((friend_tree_name + ".mva>>he"+binning).c_str(), (weight).c_str());
  TH1 * he = (TH1*)gDirectory->Get("he");  
  he->SetLineColor(1);

  for(int mode = 0; mode <= 11; ++mode) {
    for(int ccnc = 0; ccnc <= 1; ++ccnc) {
      std::string ccnc_str = "CC";
      if(ccnc == 1) ccnc_str = "NC";
      tree->Draw((friend_tree_name + ".mva>>h_"+ccnc_str+std::to_string(mode)+binning).c_str(), (weight + andstr + "ccnc == " + std::to_string(ccnc) + " && mode == " + std::to_string(mode) + " && !" + signal_definition).c_str(), "hist");
      TH1 * hist = (TH1*)gDirectory->Get(("h_"+ccnc_str+std::to_string(mode)).c_str());
      hist->Scale(1./he->Integral());
      auto im_it = mode_map.find(mode);
      if(im_it == mode_map.end()) {
	if(!other_hist) {
	  if(hist->Integral() > 0) other_hist = hist;
	}
	else other_hist->Add(hist);
      }
      else if(hist->Integral() > 0) {
	hist->SetLineColor(1);
	hist->SetFillColor(im_it->second.second + ccnc);
	stack->Add(hist);
	leg->AddEntry(hist, (ccnc_str + " " + im_it->second.first).c_str());
      }
    }
  }
  if(other_hist) {
    other_hist->SetLineColor(1);
    stack->Add(other_hist);
    leg->AddEntry(other_hist, ("Other: " + double_to_string(other_hist->Integral(0, other_hist->GetNbinsX()+1))).c_str());
  }

  stack->Draw("hist");
  stack->SetTitle(title.c_str());
  stack->GetXaxis()->SetTitle(xtitle.c_str());
  stack->GetXaxis()->CenterTitle();
  stack->GetYaxis()->SetTitle(ytitle.c_str());
  stack->GetYaxis()->CenterTitle();
  stack->GetYaxis()->SetTitleOffset(1.4);

  hist_signal->Scale(1./hist_signal->Integral());
  hist_signal->Draw("same hist");

  if(logy) {
    stack->SetMinimum(0.5);
    stack->SetMaximum(1e3 * he->GetBinContent(he->GetMaximumBin()));
    canvas->SetLogy();
  }
  else {
    stack->SetMaximum(1.1 * hist_signal->GetBinContent(hist_signal->GetMaximumBin()));
    //he->Draw("same");
    leg->Draw();
  }

  canvas->Write();
  delete stack;
  delete canvas;

}



void plot(load_trees const & lt,
		TTree * tree,
		double const sample_pot,
		std::string const & name,
		std::string const & draw,
		std::string const & binning,
		std::string const & weight ,
		std::string const & option ,
		std::string const & title ,
		std::string const & xtitle ,
		std::string const & ytitle ,
		bool const logy ,
		std::string const & method ,
		double const mva1 ,
		double const mva2 ) {



	std::map<int, std::pair<std::string, int> > mode_map = {
		{0, std::make_pair("QE", kRed)},
		{1, std::make_pair("RES", kYellow)},
		{2, std::make_pair("DIS", kGreen)},
		{3, std::make_pair("COH", kBlue)},
		{5, std::make_pair("Electron Scattering", kMagenta)},
		{10, std::make_pair("MEC", kCyan)}
	};




	double const run_pot = 6.6e20;
	std::string const signal_definition = "(is_delta_rad == 1 && true_nu_vtx_fid_contained == 1)";

	std::string andstr;
	if(weight != "") andstr = " && ";
	std::string mvastr;
	std::string mva_andstr;
	if(method != "" && (mva1 != DBL_MAX || mva2 != DBL_MAX)) {
		std::string const friend_tree_name = "bnb_cosmic_" + method;
		mvastr = "((reco_asso_tracks == 0 && " + friend_tree_name + ".mva > "+std::to_string(mva1)+") || (reco_asso_tracks > 0 && " + friend_tree_name + ".mva > "+std::to_string(mva2)+"))";
		mva_andstr = " && ";
	}

	TH1 * other_hist = nullptr;

	TCanvas * canvas = new TCanvas(name.c_str());
	THStack * stack = new THStack((name + "_stack").c_str(), "");
	TLegend * leg = new TLegend(0.55, 0.89, 0.89, 0.55);
	leg->SetLineColor(kWhite);


	/*
	   tree->Draw((draw+">>h_signal"+binning).c_str(), (weight + andstr + mvastr + signal_definition).c_str());
	   TH1 * hist_signal = (TH1*)gDirectory->Get("h_signal");  
	   if(hist_signal->Integral() > 0) {
	   hist_signal->SetLineColor(1);
	   hist_signal->SetFillColor(0);
	   stack->Add(hist_signal);
	   leg->AddEntry(hist_signal, "NC #Delta Radiative");    
	   }
	 */

	std::string sig_mvastr;
	std::string sig_mva_andstr;
	if(method != "" && (mva1 != DBL_MAX || mva2 != DBL_MAX)) {
		std::string const sig_friend_tree_name = "ncdelta_cosmic_" + method;
		sig_mvastr = "((reco_asso_tracks == 0 && " + sig_friend_tree_name + ".mva > "+std::to_string(mva1)+") || (reco_asso_tracks > 0 && " + sig_friend_tree_name + ".mva > "+std::to_string(mva2)+"))";
		sig_mva_andstr = " && ";
	}
	lt.vertex_tree_sp_cosmic->Draw((draw+">>h_signal"+binning).c_str(), (weight + andstr + sig_mvastr + sig_mva_andstr + signal_definition).c_str());
	TH1 * hist_signal = (TH1*)gDirectory->Get("h_signal");
	if(hist_signal->Integral() > 0) {
		hist_signal->SetLineColor(1);
		hist_signal->SetFillColor(kBlack);
		hist_signal->Scale(run_pot / lt.pot_sp_cosmic);
		stack->Add(hist_signal);
		
	    	std::ostringstream out;
		out << std::fixed<<std::setprecision(1) << hist_signal->Integral() ;



		leg->AddEntry(hist_signal, ("NC #Delta Radiative: " + out.str()).c_str());    
	}

	for(int mode = 0; mode <= 11; ++mode) {
		for(int ccnc = 0; ccnc <= 1; ++ccnc) {
			std::string ccnc_str = "CC";
			if(ccnc == 1) ccnc_str = "NC";
			tree->Draw((draw+">>h_"+ccnc_str+std::to_string(mode)+binning).c_str(), (weight + andstr + "ccnc == " + std::to_string(ccnc) + " && mode == " + std::to_string(mode) + " && " + mvastr + mva_andstr + "!" + signal_definition).c_str(), "hist");
			TH1 * hist = (TH1*)gDirectory->Get(("h_"+ccnc_str+std::to_string(mode)).c_str());
			hist->Scale(run_pot / sample_pot);
			auto im_it = mode_map.find(mode);
			if(im_it == mode_map.end()) {
				if(!other_hist) {
					if(hist->Integral() > 0) other_hist = hist;
				}
				else other_hist->Add(hist);
			}
			else if(hist->Integral() > 0) {
				hist->SetLineColor(1);
				hist->SetFillColor(im_it->second.second + ccnc);
				stack->Add(hist);

			    	std::ostringstream out;
				out << std::fixed<<std::setprecision(1) << hist->Integral() ;

				leg->AddEntry(hist, (ccnc_str + " " + im_it->second.first + ": "+ out.str() ).c_str());
			}
		}
	}
	if(other_hist) {
		other_hist->SetLineColor(1);
		stack->Add(other_hist);
		leg->AddEntry(other_hist, "Other");
	}

	tree->Draw((draw+">>he"+binning).c_str(), (weight + mva_andstr + mvastr).c_str());
	TH1 * he = (TH1*)gDirectory->Get("he");  
	he->Scale(run_pot / sample_pot);
	he->Add(hist_signal);
	he->SetLineColor(1);

	stack->Draw("hist");
	stack->SetTitle(title.c_str());
	stack->GetXaxis()->SetTitle(xtitle.c_str());
	stack->GetXaxis()->CenterTitle();
	stack->GetYaxis()->SetTitle(ytitle.c_str());
	stack->GetYaxis()->CenterTitle();
	stack->GetYaxis()->SetTitleOffset(1.4);

	if(logy) {
		stack->SetMinimum(0.5);
		stack->SetMaximum(1e3);
		canvas->SetLogy();
	}
	else {
		stack->SetMaximum(1.5 * he->GetBinContent(he->GetMaximumBin()));
		he->Draw("same");
		leg->Draw();
	}

	canvas->Write();
	delete stack;
	delete canvas;

}



void plotstack(std::string const & ifile_path, std::string const & friend_tree_path) {

	load_trees lt(ifile_path, friend_tree_path);

	TFile * ofile = TFile::Open("plotstack.root", "recreate");

	std::string const all_cut = "passed_swtrigger == 1 && closest_asso_shower_dist_to_flashzcenter <= 40 && totalpe_ibg_sum > 140 && reco_asso_showers == 1";

	plot(lt,
			lt.vertex_tree_bnb_cosmic,
			lt.pot_bnb_cosmic,
			"bnb_cosmic_stack",
			"summed_associated_reco_shower_energy",
			"(20, 0, 0.3)",
			"",
			"",
			"BNB + Cosmic Sample",
			"Shower Energy [GeV]",
			"Vertices per 6.6e20 POT");

	plot(lt,
			lt.vertex_tree_bnb_cosmic,
			lt.pot_bnb_cosmic,
			"bnb_cosmic_stack_precut",
			"summed_associated_reco_shower_energy",
			"(20, 0, 0.3)",
			all_cut,
			"",
			"BNB + Cosmic Sample Pre-Cut",
			"Shower Energy [GeV]",
			"Vertices per 6.6e20 POT");

	plot(lt,
			lt.vertex_tree_bnb_cosmic,
			lt.pot_bnb_cosmic,
			"bnb_cosmic_stack_bdt",
			"summed_associated_reco_shower_energy",
			"(4, 0, 0.6)",
			all_cut,
			"",
			"BDT Applied",
			"Shower Energy [GeV]",
			"Vertices per 6.6e20 POT",
			false,
			"BDT",
			0.1,
			0.1);

	plot(lt,
			lt.vertex_tree_bnb_cosmic,
			lt.pot_bnb_cosmic,
			"bnb_cosmic_stack_bdt_logy",
			"summed_associated_reco_shower_energy",
			"(4, 0, 0.6)",
			all_cut,
			"",
			"BDT Applied",
			"Shower Energy [GeV]",
			"Vertices per 6.6e20 POT",
			true,
			"BDT",
			0.1,
			0.1);

}
