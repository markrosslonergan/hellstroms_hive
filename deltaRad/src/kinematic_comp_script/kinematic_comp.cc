#include "kinematic_comp.h"
using std::cout;
using std::endl;

void kinematic_comp() {
    // Get files and trees
    //TString sampledir = "/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_v3/";
    TString sampledir = "/pnfs/uboone/persistent/users/amogan/singlePhoton/samples/";
    // My vertexed signal file
    TFile *fsig = new TFile(sampledir+"vertexed_ncpi0_PiMom_v4.1.root", "READ");
    TTree *tsig = (TTree*)fsig->Get("vertex_tree");
    // Friend tree to get my selection
    TFile *fcut_ncpi0 = new TFile("../NCpi0Cosmics_stage_friend.root", "READ");
    TTree *tcut_ncpi0 = (TTree*)fcut_ncpi0->Get("stage_cuts");
    tsig->AddFriend(tcut_ncpi0);

    // BNB+Cosmics vertexed file, to be filtered for Mark's NC pi0 backgrounds
    TFile *fbkg = new TFile(sampledir+"vertexed_BNBCosmics_PiMom_v4.1.root", "READ");
    TTree *tbkg = (TTree*)fbkg->Get("vertex_tree");
    // Friend tree to get Mark's selection
    TFile *fcut_deltarad = new TFile("quick_stage.root", "READ");
    TTree *tcut_deltarad = (TTree*)fcut_deltarad->Get("quick_stage");
    tbkg->AddFriend(tcut_deltarad);

    // Declare variables from trees. "sig" denotes my NC pi0 signal file,
    // "bkg" denotes Mark's NC pi0-tagged backgrounds
    // Large number here (25) to avoid "stack smashing"
    std::size_t const N = 20;
    //int passed_topological_selection_bkg, passed_precuts_bkg, passed_cosmic_bdt_cut_bkg, passed_bnb_bdt_cut_bkg;
    int passed_topological_selection_sig, passed_precuts_sig, passed_cosmic_bdt_cut_sig, passed_bnb_bdt_cut_sig;
    int exiting_pi0_number;
    int is_delta_rad;
    int quick_pass;
    double weight_bkg = 1, weight_sig;
    //double exiting_pi0_px_bkg[N] = {0.};
    //double exiting_pi0_py_bkg[N] = {0.};
    //double exiting_pi0_pz_bkg[N] = {0.};
    std::vector<double> *exiting_pi0_px_sig = 0;
    std::vector<double> *exiting_pi0_py_sig = 0;
    std::vector<double> *exiting_pi0_pz_sig = 0;

    std::vector<double> *exiting_pi0_px_bkg = 0;
    std::vector<double> *exiting_pi0_py_bkg = 0;
    std::vector<double> *exiting_pi0_pz_bkg = 0;

    double reco_shower_dirx_sig[N] = {0.};
    double reco_shower_diry_sig[N] = {0.};
    double reco_shower_dirz_sig[N] = {0.};
    double reco_shower_helper_energy_sig[N] = {0.};

    double reco_shower_dirx_bkg[N] = {0.};
    double reco_shower_diry_bkg[N] = {0.};
    double reco_shower_dirz_bkg[N] = {0.};
    //double reco_shower_dedx_plane0_sig[N] = {0.} = 0;
    //double reco_shower_length_sig[N] = {0.} = 0;
    //double exiting_pi0_px_sig[N] = {0.};
    //double exiting_pi0_py_sig[N] = {0.};
    //double exiting_pi0_pz_sig[N] = {0.};

    // Set branches to be used
    // Signal and background files use the same variable names, so be careful!
    TBranch *bexiting_pi0_px_bkg;
    TBranch *bexiting_pi0_py_bkg;
    TBranch *bexiting_pi0_pz_bkg;
    TBranch *bexiting_pi0_px_sig;
    TBranch *bexiting_pi0_py_sig;
    TBranch *bexiting_pi0_pz_sig;
    TBranch *breco_shower_dirx_sig = 0;
    TBranch *breco_shower_diry_sig = 0;
    TBranch *breco_shower_dirz_sig = 0;
    TBranch *breco_shower_dirx_bkg = 0;
    TBranch *breco_shower_diry_bkg = 0;
    TBranch *breco_shower_dirz_bkg = 0;
    TBranch *breco_shower_helper_energy_sig = 0;

    tbkg->SetBranchAddress("exiting_pi0_number", &exiting_pi0_number);
    tbkg->SetBranchAddress("exiting_pi0_px", &exiting_pi0_px_bkg, &bexiting_pi0_px_bkg);
    tbkg->SetBranchAddress("exiting_pi0_py", &exiting_pi0_py_bkg, &bexiting_pi0_py_bkg);
    tbkg->SetBranchAddress("exiting_pi0_pz", &exiting_pi0_pz_bkg, &bexiting_pi0_pz_bkg);
    tbkg->SetBranchAddress("reco_shower_dirx", &reco_shower_dirx_bkg, &breco_shower_dirx_bkg);
    tbkg->SetBranchAddress("reco_shower_diry", &reco_shower_diry_bkg, &breco_shower_diry_bkg);
    tbkg->SetBranchAddress("reco_shower_dirz", &reco_shower_dirz_bkg, &breco_shower_dirz_bkg);
    tbkg->SetBranchAddress("is_delta_rad", &is_delta_rad);
    //tcut_deltarad->SetBranchAddress("passed_bnb_bdt_cut", &passed_bnb_bdt_cut_bkg);
    tcut_deltarad->SetBranchAddress("quick_pass", &quick_pass);
    //tcut_deltarad->SetBranchAddress("weight", &weight_bkg);

    tsig->SetBranchAddress("exiting_pi0_px", &exiting_pi0_px_sig, &bexiting_pi0_px_sig);
    tsig->SetBranchAddress("exiting_pi0_py", &exiting_pi0_py_sig, &bexiting_pi0_py_sig);
    tsig->SetBranchAddress("exiting_pi0_pz", &exiting_pi0_pz_sig, &bexiting_pi0_pz_sig);
    tsig->SetBranchAddress("reco_shower_dirx", &reco_shower_dirx_sig, &breco_shower_dirx_sig);
    tsig->SetBranchAddress("reco_shower_diry", &reco_shower_diry_sig, &breco_shower_diry_sig);
    tsig->SetBranchAddress("reco_shower_dirz", &reco_shower_dirz_sig, &breco_shower_dirz_sig);
    tsig->SetBranchAddress("reco_shower_helper_energy", &reco_shower_helper_energy_sig, &breco_shower_helper_energy_sig);
    tcut_ncpi0->SetBranchAddress("passed_bnb_bdt_cut", &passed_bnb_bdt_cut_sig);
    tcut_ncpi0->SetBranchAddress("weight", &weight_sig);

    // Define histograms for variables of interest, and output file to write to
    TFile *fout = new TFile("output_kinematic_histos.root", "RECREATE");
    TH1D *h_exiting_pi0_px_bkg = new TH1D("h_exiting_pi0_px_bkg", "h_exiting_pi0_px_bkg", 50, 0, 1.5); 
    TH1D *h_exiting_pi0_py_bkg = new TH1D("h_exiting_pi0_py_bkg", "h_exiting_pi0_py_bkg", 50, 0, 1.5); 
    TH1D *h_exiting_pi0_pz_bkg = new TH1D("h_exiting_pi0_pz_bkg", "h_exiting_pi0_pz_bkg", 50, 0, 1.5); 
    TH1D *h_exiting_pi0_ptot_bkg = new TH1D("h_exiting_pi0_ptot_bkg", "h_exiting_pi0_ptot_bkg", 50, 0, 1.5); 
    TH1D *h_exiting_pi0_px_sig = new TH1D("h_exiting_pi0_px_sig", "h_exiting_pi0_px_sig", 50, 0, 1.5); 
    TH1D *h_exiting_pi0_py_sig = new TH1D("h_exiting_pi0_py_sig", "h_exiting_pi0_py_sig", 50, 0, 1.5); 
    TH1D *h_exiting_pi0_pz_sig = new TH1D("h_exiting_pi0_pz_sig", "h_exiting_pi0_pz_sig", 50, 0, 1.5); 
    TH1D *h_exiting_pi0_ptot_sig = new TH1D("h_exiting_pi0_ptot_sig", "h_exiting_pi0_ptot_sig", 50, 0, 1.5); 
    TH2D *h_PvsPZ_sig = new TH2D("h_PvsPZ_sig", "h_PvsPZ_sig", 50, 0, 1., 50, 0, 1.);
    TH2D *h_PvsPZ_bkg = new TH2D("h_PvsPZ_bkg", "h_PvsPZ_bkg", 50, 0, 1., 50, 0, 1.);
    TH2D *h_PvsPX_sig = new TH2D("h_PvsPX_sig", "h_PvsPX_sig", 50, 0, 1., 50, 0, 1.);
    TH2D *h_PvsPX_bkg = new TH2D("h_PvsPX_bkg", "h_PvsPX_bkg", 50, 0, 1., 50, 0, 1.);
    TH2D *h_PvsPY_sig = new TH2D("h_PvsPY_sig", "h_PvsPY_sig", 50, 0, 1., 50, 0, 1.);
    TH2D *h_PvsPY_bkg = new TH2D("h_PvsPY_bkg", "h_PvsPY_bkg", 50, 0, 1., 50, 0, 1.);
    TH2D *h_PvsOpang_sig = new TH2D("h_PvsOpang_sig", "h_PvsOpang_sig", 36, 0, 180, 50, 0, 1.);

    int passed_sig = 0;
    int passed_bkg = 0;
    int passed_deltaRad = 0;
    int passed_bkg_pi0_cut = 0;
    int passed_isDelta = 0;
    int passed_ncpi0 = 0;
    int passed_bkg_notDeltaRad = 0;
    double ptot = 0;
    double ptot_reco = 0;
    double opAng = 0;
    // Signal loop
    for (int i = 0; i < tsig->GetEntries(); i++) {
        // Fill signal histos
        tsig->GetEntry(i);
        if (passed_bnb_bdt_cut_sig != 1) continue;
        passed_ncpi0++;

        ptot = sqrt( exiting_pi0_px_sig->at(0)*exiting_pi0_px_sig->at(0) +
                     exiting_pi0_py_sig->at(0)*exiting_pi0_py_sig->at(0) +
                     exiting_pi0_pz_sig->at(0)*exiting_pi0_pz_sig->at(0)
               );
        ptot_reco = sqrt( reco_shower_helper_energy_sig[0]*reco_shower_dirx_sig[0]+
                          reco_shower_helper_energy_sig[1]*reco_shower_dirx_sig[1]+ 
                          reco_shower_helper_energy_sig[0]*reco_shower_diry_sig[0]+
                          reco_shower_helper_energy_sig[1]*reco_shower_diry_sig[1]+ 
                          reco_shower_helper_energy_sig[0]*reco_shower_dirz_sig[0]+ 
                          reco_shower_helper_energy_sig[1]*reco_shower_dirz_sig[1]
                    ); 
        opAng = 57.2958*TMath::ACos(reco_shower_dirx_sig[0]*reco_shower_dirx_sig[1]+
                                    reco_shower_diry_sig[0]*reco_shower_diry_sig[1]+
                                    reco_shower_dirz_sig[0]*reco_shower_dirz_sig[1]);

        h_PvsPZ_sig->Fill(ptot, exiting_pi0_pz_sig->at(0), weight_sig);
        h_PvsPX_sig->Fill(ptot, exiting_pi0_px_sig->at(0), weight_sig);
        h_PvsPY_sig->Fill(ptot, exiting_pi0_py_sig->at(0), weight_sig);
        h_PvsOpang_sig->Fill(opAng, ptot_reco, weight_bkg);

        h_exiting_pi0_px_sig->Fill(exiting_pi0_px_sig->at(0), weight_sig);
        h_exiting_pi0_py_sig->Fill(exiting_pi0_py_sig->at(0), weight_sig);
        h_exiting_pi0_pz_sig->Fill(exiting_pi0_pz_sig->at(0), weight_sig);
        h_exiting_pi0_ptot_sig->Fill(ptot, weight_sig);

    }
    // NC pi0 background loop
    // Fill background histos; filter so as to get Mark's NC pi0's
    // Make sure it (a) passes single photon code, (b) has one exiting pi0, 
    // and (c) isn't a true delta rad. Should get Mark's NC pi0 backgrounds
    for (int j = 0; j < tbkg->GetEntries(); j++) {
        tbkg->GetEntry(j);
        if (quick_pass != 1) continue;
        passed_deltaRad++;

        if (exiting_pi0_number < 1) continue;
        passed_bkg_pi0_cut++;

        if (is_delta_rad == 1) continue;
        passed_isDelta++;

        ptot = sqrt( exiting_pi0_px_bkg->at(0)*exiting_pi0_px_bkg->at(0) +
                     exiting_pi0_py_bkg->at(0)*exiting_pi0_py_bkg->at(0) +
                     exiting_pi0_pz_bkg->at(0)*exiting_pi0_pz_bkg->at(0)
               );

        h_PvsPZ_bkg->Fill(ptot, exiting_pi0_pz_bkg->at(0), weight_bkg);
        h_PvsPX_bkg->Fill(ptot, exiting_pi0_px_bkg->at(0), weight_bkg);
        h_PvsPY_bkg->Fill(ptot, exiting_pi0_py_bkg->at(0), weight_bkg);

        h_exiting_pi0_px_bkg->Fill(exiting_pi0_px_bkg->at(0), weight_bkg);
        h_exiting_pi0_py_bkg->Fill(exiting_pi0_py_bkg->at(0), weight_bkg);
        h_exiting_pi0_pz_bkg->Fill(exiting_pi0_pz_bkg->at(0), weight_bkg);
        h_exiting_pi0_ptot_bkg->Fill(ptot, weight_bkg);

        /*
        h_PvsPZ_bkg->Fill(ptot, exiting_pi0_pz_bkg->at(0), weight_bkg);

        h_exiting_pi0_px_bkg->Fill(exiting_pi0_px_bkg->at(0), weight_bkg);
        h_exiting_pi0_py_bkg->Fill(exiting_pi0_py_bkg->at(0), weight_bkg);
        h_exiting_pi0_pz_bkg->Fill(exiting_pi0_pz_bkg->at(0), weight_bkg);
        h_exiting_pi0_ptot_bkg->Fill(ptot, weight_bkg);
        */
 
    }
    cout << "Passes Mark's selection: " << passed_deltaRad << endl;
    cout << "After pi0==1 cut: " << passed_bkg_pi0_cut << endl;
    cout << "After !is_delta_rad cut: " << passed_isDelta << endl;
    cout << "----------------------" << endl;
    cout << "Passes my selection: " << passed_ncpi0 << endl;

    fout->Write();
    fout->Close();

}

int main() {
    kinematic_comp();
    return 0;
}

