#include "energy_cal.h"
#include "LandauGaussian.h"
using std::cout;
using std::endl;
std::pair<double,double> getFitRange(TH1D*);

void langausFit() {

    TFile *fin = new TFile("output_energy_calibration.root", "READ");
    if (!fin) {
        cout << "Bad input file" << endl;
        return;
    }

    TF1 *langaus = new TF1("langaus", landauGaussian, 0, 0.5, 4);
    langaus->SetParNames("Landau width","Peak value","Normalization","Gaussian width");

    TCanvas *c_lead = new TCanvas("c_lead", "c_lead", 1900, 1000);
    c_lead->Divide(4,2);
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(1);
    gROOT->ForceStyle();
    TString histName;
    std::pair<double,double> fitRange;
    TFile *fout = new TFile("output_langaus_fits.root", "RECREATE");

    for (int i = 0; i < 7; i++) {
        c_lead->cd(i+1);
        histName = Form("h%i_leading", i+1);
        TH1D *h = (TH1D*)fin->Get(histName);
        if (!h) {
            cout << "Bad histogram" << endl;
            return;
        }
        fitRange = getFitRange(h);
        cout << "Fit from " << fitRange.first << " to " << fitRange.second << endl;
        int maxBin = h->GetMaximumBin();
        double peakVal = h->GetXaxis()->GetBinCenter(maxBin);
        int nentries = h->GetEntries();

        //langaus->SetParameters(0.004, peakVal, nentries, 0.005);
        double width = h->GetStdDev();
        langaus->SetParameters(0.004, peakVal, 0.8*nentries, 0.005);
        h->Fit(langaus,"q", "", fitRange.first, fitRange.second);
        h->GetXaxis()->SetTitleOffset(1.2);
        h->Draw();

    }
    c_lead->SaveAs("langausfits.png", "png");
}

int main() {
    langausFit();
    return 0;
}

std::pair<double, double> getFitRange(TH1D *h1) {
    int maxBin = h1->GetMaximumBin();
    double peakVal = h1->GetXaxis()->GetBinCenter(maxBin);
    cout << "Peak val = " << peakVal << endl;
    std::pair<double, double> range = std::make_pair(peakVal*0.5, peakVal*1.5);

    return range;
}



