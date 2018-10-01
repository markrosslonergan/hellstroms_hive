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
    TH2D *h_cal2 = (TH2D*)fin->Get("h_cal2");
    TH2D *h_cal_corr = (TH2D*)fin->Get("h_cal_corr");
    TH2D *h_cal2_corr = (TH2D*)fin->Get("h_cal2_corr");
    TH2D *h_cal3 = (TH2D*)fin->Get("h_cal3");
    TH1D *h_invar = (TH1D*)fin->Get("h_invar");
    TH1D *h_invar_corr = (TH1D*)fin->Get("h_invar_corr");

    // Get binning for projections, i.e., "slices" of 2D histos; can change
    // this in the header file
    std::vector< std::pair<int,int> > leadingBins = leadingProjBins();
    std::vector< std::pair<int,int> > subleadingBins = subleadingProjBins();
    // Get projections from 2D histograms
    std::vector<TH1D*> h_cal_projections(leadingBins.size() );
    std::vector<TH1D*> h_cal_projections_corr(leadingBins.size() );
    std::vector<TH1D*> h_cal2_projections(subleadingBins.size() );
    std::vector<TH1D*> h_cal2_projections_corr(subleadingBins.size() );
    std::vector<TH1D*> h_cal3_projections(leadingBins.size() );
    // Prepare arrays for x- and y-values, for graphing
    double xvals[leadingBins.size()], yvals[leadingBins.size()];
    double xvals2[subleadingBins.size()], yvals2[subleadingBins.size()];
    double xvals_corr[leadingBins.size()], yvals_corr[leadingBins.size()];
    double xvals2_corr[subleadingBins.size()], yvals2_corr[subleadingBins.size()];
    double xvals3[leadingBins.size()], yvals3[leadingBins.size()];
    // x-errors are just one-half slice width; should be the same for 
    // uncorrected and corrected histograms
    double xerrs[leadingBins.size()], xerrs2[subleadingBins.size()];
    double yerrs[leadingBins.size()], yerrs2[subleadingBins.size()];
    double yerrs_corr[leadingBins.size()], yerrs2_corr[subleadingBins.size()];
    // For now, just fill y-vals with some small, nonzero value; will change later
    std::fill_n(yerrs, leadingBins.size(), 1e-7); 
    std::fill_n(yerrs_corr, leadingBins.size(), 1e-7); 
    std::fill_n(yerrs2, subleadingBins.size(), 1e-7); 
    std::fill_n(yerrs2_corr, subleadingBins.size(), 1e-7); 

    // Loop over bin ranges, get projections, fill x- and y-vals/errs accordingly
    for(int i = 0; i < leadingBins.size(); i++) {
        // Projections for leading photon
        TString projName = Form("h%i_leading", i+1);
        TString projName_corr = Form("h%i_leading_corr", i+1);
        h_cal_projections.at(i) = (TH1D*)h_cal->ProjectionY(projName, 
                                                leadingBins.at(i).first, 
                                                leadingBins.at(i).second );

        h_cal_projections_corr.at(i) = (TH1D*)h_cal_corr->ProjectionY(projName_corr,
                                                          leadingBins.at(i).first, 
                                                          leadingBins.at(i).second );

        int maxBin = h_cal_projections.at(i)->GetMaximumBin();
        int maxBin_corr = h_cal_projections_corr.at(i)->GetMaximumBin();
        double binToMev = 0.01; // 0.01 MeV per bin, since bins are in GeV 
        // Fill xvals with center of true energy slices; x-errors are just
        // one-half slice width 
        xvals[i] = (leadingBins.at(i).first + (leadingBins.at(i).second - leadingBins.at(i).first)/2.)*binToMev;
        xvals_corr[i] = (leadingBins.at(i).first + (leadingBins.at(i).second - leadingBins.at(i).first)/2.)*binToMev;
        xerrs[i] = (leadingBins.at(i).second - leadingBins.at(i).first)/2.*binToMev;
        yvals[i] = h_cal_projections.at(i)->GetXaxis()->GetBinCenter(maxBin);
        yvals_corr[i] = h_cal_projections_corr.at(i)->GetXaxis()->GetBinCenter(maxBin_corr);


        // Same as above, but for subleading photon
        if (i < subleadingBins.size() ) {
            TString projName2 = Form("h%i_subleading", i+1);
            TString projName2_corr = Form("h%i_subleading_corr", i+1);
            h_cal2_projections.at(i) = (TH1D*)h_cal2->ProjectionY(projName2, 
                                                      subleadingBins.at(i).first, 
                                                      subleadingBins.at(i).second );

            h_cal2_projections_corr.at(i) = (TH1D*)h_cal2_corr->ProjectionY(projName2_corr, 
                                                                subleadingBins.at(i).first, 
                                                                subleadingBins.at(i).second );
            int maxBin2 = h_cal2_projections.at(i)->GetMaximumBin();
            int maxBin2_corr = h_cal2_projections_corr.at(i)->GetMaximumBin();
            xvals2[i] = (subleadingBins.at(i).first + (subleadingBins.at(i).second - subleadingBins.at(i).first)/2.)*binToMev;
            xvals2_corr[i] = xvals2[i];
            xerrs2[i] = (subleadingBins.at(i).second - subleadingBins.at(i).first)/2.*binToMev;
            yvals2[i] = h_cal2_projections.at(i)->GetXaxis()->GetBinCenter(maxBin2);
            yvals2_corr[i] = h_cal2_projections_corr.at(i)->GetXaxis()->GetBinCenter(maxBin2_corr);

        }

        // Same as above, for both photons
        TString projName3 = Form("h%i_both", i+1);
        h_cal3_projections.at(i) = (TH1D*)h_cal3->ProjectionY(projName3,
                                                  leadingBins.at(i).first,
                                                  leadingBins.at(i).second );

        int maxBin3 = h_cal3_projections.at(i)->GetMaximumBin();
        //int maxBin_corr = h_cal_projections_corr.at(i)->GetMaximumBin();
        xvals3[i] = (leadingBins.at(i).first + (leadingBins.at(i).second - leadingBins.at(i).first)/2.)*binToMev;
        //xvals_corr[i] = (leadingBins.at(i).first + (leadingBins.at(i).second - leadingBins.at(i).first)/2.)*binToMev;
        //xerrs3[i] = (leadingBins.at(i).second - leadingBins.at(i).first)/2.*binToMev;
        yvals3[i] = h_cal3_projections.at(i)->GetXaxis()->GetBinCenter(maxBin);
        //yvals_corr[i] = h_cal_projections_corr.at(i)->GetXaxis()->GetBinCenter(maxBin_corr);

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
    h_cal->GetXaxis()->SetTitle("True Leading Photon Energy [GeV]");
    h_cal->GetYaxis()->SetTitle("Reconstructed Leading Photon Energy [GeV]");
    h_cal->SetTitle("");
    h_cal->Draw("colz");

    // y = x line, for comparison
    TF1 *f1 = new TF1("f1", "x", 0, 0.5);
    f1->SetLineColor(kBlack);
    f1->Draw("same");

    // Fit line
    TF1 *fit = new TF1("fit", "pol1", 0.05, h_cal->GetNbinsX() );
    fit->SetLineColor(kPink+9);

    TGraphErrors *g = new TGraphErrors(leadingBins.size(), xvals, yvals, xerrs, yerrs);
    g->SetMarkerStyle(23);
    g->SetMarkerSize(1.5);
    g->SetLineColor(kPink+9);
    g->SetMarkerColor(kPink+9);
    g->Fit("fit", "R");
    g->Draw("ep same");

    c1->SaveAs("plot_finalCal_leading.png", "PNG");

    TCanvas *c2 = new TCanvas("c2", "c2", 1000, 700);
    c2->cd();
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

    TGraphErrors *g_corr = new TGraphErrors(leadingBins.size(), xvals, yvals_corr, xerrs, yerrs_corr);
    g_corr->SetMarkerStyle(22);
    g_corr->SetMarkerSize(1.5);
    g_corr->SetLineColor(kWhite);
    g_corr->SetMarkerColor(kWhite);
    g_corr->Fit("fit_corr", "R");
    g_corr->Draw("ep same");

    c2->SaveAs("plot_finalCal_leading_corr.png", "PNG");

    TCanvas *c3 = new TCanvas("c3", "c3", 1000, 700);
    c3->cd();
    gStyle->SetOptStat(0);

    // Drawing options
    // Set minimum < 0 so that 0 bins aren't white (which looks bad)
    h_cal2->SetMinimum(-1e-7);
    h_cal2->GetXaxis()->SetTitle("True Subleading Photon Energy [GeV]");
    h_cal2->GetYaxis()->SetTitle("Reconstructed Subleading Photon Energy [GeV]");
    h_cal2->SetTitle("");
    h_cal2->Draw("colz");

    // Fit line
    TF1 *fit2 = new TF1("fit2", "pol1", 0.05, h_cal2->GetNbinsX() );
    fit_corr->SetLineColor(kPink+9);
    fit_corr->SetLineStyle(8);

    TGraphErrors *g2 = new TGraphErrors(subleadingBins.size(), xvals2, yvals2, xerrs2, yerrs2);
    g2->SetMarkerStyle(22);
    g2->SetMarkerSize(1.5);
    g2->SetLineColor(kPink+9);
    g2->SetMarkerColor(kPink+9);
    g2->Fit("fit2", "R");
    g2->Draw("ep same");

    f1->Draw("same");

    c3->SaveAs("plot_finalCal_subleading.png", "PNG");

    TCanvas *c4 = new TCanvas("c4", "c4", 1000, 700);
    c4->cd();
    gStyle->SetOptStat(0);

    h_cal2_corr->SetMinimum(-1e-7);
    h_cal2_corr->GetXaxis()->SetTitle("True Subleading Photon Energy [GeV]");
    h_cal2_corr->GetYaxis()->SetTitle("Corrected Reco. Subleading Photon Energy [GeV]");
    h_cal2_corr->SetTitle("");
    h_cal2_corr->Draw("colz");

    // Fit line
    TF1 *fit2_corr = new TF1("fit2_corr", "pol1", 0.03, h_cal2_corr->GetNbinsX() );
    fit2_corr->SetLineColor(kWhite);
    fit2_corr->SetLineStyle(8);

    TGraphErrors *g2_corr = new TGraphErrors(subleadingBins.size(), xvals2, yvals2_corr, xerrs2, yerrs2);
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

    // Drawing options
    // Set minimum < 0 so that 0 bins aren't white (which looks bad)
    h_cal3->SetMinimum(-1e-7);
    h_cal3->GetXaxis()->SetTitle("True Energy [GeV]");
    h_cal3->GetYaxis()->SetTitle("Reconstructed Energy [GeV]");
    h_cal3->SetTitle("Both Photons");
    h_cal3->Draw("colz");

    f1->Draw("same");

    // Fit line
    TF1 *fit3 = new TF1("fit3", "pol1", 0.05, h_cal3->GetNbinsX() );
    fit->SetLineColor(kPink+9);

    TGraphErrors *g3 = new TGraphErrors(leadingBins.size(), xvals3, yvals3, xerrs, yerrs);
    g3->SetMarkerStyle(23);
    g3->SetMarkerSize(1.5);
    g3->SetLineColor(kPink+9);
    g3->SetMarkerColor(kPink+9);
    g3->Fit("fit3", "R");
    g3->Draw("ep same");

    c5->SaveAs("plot_finalCal_both.png", "PNG");

    fout->Write();
    fout->Close();
}

int main() {
    doCalibration();

    return 0;
}

