


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

std::vector<std::string> friend_tree_names;

void init(std::string dir, std::string const & friend_tree_file = "") {

  if(dir != "") dir += "/";
  std::string gdir;
  if(dir.find("old") == std::string::npos) gdir = "LEEPhoton/";

  ifile_sp = TFile::Open((dir+"runmv_sp.root").c_str());
  if(ifile_sp) {
    event_tree_sp = (TTree*)ifile_sp->Get((gdir+"event_tree").c_str());
    vertex_tree_sp = (TTree*)ifile_sp->Get((gdir+"vertex_tree").c_str());  
    TTree * pot_tree_sp = (TTree*)ifile_sp->Get((gdir+"get_pot").c_str());
    pot_tree_sp->SetBranchAddress("pot", &pot_sp);
    pot_tree_sp->GetEntry(0);
  }    

  ifile_bnb = TFile::Open((dir+"runmv_bnb.root").c_str());
  if(ifile_bnb) {
    event_tree_bnb = (TTree*)ifile_bnb->Get((gdir+"event_tree").c_str());
    vertex_tree_bnb = (TTree*)ifile_bnb->Get((gdir+"vertex_tree").c_str());
    TTree * pot_tree_bnb = (TTree*)ifile_bnb->Get((gdir+"get_pot").c_str());
    pot_tree_bnb->SetBranchAddress("pot", &pot_bnb);
    pot_tree_bnb->GetEntry(0);
  }

  ifile_bnb_cosmic = TFile::Open((dir+"runmv_bnb_cosmic.root").c_str());
  if(ifile_bnb_cosmic) {
    event_tree_bnb_cosmic = (TTree*)ifile_bnb_cosmic->Get((gdir+"event_tree").c_str());
    vertex_tree_bnb_cosmic = (TTree*)ifile_bnb_cosmic->Get((gdir+"vertex_tree").c_str());
    TTree * pot_tree_bnb_cosmic = (TTree*)ifile_bnb_cosmic->Get((gdir+"get_pot").c_str());
    pot_tree_bnb_cosmic->SetBranchAddress("pot", &pot_bnb_cosmic);
    int temp_ngenbnbcosmic;
    pot_tree_bnb_cosmic->SetBranchAddress("number_of_events", &temp_ngenbnbcosmic);
    pot_tree_bnb_cosmic->GetEntry(0);
    ngenbnbcosmic = temp_ngenbnbcosmic;
  }

  ifile_cosmic = TFile::Open((dir+"runmv_cosmic.root").c_str());
  if(ifile_cosmic) {
    event_tree_cosmic = (TTree*)ifile_cosmic->Get((gdir+"event_tree").c_str());
    vertex_tree_cosmic = (TTree*)ifile_cosmic->Get((gdir+"vertex_tree").c_str());
    TTree * pot_tree_cosmic = (TTree*)ifile_cosmic->Get((gdir+"get_pot").c_str());
    int temp_ngencosmic;
    pot_tree_cosmic->SetBranchAddress("number_of_events", &temp_ngencosmic);
    pot_tree_cosmic->GetEntry(0);
    ngencosmic = temp_ngencosmic;
  }

  ifile_sp_cosmic = TFile::Open((dir+"runmv_sp_cosmic.root").c_str());
  if(ifile_sp_cosmic) {
    event_tree_sp_cosmic = (TTree*)ifile_sp_cosmic->Get((gdir+"event_tree").c_str());
    vertex_tree_sp_cosmic = (TTree*)ifile_sp_cosmic->Get((gdir+"vertex_tree").c_str());  
    TTree * pot_tree_sp_cosmic = (TTree*)ifile_sp_cosmic->Get((gdir+"get_pot").c_str());
    pot_tree_sp_cosmic->SetBranchAddress("pot", &pot_sp_cosmic);
    pot_tree_sp_cosmic->GetEntry(0);
  }

  if(friend_tree_file != "") {
    std::vector<std::string> const method_v = {
      "BDTG",
      "BDT",
      "BDTB",
      "BDTD",
      "BDTF",
      "RuleFit"
    };

    for(std::string const & method : method_v) {
      friend_tree_names.push_back(std::string(vertex_tree_sp->AddFriend(("ncdelta_"+method).c_str(), friend_tree_file.c_str())->GetTree()->GetName()));
      friend_tree_names.push_back(std::string(vertex_tree_sp_cosmic->AddFriend(("ncdelta_cosmic_"+method).c_str(), friend_tree_file.c_str())->GetTree()->GetName()));
      friend_tree_names.push_back(std::string(vertex_tree_bnb_cosmic->AddFriend(("bnb_cosmic_"+method).c_str(), friend_tree_file.c_str())->GetTree()->GetName()));
    }
  }

  ofile = TFile::Open("plotsupimp.root", "recreate");
  
}


