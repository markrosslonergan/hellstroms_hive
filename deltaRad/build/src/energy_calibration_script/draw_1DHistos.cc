#include "energy_cal.h"
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
    std::vector< std::pair<int,int> > subleadingBins = subleadingProjBins();
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
        // Convert axis labels to actual MeV values in each slice
        /*
        for (int n = 0; n < numdiv; n++) {
            double firstBinVal = ((i+1)*leadingBins.at(0).first + a->GetBinLowEdge(1))*10;
            double currentBinVal = i*leadingBins.at(0).first + a->GetBinCenter(n);
            cout << "firstBinVal = " << firstBinVal << endl;
            cout << "currentBinVal = " << currentBinVal << endl;
            cout << "Bin " << n << " low edge: " << currentBinVal << endl;
            TString mevLabel = Form("%i", a->GetBinLowEdge(n) );
            //cout << "Setting bin " << n << " in slice " << i << " to " << mevLabel << endl;
            a->ChangeLabel(n, -1, -1, -1, -1, -1, mevLabel);
        }
        cout << "-----------------------------------" << endl;
        */
        a->SetTitleOffset(1.1);
        a->SetTitle("Reco. Leading Photon Energy [MeV]");
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
}

int main() {
    draw_1DHistos();
    return 0;
}
