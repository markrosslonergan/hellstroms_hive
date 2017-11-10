


#include "get_mva_response_hists.h"


TH1 *  get_hist(std::string const & hname,
		TTree * tree,
		std::string const & mva_tree_name,
		std::string const & draw,
		std::string const & bins,
		std::string const & cut) {

  tree->Draw((mva_tree_name+"."+draw+">>"+hname).c_str(), cut.c_str());
  TH1F * hist = (TH1F*)gDirectory->Get(hname.c_str());
  if(hist->GetBinContent(0) > 0 || hist->GetBinContent(hist->GetNbinsX()+1) > 0) {
    std::cout << "WARNING:\n"
	      << __LINE__ << " " << __PRETTY_FUNCTION__ << "\n"
	      << "Historgram " << hname << " has entries in under/overflow bins: " << hist->GetBinContent(0) << " " << hist->GetBinContent(hist->GetNbinsX()+1) << "\n";
    tree->Draw((mva_tree_name+"."+draw+">>herror").c_str(), cut.c_str());
    TH1F * herror = (TH1F*)gDirectory->Get("herror");
    herror->Write();
  }
  std::string const binning = "(" + bins + ", " + std::to_string(hist->GetBinLowEdge(1)) + ", " + std::to_string(hist->GetBinLowEdge(hist->GetNbinsX()+1)) + ")";
  delete hist;
  tree->Draw((mva_tree_name+"."+draw+">>"+hname+binning).c_str(), cut.c_str());
  return (TH1F*)gDirectory->Get(hname.c_str());

}



void draw_mva_response_hist(std::string const & name,
			    TTree * tree,
			    std::string const & mva_tree_name,
			    std::string const & draw,
			    std::string const & bins,
			    std::string const & all_cut,
			    std::string const & cut1,
			    std::string const & cut2) {
  
  TCanvas * ctemp = new TCanvas("ctemp");
  
  TH1 * h1 = get_hist("h1", tree, mva_tree_name, draw, bins, all_cut + " && " + cut1);
  TH1 * h2 = get_hist("h2", tree, mva_tree_name, draw, bins, all_cut + " && " + cut2);

  if(h1->GetNbinsX() != h2->GetNbinsX()) {
    std::cout << "WARNING:\n"
	      << __LINE__ << " " << __PRETTY_FUNCTION__ << "\n"
	      << "Histograms have a different number of bins - h1: " << h1->GetNbinsX() << " h2: " << h2->GetNbinsX() << "\n";
  }

  TH1F * h = new TH1F(name.c_str(), "", h1->GetNbinsX() + h2->GetNbinsX(), 0, 1);
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
			    std::string const & bins,
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
			       bins,
			       all_cut,
			       cut1,
			       cut2);

      }

    }

  }

  ofile->Close();

}