void setlogx(TH1 * h, std::string const & name, std::string const & binning) {

  int binno;
  double xmin;
  double xmax;

  {
    size_t const lbrac = binning.find("(");
    size_t const com1 = binning.find(",");
    size_t const com2 = binning.find(",", com1 + 1);
    size_t const rbrac = binning.rfind(")");

    binno = std::stoi(binning.substr(lbrac + 1, com1 - lbrac - 1));
    xmin = std::stod(binning.substr(com1 + 1, com2 - com1 - 1));
    xmax = std::stod(binning.substr(com2 + 1, rbrac - com2 - 1));

  }

  double width = (xmax - xmin) / binno;
  std::vector<double> bins;
  for(int i = 0; i <= binno; ++i) bins.push_back(log(xmin + i * width));

  h = new TH1F(name.c_str(), "", bins.size()-1, &bins.at(0));

}


void hist_errors(TH1 * hist, double const scaling) {

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

  histe->Draw("hist samee");

}


TH1 * draw_hist(TTree * tree,
		std::string const & draw,
		std::string const & binning,
		std::string we,
		std::string const & we_other,
		int const color,
		bool const logx,
		std::string const & friend_tree_name,
		double const mva1,
		double const mva2) {

  TH1 * h = nullptr;
  std::string drbinning = binning;
  if(logx) drbinning = "";
  if(we != "" && we_other != "") we = we + " && " + we_other; 
  if(mva1 != DBL_MAX || mva2 != DBL_MAX) {
    if(we != "") we += " && ";
    we  += "((reco_asso_tracks == 0 && " + friend_tree_name + ".mva > "+std::to_string(mva1)+") || (reco_asso_tracks > 0 && " + friend_tree_name + ".mva > "+std::to_string(mva2)+"))";
  }
  std::string const hist_name = "h_" + friend_tree_name;
  if(logx) setlogx(h, hist_name, binning);
  tree->Draw((draw+">>"+hist_name+drbinning).c_str(), we.c_str());
  h = (TH1*)gDirectory->Get(hist_name.c_str());
  h->SetLineColor(color);
  h->SetLineWidth(3);
  
  return h;

}


double get_scaling(std::string const & name) {

  double const run_pot = 6.6e20;

  if(name == "ncdelta") {
    return run_pot / pot_sp;
  }
  if(name == "ncdelta_cosmic") {
    return run_pot / pot_sp_cosmic;
  }
  if(name == "bnb") {
    return run_pot / pot_bnb;
  }
  if(name == "bnb_cosmic") {
    return run_pot / pot_bnb_cosmic;
  }
  if(name == "intime_cosmic") {
    return run_pot * ngenbnbcosmic / ngencosmic * 10.729 / pot_bnb_cosmic;
  }
  else {
    std::cout << "WARNING: could not find name: " << name << "\n";
    return 0;
  }

}


