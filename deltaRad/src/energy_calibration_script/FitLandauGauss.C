/**
 *  * example script for how to fit a Landau Gaussian distribution
 *   */ 

#include "energy_cal.h"
#include "LandauGaussian.h"

void FitLandauGauss()
{
    TFile *fin = new TFile("output_energy_calibration.root", "READ");
    if (!fin) {
        cout << "Bad input file" << endl;
        return;
    }

    TCanvas *c1 = new TCanvas();
    TH1D *h = (TH1D*)fin->Get("h1_leading");

    TF1 *langaus = new TF1("langaus", landauGaussian, 0, 10, 4);
    langaus->SetParNames("Landau width","Peak value","Normalization","Gaussian width");

    gStyle->SetOptStat(0);
    gStyle->SetOptFit(1);
    gROOT->ForceStyle();

    langaus->SetParameters(0.004, .055, 87, 0.005);
    // Uncomment for data fit
    h->Fit(langaus,"", "", 0.04, 0.09);
    h->Draw();

    c1->SaveAs("langausFit.png", "PNG");
}
