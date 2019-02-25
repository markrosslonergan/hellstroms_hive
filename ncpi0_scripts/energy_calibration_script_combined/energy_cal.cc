#include "energy_cal.h"
using std::cout;
using std::endl;
    
void doCalibration() {
    TFile *fin = new TFile("output_energy_histos.root", "READ");
    TFile *fout = new TFile("output_energy_calibration.root", "RECREATE");
    if (!fin) {
        cout << "Bad file" << endl;
        return;
    }
    TH2D *h_cal = (TH2D*)fin->Get("h_cal");
    TH2D *h_cal_corr = (TH2D*)fin->Get("h_cal_corr");
    TH2D *h_cal_lg = (TH2D*)h_cal->Clone("h_cal");
    TH1D *h_invar = (TH1D*)fin->Get("h_invar");
    TH1D *h_invar_corr = (TH1D*)fin->Get("h_invar_corr");

    // Get binning for projections, i.e., "slices" of 2D histos; can change
    // this in the header file
    std::vector< std::pair<int,int> > bins = projBins();

    // Prepare vectors for projections from 2D histograms
    std::vector<TH1D*> h_cal_projections(bins.size() );
    std::vector<TH1D*> h_cal_projections_corr(bins.size() );

    // Prepare arrays for x- and y-values, for graphing
    double xvals[bins.size()], yvals[bins.size()];
    double xvals_corr[bins.size()], yvals_corr[bins.size()];

    // x-errors are just one-half slice width; should be the same for 
    // uncorrected and corrected histograms
    double xerrs[bins.size()], xerrs_corr[bins.size()];
    double yerrs[bins.size()], yerrs_corr[bins.size()];

    // For now, just fill y-vals with some small, nonzero value; will change later
    std::fill_n(yerrs, bins.size(), 1e-7); 
    std::fill_n(yerrs_corr, bins.size(), 1e-7); 

    // Loop over bin ranges, get projections, fill x- and y-vals/errs accordingly
    for(int i = 0; i < bins.size(); i++) {
        // Projections for leading photon
        TString projName = Form("h%i", i+1);
        TString projName_corr = Form("h%i_corr", i+1);
        h_cal_projections.at(i) = (TH1D*)h_cal->ProjectionY(projName, 
                                                bins.at(i).first, 
                                                bins.at(i).second );

        h_cal_projections_corr.at(i) = (TH1D*)h_cal_corr->ProjectionY(projName_corr,
                                                          bins.at(i).first, 
                                                          bins.at(i).second );

        //cout << "Entries in slice " << i << ": " << h_cal_projections.at(i)->GetEntries() << endl;
        int maxBin = h_cal_projections.at(i)->GetMaximumBin();
        int maxBin_corr = h_cal_projections_corr.at(i)->GetMaximumBin();
        double binToMev = 0.01; // 0.01 MeV per bin, since bins are in GeV 

        // Fill xvals with center of true energy slices; x-errors are just
        // one-half slice width 
        xvals[i] = (bins.at(i).first + (bins.at(i).second - bins.at(i).first)/2.)*binToMev;
        //xvals_corr[i] = (bins.at(i).first + (bins.at(i).second - bins.at(i).first)/2.)*binToMev;
        xerrs[i] = (bins.at(i).second - bins.at(i).first)/2.*binToMev;

        // Fill yvals with center of reconstructed energy 
        yvals[i] = h_cal_projections.at(i)->GetXaxis()->GetBinCenter(maxBin);
        yvals_corr[i] = h_cal_projections_corr.at(i)->GetXaxis()->GetBinCenter(maxBin_corr);
    }
    
    // Now read in peak values from Landau-Gaussian for a (hopefully) better fit
    std::ifstream in;
    in.open("peakvals.dat");
    double yvals_lg[bins.size()] = {0.};
    double peakVal;
    int n = 0;
    while(1) {
        in >> peakVal;
        if (!in.good()) break;
        yvals_lg[n] = peakVal;
        n++;
    }
     
    /////////////////////////////////////////////////////////
    ////////////// DRAW AND SAVE HISTOS ////////////////////
    ////////////////////////////////////////////////////////
    TCanvas *c1 = new TCanvas("c1", "c1", 1000, 700);
    c1->cd();
    gStyle->SetOptStat(0);

    // Drawing options
    // Set minimum < 0 so that 0 bins aren't white (which looks bad)
    h_cal->SetMinimum(-1e-7);
    h_cal->GetXaxis()->SetTitle("True Photon Energy [GeV]");
    h_cal->GetYaxis()->SetTitle("Reconstructed Photon Energy [GeV]");
    h_cal->SetTitle("");
    h_cal->Draw("colz");

    // y = x line, for comparison
    TF1 *f1 = new TF1("f1", "x", 0, 0.5);
    f1->SetLineColor(kBlack);
    f1->Draw("same");

    // Fit line
    TF1 *fit = new TF1("fit", "pol1", 0.04, h_cal->GetNbinsX() );
    fit->SetLineColor(kPink+9);

    TGraphErrors *g = new TGraphErrors(bins.size(), xvals, yvals, xerrs, yerrs);
    g->SetMarkerStyle(23);
    g->SetMarkerSize(1.5);
    g->SetLineColor(kPink+9);
    g->SetMarkerColor(kPink+9);
    g->Fit("fit", "R");
    g->Draw("ep same");

    cout << "Correction equation is " << 1/fit->GetParameter(1) << "*E_reco + "
         << fit->GetParameter(0)/fit->GetParameter(1) << endl;

    std::ofstream of;
    of.open("fitParams.dat");
    of << fit->GetParameter(0) << endl;
    of << fit->GetParameter(1) << endl;

    c1->SaveAs("plot_finalCal.png", "PNG");

    TCanvas *c_corr = new TCanvas("c_corr", "c_corr", 1000, 700);
    c_corr->cd();
    gStyle->SetOptStat(0);

    // Drawing options
    // Set minimum < 0 so that 0 bins aren't white (which looks bad)
    h_cal_corr->SetMinimum(-1e-7);
    h_cal_corr->GetXaxis()->SetTitle("True Leading Photon Energy [GeV]");
    h_cal_corr->GetYaxis()->SetTitle("Corrected Reco. Leading Photon Energy [GeV]");
    h_cal_corr->SetTitle("");
    h_cal_corr->Draw("colz");

    f1->Draw("same");

    // Fit line
    TF1 *fit_corr = new TF1("fit_corr", "pol1", 0.05, h_cal_corr->GetNbinsX() );
    fit_corr->SetLineColor(kWhite);
    fit_corr->SetLineStyle(8);

    TGraphErrors *g_corr = new TGraphErrors(bins.size(), xvals, yvals_corr, xerrs, yerrs_corr);
    g_corr->SetMarkerStyle(22);
    g_corr->SetMarkerSize(1.5);
    g_corr->SetLineColor(kWhite);
    g_corr->SetMarkerColor(kWhite);
    g_corr->Fit("fit_corr", "R");
    g_corr->Draw("ep same");

    c_corr->SaveAs("plot_finalCal_corr.png", "PNG");

    // Couldn't get invariant mass histos to write properly. So, here's
    // the explicit command which works for some reason
    h_invar->Write();
    h_invar_corr->Write();

    fout->Write();
    fout->Close();
}

int main() {
    doCalibration();

    return 0;
}

