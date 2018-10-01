#include "energy_cal.h"
using std::cout;
using std::endl;
double leadingCorr(double);
double subleadingCorr(double);

void make_2dHisto() {
    /////////////////////////////////////////////////////////
    ////////////// INITIALIZATION /////////////////////
    ////////////////////////////////////////////////////////
    
    //TString dir = "/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_v3/";
    TString dir = "/pnfs/uboone/persistent/users/amogan/singlePhoton/samples/";
    // Get signal file and TTree
    TFile *fin = new TFile(dir+"vertexed_ncpi0_PiMom_v4.1.root", "READ");
    TTree *t = (TTree*)fin->Get("vertex_tree");
    // Friend tree for events that pass full NC pi0 selection
    TFile *friend_file = new TFile("../NCpi0Cosmics_stage_friend.root", "READ");
    TTree *t_friend = (TTree*)friend_file->Get("stage_cuts");
    t->AddFriend(t_friend);

    // Declare necessary tree variables
    std::size_t const N = 4; // Avoid segfaults; not totally sure why
    int most_energetic_shower_index = 0;
    int second_most_energetic_shower_index = 0;
    double reco_shower_helper_energy[N] = {0.};
    double reco_shower_dirx[N] = {0.};
    double reco_shower_diry[N] = {0.};
    double reco_shower_dirz[N] = {0.};
    std::vector<double> *true_shower_energy = 0;
    double weight;
    int passed_bnb_bdt_cut;

    // Necessary for vectors, for some reason
    TBranch *breco_shower_helper_energy = 0;
    TBranch *btrue_shower_energy = 0;
    TBranch *breco_shower_dirx = 0;
    TBranch *breco_shower_diry = 0;
    TBranch *breco_shower_dirz = 0;
    
    t->SetBranchAddress("most_energetic_shower_index", &most_energetic_shower_index);
    t->SetBranchAddress("second_most_energetic_shower_index", &second_most_energetic_shower_index);
    t->SetBranchAddress("reco_shower_helper_energy", &reco_shower_helper_energy, &breco_shower_helper_energy);
    t->SetBranchAddress("true_shower_energy", &true_shower_energy, &btrue_shower_energy);
    t->SetBranchAddress("reco_shower_dirx", &reco_shower_dirx, &breco_shower_dirx);
    t->SetBranchAddress("reco_shower_diry", &reco_shower_diry, &breco_shower_diry);
    t->SetBranchAddress("reco_shower_dirz", &reco_shower_dirz, &breco_shower_dirz);
    t_friend->SetBranchAddress("weight", &weight);
    t_friend->SetBranchAddress("passed_bnb_bdt_cut", &passed_bnb_bdt_cut);

    // Define output file, 2D energy histo
    TFile *fout = new TFile("output_energy_histos.root", "RECREATE");
    TH2D *h_cal = new TH2D("h_cal", "h_cal", 50, 0, 0.5, 50, 0, 0.5);
    TH2D *h_cal2 = new TH2D("h_cal2", "h_cal2", 50, 0, 0.5, 50, 0, 0.5);
    TH2D *h_cal_corr = new TH2D("h_cal_corr", "h_cal_corr", 50, 0, 0.5, 50, 0, 0.5);
    TH2D *h_cal2_corr = new TH2D("h_cal2_corr", "h_cal2_corr", 50, 0, 0.5, 50, 0, 0.5);
    TH1D *h_invar = new TH1D("h_invar", "h_invar", 50, 0, 1);
    TH1D *h_invar_corr = new TH1D("h_invar_corr", "h_invar_corr", 50, 0, 1);
    TH2D *h_cal3 = new TH2D("h_cal3", "h_cal3", 50, 0, 0.5, 50, 0, 0.5);

    /////////////////////////////////////////////////////////
    ////////////// HISTO FILL LOOP /////////////////////////
    ////////////////////////////////////////////////////////
    int count = 0;
    Int_t leadingIndex = 0; // Leading shower index may be 0 or 1
    // Signal loop
    for (int i = 0; i < t->GetEntries(); i++) {
        t->GetEntry(i);
        if (passed_bnb_bdt_cut != 1) continue; 
        count++;

        // Figure out whether leading shower index is 0 or 1; fill accordingly
        leadingIndex = (most_energetic_shower_index > second_most_energetic_shower_index);
        Double_t corr1, corr2;
        if (leadingIndex == 0) {
            h_cal->Fill(true_shower_energy->at(0), 
                        reco_shower_helper_energy[0], 
                        weight);        
            h_cal2->Fill(true_shower_energy->at(1), 
                         reco_shower_helper_energy[1], 
                         weight);        
            corr1 = leadingCorr(reco_shower_helper_energy[0]);
            corr2 = subleadingCorr(reco_shower_helper_energy[1]);
            h_cal_corr->Fill(true_shower_energy->at(0),
                             corr1, weight);
            h_cal2_corr->Fill(true_shower_energy->at(1),
                              corr2, weight);
        }
        else {
            h_cal->Fill(true_shower_energy->at(1), 
                        reco_shower_helper_energy[1], 
                        weight);        
            h_cal2->Fill(true_shower_energy->at(0), 
                         reco_shower_helper_energy[0], 
                         weight);        
            corr1 = leadingCorr(reco_shower_helper_energy[1]);
            corr2 = subleadingCorr(reco_shower_helper_energy[0]);
            h_cal_corr->Fill(true_shower_energy->at(1),
                             corr1, weight);
            h_cal2_corr->Fill(true_shower_energy->at(0),
                             corr2, weight);
        }
        h_cal3->Fill(true_shower_energy->at(0), reco_shower_helper_energy[0], weight);
        h_cal3->Fill(true_shower_energy->at(1), reco_shower_helper_energy[1], weight);

        double opAng = reco_shower_dirx[0]*reco_shower_dirx[1]+reco_shower_diry[0]*reco_shower_diry[1]+reco_shower_dirz[0]*reco_shower_dirz[1];
        double invMass = sqrt(2*reco_shower_helper_energy[0]*reco_shower_helper_energy[1]*(1 - opAng) );
        double invMass_corr = sqrt(2*corr1*corr2*(1 - opAng) );
        h_invar->Fill(invMass);
        h_invar_corr->Fill(invMass_corr);
    }
    cout << "Total passed: " << count << endl;
    
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
    TF1 *f1 = new TF1("f", "x", 0, 0.5);
    f1->SetLineColor(kBlack);
    f1->Draw("same");

    c1->SaveAs("plot_energy_cal_leading.png", "PNG");

    TCanvas *c2 = new TCanvas("c2", "c2", 1000, 700);
    c2->cd();
    gStyle->SetOptStat(0);
 
    // Drawing options
    // Set minimum < 0 so that 0 bins aren't white (which looks bad)
    h_cal2->SetMinimum(-1e-7);
    h_cal2->GetXaxis()->SetTitle("True Subleading Photon Energy [GeV]");
    h_cal2->GetYaxis()->SetTitle("Reconstructed Subleading Photon Energy [GeV]");
    h_cal2->SetTitle("");
    h_cal2->Draw("colz");

    f1->Draw("same");

    c2->SaveAs("plot_energy_cal_subleading.png", "PNG");

    TCanvas *c3 = new TCanvas("c3", "c3", 1000, 700);
    c3->cd();
    gStyle->SetOptStat(0);
 
    // Drawing options
    // Set minimum < 0 so that 0 bins aren't white (which looks bad)
    h_cal_corr->SetMinimum(-1e-7);
    h_cal_corr->GetXaxis()->SetTitle("True Leading Photon Energy [GeV]");
    h_cal_corr->GetYaxis()->SetTitle("Corrected Leading Photon Energy [GeV]");
    h_cal_corr->SetTitle("");
    h_cal_corr->Draw("colz");

    f1->Draw("same");

    c3->SaveAs("plot_energy_cal_leading_corr.png", "PNG");

    TCanvas *c4 = new TCanvas("c4", "c4", 1000, 700);
    c4->cd();
    gStyle->SetOptStat(0);
 
    // Drawing options
    // Set minimum < 0 so that 0 bins aren't white (which looks bad)
    h_cal2_corr->SetMinimum(-1e-7);
    h_cal2_corr->GetXaxis()->SetTitle("True Subleading Photon Energy [GeV]");
    h_cal2_corr->GetYaxis()->SetTitle("Corrected Subleading Photon Energy [GeV]");
    h_cal2_corr->SetTitle("");
    h_cal2_corr->Draw("colz");

    f1->Draw("same");

    c4->SaveAs("plot_energy_cal_subleading_corr.png", "PNG");

    TCanvas *c5 = new TCanvas("c5", "c5", 1000, 700);
    c5->cd();
    gStyle->SetOptStat(0);
 
    // Drawing options
    // Set minimum < 0 so that 0 bins aren't white (which looks bad)
    h_invar->GetXaxis()->SetTitle("Reconstructed Diphoton Mass [GeV]");
    h_invar->SetTitle("");
    h_invar->Draw();

    c5->SaveAs("plot_invariant_mass.png", "PNG");

    TCanvas *c6 = new TCanvas("c6", "c6", 1000, 700);
    c6->cd();
    gStyle->SetOptStat(0);
 
    // Drawing options
    // Set minimum < 0 so that 0 bins aren't white (which looks bad)
    h_invar_corr->GetXaxis()->SetTitle("Reconstructed Diphoton Mass [GeV]");
    h_invar_corr->SetTitle("");

    c6->SaveAs("plot_invariant_mass_corr.png", "PNG");

    TCanvas *c7 = new TCanvas("c7", "c7", 1000, 700);
    c7->cd();
    gStyle->SetOptStat(0);
 
    // Drawing options
    // Set minimum < 0 so that 0 bins aren't white (which looks bad)
    h_cal->SetMinimum(-1e-7);
    h_cal->GetXaxis()->SetTitle("True Energy [GeV]");
    h_cal->GetYaxis()->SetTitle("Reconstructed Energy [GeV]");
    h_cal->SetTitle("Both Photons");
    h_cal->Draw("colz");

    f1->Draw("same");

    c1->SaveAs("plot_energy_cal_both.png", "PNG");

    fout->Write();
    fout->Close();
}

int main() {
    make_2dHisto();
    return 0;
}

// Energy correction functions
// Parameters extracted from fit performed in energy_cal.cc
double leadingCorr(double energy) {
    double corr = 1.24476*energy + 0.015528;
    return corr;
}

double subleadingCorr(double energy) {
    double corr = 1.18413*energy + 0.02409;
    return corr;
}

