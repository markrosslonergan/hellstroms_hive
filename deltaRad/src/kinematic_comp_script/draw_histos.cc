#include "kinematic_comp.h"
using std::cout;
using std::endl;
using std::vector;
using std::pair;
std::pair<TString,TString> get_hist_info(int);

int nhistos = 3;

void draw_all_histos() {
    // Read histograms output by kinematic_comp.cc, modify them,
    // then save those modified histo's to a new file
    TFile *fin = new TFile("output_kinematic_histos.root", "READ");
    if (!fin) {
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
}

void phaseSpaceOverlay() {
    TFile *fin = new TFile("output_kinematic_histos.root", "READ");
    if (!fin) {
        cout << "Bad input file" << endl;
        return;
    }

    gStyle->SetOptStat(0);
    gStyle->SetTitleFont(132);
    gStyle->SetLegendFillColor(0);
    gStyle->SetLegendFont(132);
    gStyle->SetLegendTextSize(0.05);

    TCanvas *c_PvsPZ = new TCanvas("c_PvsPZ", "c_PvsPZ", 1000, 700);
    TH2D *h_PvsPZ_bkg = (TH2D*)fin->Get("h_PvsPZ_bkg");
    h_PvsPZ_bkg->SetTitle("");
    h_PvsPZ_bkg->SetLineColor(kRed+1);
    h_PvsPZ_bkg->SetMarkerColor(kRed+1);
    h_PvsPZ_bkg->SetFillColor(kRed+1);
    h_PvsPZ_bkg->SetMarkerStyle(22);
    h_PvsPZ_bkg->SetMarkerSize(1.5);
    h_PvsPZ_bkg->GetXaxis()->SetTitle("True Total Momentum [GeV]");
    h_PvsPZ_bkg->GetXaxis()->SetTitleSize(0.05);
    h_PvsPZ_bkg->GetYaxis()->SetTitle("True z-Momentum [GeV]");
    h_PvsPZ_bkg->GetYaxis()->SetTitleSize(0.05);
    h_PvsPZ_bkg->Draw();

    TH2D *h_PvsPZ_sig = (TH2D*)fin->Get("h_PvsPZ_sig");
    h_PvsPZ_sig->SetLineColor(kAzure+1);
    h_PvsPZ_sig->SetMarkerColor(kAzure+1);
    h_PvsPZ_sig->SetFillColor(kAzure+1);
    h_PvsPZ_sig->SetMarkerStyle(23);
    h_PvsPZ_sig->SetMarkerSize(1.5);
    h_PvsPZ_sig->Draw("same");

    TLegend *legPZ = new TLegend(0.1, 0.7, 0.6, 0.9);
    legPZ->AddEntry(h_PvsPZ_bkg, "NC #Delta Rad #pi^{0} Background", "p");
    legPZ->AddEntry(h_PvsPZ_sig, "NC #pi^{0} Selection", "p");
    legPZ->Draw("same");

    c_PvsPZ->SaveAs("plot_PvsPZ.png", "PNG");

    TCanvas *c_PvsPX = new TCanvas("c_PvsPX", "c_PvsPX", 1000, 700);
    TH2D *h_PvsPX_bkg = (TH2D*)fin->Get("h_PvsPX_bkg");
    h_PvsPX_bkg->SetTitle("");
    h_PvsPX_bkg->SetLineColor(kRed+1);
    h_PvsPX_bkg->SetMarkerColor(kRed+1);
    h_PvsPX_bkg->SetFillColor(kRed+1);
    h_PvsPX_bkg->SetMarkerStyle(22);
    h_PvsPX_bkg->SetMarkerSize(1.5);
    h_PvsPX_bkg->GetXaxis()->SetTitle("True Total Momentum [GeV]");
    h_PvsPX_bkg->GetXaxis()->SetTitleSize(0.05);
    h_PvsPX_bkg->GetYaxis()->SetTitle("True x-Momentum [GeV]");
    h_PvsPX_bkg->GetYaxis()->SetTitleSize(0.05);
    h_PvsPX_bkg->Draw();

    TH2D *h_PvsPX_sig = (TH2D*)fin->Get("h_PvsPX_sig");
    h_PvsPX_sig->SetLineColor(kAzure+1);
    h_PvsPX_sig->SetMarkerColor(kAzure+1);
    h_PvsPX_sig->SetFillColor(kAzure+1);
    h_PvsPX_sig->SetMarkerStyle(23);
    h_PvsPX_sig->SetMarkerSize(1.5);
    h_PvsPX_sig->Draw("same");

    TLegend *legPX = new TLegend(0.1, 0.7, 0.6, 0.9);
    legPX->AddEntry(h_PvsPX_bkg, "NC #Delta Rad #pi^{0} Background", "p");
    legPX->AddEntry(h_PvsPX_sig, "NC #pi^{0} Selection", "p");
    legPX->Draw("same");

    c_PvsPX->SaveAs("plot_PvsPX.png", "PNG");

    TCanvas *c_PvsPY = new TCanvas("c_PvsPY", "c_PvsPY", 1100, 800);
    TH2D *h_PvsPY_bkg = (TH2D*)fin->Get("h_PvsPY_bkg");
    h_PvsPY_bkg->SetTitle("");
    h_PvsPY_bkg->SetLineColor(kRed+1);
    h_PvsPY_bkg->SetMarkerColor(kRed+1);
    h_PvsPY_bkg->SetFillColor(kRed+1);
    h_PvsPY_bkg->SetMarkerStyle(22);
    h_PvsPY_bkg->SetMarkerSize(1.5);
    h_PvsPY_bkg->GetXaxis()->SetTitle("True Total Momentum [GeV]");
    h_PvsPY_bkg->GetXaxis()->SetTitleSize(0.05);
    h_PvsPY_bkg->GetYaxis()->SetTitle("True y-Momentum [GeV]");
    h_PvsPY_bkg->GetYaxis()->SetTitleSize(0.05);
    h_PvsPY_bkg->Draw();

    TH2D *h_PvsPY_sig = (TH2D*)fin->Get("h_PvsPY_sig");
    h_PvsPY_sig->SetLineColor(kAzure+1);
    h_PvsPY_sig->SetMarkerColor(kAzure+1);
    h_PvsPY_sig->SetFillColor(kAzure+1);
    h_PvsPY_sig->SetMarkerStyle(23);
    h_PvsPY_sig->SetMarkerSize(1.5);
    h_PvsPY_sig->Draw("same");

    TLegend *legPY = new TLegend(0.1, 0.7, 0.6, 0.9);
    legPY->AddEntry(h_PvsPY_bkg, "NC #Delta Rad #pi^{0} Background", "p");
    legPY->AddEntry(h_PvsPY_sig, "NC #pi^{0} Selection", "p");
    legPY->Draw("same");

    c_PvsPY->SaveAs("plot_PvsPY.png", "PNG");

    TCanvas *c_PvsOpang = new TCanvas("c_PvsOpang", "c_PvsOpang", 1100, 800);
    TH2D *h_PvsOpang_sig = (TH2D*)fin->Get("h_PvsOpang_sig");
    h_PvsOpang_sig->SetTitle("");
    h_PvsOpang_sig->SetLineColor(kRed+1);
    //h_PvsOpang_sig->SetMarkerColor(kRed+1);
    //h_PvsOpang_sig->SetFillColor(kRed+1);
    h_PvsOpang_sig->SetMarkerStyle(7);
    h_PvsOpang_sig->SetMarkerSize(1.5);
    h_PvsOpang_sig->GetXaxis()->SetTitle("Opening Angle [Degrees]");
    h_PvsOpang_sig->GetXaxis()->SetTitleSize(0.05);
    h_PvsOpang_sig->GetYaxis()->SetTitle("Pion Momentum [GeV]");
    h_PvsOpang_sig->GetYaxis()->SetTitleSize(0.05);
    h_PvsOpang_sig->Draw();

    c_PvsOpang->SaveAs("plot_PvsOpang.png", "PNG");
}

int main() {
    //draw_all_histos();
    phaseSpaceOverlay();
    return 0;
}

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


