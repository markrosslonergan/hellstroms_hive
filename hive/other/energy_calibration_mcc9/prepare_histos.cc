#include "energy_cal.h"
using std::cout;
using std::endl;
double energyCorr(double, TString);

void make_2dHisto() {
    /////////////////////////////////////////////////////////
    ////////////// INITIALIZATION /////////////////////
    ////////////////////////////////////////////////////////
    
    // Get signal file and TTree
    TString dir = "/uboone/data/users/markross/Mar2020/";
    // TODO try using final selection sbnfit file for comparison
    //TFile *fin = new TFile("/uboone/app/users/amogan/hive_v2.5_xgboost/hellstroms_hive/hive/build/bin/sbnfit_pigLEE_combined_stage_2_NCPi0Combined_1p_and_0p_v5.root", "READ");
    //TFile *fin = new TFile(dir+"ncpi0_overlay_run3g_v33.3.uniq.root", "READ");
    //TFile *fin = new TFile(dir+"ncpi0_overlay_run1_v33.3.uniq.root", "READ");
    TFile *fin = new TFile(dir+"ncpi0_overlay_combined_run13_v33.3.uniq.root", "READ");
    TString tag = "all";
    //TTree *t = (TTree*)fin->Get("singlephotonana/vertex_tree");
    TTree *t = (TTree*)fin->Get("singlephotonana/vertex_tree");
    if (!t || !fin) {
      cout << "ERROR Input file doesn't exist or TTree name is incorrect" << endl;
      return;
    }

    // Declare necessary tree variables
    int reco_asso_showers = 0;
    std::vector<unsigned long> *reco_shower_ordered_energy_index = 0;
    std::vector<double> *reco_shower_energy_max = 0;
    std::vector<double> *reco_shower_energy_plane0 = 0;
    std::vector<double> *reco_shower_energy_plane1 = 0;
    std::vector<double> *reco_shower_energy_plane2 = 0;
    std::vector<double> *reco_shower_dirx = 0;
    std::vector<double> *reco_shower_diry = 0;
    std::vector<double> *reco_shower_dirz = 0;
    std::vector<double> *sim_shower_energy = 0;
    std::vector<int>    *sim_shower_pdg = 0;
    std::vector<double> *sim_shower_overlay_fraction = 0;

    // Necessary for vectors, for some reason
    TBranch *breco_shower_ordered_energy_index = 0;
    TBranch *breco_shower_energy_max = 0;
    TBranch *breco_shower_energy_plane0 = 0;
    TBranch *breco_shower_energy_plane1 = 0;
    TBranch *breco_shower_energy_plane2 = 0;
    TBranch *bsim_shower_energy = 0;
    TBranch *breco_shower_dirx = 0;
    TBranch *breco_shower_diry = 0;
    TBranch *breco_shower_dirz = 0;
    TBranch *bsim_shower_pdg = 0;
    TBranch *bsim_shower_overlay_fraction = 0;
    
    t->SetBranchAddress("reco_asso_showers", &reco_asso_showers);
    t->SetBranchAddress("reco_shower_ordered_energy_index", &reco_shower_ordered_energy_index, &breco_shower_ordered_energy_index);
    t->SetBranchAddress("reco_shower_energy_max", &reco_shower_energy_max, &breco_shower_energy_max);
    t->SetBranchAddress("reco_shower_energy_plane0", &reco_shower_energy_plane0, &breco_shower_energy_plane0);
    t->SetBranchAddress("reco_shower_energy_plane1", &reco_shower_energy_plane1, &breco_shower_energy_plane1);
    t->SetBranchAddress("reco_shower_energy_plane2", &reco_shower_energy_plane2, &breco_shower_energy_plane2);
    t->SetBranchAddress("sim_shower_energy", &sim_shower_energy, &bsim_shower_energy);
    t->SetBranchAddress("reco_shower_dirx", &reco_shower_dirx, &breco_shower_dirx);
    t->SetBranchAddress("reco_shower_diry", &reco_shower_diry, &breco_shower_diry);
    t->SetBranchAddress("reco_shower_dirz", &reco_shower_dirz, &breco_shower_dirz);
    t->SetBranchAddress("sim_shower_pdg", &sim_shower_pdg, &bsim_shower_pdg);
    t->SetBranchAddress("sim_shower_overlay_fraction", &sim_shower_overlay_fraction, &bsim_shower_overlay_fraction);

    // Define output file, 2D energy histo
    TFile *fout = new TFile("output_energy_histos_"+tag+".root", "RECREATE");
    TH1D *h_num_showers = new TH1D("h_num_showers", "h_num_showers", 4, 0, 4);
    //TH2D *h_max = new TH2D("h_max", "h_max", 50, 0, 500, 50, 0, 500);
    //TH2D *h_max_corr = new TH2D("h_max_corr", "h_max_corr", 50, 0, 500, 50, 0, 500);
    TH2D *h_max = new TH2D("h_max", "h_max", 40, 0, 400, 40, 0, 400);
    TH2D *h_plane0 = new TH2D("h_plane0", "h_plane0", 40, 0, 400, 40, 0, 400);
    TH2D *h_plane1 = new TH2D("h_plane1", "h_plane1", 40, 0, 400, 40, 0, 400);
    TH2D *h_plane2 = new TH2D("h_plane2", "h_plane2", 40, 0, 400, 40, 0, 400);
    TH2D *h_max_corr = new TH2D("h_max_corr", "h_max_corr", 40, 0, 400, 40, 0, 400);
    TH2D *h_plane0_corr = new TH2D("h_plane0_corr", "h_plane0_corr", 40, 0, 400, 40, 0, 400);
    TH2D *h_plane1_corr = new TH2D("h_plane1_corr", "h_plane1_corr", 40, 0, 400, 40, 0, 400);
    TH2D *h_plane2_corr = new TH2D("h_plane2_corr", "h_plane2_corr", 40, 0, 400, 40, 0, 400);
    TH1D *h_invar_plane0 = new TH1D("h_invar_plane0", "h_invar_plane0", 50, 0, 500);
    TH1D *h_invar_plane0_corr = new TH1D("h_invar_plane0_corr", "h_invar_plane0_corr", 50, 0, 500);
    TH1D *h_invar_plane1 = new TH1D("h_invar_plane1", "h_invar_plane1", 50, 0, 500);
    TH1D *h_invar_plane1_corr = new TH1D("h_invar_plane1_corr", "h_invar_plane1_corr", 50, 0, 500);
    TH1D *h_invar_plane2 = new TH1D("h_invar_plane2", "h_invar_plane2", 50, 0, 500);
    TH1D *h_invar_plane2_corr = new TH1D("h_invar_plane2_corr", "h_invar_plane2_corr", 50, 0, 500);
    TH1D *h_invar_max = new TH1D("h_invar_max", "h_invar_max", 50, 0, 500);
    TH1D *h_invar_max_corr = new TH1D("h_invar_max_corr", "h_invar_max_corr", 50, 0, 500);
    TH1I *h_sim_shower_pdg = new TH1I("h_sim_shower_pdg", "h_sim_shower_pdg", 100, 0, 2500);

    /////////////////////////////////////////////////////////
    ////////////// HISTO FILL LOOP /////////////////////////
    ////////////////////////////////////////////////////////
    int pass_shr = 0;
    int pass_energy = 0;
    int pass_pdg = 0;
    int pass_overlay = 0;
    double energy_cut = 20.0;
    double overlay_fraction_cut = 0.3;
    for (int i = 0; i < t->GetEntries(); i++) {
        t->GetEntry(i);
        h_num_showers->Fill(reco_asso_showers);

        if (reco_asso_showers < 1) continue;
        pass_shr++;

        for (int i_shr = 0; i_shr < reco_asso_showers; i_shr++) {
            h_sim_shower_pdg->Fill(sim_shower_pdg->at(reco_shower_ordered_energy_index->at(i_shr)));

            // Energy cut
            // Factor of 1000 for sim energy because it's in different units. Thanks, Mark.
            if (reco_shower_energy_max->at(reco_shower_ordered_energy_index->at(i_shr)) < energy_cut) continue;
                //&& 1000*sim_shower_energy->at(reco_shower_ordered_energy_index->at(i_shr)) < energy_cut ) continue;
            pass_energy++;

            // Make sure shower is matched to photon
            if ((sim_shower_pdg->at(reco_shower_ordered_energy_index->at(i_shr))) != 22) continue;
            pass_pdg++;

            // Check overlay fraction
            if ((sim_shower_overlay_fraction->at(reco_shower_ordered_energy_index->at(i_shr))) > overlay_fraction_cut) continue;
            pass_overlay++;

            // Fill max energy
            double max_energy_corr;
            h_max->Fill(1000*sim_shower_energy->at(reco_shower_ordered_energy_index->at(i_shr)), 
                        reco_shower_energy_max->at(reco_shower_ordered_energy_index->at(i_shr)) );        

            max_energy_corr = energyCorr(reco_shower_energy_max->at(reco_shower_ordered_energy_index->at(i_shr)), tag);
            h_max_corr->Fill(1000*sim_shower_energy->at(reco_shower_ordered_energy_index->at(i_shr)),
                             max_energy_corr);

            // Fill plane 0
            double plane0_energy_corr = -999;
            if (reco_shower_energy_plane0->at(reco_shower_ordered_energy_index->at(i_shr)) > 0. ) {
              //cout << "Shower " << i_shr << " plane0 energy " << reco_shower_energy_plane0->at(reco_shower_ordered_energy_index->at(i_shr)) << endl;
              h_plane0->Fill(1000*sim_shower_energy->at(reco_shower_ordered_energy_index->at(i_shr)), 
                          reco_shower_energy_plane0->at(reco_shower_ordered_energy_index->at(i_shr)) );        

              plane0_energy_corr = energyCorr(reco_shower_energy_plane0->at(reco_shower_ordered_energy_index->at(i_shr)), tag);
              h_plane0_corr->Fill(1000*sim_shower_energy->at(reco_shower_ordered_energy_index->at(i_shr)),
                               plane0_energy_corr);
            }

            // Fill plane 1
            double plane1_energy_corr = -999;
            if (reco_shower_energy_plane1->at(reco_shower_ordered_energy_index->at(i_shr)) > 0. ) {
              //cout << "Shower " << i_shr << " plane1 energy " << reco_shower_energy_plane1->at(reco_shower_ordered_energy_index->at(i_shr)) << endl;
              h_plane1->Fill(1000*sim_shower_energy->at(reco_shower_ordered_energy_index->at(i_shr)), 
                          reco_shower_energy_plane1->at(reco_shower_ordered_energy_index->at(i_shr)) );        

              plane1_energy_corr = energyCorr(reco_shower_energy_plane1->at(reco_shower_ordered_energy_index->at(i_shr)), tag);
              h_plane1_corr->Fill(1000*sim_shower_energy->at(reco_shower_ordered_energy_index->at(i_shr)),
                               plane1_energy_corr);
            }

            // Fill plane 2
            double plane2_energy_corr = -999;
            if (reco_shower_energy_plane2->at(reco_shower_ordered_energy_index->at(i_shr)) > 0. ) {
              //cout << "Shower " << i_shr << " plane2 energy " << reco_shower_energy_plane2->at(reco_shower_ordered_energy_index->at(i_shr)) << endl;
              h_plane2->Fill(1000*sim_shower_energy->at(reco_shower_ordered_energy_index->at(i_shr)), 
                          reco_shower_energy_plane2->at(reco_shower_ordered_energy_index->at(i_shr)) );        

              plane2_energy_corr = energyCorr(reco_shower_energy_plane2->at(reco_shower_ordered_energy_index->at(i_shr)), tag);
              h_plane2_corr->Fill(1000*sim_shower_energy->at(reco_shower_ordered_energy_index->at(i_shr)),
                               plane2_energy_corr);
            }

            // Invariant mass using individual corrections
            double opAng = -999;
            double invMass_plane0 = -999, invMass_plane0_corr = -999;
            double invMass_plane1 = -999, invMass_plane1_corr = -999;
            double invMass_plane2 = -999, invMass_plane2_corr = -999;
            double invMass_max    = -999, invMass_max_corr    = -999;
            if (reco_asso_showers == 2) {
              opAng = reco_shower_dirx->at(0)*reco_shower_dirx->at(1)+
                      reco_shower_diry->at(0)*reco_shower_diry->at(1)+
                      reco_shower_dirz->at(0)*reco_shower_dirz->at(1);

              if (reco_shower_energy_plane0->size() == 2) {
                invMass_plane0 = sqrt(2*reco_shower_energy_plane0->at(reco_shower_ordered_energy_index->at(0))*reco_shower_energy_plane0->at(reco_shower_ordered_energy_index->at(1))*(1 - opAng) );
                invMass_plane0_corr = sqrt(2*plane0_energy_corr*plane0_energy_corr*(1 - opAng) );
              }

              if (reco_shower_energy_plane1->size() == 2) {
                invMass_plane1 = sqrt(2*reco_shower_energy_plane1->at(reco_shower_ordered_energy_index->at(0))*reco_shower_energy_plane1->at(reco_shower_ordered_energy_index->at(1))*(1 - opAng) );
                invMass_plane1_corr = sqrt(2*plane1_energy_corr*plane1_energy_corr*(1 - opAng) );
              }

              if (reco_shower_energy_plane2->size() == 2) {
                invMass_plane2 = sqrt(2*reco_shower_energy_plane2->at(reco_shower_ordered_energy_index->at(0))*reco_shower_energy_plane2->at(reco_shower_ordered_energy_index->at(1))*(1 - opAng) );
                invMass_plane2_corr = sqrt(2*plane2_energy_corr*plane2_energy_corr*(1 - opAng) );
              }

              if (reco_shower_energy_max->size() == 2) {
                invMass_max = sqrt(2*reco_shower_energy_max->at(reco_shower_ordered_energy_index->at(0))*reco_shower_energy_max->at(reco_shower_ordered_energy_index->at(1))*(1 - opAng) );
                invMass_max_corr = sqrt(2*max_energy_corr*max_energy_corr*(1 - opAng) );
              }
            }

            // Invariant mass histograms
            h_invar_plane0->Fill(invMass_plane0);
            h_invar_plane0_corr->Fill(invMass_plane0_corr);
            h_invar_plane1->Fill(invMass_plane1);
            h_invar_plane1_corr->Fill(invMass_plane1_corr);
            h_invar_plane2->Fill(invMass_plane2);
            h_invar_plane2_corr->Fill(invMass_plane2_corr);
            h_invar_max->Fill(invMass_max);
            h_invar_max_corr->Fill(invMass_max_corr);
          }
    }
    cout << "Events with at least one associated shower: " << pass_shr << endl;
    cout << "Showers with at least 20 MeV: " << pass_energy << endl;
    cout << "Showers matched to photon: " << pass_pdg << endl;
    cout << "Showers with overlay < 30%: " << pass_overlay << endl;
    
    /*
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
    h_invar_corr->GetXaxis()->SetTitle("Corrected Diphoton Mass [GeV]");
    h_invar_corr->SetTitle("");

    c6->SaveAs("plot_invariant_mass_corr.png", "PNG");

    TCanvas *c7 = new TCanvas("c7", "c7", 1000, 700);
    c7->cd();
    gStyle->SetOptStat(0);
    */
 
    fout->Write();
    fout->Close();
}

int main() {
    make_2dHisto();
    return 0;
}

// Energy correction functions
// Parameters extracted from fit performed in energy_cal.cc
double energyCorr(double energy, TString tag) {
    // MCC8
    //double corr = 1.24476*energy + 0.015528;
    // MCC9ish (v10)
    //double corr = 1.24288*energy + 8.64122;
    // MCC9ish v13
    //double corr = 1.29888*energy + 1.68323
    // MCC9.0 v17
    //double corr = 1.21207*energy + 6.69335;
    // MCC9.0 v17.1
    //double corr = 1.24607*energy + 4.11138; 
    
    // MCC9.1 v26.5
    //double corr = 1.23833*energy + 4.78805;
    // Run 1
    //double corr = 1.21989*energy + 8.50486;
    // Run 3 
    //double corr = 1.21061*energy + 8.38835; 
    double corr;
    if      (tag=="run1") corr = 1.21183*energy + 9.87837;
    else if (tag=="run3") corr = 1.20421*energy + 9.51016;
    else if (tag=="all")  corr = 1.21183*energy + 9.87837;
    else {
      std::cout << "[energyCorr] Invalid tag. Returning uncorrected energy" << std::endl; 
      corr = energy;
    }
    return corr;
}


