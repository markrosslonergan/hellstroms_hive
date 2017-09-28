


#include "TCanvas.h"
#include "TH1F.h"

#include "TFriendElement.h"



void draw_mva_response_hist(std::string const & name,
			    TTree * tree,
			    std::string const & mva_tree_name,
			    std::string const & draw,
			    std::string const & binning,
			    std::string const & cut1,
			    std::string const & cut2) {
  
  TCanvas * ctemp = new TCanvas("ctemp");
  
  std::string const h1_name = "h1";
  tree->Draw((mva_tree_name+"."+draw+">>"+h1_name+binning).c_str(), cut1.c_str());
  TH1F * h1 = (TH1F*)gDirectory->Get(h1_name.c_str());
  
  std::string const h2_name = "h2";
  tree->Draw((mva_tree_name+"."+draw+">>"+h2_name+binning).c_str(), cut2.c_str());
  TH1F * h2 = (TH1F*)gDirectory->Get(h2_name.c_str());
  
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
			    std::string const & binning,
			    std::vector<std::string> const & tree_cuts,
			    std::string const & cut1,
			    std::string const & cut2) {

  TFile * ofile = TFile::Open(ofile_path.c_str(), "recreate");

  for(size_t i = 0; i < trees.size(); ++i) {

    auto const & p = trees.at(i);
    std::string const & all_cut = tree_cuts.at(i);

    std::string const all_cut1 = all_cut + " && " + cut1;
    std::string const all_cut2 = all_cut + " && " + cut2;

    for(method_struct const & method : methods) {

      for(auto const & bp : branches) {

	TFriendElement * tfe = p.first->AddFriend((p.second + "_" + method.str).c_str(), ifile_path.c_str());
	draw_mva_response_hist(p.second + "_" + method.str + "_" + bp.first,
			       p.first,
			       tfe->GetTree()->GetName(),
			       bp.first,
			       binning,
			       all_cut1,
			       all_cut2);
      }

    }

  }

  ofile->Close();

}
