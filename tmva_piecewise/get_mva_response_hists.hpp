


#include "TCanvas.h"
#include "TH1F.h"

#include "TFriendElement.h"



void draw_mva_response_hist(std::string const & name,
			    TTree * tree,
			    std::string const & mva_tree_name,
			    std::string const & draw,
			    std::string const & all_cut,
			    std::string const & cut1,
			    std::string const & cut2) {

  std::string binning;
  
  TCanvas * ctemp = new TCanvas("ctemp");
  
  std::string const h1_name = "h1";
  tree->Draw((mva_tree_name+"."+draw+">>"+h1_name+binning).c_str(), (all_cut + " && " + cut1).c_str());
  TH1F * h1 = (TH1F*)gDirectory->Get(h1_name.c_str());
  if(h1->GetBinContent(0) > 0 || h1->GetBinContent(h1->GetNbinsX()+1) > 0) {
    std::cout << "WARNING:\n"
	      << __LINE__ << " " << __PRETTY_FUNCTION__ << "\n"
	      << "Historgram h1 has entries in under/overflow bins: " << h1->GetBinContent(0) << " " << h1->GetBinContent(h1->GetNbinsX()+1) << "\n";
    tree->Draw((mva_tree_name+"."+draw+">>herror").c_str(), (all_cut + " && " + cut1).c_str());
    TH1F * herror = (TH1F*)gDirectory->Get("herror");
    herror->Write();
  }
  
  std::string const h2_name = "h2";
  tree->Draw((mva_tree_name+"."+draw+">>"+h2_name+binning).c_str(), (all_cut + " && " + cut2).c_str());
  TH1F * h2 = (TH1F*)gDirectory->Get(h2_name.c_str());
  if(h2->GetBinContent(0) > 0 || h2->GetBinContent(h2->GetNbinsX()+1) > 0) {
    std::cout << "WARNING:\n"
	      << __LINE__ << " " << __PRETTY_FUNCTION__ << "\n"
	      << "Historgram h2 has entries in under/overflow bins: " << h2->GetBinContent(0) << " " << h2->GetBinContent(h2->GetNbinsX()+1) << "\n";
    tree->Draw((mva_tree_name+"."+draw+">>herror").c_str(), (all_cut + " && " + cut2).c_str());
    TH1F * herror = (TH1F*)gDirectory->Get("herror");
    herror->Write();
  }
  
  if(h1->GetNbinsX() != h2->GetNbinsX()) {
    std::cout << "WARNING:\n"
	      << __LINE__ << " " << __PRETTY_FUNCTION__ << "\n"
	      << "Histograms have a different number of bins - h1: " << h1->GetNbinsX() << " h2: " << h2->GetNbinsX() << "\n";
  }

  TH1F * h = new TH1F(name.c_str(), "", h1->GetNbinsX() + h2->GetNbinsX(), h1->GetBinLowEdge(1), h2->GetBinLowEdge(h2->GetNbinsX()) + h2->GetBinWidth(h2->GetNbinsX()));
  for(int i = 1; i <= h->GetNbinsX(); ++i) {
    if(i <= h1->GetNbinsX()) h->SetBinContent(i, h1->GetBinContent(i));
    else h->SetBinContent(i, h2->GetBinContent(i-h1->GetNbinsX()));
  }
  
  h->Write();
  
  delete h;
  delete h1;
  delete h2;
  delete ctemp;
  
}



void get_mva_response_hists(std::string const & ofile_path,
			    std::string const & ifile_path,
			    std::vector<std::pair<TTree *, std::string>> const & trees,
			    std::vector<method_struct> const & methods,
			    std::vector<std::pair<std::string, std::string>> const & branches,
			    std::vector<std::string> const & tree_cuts,
			    std::string const & cut1,
			    std::string const & cut2) {

  TFile * ofile = TFile::Open(ofile_path.c_str(), "recreate");

  for(size_t i = 0; i < trees.size(); ++i) {

    auto const & p = trees.at(i);
    std::string const & all_cut = tree_cuts.at(i);

    for(method_struct const & method : methods) {

      for(auto const & bp : branches) {

	TFriendElement * tfe = p.first->AddFriend((p.second + "_" + method.str).c_str(), ifile_path.c_str());
	draw_mva_response_hist(p.second + "_" + method.str + "_" + bp.first,
			       p.first,
			       tfe->GetTree()->GetName(),
			       bp.first,
			       all_cut,
			       cut1,
			       cut2);
      }

    }

  }

  ofile->Close();

}
