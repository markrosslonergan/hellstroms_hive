#include "draw_histos.h"
using std::cout;
using std::endl;
using std::vector;
using std::pair;

int nhistos = 3;
// First, save hist info: hist name in ouput_kinematic_histos.root
// (minus the "sig" or "bkg" suffix) and x-axis title you want to set
std::pair<TString, TString> get_hist_info (int id) {
    if (id == 0) 
        return std::make_pair("h_dedx_plane0",
                              "Collection Plane Energy [MeV/cm]");
    if (id == 1) 
        return std::make_pair("h_shower_energy",
                              "Reconstructed Shower Energy [GeV]");
    if (id == 2) 
        return std::make_pair("h_shower_length",
                              "Reconstructed Shower Length [cm]");
    if (id == 3) 
        return std::make_pair("h_exiting_pi0_px",
                              "Reconstructed Pion X-Momentum [GeV]");
    if (id == 4) 
        return std::make_pair("h_exiting_pi0_py",
                              "Reconstructed Pion Y-Momentum [GeV]");
    if (id == 5) 
        return std::make_pair("h_exiting_pi0_pz",
                              "Reconstructed Pion Z-Momentum [GeV]");
    throw std::invalid_argument("id");
}

void draw_histos() {
    // Read histograms output by kinematic_comp.cc, modify them,
    // then save those modified histo's to a new file
    TFile *fin = new TFile("output_kinematic_histos.root", "READ");
    TFile *fout = new TFile("final_kinematic_histos.root", "RECREATE");
    if (!fin || !fout) {
        cout << "Bad file" << endl;
        return;
    }

    // Pair of histos: first one from the vertex file (for comparison), second
    // one from output_kinematic_histos.root
    vector< pair<TH1D*,TH1D*> > histos (nhistos);
    vector<TCanvas> c (nhistos);
    // Get desired histos, and draw as desired
    for (int i = 0; i < nhistos; i++) {
        auto hist_info = get_hist_info(i);
        c.at(i).cd();
        c.at(i).SetCanvasSize(1600, 800);
        c.at(i).Divide(2, 1);

        c.at(i).cd(1); 
        histos.at(i).first = (TH1D*)fin->Get(hist_info.first+"_sig"); 
        histos.at(i).first->GetXaxis()->SetTitle(hist_info.second);
        histos.at(i).first->SetFillColor(kRed-7);
        histos.at(i).first->SetLineColor(kRed-7);
        histos.at(i).first->Draw();
        
        c.at(i).cd(2);
        histos.at(i).second = (TH1D*)fin->Get(hist_info.first+"_bkg");
        histos.at(i).second->GetXaxis()->SetTitle(hist_info.second);
        histos.at(i).second->SetFillColor(kRed-7);
        histos.at(i).second->SetLineColor(kRed-7);
        histos.at(i).second->Draw();

        c.at(i).SaveAs(hist_info.first+".png", "png"); 
    }

    fout->Write();
    fout->Close(); 
}

int main() {
    draw_histos();
    return 0;
}