void plot(std::string const & cname,
	  std::string const & draw,
	  std::string const & binning = "",
	  std::string const & we = "",
	  std::string opt = "",
	  std::string const & title = "",
	  std::string const & xtitle = "",
	  std::string const & ytitle = "",
	  bool const absolute_normalization = false,
	  bool const logx = false,
	  bool const logy = false,
	  int const setstats = 0,
	  std::string const & method = "",
	  double const mva1 = DBL_MAX,
	  double const mva2 = DBL_MAX) {
  
  int const color_offset = 2;
  TCanvas * canvas = nullptr;
  if(absolute_normalization) canvas = new TCanvas((cname+"_abs").c_str());
  else canvas = new TCanvas(cname.c_str());

  std::map<std::string, TH1*> hist_m;
  
  if(ifile_sp && !(method != "" && std::find(friend_tree_names.begin(), friend_tree_names.end(), "ncdelta_"+method) == friend_tree_names.end())) {
    std::string hist_name = "ncdelta";
    if(method != "") hist_name = "ncdelta_" + method;
    hist_m.emplace("ncdelta", draw_hist(vertex_tree_sp, draw, binning, we, signal_definition, kBlue+color_offset, logx, hist_name, mva1, mva2));
  }

  if(ifile_bnb && !(method != "" && std::find(friend_tree_names.begin(), friend_tree_names.end(), "bnb_"+method) == friend_tree_names.end())) {
    std::string hist_name = "bnb";
    if(method != "") hist_name = "bnb_" + method;
    hist_m.emplace("bnb", draw_hist(vertex_tree_bnb, draw, binning, we, "", kRed+color_offset, logx, hist_name, mva1, mva2));
  }

  if(ifile_bnb_cosmic && !(method != "" && std::find(friend_tree_names.begin(), friend_tree_names.end(), "bnb_cosmic_"+method) == friend_tree_names.end())) {
    std::string hist_name = "bnb_cosmic";
    if(method != "") hist_name = "bnb_cosmic_" + method;
    hist_m.emplace("bnb_cosmic", draw_hist(vertex_tree_bnb_cosmic, draw, binning, we, "", kGreen+color_offset, logx, hist_name, mva1, mva2));
  }

  if(ifile_cosmic && !(method != "" && std::find(friend_tree_names.begin(), friend_tree_names.end(), "intime_cosmic_"+method) == friend_tree_names.end())) {
    std::string hist_name = "intime_cosmic";
    if(method != "") hist_name = "intime_cosmic_" + method;
    hist_m.emplace("intime_cosmic", draw_hist(vertex_tree_cosmic, draw, binning, we, "", kMagenta+color_offset, logx, hist_name, mva1, mva2));
  }

  if(ifile_sp_cosmic && !(method != "" && std::find(friend_tree_names.begin(), friend_tree_names.end(), "ncdelta_cosmic_"+method) == friend_tree_names.end())) {
    std::string hist_name = "ncdelta_cosmic";
    if(method != "") hist_name = "ncdelta_cosmic_" + method;
    hist_m.emplace("ncdelta_cosmic", draw_hist(vertex_tree_sp_cosmic, draw, binning, we, signal_definition, kCyan+color_offset, logx, hist_name, mva1, mva2));
  }

  if(!hist_m.empty()) {

    TH1 * hfirst = hist_m.begin()->second;
    hfirst->SetStats(setstats);
    hfirst->SetTitle(title.c_str());
    hfirst->GetXaxis()->SetTitle(xtitle.c_str());
    hfirst->GetXaxis()->CenterTitle(); 
    hfirst->GetYaxis()->SetTitle(ytitle.c_str());
    hfirst->GetYaxis()->CenterTitle();

    double ymax = -DBL_MAX;
    for(auto const & p : hist_m) {
      TH1 * h = p.second;
      if(ymax == -DBL_MAX) h->Draw("hist");
      else h->Draw("samehist");
      if(absolute_normalization) hist_errors(h, get_scaling(p.first));
      else hist_errors(h, 1./h->Integral(1,h->GetNbinsX()));
      if(h->GetBinContent(h->GetMaximumBin()) > ymax) ymax = h->GetBinContent(h->GetMaximumBin());
    }
    
    if(logy) {
      hfirst->GetYaxis()->SetRangeUser(1e-5, ymax*1e7);
      canvas->SetLogy();
    }
    else {
      hfirst->GetYaxis()->SetRangeUser(0, ymax*1.6);
    }
    
    TLegend * l = new TLegend(0.6, 0.9, 0.9, 0.6);
    std::map<std::string, TH1*>::iterator hm_it = hist_m.find("ncdelta");
    if(hm_it != hist_m.end()) l->AddEntry(hm_it->second, "NC #Delta Rad");
    hm_it = hist_m.find("ncdelta_cosmic");
    if(hm_it != hist_m.end()) l->AddEntry(hm_it->second, "NC #Delta Rad + Cosmic");
    hm_it = hist_m.find("bnb");
    if(hm_it != hist_m.end()) l->AddEntry(hm_it->second, "BNB Inclusive");
    hm_it = hist_m.find("bnb_cosmic");
    if(hm_it != hist_m.end()) l->AddEntry(hm_it->second, "BNB + Cosmic");
    hm_it = hist_m.find("intime_cosmic");
    if(hm_it != hist_m.end()) l->AddEntry(hm_it->second, "In-time Cosmic");
    l->Draw();
    
    canvas->Write();
    delete l;
    for(auto const & p : hist_m) delete p.second;

  }

  delete canvas;

}



