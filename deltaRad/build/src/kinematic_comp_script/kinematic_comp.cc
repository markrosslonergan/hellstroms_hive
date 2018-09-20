#include "kinematic_comp.h"
using std::cout;
using std::endl;

void kinematic_comp() {
    // Get files and trees
    TString sampledir = "/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_v3/";
    // My vertexed signal file
    TFile *fsig = new TFile(sampledir+"vertexed_ncpi0cosmics_fltr_fresh_v4.1.root", "READ");
    TTree *tsig = (TTree*)fsig->Get("vertex_tree");
    // Friend tree to get my selection
    TFile *fcut_ncpi0 = new TFile("../NCpi0Cosmics_stage_friend.root", "READ");
    TTree *tcut_ncpi0 = (TTree*)fcut_ncpi0->Get("stage_cuts");
    tsig->AddFriend(tcut_ncpi0);

    // BNB+Cosmics vertexed file, to be filtered for Mark's NC pi0 backgrounds
    TFile *fbkg = new TFile(sampledir+"vertexed_bnbcosmics_fresh_v4.1.root", "READ");
    TTree *tbkg = (TTree*)fbkg->Get("vertex_tree");
    // Friend tree to get Mark's selection
    TFile *fcut_deltarad = new TFile("../BNBCosmics_stage_friend.root", "READ");
    TTree *tcut_deltarad = (TTree*)fcut_deltarad->Get("stage_cuts");
    tbkg->AddFriend(tcut_deltarad);

    // Declare variables from trees
    // Large number here (25) to avoid "stack smashing"
    std::size_t const N = 25;
    int passed_topological_selection_bkg, passed_precuts_bkg, passed_cosmic_bdt_cut_bkg, passed_bnb_bdt_cut_bkg;
    int passed_topological_selection_sig, passed_precuts_sig, passed_cosmic_bdt_cut_sig, passed_bnb_bdt_cut_sig;
    int exiting_pi0_number;
    double weight_bkg, weight_sig;
    double reco_shower_helper_energy_bkg[N] = {0.};
    double reco_shower_dedx_plane0_bkg[N] = {0.};
    double reco_shower_length_bkg[N] = {0.};
    double exiting_pi0_px_bkg[N] = {0.};
    double exiting_pi0_py_bkg[N] = {0.};
    double exiting_pi0_pz_bkg[N] = {0.};
    double reco_shower_helper_energy_sig[N] = {0.};
    double reco_shower_dedx_plane0_sig[N] = {0.};
    double reco_shower_length_sig[N] = {0.};
    double exiting_pi0_px_sig[N] = {0.};
    double exiting_pi0_py_sig[N] = {0.};
    double exiting_pi0_pz_sig[N] = {0.};

    // Set branches to be used
    // Signal and background files use the same variable names, so be careful!
    TBranch *breco_shower_energy_bkg = 0;
    TBranch *breco_shower_dedx_plane0_bkg = 0;
    TBranch *breco_shower_length_bkg = 0;
    TBranch *bexiting_pi0_px_bkg = 0;
    TBranch *bexiting_pi0_py_bkg = 0;
    TBranch *bexiting_pi0_pz_bkg = 0;
    TBranch *breco_shower_energy_sig = 0;
    TBranch *breco_shower_dedx_plane0_sig = 0;
    TBranch *breco_shower_length_sig = 0;
    TBranch *bexiting_pi0_px_sig = 0;
    TBranch *bexiting_pi0_py_sig = 0;
    TBranch *bexiting_pi0_pz_sig = 0;

    tbkg->SetBranchAddress("exiting_pi0_number", &exiting_pi0_number);
    tbkg->SetBranchAddress("reco_shower_dedx_plane0", &reco_shower_dedx_plane0_bkg, &breco_shower_dedx_plane0_bkg);
    tbkg->SetBranchAddress("reco_shower_helper_energy", &reco_shower_helper_energy_bkg, &breco_shower_energy_bkg);
    tbkg->SetBranchAddress("reco_shower_length", &reco_shower_length_bkg, &breco_shower_length_bkg);
    tbkg->SetBranchAddress("exiting_pi0_px", &exiting_pi0_px_bkg, &bexiting_pi0_px_bkg);
    tbkg->SetBranchAddress("exiting_pi0_py", &exiting_pi0_py_bkg, &bexiting_pi0_py_bkg);
    tbkg->SetBranchAddress("exiting_pi0_pz", &exiting_pi0_pz_bkg, &bexiting_pi0_pz_bkg);
    tcut_deltarad->SetBranchAddress("passed_bnb_bdt_cut", &passed_bnb_bdt_cut_bkg);
    tcut_deltarad->SetBranchAddress("weight", &weight_bkg);
    tsig->SetBranchAddress("reco_shower_dedx_plane0", &reco_shower_dedx_plane0_sig, &breco_shower_dedx_plane0_sig);
    tsig->SetBranchAddress("reco_shower_helper_energy", &reco_shower_helper_energy_sig, &breco_shower_energy_sig);
    tsig->SetBranchAddress("reco_shower_length", &reco_shower_length_sig, &breco_shower_length_sig);
    tsig->SetBranchAddress("exiting_pi0_px", &exiting_pi0_px_sig, &bexiting_pi0_px_sig);
    tsig->SetBranchAddress("exiting_pi0_py", &exiting_pi0_py_sig, &bexiting_pi0_py_sig);
    tsig->SetBranchAddress("exiting_pi0_pz", &exiting_pi0_pz_sig, &bexiting_pi0_pz_sig);
    tcut_ncpi0->SetBranchAddress("passed_bnb_bdt_cut", &passed_bnb_bdt_cut_sig);
    tcut_ncpi0->SetBranchAddress("weight", &weight_sig);

    // Define histograms for variables of interest, and output file to write to
    // "bkg" for backgrounds from Mark's selection, "sig" for histos from my vertexed
    // signal file
    TFile *fout = new TFile("output_kinematic_histos.root", "RECREATE");
    TH1D *h_dedx_plane0_bkg = new TH1D("h_dedx_plane0_bkg", "h_dedx_plane0", 48, 0, 15); 
    TH1D *h_shower_energy_bkg = new TH1D("h_shower_energy_bkg", "h_shower_energy", 50, 0, 1.5);
    TH1D *h_shower_length_bkg = new TH1D("h_shower_length_bkg", "h_shower_length", 48, 0, 100);
    TH1D *h_exiting_pi0_px_bkg = new TH1D("h_exiting_pi0_px_bkg", "h_exiting_pi0_px_bkg", 50, 0, 1.5); 
    TH1D *h_exiting_pi0_py_bkg = new TH1D("h_exiting_pi0_py_bkg", "h_exiting_pi0_py_bkg", 50, 0, 1.5); 
    TH1D *h_exiting_pi0_pz_bkg = new TH1D("h_exiting_pi0_pz_bkg", "h_exiting_pi0_pz_bkg", 50, 0, 1.5); 
    TH1D *h_dedx_plane0_sig = new TH1D("h_dedx_plane0_sig", "h_dedx_plane0", 48, 0, 15); 
    TH1D *h_shower_energy_sig = new TH1D("h_shower_energy_sig", "h_shower_energy", 50, 0, 1.5);
    TH1D *h_shower_length_sig = new TH1D("h_shower_length_sig", "h_shower_length", 48, 0, 100);
    TH1D *h_exiting_pi0_px_sig = new TH1D("h_exiting_pi0_px_sig", "h_exiting_pi0_px_sig", 50, 0, 1.5); 
    TH1D *h_exiting_pi0_py_sig = new TH1D("h_exiting_pi0_py_sig", "h_exiting_pi0_py_sig", 50, 0, 1.5); 
    TH1D *h_exiting_pi0_pz_sig = new TH1D("h_exiting_pi0_pz_sig", "h_exiting_pi0_pz_sig", 50, 0, 1.5); 

    int nevents = 0;
    int passed_bnb = 0;
    int passed_pi0_cut = 0;
    int passed_ncpi0 = 0;
    // Signal loop
    for (int i = 0; i < tsig->GetEntries(); i++) {
        // Fill signal histos
        tsig->GetEntry(i);
        if (passed_bnb_bdt_cut_sig != 1) continue;
        passed_ncpi0++;

        h_dedx_plane0_sig->Fill(reco_shower_dedx_plane0_sig[0], weight_sig);
        h_shower_energy_sig->Fill(reco_shower_helper_energy_sig[0], weight_sig);
        h_shower_length_sig->Fill(reco_shower_length_sig[0], weight_sig);
        h_exiting_pi0_px_sig->Fill(exiting_pi0_px_sig[0], weight_sig);
        h_exiting_pi0_py_sig->Fill(exiting_pi0_py_sig[0], weight_sig);
        h_exiting_pi0_pz_sig->Fill(exiting_pi0_pz_sig[0], weight_sig);

        // Fill background histos; filter so as to get Mark's NC pi0's
        // Make sure it (a) passes single photon code, (b) has one exiting pi0, 
        // and (c) isn't a true delta rad. Should get Mark's NC pi0 backgrounds
    }
    // NC pi0 background loop
    for (int j = 0; j < tbkg->GetEntries(); j++) {
        tbkg->GetEntry(j);
        if (passed_bnb_bdt_cut_bkg != 1) continue;
        passed_bnb++;

        if (exiting_pi0_number != 1) continue;
        passed_pi0_cut++;

        nevents++;
        h_dedx_plane0_bkg->Fill(reco_shower_dedx_plane0_bkg[0], weight_bkg);
        h_shower_energy_bkg->Fill(reco_shower_helper_energy_bkg[0], weight_bkg);
        h_shower_length_bkg->Fill(reco_shower_length_bkg[0], weight_bkg);
        h_exiting_pi0_px_bkg->Fill(exiting_pi0_px_bkg[0], weight_bkg);
        h_exiting_pi0_py_bkg->Fill(exiting_pi0_py_bkg[0], weight_bkg);
        h_exiting_pi0_pz_bkg->Fill(exiting_pi0_pz_bkg[0], weight_bkg);
 
    }
    cout << "Passes Mark's selection: " << passed_bnb << endl;
    cout << "After pi0==1 cut: " << passed_pi0_cut << endl;
    cout << "Final passing events: " << nevents << endl;
    cout << "----------------------" << endl;
    cout << "Passes my selection: " << passed_ncpi0 << endl;

    fout->Write();
    fout->Close();

}

int main() {
    kinematic_comp();
    return 0;
}

