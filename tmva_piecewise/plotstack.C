


#include "load_trees.hpp"



std::map<int, std::pair<std::string, int>> mode_map = {
  {0, std::make_pair("QE", kRed)},
  {1, std::make_pair("RES", kYellow)},
  {2, std::make_pair("DIS", kGreen)},
  {3, std::make_pair("COH", kBlue)},
  {5, std::make_pair("Electron Scattering", kMagenta)},
  {10, std::make_pair("MEC", kCyan)}
};



void plot(load_trees const & lt,
	  TTree * tree,
	  double const sample_pot,
	  std::string const & name,
	  std::string const & draw,
	  std::string const & binning,
	  std::string const & weight = "",
	  std::string const & option = "",
	  std::string const & title = "",
	  std::string const & xtitle = "",
	  std::string const & ytitle = "",
	  bool const logy = false,
	  std::string const & method = "",
	  double const mva1 = DBL_MAX,
	  double const mva2 = DBL_MAX) {

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
  TLegend * leg = new TLegend(0.6, 0.9, 0.9, 0.6);

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
    hist_signal->SetFillColor(0);
    hist_signal->Scale(run_pot / lt.pot_sp_cosmic);
    stack->Add(hist_signal);
    leg->AddEntry(hist_signal, "NC #Delta Radiative");    
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
	leg->AddEntry(hist, (ccnc_str + " " + im_it->second.first).c_str());
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