void divide_hist(TCanvas * canvas, TH1 * hspc, TH1 * hsp, bool first = false) {
  
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
    if(bin_errors.at(i-1)) hspce->SetBinError(i, bin_errors.at(i-1));
  }
  
  hspce->SetLineColor(hspc->GetLineColor());
  hspce->SetLineWidth(hspc->GetLineWidth());
  
  canvas->cd();
  if(first) hspc->Draw("hist");
  else hspc->Draw("samehist");
  hspce->Draw("sameep");
  
}



void plot_efficiency(std::string const & name,
		     std::string const & draw,
		     std::string const & binning,
		     std::string const & cut,
		     std::string const & cut_eff,
		     std::string const & title = "",
		     std::string const & xtitle = "",
		     std::string const & ytitle = "",
		     std::string const & method = "",
		     double const mva1 = DBL_MAX,
		     double const mva2 = DBL_MAX) {

  std::string andstr = "";
  std::string andstreff = "";
  if(cut != "") andstr = " && ";
  if(cut_eff != "") andstreff = " && ";
  int color_offset = 2;

  TCanvas * canvas_eff = new TCanvas(name.c_str());

  vertex_tree_sp->Draw((draw+">>hsp_"+name+binning).c_str(), (cut+andstr+signal_definition).c_str());
  TH1 * hsp = (TH1*)gDirectory->Get(("hsp_"+name).c_str());
  if(method != "" && (mva1 != DBL_MAX || mva2 != DBL_MAX)) {
    vertex_tree_sp->Draw((draw+">>hspc_"+name+binning).c_str(), (cut_eff+andstreff+"((reco_asso_tracks == 0 && ncdelta_" + method + ".mva > "+std::to_string(mva1)+") || (reco_asso_tracks > 0 && ncdelta_" + method + ".mva > "+std::to_string(mva2)+")) && "+signal_definition).c_str());
  }
  else { 
    vertex_tree_sp->Draw((draw+">>hspc_"+name+binning).c_str(), (cut_eff+andstreff+signal_definition).c_str());
  }
  TH1 * hspc = (TH1*)gDirectory->Get(("hspc_"+name).c_str());

  cout << hspc->Integral(0, hspc->GetNbinsX()+1) / hsp->Integral(0, hsp->GetNbinsX()+1) << "\n";

  hspc->SetStats(0);
  hspc->SetTitle(title.c_str());
  hspc->GetXaxis()->SetTitle(xtitle.c_str());
  hspc->GetXaxis()->CenterTitle();
  hspc->GetYaxis()->SetTitle(ytitle.c_str());
  hspc->GetYaxis()->CenterTitle();
  hspc->SetLineWidth(3);
  hspc->SetLineColor(kBlue+color_offset);
  divide_hist(canvas_eff, hspc, hsp, true);
  hspc->GetYaxis()->SetRangeUser(0, 0.5);
  double ymax = -DBL_MAX;
  if(hspc->GetBinContent(hspc->GetMaximumBin()) > ymax) ymax = hspc->GetBinContent(hspc->GetMaximumBin());  

  TCanvas * ctemp = new TCanvas("ctemp");
  vertex_tree_sp_cosmic->Draw((draw+">>hspcosmic_"+name+binning).c_str(), (cut+andstr+signal_definition).c_str());
  TH1 * hspcosmic = (TH1*)gDirectory->Get(("hspcosmic_"+name).c_str());
  if(method != "" && (mva1 != DBL_MAX || mva2 != DBL_MAX)) {
    vertex_tree_sp_cosmic->Draw((draw+">>hspcosmicc_"+name+binning).c_str(), (cut_eff+andstreff+"((reco_asso_tracks == 0 && ncdelta_cosmic_" + method + ".mva > "+std::to_string(mva1)+") || (reco_asso_tracks > 0 && ncdelta_cosmic_" + method + ".mva > "+std::to_string(mva2)+")) && "+signal_definition).c_str());
  }
  else { 
    vertex_tree_sp_cosmic->Draw((draw+">>hspcosmicc_"+name+binning).c_str(), (cut_eff+andstreff+signal_definition).c_str());
  }
  TH1 * hspcosmicc = (TH1*)gDirectory->Get(("hspcosmicc_"+name).c_str());

  hspcosmicc->SetLineWidth(3);
  hspcosmicc->SetLineColor(kCyan+color_offset);
  divide_hist(canvas_eff, hspcosmicc, hspcosmic);
  if(hspcosmicc->GetBinContent(hspcosmicc->GetMaximumBin()) > ymax) ymax = hspcosmicc->GetBinContent(hspcosmicc->GetMaximumBin());    

  ctemp->cd();
  vertex_tree_bnb_cosmic->Draw((draw+">> hbnbcosmic_"+name+binning).c_str(), (cut).c_str());
  TH1 * hbnbcosmic = (TH1*)gDirectory->Get(("hbnbcosmic_"+name).c_str());
  if(method != "" && (mva1 != DBL_MAX || mva2 != DBL_MAX)) {
    vertex_tree_bnb_cosmic->Draw((draw+">>hbnbcosmicc_"+name+binning).c_str(), (cut_eff+andstreff+"((reco_asso_tracks == 0 && bnb_cosmic_" + method + ".mva > "+std::to_string(mva1)+") || (reco_asso_tracks > 0 && bnb_cosmic_" + method + ".mva > "+std::to_string(mva2)+"))").c_str());
  }
  else { 
    vertex_tree_bnb_cosmic->Draw((draw+">>hbnbcosmicc_"+name+binning).c_str(), cut_eff.c_str());
  }
  TH1 * hbnbcosmicc = (TH1*)gDirectory->Get(("hbnbcosmicc_"+name).c_str());
  hbnbcosmicc->SetLineWidth(3);
  hbnbcosmicc->SetLineColor(kGreen+color_offset);    
  divide_hist(canvas_eff, hbnbcosmicc, hbnbcosmic);
  if(hbnbcosmicc->GetBinContent(hbnbcosmicc->GetMaximumBin()) > ymax) ymax = hbnbcosmicc->GetBinContent(hbnbcosmicc->GetMaximumBin());

  TLegend * leg = new TLegend(0.6, 0.9, 0.9, 0.6);
  leg->AddEntry(hspc, "NC #Delta Radiative");
  leg->AddEntry(hspcosmicc, "NC #Delta Radiative + Cosmic");    
  leg->AddEntry(hbnbcosmicc, "BNB + Cosmic");    
  leg->Draw();
  
  canvas_eff->Write();
  
  delete hsp;
  delete hspc;
  delete hspcosmic;
  delete hspcosmicc;
  delete canvas_eff;
  delete leg;
  delete ctemp;
  
}



