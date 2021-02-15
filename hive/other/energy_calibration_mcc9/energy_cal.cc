#include "energy_cal.h"
#include "LandauGaussian.h"
using std::cout;
using std::endl;
    
void doCalibration() {
    TString tag = "all";
    TFile *fin = new TFile("output_energy_histos_"+tag+".root", "READ");
    TFile *fout = new TFile("output_energy_correction_"+tag+".root", "RECREATE");
    if (!fin) {
        cout << "Bad file" << endl;
        return;
    }
    TH2D *h_max = (TH2D*)fin->Get("h_max");
    TH2D *h_max_corr = (TH2D*)fin->Get("h_max_corr");

    // Get binning for projections, i.e., "slices" of 2D histos; can change
    // this in the header file
    //std::vector< std::pair<int,int> > fitSlices = leadingProjBins();
    std::vector< std::pair<int, int> > fitSlices = leadingProjBins();

    // Get projections from 2D histograms
    std::vector<TH1D*> h_max_projections(fitSlices.size() );
    std::vector<TH1D*> h_max_projections_corr(fitSlices.size() );
    
    // Prepare arrays for x- and y-values, for graphing
    double xvals[fitSlices.size()], yvals[fitSlices.size()];
    double xvals_corr[fitSlices.size()], yvals_corr[fitSlices.size()];
    
    // x-errors are just one-half slice width; should be the same for 
    // uncorrected and corrected histograms
    double xerrs[fitSlices.size()];
    double yerrs[fitSlices.size()];
    double yerrs_corr[fitSlices.size()];

    double MeVPerBin = (h_max->GetXaxis()->GetBinUpEdge(h_max->GetNbinsX()) - h_max->GetXaxis()->GetBinLowEdge(1) )/h_max->GetNbinsX(); // To make units work out
    
    // For now, just fill y-vals with some small, nonzero value; will change later
    //std::fill_n(yerrs, fitSlices.size(), 1e-7); 
    //std::fill_n(yerrs_corr, fitSlices.size(), 1e-7); 
    // Fill y-errors with one-half bin width. Assuming same number of bins in
    // x and y, should just be MeVPerBin/2
    std::fill_n(yerrs, fitSlices.size(), MeVPerBin/2.); 
    std::fill_n(yerrs_corr, fitSlices.size(), MeVPerBin/2.); 


    // Loop over bin ranges, get projections, fill x- and y-vals/errs accordingly
    for(int i = 0; i < fitSlices.size(); i++) {
        //cout << "Slice range: " << fitSlices.at(i).first << " to " << fitSlices.at(i).second << endl;
        // Projections for photon energies in slice range
        TString projName = Form("h%i", i+1);
        TString projName_corr = Form("h%i_corr", i+1);
        h_max_projections.at(i) = (TH1D*)h_max->ProjectionY(projName, 
                                                fitSlices.at(i).first/ MeVPerBin, 
                                                fitSlices.at(i).second/MeVPerBin );
        std::cout << "Getting projection from bin " << fitSlices.at(i).first << 
          " to " << fitSlices.at(i).second << std::endl;

        h_max_projections_corr.at(i) = (TH1D*)h_max_corr->ProjectionY(projName_corr,
                                                          fitSlices.at(i).first, 
                                                          fitSlices.at(i).second );

        std::cout << "Entries in this slice: " << h_max_projections.at(i)->GetEntries() << std::endl;

        int maxBin = h_max_projections.at(i)->GetMaximumBin();
        std::cout << "maxBin = " << maxBin << std::endl;
        int maxBin_corr = h_max_projections_corr.at(i)->GetMaximumBin();

        // Fill xvals with center of true energy slices; x-errors are just
        // one-half slice width 
        xvals[i] = (fitSlices.at(i).first/MeVPerBin + (fitSlices.at(i).second - fitSlices.at(i).first)/(MeVPerBin*2.) )*MeVPerBin;
        xvals_corr[i] = (fitSlices.at(i).first + (fitSlices.at(i).second - fitSlices.at(i).first)/2.)*MeVPerBin;
        //xerrs[i] = (fitSlices.at(i).second - fitSlices.at(i).first)/2.*MeVPerBin;
        xerrs[i] = (fitSlices.at(i).second - fitSlices.at(i).first)/2.;
        cout << "xval: " << xvals[i] << endl;
        //xvals_corr[i] = (fitSlices.at(i).first + (fitSlices.at(i).second - fitSlices.at(i).first)/2.);
        //xerrs[i] = (fitSlices.at(i).second - fitSlices.at(i).first)/2.;
        yvals[i] = h_max_projections.at(i)->GetXaxis()->GetBinCenter(maxBin);
        std::cout << "yval: " << yvals[i] << std::endl;
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
    c1->SetLeftMargin(0.12);
    c1->SetBottomMargin(0.12);
    gStyle->SetOptStat(0);

    // Drawing options
    // Set minimum < 0 so that 0 bins aren't white (which looks bad)
    h_max->SetMinimum(-1e-7);
    h_max->GetXaxis()->SetTitle("True Shower Energy [MeV]");
    h_max->GetYaxis()->SetTitle("Reconstructed Shower Energy [MeV]");
    h_max->GetXaxis()->SetTitleSize(0.05);
    h_max->GetYaxis()->SetTitleSize(0.05);
    h_max->SetTitle("");
    h_max->SetContour(1000);
    h_max->Draw("colz");

    // y = x line, for comparison
    TF1 *f1 = new TF1("f1", "x", 0, 500);
    f1->SetLineColor(kBlack);
    f1->Draw("same");

    // Fit line
    //TF1 *fit = new TF1("fit", "pol1", 30, 500 );
    TF1 *fit = new TF1("fit", "pol1", 20, 500 );
    //TF1 *fit = new TF1("fit", "pol1", 400, h_max->GetNbinsX() );
    fit->SetLineColor(kPink+9);

    TGraphErrors *g = new TGraphErrors(fitSlices.size(), xvals, yvals, xerrs, yerrs);
    g->SetMarkerStyle(23);
    g->SetMarkerSize(1.2);
    g->SetLineColor(kPink+9);
    g->SetMarkerColor(kPink+9);
    g->Fit("fit", "R");
    g->Draw("ep same");

    TPaveText *pt = new TPaveText(0.14, 0.68, 0.4, 0.85, "NDC");
    double textSize = 0.04;
    pt->SetTextAlign(12);
    pt->SetFillStyle(0);
    pt->SetTextSize(textSize);
    pt->SetBorderSize(0);
    pt->SetTextColor(kWhite);
    TString simString = Form("MicroBooNE Simulation");
    TString chi2_text = Form("#chi^{2}/NDF: %0.2f/%i", fit->GetChisquare(), fit->GetNDF() );
    pt->AddText(simString);
    if (tag=="run1")          pt->AddText("Run 1");
    else if (tag=="run3")     pt->AddText("Run 3");
    else if (tag=="all")      pt->AddText("Runs 1+3");
    else if (tag=="finalSel") pt->AddText("NC #pi^{0} Final Selection");
    pt->AddText(chi2_text);
    pt->Draw("same");

    double fit_equn_par1_err = (1/(fit->GetParameter(1)-fit->GetParError(1) ) - 1/(fit->GetParameter(1)+fit->GetParError(1) ) )/2.;
    double fit_equn_par0_err = ( (fit->GetParameter(0)-fit->GetParError(0))/(fit->GetParameter(1)-fit->GetParError(1)) - (fit->GetParameter(0)+fit->GetParError(0))/(fit->GetParameter(1)+fit->GetParError(1)) )/2.;
    cout << "Correction equation is (" << 1/fit->GetParameter(1) << " +/- " << fit_equn_par1_err << ")*E_reco - ("
         << fit->GetParameter(0)/fit->GetParameter(1) << " +/- " << fit_equn_par0_err << ") MeV" << endl;
    cout << "Uncorrected fit chi^2 is " << fit->GetChisquare() << " / " << fit->GetNDF() << " NDF" << endl;

    c1->SaveAs("plot_max_"+tag+".png", "PNG");
    c1->SaveAs("plot_max_"+tag+".pdf", "PDF");

    TCanvas *c2 = new TCanvas("c2", "c2", 1000, 700);
    c2->cd();
    c2->SetLeftMargin(0.12);
    c2->SetBottomMargin(0.12);
    gStyle->SetOptStat(0);

    // Drawing options
    // Set minimum < 0 so that 0 bins aren't white (which looks bad)
    h_max_corr->SetMinimum(-1e-7);
    h_max_corr->GetXaxis()->SetTitle("True Shower Energy [GeV]");
    h_max_corr->GetYaxis()->SetTitle("Corrected Shower Energy [GeV]");
    h_max_corr->GetXaxis()->SetTitleSize(0.05);
    h_max_corr->GetYaxis()->SetTitleSize(0.05);
    h_max_corr->SetTitle("");
    h_max_corr->SetContour(1000);
    h_max_corr->Draw("colz");

    f1->Draw("same");

    // Fit line
    //TF1 *fit_corr = new TF1("fit_corr", "pol1", 400, h_max_corr->GetNbinsX() );
    TF1 *fit_corr = new TF1("fit_corr", "pol1", 20, 400 );
    fit_corr->SetLineColor(kWhite);
    fit_corr->SetLineStyle(8);

    TGraphErrors *g_corr = new TGraphErrors(fitSlices.size(), xvals, yvals_corr, xerrs, yerrs_corr);
    g_corr->SetMarkerStyle(22);
    g_corr->SetMarkerSize(1.2);
    g_corr->SetLineColor(kWhite);
    g_corr->SetMarkerColor(kWhite);
    g_corr->Fit("fit_corr", "R");
    g_corr->Draw("ep same");

    TPaveText *pt_corr = new TPaveText(0.14, 0.68, 0.4, 0.85, "NDC");
    pt_corr->SetTextAlign(12);
    pt_corr->SetFillStyle(0);
    pt_corr->SetTextSize(textSize);
    pt_corr->SetBorderSize(0);
    pt_corr->SetTextColor(kWhite);
    TString simString_corr = Form("MicroBooNE Simulation");
    TString chi2_text_corr = Form("#chi^{2}/NDF: %0.2f/%i", fit_corr->GetChisquare(), fit_corr->GetNDF() );
    pt_corr->AddText(simString_corr);
    if (tag=="run1") pt_corr->AddText("Run 1");
    else if (tag=="run3") pt_corr->AddText("Run 3");
    pt_corr->AddText(chi2_text_corr);
    pt_corr->Draw("same");

    cout << "Corrected fit chi^2 is " << fit_corr->GetChisquare() << " / " << fit_corr->GetNDF() << " NDF" << endl;

    c2->SaveAs("plot_max_corr_"+tag+".png", "PNG");
    c2->SaveAs("plot_max_corr_"+tag+".pdf", "PDF");
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

