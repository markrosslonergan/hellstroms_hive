#include "plot_mva_response_hists.h"



void draw_mva_response_hists(std::string const & ifile_path,
		std::vector<std::pair<TTree *, std::string>> const & trees,
		std::string const & method, 
		std::string const & branch) {

	object_helper<TH1> oh;

	TCanvas * canvas = new TCanvas((method+"_"+branch).c_str());
	TLegend * legend = new TLegend(0.11, 0.65, 0.45, 0.89);
	legend->SetLineColor(kWhite);
	legend->SetFillStyle(0);
	
	double ymax = 0;
	TH1 * first_hist = nullptr;
	unsigned int bins = 0;
	
	//kWhite  = 0,   kBlack  = 1,   kGray    = 920,  kRed    = 632,  kGreen  = 416,
	//kBlue   = 600, kYellow = 400, kMagenta = 616,  kCyan   = 432,  kOrange = 800,
	//kSpring = 820, kTeal   = 840, kAzure   =  860, kViolet = 880,  kPink   = 900

	std::vector<int> mycol = {416-6, 800+3, 616+1, 632-7, 600-7, 432+1, 900}; 				

	for(size_t i = 0; i < trees.size(); ++i) {
		auto const & p = trees.at(i);
		TH1 * hist = oh.GetObject(ifile_path.c_str(), p.second+"_"+method+"_"+branch);
		if(i == 0) {
			hist->SetStats(0);
			hist->SetTitle(method.c_str());
			hist->GetXaxis()->SetTitle(branch.c_str());
			hist->GetXaxis()->CenterTitle();
			hist->GetYaxis()->SetTitle("Area Normalized");
			hist->GetYaxis()->CenterTitle();   
			hist->SetLineColor(mycol.at(i));
			hist->SetLineWidth(2);   
			hist->Draw("hist");
			first_hist = hist;
			bins = hist->GetNbinsX();
		}
		else {
			hist->SetLineColor(mycol.at(i));
			hist->SetLineWidth(2);  
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