void plot_cut(std::string const & name, std::string const & title, std::string const & cut, bool const absolute_normalization = false, bool const logy = false, std::string const & method = "", double const mva1 = DBL_MAX, double const mva2 = DBL_MAX) {

  std::string andstr = "";
  if(cut != "") andstr = " && ";

  plot(name+"_closest_asso_shower_dist_to_flashzcenter",
       "closest_asso_shower_dist_to_flashzcenter",
       "(25, 0, 1000)",
       cut,
       "",
       title,
       "Distance Between Associated Shower and Closest In-Time Flash Z [cm]",
       "Area Normalized",
       absolute_normalization,
       false,
       logy,
       0,
       method,
       mva1,
       mva2);
  
  plot(name+"_closest_asso_shower_dist_to_flashzcenter_log",
       "log(closest_asso_shower_dist_to_flashzcenter)",
       "(25, 0.9, 1000)",
       cut,
       "",
       title,
       "Distance Between Associated Shower and Closest In-Time Flash Z [cm] (log)",
       "Area Normalized",
       absolute_normalization,
       true,
       logy,
       0,
       method,
       mva1,
       mva2);

  plot(name+"_totalpe_ibg_sum",
       "totalpe_ibg_sum",
       "(25, 0, 2000)",
       cut,
       "",
       title,
       "Total Reconstructed In-Time PE",
       "Area Normalized",
       absolute_normalization,
       false,
       logy,
       0,
       method,
       mva1,
       mva2);

  plot(name+"_totalpe_bbg_sum",
       "totalpe_bbg_sum",
       "(25, 0, 2000)",
       cut,
       "",
       title,
       "Total Reconstructed Before-Beam PE",
       "Area Normalized",
       absolute_normalization,
       false,
       logy,
       0,
       method,
       mva1,
       mva2);
  
  plot(name+"_totalpe_bbg_sum_log",
       "totalpe_bbg_sum",
       "(25, 0, 5000)",
       cut,
       "",
       title,
       "Total Reconstructed Before-Beam PE",
       "Area Normalized",
       absolute_normalization,
       true,
       logy,
       0,
       method,
       mva1,
       mva2);

  plot(name+"_summed_associated_reco_shower_energy",
       "summed_associated_reco_shower_energy",
       "(25, 0, 0.5)",
       cut,
       "",
       title,
       "Summed Associated Shower Reco Energy [GeV]",
       "Area Normalized",
       absolute_normalization,
       false,
       logy,
       0,
       method,
       mva1,
       mva2);

  /*
  plot(name+"_most_energetic_shower_reco_energy",
       "most_energetic_shower_reco_energy",
       "(25, 0, 0.5)",
       cut,
       "",
       title,
       "Most Energetic Associated Shower Reco Energy [GeV]",
       "Area Normalized",
       absolute_normalization,
       false,
       logy,
       0,
       method,
       mva1,
       mva2);
  
  plot(name+"_summed_associated_helper_shower_energy",
       "summed_associated_helper_shower_energy",
       "(25, 0, 0.5)",
       cut,
       "",
       title,
       "Summed Associated Shower Helper Energy [GeV]",
       "Area Normalized",
       absolute_normalization,
       false,
       logy,
       0,
       method,
       mva1,
       mva2);
  */  

  plot(name+"_reco_nu_vtx_dist_to_closest_tpc_wall",
       "reco_nu_vtx_dist_to_closest_tpc_wall",
       "(25, 0, 125)",
       cut,
       "",
       title,
       "Reco Vtx Distance to Closest TPC Wall [cm]",
       "Area Normalized",
       absolute_normalization,
       false,
       logy,
       0,
       method,
       mva1,
       mva2);
  
  plot(name+"_most_energetic_shower_reco_thetaxz",
       "most_energetic_shower_reco_thetaxz",
       "(25, -1.7, 1.7)",
       cut,
       "",
       title,
       "Most Energetic Shower #theta_{xz} [radians]",
       "Area Normalized",
       absolute_normalization,
       false,
       logy,
       0,
       method,
       mva1,
       mva2);
  
  plot(name+"_most_energetic_shower_reco_thetayz",
       "most_energetic_shower_reco_thetayz",
       "(25, -1.7, 1.7)",
       cut,
       "",
       title,
       "Most Energetic Shower #theta_{yz} [radians]",
       "Area Normalized",
       absolute_normalization,
       false,
       logy,
       0,
       method,
       mva1,
       mva2);

  /*
  plot(name+"_most_energetic_shower_reco_thetay",
       "acos(-most_energetic_shower_reco_diry)",
       "(25, 0, 3.14)",
       cut,
       "",
       title,
       "Most Energetic Shower #theta_{-y} [radians]",
       "Area Normalized",
       absolute_normalization,
       false,
       logy,
       0,
       method,
       mva1,
       mva2);  
  
  plot(name+"_most_energetic_shower_reco_thetaz",
       "acos(most_energetic_shower_reco_dirz)",
       "(25, 0, 3.14)",
       cut,
       "",
       title,
       "Most Energetic Shower #theta_{z} [radians]",
       "Area Normalized",
       absolute_normalization,
       false,
       logy,
       0,
       method,
       mva1,
       mva2);
  */

  plot(name+"_most_energetic_shower_bp_dist_to_tpc",
       "most_energetic_shower_bp_dist_to_tpc",
       "(25, 0, 1000)",
       cut,
       "",
       title,
       "Backwards Procted Shower Distance to TPC Wall [cm]",
       "Area Normalized",
       absolute_normalization,
       false,
       logy,
       0,
       method,
       mva1,
       mva2);
  
  plot(name+"_reco_shower_dedx_plane0",
       "reco_shower_dedx_plane0",
       "(48, 0, 10)",
       cut,
       "",
       title + " Plane 0",
       "Most Energetic Shower dE/dx [MeV/cm]",
       "Area Normalized",
       absolute_normalization,
       false,
       logy,
       0,
       method,
       mva1,
       mva2);
  
  plot(name+"_reco_shower_dedx_plane1",
       "reco_shower_dedx_plane1",
       "(48, 0, 10)",
       cut,
       "",
       title + " Plane 1",
       "Most Energetic Shower dE/dx [MeV/cm]",
       "Area Normalized",
       absolute_normalization,
       false,
       logy,
       0,
       method,
       mva1,
       mva2);
  
  plot(name+"_reco_shower_dedx_plane2",
       "reco_shower_dedx_plane2",
       "(48, 0, 10)",
       cut,
       "",
       title + " Plane 2",
       "Most Energetic Shower dE/dx [MeV/cm]",
       "Area Normalized",
       absolute_normalization,
       false,
       logy,
       0,
       method,
       mva1,
       mva2);
  
  plot(name+"_shortest_asso_shower_to_vert_dist",
       "shortest_asso_shower_to_vert_dist",
       "(25, 0, 10)",
       cut + andstr + "reco_asso_tracks > 0",
       "",
       title,
       "Distance Between Closest Associated Shower Start and Reco Vtx [cm]",
       "Area Normalized",
       absolute_normalization,
       false,
       logy,
       0,
       method,
       mva1,
       mva2);
  
  plot(name+"_longest_asso_track_thetaxz",
       "longest_asso_track_thetaxz",
       "(25, -1.7, 1.7)",
       cut + andstr + "reco_asso_tracks > 0",
       "",
       title,
       "Longest Associated Track #theta_{xz} [radians]",
       "Area Normalized",
       absolute_normalization,
       false,
       logy,
       0,
       method,
       mva1,
       mva2);
  
  plot(name+"_longest_asso_track_thetayz",
       "longest_asso_track_thetayz",
       "(25, -1.7, 1.7)",
       cut + andstr + "reco_asso_tracks > 0",
       "",
       title,
       "Longest Associated Track #theta_{yz} [radians]",
       "Area Normalized",
       absolute_normalization,
       false,
       logy,
       0,
       method,
       mva1,
       mva2);

  /*  
  plot(name+"_longest_asso_track_thetay",
       "acos(-longest_asso_track_reco_diry)",
       "(25, 0, 3.14)",
       cut + andstr + "reco_asso_tracks > 0",
       "",
       title,
       "Longest Associated Track #theta_{-y} [radians]",
       "Area Normalized",
       absolute_normalization,
       false,
       logy,
       0,
       method,
       mva1,
       mva2);

  plot(name+"_longest_asso_track_thetaz",
       "acos(longest_asso_track_reco_dirz)",
       "(25, 0, 3.14)",
       cut + andstr + "reco_asso_tracks > 0",
       "",
       title,
       "Longest Associated Track #theta_{z} [radians]",
       "Area Normalized",
       absolute_normalization,
       false,
       logy,
       0,
       method,
       mva1,
       mva2); 
  */  

  plot(name+"_reco_asso_tracks",
       "reco_asso_tracks",
       "(5, 0, 5)",
       cut + andstr + "reco_asso_tracks > 0",
       "",
       title,
       "Number of Associated Tracks",
       "Area Normalized",
       absolute_normalization,
       false,
       logy,
       0,
       method,
       mva1,
       mva2);
  
  plot(name+"_longest_asso_track_displacement",
       "longest_asso_track_displacement",
       "(25, 0, 300)",
       cut + andstr + "reco_asso_tracks > 0",
       "",
       title,
       "Longest Associated Track Length [cm]",
       "Area Normalized",
       absolute_normalization,
       false,
       logy,
       0,
       method,
       mva1,
       mva2);
  
}



