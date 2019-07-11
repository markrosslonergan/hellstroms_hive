#include "energy_cal.h"
using std::cout;
using std::endl;
    
void doCalibration() {
    TFile *fin = new TFile("output_energy_histos.root", "READ");
    TFile *fout = new TFile("output_energy_correction.root", "RECREATE");
    if (!fin) {
        cout << "Bad file" << endl;
        return;
    }
    TH2D *h_max = (TH2D*)fin->Get("h_max");
    TH2D *h_max_corr = (TH2D*)fin->Get("h_max_corr");

    // Get binning for projections, i.e., "slices" of 2D histos; can change
    // this in the header file
    std::vector< std::pair<int,int> > fitSlices = leadingProjBins();

    // Get projections from 2D histograms
    std::vector<TH1D*> h_max_projections(fitSlices.size() );
    std::vector<TH1D*> h_max_projections_corr(fitSlices.size() );
    cout << "Fit slices size: " << fitSlices.size() << endl;
    
    // Prepare arrays for x- and y-values, for graphing
    double xvals[fitSlices.size()], yvals[fitSlices.size()];
    double xvals_corr[fitSlices.size()], yvals_corr[fitSlices.size()];
    
    // x-errors are just one-half slice width; should be the same for 
    // uncorrected and corrected histograms
    double xerrs[fitSlices.size()];
    double yerrs[fitSlices.size()];
    double yerrs_corr[fitSlices.size()];

    // For now, just fill y-vals with some small, nonzero value; will change later
    std::fill_n(yerrs, fitSlices.size(), 1e-7); 
    std::fill_n(yerrs_corr, fitSlices.size(), 1e-7); 

    // Loop over bin ranges, get projections, fill x- and y-vals/errs accordingly
    for(int i = 0; i < fitSlices.size(); i++) {
        cout << "Slice range: " << fitSlices.at(i).first << " to " << fitSlices.at(i).second << endl;
        // Projections for leading photon
        TString projName = Form("h%i", i+1);
        TString projName_corr = Form("h%i_corr", i+1);
        h_max_projections.at(i) = (TH1D*)h_max->ProjectionY(projName, 
                                                fitSlices.at(i).first, 
                                                fitSlices.at(i).second );

        cout << "Entries in projection " << i << ": " << h_max_projections.at(i)->GetEntries() << endl;

        h_max_projections_corr.at(i) = (TH1D*)h_max_corr->ProjectionY(projName_corr,
                                                          fitSlices.at(i).first, 
                                                          fitSlices.at(i).second );

        int maxBin = h_max_projections.at(i)->GetMaximumBin();
        cout << "maxBin = " << maxBin << endl;
        int maxBin_corr = h_max_projections_corr.at(i)->GetMaximumBin();

        // Fill xvals with center of true energy slices; x-errors are just
        // one-half slice width 
        double binConv = 10; // To make units work out
        xvals[i] = (fitSlices.at(i).first + (fitSlices.at(i).second - fitSlices.at(i).first)/2.)*binConv;
        xvals_corr[i] = (fitSlices.at(i).first + (fitSlices.at(i).second - fitSlices.at(i).first)/2.)*binConv;
        xerrs[i] = (fitSlices.at(i).second - fitSlices.at(i).first)/2.*binConv;
        //xvals[i] = (fitSlices.at(i).first + (fitSlices.at(i).second - fitSlices.at(i).first)/2.);
        //cout << "xval: " << xvals[i] << endl;
        //xvals_corr[i] = (fitSlices.at(i).first + (fitSlices.at(i).second - fitSlices.at(i).first)/2.);
        //xerrs[i] = (fitSlices.at(i).second - fitSlices.at(i).first)/2.;
        yvals[i] = h_max_projections.at(i)->GetXaxis()->GetBinCenter(maxBin);
        cout << "yval: " << yvals[i] << endl;
        yvals_corr[i] = h_max_projections_corr.at(i)->GetXaxis()->GetBinCenter(maxBin_corr);
    }
    
    // Now read in peak values from Landau-Gaussian fit for a (hopefully) better fit
    /*
    std::ifstream in;
    in.open("peakvals.dat");
    double yvals4[7] = {0.};
    double peakVal;
    int n = 0;
    while(1) {
        in >> peakVal;
        if (!in.good()) break;
        yvals4[n] = peakVal;
        n++;
    }
    */

    /////////////////////////////////////////////////////////
    ////////////// DRAW AND SAVE HISTOS ////////////////////
    ////////////////////////////////////////////////////////
    TCanvas *c1 = new TCanvas("c1", "c1", 1000, 700);
    c1->cd();
    gStyle->SetOptStat(0);

    // Drawing options
    // Set minimum < 0 so that 0 bins aren't white (which looks bad)
    h_max->SetMinimum(-1e-7);
    h_max->GetXaxis()->SetTitle("True Shower Energy [MeV]");
    h_max->GetYaxis()->SetTitle("Reconstructed Shower Energy [MeV]");
    h_max->SetTitle("");
    h_max->Draw("colz");

    // y = x line, for comparison
    TF1 *f1 = new TF1("f1", "x", 0, 500);
    f1->SetLineColor(kBlack);
    f1->Draw("same");

    // Fit line
    TF1 *fit = new TF1("fit", "pol1", 500, h_max->GetNbinsX() );
    fit->SetLineColor(kPink+9);

    TGraphErrors *g = new TGraphErrors(fitSlices.size(), xvals, yvals, xerrs, yerrs);
    g->SetMarkerStyle(23);
    g->SetMarkerSize(1.5);
    g->SetLineColor(kPink+9);
    g->SetMarkerColor(kPink+9);
    g->Fit("fit", "R");
    g->Draw("ep same");

    cout << "Correction equation is " << 1/fit->GetParameter(1) << "*E_reco - "
         << fit->GetParameter(0)/fit->GetParameter(1) << endl;

    c1->SaveAs("plot_max.png", "PNG");

    TCanvas *c2 = new TCanvas("c2", "c2", 1000, 700);
    c2->cd();
    gStyle->SetOptStat(0);

    // Drawing options
    // Set minimum < 0 so that 0 bins aren't white (which looks bad)
    h_max_corr->SetMinimum(-1e-7);
    h_max_corr->GetXaxis()->SetTitle("True Leading Photon Energy [GeV]");
    h_max_corr->GetYaxis()->SetTitle("Corrected Reco. Leading Photon Energy [GeV]");
    h_max_corr->SetTitle("");
    h_max_corr->Draw("colz");

    f1->Draw("same");

    // Fit line
    TF1 *fit_corr = new TF1("fit_corr", "pol1", 500, h_max_corr->GetNbinsX() );
    fit_corr->SetLineColor(kWhite);
    fit_corr->SetLineStyle(8);

    TGraphErrors *g_corr = new TGraphErrors(fitSlices.size(), xvals, yvals_corr, xerrs, yerrs_corr);
    g_corr->SetMarkerStyle(22);
    g_corr->SetMarkerSize(1.5);
    g_corr->SetLineColor(kWhite);
    g_corr->SetMarkerColor(kWhite);
    g_corr->Fit("fit_corr", "R");
    g_corr->Draw("ep same");

    c2->SaveAs("plot_max_corr.png", "PNG");
    /*

    TCanvas *c3 = new TCanvas("c3", "c3", 1000, 700);
    c3->cd();
    gStyle->SetOptStat(0);

    // Drawing options
    // Set minimum < 0 so that 0 bins aren't white (which looks bad)
    h_max2->SetMinimum(-1e-7);
    h_max2->GetXaxis()->SetTitle("True Subleading Photon Energy [GeV]");
    h_max2->GetYaxis()->SetTitle("Reconstructed Subleading Photon Energy [GeV]");
    h_max2->SetTitle("");
    h_max2->Draw("colz");

    // Fit line
    TF1 *fit2 = new TF1("fit2", "pol1", 0.05, h_max2->GetNbinsX() );
    fit_corr->SetLineColor(kPink+9);
    fit_corr->SetLineStyle(8);

    TGraphErrors *g2 = new TGraphErrors(subfitSlices.size(), xvals2, yvals2, xerrs2, yerrs2);
    g2->SetMarkerStyle(22);
    g2->SetMarkerSize(1.5);
    g2->SetLineColor(kPink+9);
    g2->SetMarkerColor(kPink+9);
    g2->Fit("fit2", "R");
    g2->Draw("ep same");

    cout << "Correction equation is " << 1/fit2->GetParameter(1) << "*E_reco - "
         << fit2->GetParameter(0)/fit2->GetParameter(1) << endl;

    f1->Draw("same");

    c3->SaveAs("plot_finalCal_subleading.png", "PNG");

    TCanvas *c4 = new TCanvas("c4", "c4", 1000, 700);
    c4->cd();
    gStyle->SetOptStat(0);

    h_max2_corr->SetMinimum(-1e-7);
    h_max2_corr->GetXaxis()->SetTitle("True Subleading Photon Energy [GeV]");
    h_max2_corr->GetYaxis()->SetTitle("Corrected Reco. Subleading Photon Energy [GeV]");
    h_max2_corr->SetTitle("");
    h_max2_corr->Draw("colz");

    // Fit line
    TF1 *fit2_corr = new TF1("fit2_corr", "pol1", 0.03, h_max2_corr->GetNbinsX() );
    fit2_corr->SetLineColor(kWhite);
    fit2_corr->SetLineStyle(8);

    TGraphErrors *g2_corr = new TGraphErrors(subfitSlices.size(), xvals2, yvals2_corr, xerrs2, yerrs2);
    g2_corr->SetMarkerStyle(22);
    g2_corr->SetMarkerSize(1.5);
    g2_corr->SetLineColor(kWhite);
    g2_corr->SetMarkerColor(kWhite);
    g2_corr->Fit("fit2_corr", "R");
    g2_corr->Draw("ep same");

    f1->Draw("same");

    c4->SaveAs("plot_finalCal_subleading_corr.png", "PNG");

    // Couldn't get invariant mass histos to write properly. So, here's
    // the explicit command which works for some reason
    h_invar->Write();
    h_invar_corr->Write();

    TCanvas *c5 = new TCanvas("c5", "c5", 1000, 700);
    c5->cd();
    gStyle->SetOptStat(0);

    TCanvas *c6 = new TCanvas("c6", "c6", 1000, 700);
    c6->cd();
    gStyle->SetOptStat(0);

    // Drawing options
    // Set minimum < 0 so that 0 bins aren't white (which looks bad)
    h_max4->SetMinimum(-1e-7);
    h_max4->GetXaxis()->SetTitle("True Leading Photon Energy [GeV]");
    h_max4->GetYaxis()->SetTitle("Reconstructed Leading Photon Energy [GeV]");
    h_max4->SetTitle("");
    h_max4->Draw("colz");

    // Fit line
    TF1 *fit4 = new TF1("fit4", "pol1", 0.05, h_max->GetNbinsX() );
    fit4->SetLineColor(kPink+9);

    TGraphErrors *g4 = new TGraphErrors(fitSlices.size(), xvals, yvals4, xerrs, yerrs);
    g4->SetMarkerStyle(23);
    g4->SetMarkerSize(1.5);
    g4->SetLineColor(kPink+9);
    g4->SetMarkerColor(kPink+9);
    g4->Fit("fit", "R");
    g4->Draw("ep same");

    f1->Draw("same");

    c6->SaveAs("plot_finalCal_leading_langaus.png", "PNG");

    TCanvas *c7 = new TCanvas("c7", "c7", 1000, 700);
    c7->cd();
    */

    fout->Write();
    fout->Close();
}

int main() {
    doCalibration();

    return 0;
}

