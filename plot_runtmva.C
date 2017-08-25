



TFile * ifile_sp = nullptr;
TFile * ifile_bnb_cosmic = nullptr;
TFile * ifile_cosmic = nullptr;

TFile * ofile = nullptr;

TTree * vertex_tree_sp = nullptr;
TTree * vertex_tree_bnb_cosmic = nullptr;
TTree * vertex_tree_cosmic = nullptr;

double pot_sp = -1;
double pot_sp_cosmic = -1;
double pot_bnb_cosmic = -1;
double ngenbnbcosmic = -1;
double ngencosmic = -1;

double const run_pot = 6.6e20;

std::string const method = "BDT";
std::string name;


void init(std::string dir, std::string const & iname) {

  name = iname;

  std::string gdir;
  if(dir.find("old") == std::string::npos) gdir = "LEEPhoton/";
  std::string const app_file = name + "_app.root";

  ifile_sp = TFile::Open((dir+"runmv_sp.root").c_str());
  if(!ifile_sp) {
    std::cout << "Could not find file\n";
    exit(1);
  }
  vertex_tree_sp = (TTree*)ifile_sp->Get((gdir+"vertex_tree").c_str());
  vertex_tree_sp->AddFriend((name+"_sp_"+method).c_str(), app_file.c_str());  
  TTree * pot_tree_sp = (TTree*)ifile_sp->Get((gdir+"get_pot").c_str());
  pot_tree_sp->SetBranchAddress("pot", &pot_sp);
  pot_tree_sp->GetEntry(0);
  
  ifile_bnb_cosmic = TFile::Open((dir+"runmv_bnb_cosmic.root").c_str());
  if(!ifile_bnb_cosmic) {
    std::cout << "Could not find file\n";
    exit(1);
  }
  vertex_tree_bnb_cosmic = (TTree*)ifile_bnb_cosmic->Get((gdir+"vertex_tree").c_str());
  vertex_tree_bnb_cosmic->AddFriend((name+"_bnb_cosmic_"+method).c_str(), app_file.c_str());  
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
  vertex_tree_cosmic = (TTree*)ifile_cosmic->Get((gdir+"vertex_tree").c_str());
  vertex_tree_cosmic->AddFriend((name+"_cosmic_"+method).c_str(), app_file.c_str());  
  TTree * pot_tree_cosmic = (TTree*)ifile_cosmic->Get((gdir+"get_pot").c_str());
  int temp_ngencosmic;
  pot_tree_cosmic->SetBranchAddress("number_of_events", &temp_ngencosmic);
  pot_tree_cosmic->GetEntry(0);
  ngencosmic = temp_ngencosmic;

  ofile = TFile::Open("plot_runtmva.root", "recreate");
  
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



void plotstack(std::string const & cname,
	       std::string const & draw,
	       std::string const & binning,
	       std::string const & weight,
	       std::string const & mark_weight,
	       std::string const & xtitle = "",
	       std::string const & ytitle = "",
	       std::string const & title = "") {

  std::string const cut = "passed_swtrigger == 1";
  std::string const signal_definition = cut + " && is_delta_rad == 1 && true_nu_vtx_fid_contained == 1";
  std::string const mva = "0.1";

  std::string signal_weight = signal_definition + " && " + name + "_sp_" + method + ".mva > " + mva;
  if(weight != "") signal_weight = signal_weight + " && " + weight;
  if(mark_weight == "true_nu_E") signal_weight = "("+signal_weight+") * get_nu_weight(true_nu_E)";
  else if(mark_weight == "delta_photon_energy") signal_weight = "("+signal_weight+") * get_gamma_weight(delta_photon_energy)";

  TCanvas * canvas = new TCanvas(cname.c_str());
  vertex_tree_sp->Draw((draw+">>hsp"+binning).c_str(), signal_weight.c_str());
  TH1F * hsp = (TH1F*)gDirectory->Get("hsp");
  hsp->SetLineColor(1);
  hsp->SetFillColor(kBlue+2);
  hsp->Scale(run_pot / pot_sp);

  std::string background_weight = cut;
  if(weight != "") background_weight = background_weight + " && " + weight;

  vertex_tree_bnb_cosmic->Draw((draw+">>hbc"+binning).c_str(), (background_weight + " && " + name + "_bnb_cosmic_" + method + ".mva > " + mva).c_str());
  TH1F * hbc = (TH1F*)gDirectory->Get("hbc");
  hbc->SetLineColor(1);
  hbc->SetFillColor(kGreen+2);
  hbc->Scale(run_pot / pot_bnb_cosmic);

  vertex_tree_cosmic->Draw((draw+">>hc"+binning).c_str(), (background_weight + " && " + name + "_cosmic_" + method + ".mva > " + mva).c_str());
  TH1F * hc = (TH1F*)gDirectory->Get("hc");
  hc->SetLineColor(1);
  hc->SetFillColor(kMagenta+2);
  hc->Scale(ngenbnbcosmic/ngencosmic*10.729 / pot_bnb_cosmic * run_pot);

  THStack * stack = new THStack("stack", "");
  stack->Add(hsp);
  stack->Add(hbc);
  stack->Add(hc);

  TLegend * leg = new TLegend(0.6, 0.9, 0.9, 0.6);
  leg->AddEntry(hc, "Cosmic");
  leg->AddEntry(hbc, "BNB + Cosmic");
  leg->AddEntry(hsp, "NC #Delta Radiative");
  
  stack->Draw("hist");
  stack->SetTitle(title.c_str());
  stack->GetXaxis()->SetTitle(xtitle.c_str());
  stack->GetXaxis()->CenterTitle();
  stack->GetYaxis()->SetTitle(ytitle.c_str());
  stack->GetYaxis()->CenterTitle();

  leg->Draw();

  stack->SetMinimum(0.1);
  stack->SetMaximum(1e6);
  canvas->SetLogy();
  
  canvas->Write();

  std::cout << hsp->Integral(0, hsp->GetNbinsX()+1) / sqrt(hbc->Integral(0, hbc->GetNbinsX()+1) + hc->Integral(0, hc->GetNbinsX()+1)) << "\n";

  /*
  TCanvas * temp = new TCanvas((cname+"_signal").c_str());
  hsp->Draw("hist");
  hsp->SetTitle(title.c_str());
  hsp->GetXaxis()->SetTitle(xtitle.c_str());
  hsp->GetXaxis()->CenterTitle();
  hsp->GetYaxis()->SetTitle(ytitle.c_str());
  hsp->GetXaxis()->CenterTitle();
  temp->Write();
  */

  hsp->SetName((cname + "_hsp").c_str());
  hsp->Write();

  delete canvas;
  delete stack;
  delete leg;

}



void plot_runtmva(std::string dir, std::string const iname) {

  if(dir != "") dir += "/";
  init(dir, iname);

  plotstack("summed_associated_reco_shower_energy_nuweighted",
	    "summed_associated_reco_shower_energy",
	    "(15, 0, 0.75)",
	    "",
	    "true_nu_E",
	    "Summed Associated Reco Shower Energy [GeV]",
	    "Number of Vertices per 6.6e20 POT",
	    "MiniBooNE Weights Applied");

  plotstack("summed_associated_reco_shower_energy_pweighted",
	    "summed_associated_reco_shower_energy",
	    "(15, 0, 0.75)",
	    "",
	    "delta_photon_energy",
	    "Summed Associated Reco Shower Energy [GeV]",
	    "Number of Vertices per 6.6e20 POT",
	    "MiniBooNE Weights Applied");

  plotstack("summed_associated_reco_shower_energy",
	    "summed_associated_reco_shower_energy",
	    "(15, 0, 0.75)",
	    "",
	    "",
	    "Summed Associated Reco Shower Energy [GeV]",
	    "Number of Vertices per 6.6e20 POT",
	    "");
  
}