void plotsupimp(std::string const dir, std::string const friend_tree_file = "") {

  init(dir, friend_tree_file);
  
  std::string const all_cut = "passed_swtrigger == 1 && closest_asso_shower_dist_to_flashzcenter <= 40 && totalpe_ibg_sum > 140 && reco_asso_showers == 1";

  plot_cut("all", "All", "");
  plot_cut("all", "All", "", true, true);
  plot_cut("precut", "Pre-Cut", all_cut);
  plot_cut("precut", "Pre-Cut", all_cut, true, true);

  if(friend_tree_file != "") {
    plot_cut("bdt", "BDT Response Applied", all_cut, false, false, "BDT", 0.1, 0.1);
    plot_cut("bdt", "BDT Response Applied", all_cut, true, true, "BDT", 0.1, 0.1);
    plot_efficiency("all_totalpe_cut",
		    "true_nu_E",
		    "(15, 0, 2.2)",
		    "",
		    "reco_asso_showers == 1 && passed_swtrigger == 1 && reco_asso_showers == 1 && closest_asso_shower_dist_to_flashzcenter <= 40 && totalpe_ibg_sum > 140",
		    "",
		    "True Neutrino Energy [GeV]",
		    "Efficiency",
		    "BDTD",
		    0.1, 0.1);
  }

}

