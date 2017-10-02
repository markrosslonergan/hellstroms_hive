


#include "TLegend.h"

#include "method_struct.hpp"
#include "object_helper.hpp"



void draw_mva_response_hists(std::string const & ifile_path,
			     std::vector<std::pair<TTree *, std::string>> const & trees,
			     std::string const & method, 
			     std::string const & branch) {

  object_helper oh;

  TCanvas * canvas = new TCanvas((method+"_"+branch).c_str());
  TLegend * legend = new TLegend(0.6, 0.9, 0.9, 0.6);
  double ymax = 0;
  TH1 * first_hist = nullptr;
  unsigned int bins = 0;

  for(size_t i = 0; i < trees.size(); ++i) {
    auto const & p = trees.at(i);
    TH1 * hist = oh.GetObject<TH1>(ifile_path.c_str(), p.second+"_"+method+"_"+branch);
    if(i == 0) {
      hist->SetStats(0);
      hist->SetTitle(method.c_str());
      hist->GetXaxis()->SetTitle(branch.c_str());
      hist->GetXaxis()->CenterTitle();
      hist->GetYaxis()->SetTitle("Area Normalized");
      hist->GetYaxis()->CenterTitle();      
      hist->Draw("hist");
      first_hist = hist;
      bins = hist->GetNbinsX();
    }
    else {
      hist->Draw("hist same");
      if(hist->GetNbinsX() != bins) {
	std::cout << "WARNING:\n"
		  << __LINE__ << " " << __PRETTY_FUNCTION__ << "\n"
		  << "Different number of bins: " << hist->GetNbinsX() << " " << bins << "\n";
      }
    }
    hist->Scale(1./hist->Integral(1, hist->GetNbinsX()));
    hist->SetLineColor(i+1);
    legend->AddEntry(hist, p.second.c_str());
    if(hist->GetBinContent(hist->GetMaximumBin()) > ymax) ymax = hist->GetBinContent(hist->GetMaximumBin());
  }
  
  first_hist->GetYaxis()->SetRangeUser(0, 1.1*ymax);

  legend->Draw();

  canvas->Write();

}



void plot_mva_response_hists(std::string const & ofile_path,
			     std::string const & ifile_path,
			     std::vector<std::pair<TTree *, std::string>> const & trees,
			     std::vector<method_struct> const & methods,
			     std::vector<std::pair<std::string, std::string>> const & branches) {

  TFile * ofile = TFile::Open(ofile_path.c_str(), "recreate");

  for(method_struct const & method : methods) {
    for(auto const & bp : branches) {
      draw_mva_response_hists(ifile_path, trees, method.str, bp.first);
    }
  }

  ofile->Close();

}
