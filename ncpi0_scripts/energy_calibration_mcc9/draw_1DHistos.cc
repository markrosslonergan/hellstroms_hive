#include "energy_cal.h"
#include "LandauGaussian.h"
using std::cout;
using std::endl;

void draw_1DHistos() {
    TFile *fin = new TFile("output_energy_calibration.root", "READ");
    if (!fin) {
        cout << "Bad input file" << endl;
        return;
    }

    // Get binning from header file
    std::vector< std::pair<int,int> > leadingBins = leadingProjBins();
    double binToMev = 1000; // Bins in GeV by default
    // Prepare canvases
    TCanvas *c_lead = new TCanvas("c_lead", "c_lead", 1900, 1000);
    c_lead->Divide(4,2);
    TCanvas *c_sub = new TCanvas("c_sub", "c_sub", 1400, 1200);
    c_sub->Divide(2,2);   
    TCanvas *c_lead_corr = new TCanvas("c_lead_corr", "c_lead_corr", 1400, 1200);
    c_lead_corr->Divide(3,3);
    TCanvas *c_sub_corr = new TCanvas("c_sub_corr", "c_sub_corr", 1400, 1200);
    c_sub_corr->Divide(2,2);   
    TCanvas *c_invar = new TCanvas("c_invar", "c_invar", 1200, 800);
    TCanvas *c_invar_corr = new TCanvas("c_invar_corr", "c_invar_corr", 1200, 800);

    // Prepare Landau Gaussian fit function
    TF1 *langaus = new TF1("langaus", landauGaussian, 0, 10, 4);
    langaus->SetParNames("Landau width","Peak value","Normalization","Gaussian width");
    //gStyle->SetOptStat(0);
    gStyle->SetOptFit(1);
    gStyle->SetOptTitle(0);
    
    // Loop over projections, draw and save
    for (int i = 0; i < leadingBins.size(); i++) {
        // Uncorrected histos for leading photon
        c_lead->cd(i+1);
        TString histoName = Form("h%i_leading", i+1);
        TString subhistoName = Form("h%i_subleading", i+1);
        TH1D *h = (TH1D*)fin->Get(histoName);
        h->SetMarkerColor(kAzure+2);
        h->SetLineColor(kAzure+2);
        TAxis *a = h->GetXaxis();
        Int_t numdiv = leadingBins.at(i).second - leadingBins.at(i).first;
        a->SetNdivisions(numdiv);
        a->SetTitleOffset(1.1);
        a->SetTitle("Reco. Leading Photon Energy [MeV]");
        h->Fit(langaus, "", "" , 0, 0.5);
        h->Draw();
        if (i < subleadingBins.size() ) {
            c_sub->cd(i+1);
            TH1D *h2 = (TH1D*)fin->Get(subhistoName);
            h2->SetMarkerColor(kAzure+2);
            h2->SetLineColor(kAzure+2);
            TAxis *b = h2->GetXaxis();
            Int_t numdiv2 = subleadingBins.at(i).second - subleadingBins.at(i).first;
            b->SetNdivisions(numdiv2);
            b->SetTitleOffset(1.1);
            b->SetTitle("Reco. Subleading Photon Energy [MeV]");
            b->SetRangeUser(0, 0.3);
            h2->Draw();
        }
    }
    c_lead->SaveAs("projections_leading.png", "PNG");
    c_sub->SaveAs("projections_subleading.png", "PNG");

    // Get invariant mass histogram (created in prepare_histos.cc)
    c_invar->cd();
    // Draw verticle line at 135 MeV
    TLine *l = new TLine(0.135, 1e-8, 0.135, 175);
    l->SetLineColor(kRed+1);
    l->SetLineWidth(2);

    TH1D *h_invar = (TH1D*)fin->Get("h_invar");
    h_invar->GetXaxis()->SetRangeUser(0, 0.5);
    h_invar->SetLineColor(kAzure+2);
    h_invar->SetFillStyle(1001);
    h_invar->SetFillColor(kAzure+2);
    h_invar->Draw();
    l->Draw("same");

    c_invar->SaveAs("invariant_mass.png", "png");
    
    c_invar_corr->cd();
    TH1D *h_invar_corr = (TH1D*)fin->Get("h_invar_corr");
    h_invar_corr->SetStats(1);
    h_invar_corr->GetXaxis()->SetRangeUser(0, 0.5);
    h_invar_corr->SetLineColor(kAzure+2);
    h_invar_corr->SetFillStyle(1001);
    h_invar_corr->SetFillColor(kAzure+2);
    h_invar_corr->Draw();
    l->Draw("same");

    c_invar_corr->SaveAs("invariant_mass_corr.png", "png");
}

int main() {
    draw_1DHistos();
    return 0;
}
