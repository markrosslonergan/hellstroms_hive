#include "kinematic_comp.h"
using std::cout;
using std::endl;

void kinematic_comp() {
    // Get files and trees
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

    // Declare variables from trees. "sig" denotes my NC pi0 signal file,
    // "bkg" denotes Mark's NC pi0-tagged backgrounds
    // Large number here (25) to avoid "stack smashing"
    //std::size_t const N = 100;
    int const numLines = 154; // From STAGE_LIST_BNB
    int run_bkg[numLines], subrun_bkg[numLines], event_bkg[numLines];
    int passed_topological_selection_sig, passed_precuts_sig, passed_cosmic_bdt_cut_sig, passed_bnb_bdt_cut_sig;
    int exiting_pi0_number;
    int is_delta_rad;
    double weight_bkg, weight_sig;
    double reco_nuvertx[numLines];

    int run_number_bkg, subrun_number_bkg, event_number_bkg;
    double reco_nuvertx_bkg;

    std::vector<double> *exiting_pi0_px_bkg = 0;
    std::vector<double> *exiting_pi0_py_bkg = 0;
    std::vector<double> *exiting_pi0_pz_bkg = 0;
    std::vector<double> *exiting_pi0_px_sig = 0;
    std::vector<double> *exiting_pi0_py_sig = 0;
    std::vector<double> *exiting_pi0_pz_sig = 0;

    // Set branches to be used
    // Signal and background files use the same variable names, so be careful!
    TBranch *bexiting_pi0_px_bkg;
    TBranch *bexiting_pi0_py_bkg;
    TBranch *bexiting_pi0_pz_bkg;
    TBranch *bexiting_pi0_px_sig;
    TBranch *bexiting_pi0_py_sig;
    TBranch *bexiting_pi0_pz_sig;

    tbkg->SetBranchAddress("exiting_pi0_number", &exiting_pi0_number);
    tbkg->SetBranchAddress("exiting_pi0_px", &exiting_pi0_px_bkg, &bexiting_pi0_px_bkg);
    tbkg->SetBranchAddress("exiting_pi0_py", &exiting_pi0_py_bkg, &bexiting_pi0_py_bkg);
    tbkg->SetBranchAddress("exiting_pi0_pz", &exiting_pi0_pz_bkg, &bexiting_pi0_pz_bkg);
    tbkg->SetBranchAddress("run_number", &run_number_bkg);
    tbkg->SetBranchAddress("subrun_number", &subrun_number_bkg);
    tbkg->SetBranchAddress("event_number", &event_number_bkg);
    tbkg->SetBranchAddress("reco_nuvertx", &reco_nuvertx_bkg);
    tbkg->SetBranchAddress("is_delta_rad", &is_delta_rad);

    tsig->SetBranchAddress("exiting_pi0_px", &exiting_pi0_px_sig, &bexiting_pi0_px_sig);
    tsig->SetBranchAddress("exiting_pi0_py", &exiting_pi0_py_sig, &bexiting_pi0_py_sig);
    tsig->SetBranchAddress("exiting_pi0_pz", &exiting_pi0_pz_sig, &bexiting_pi0_pz_sig);
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
    TH2D *h_PvsPZ_sig = new TH2D("h_PvsPZ_sig", "h_PvsPZ_sig", 50, 0, 1.5, 50, 0, 1.5);
    TH2D *h_PvsPZ_bkg = new TH2D("h_PvsPZ_bkg", "h_PvsPZ_bkg", 50, 0, 1.5, 50, 0, 1.5);

    int passed_sig = 0;
    int passed_bkg = 0;
    int passed_deltaRad = 0;
    int passed_bkg_pi0_cut = 0;
    int passed_ncpi0 = 0;
    int passed_bkg_notDeltaRad = 0;
    double ptot = 0;
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
        h_PvsPZ_sig->Fill(ptot, exiting_pi0_pz_sig->at(0), weight_sig);

        h_exiting_pi0_px_sig->Fill(exiting_pi0_px_sig->at(0), weight_sig);
        h_exiting_pi0_py_sig->Fill(exiting_pi0_py_sig->at(0), weight_sig);
        h_exiting_pi0_pz_sig->Fill(exiting_pi0_pz_sig->at(0), weight_sig);
        h_exiting_pi0_ptot_sig->Fill(ptot, weight_sig);

    }
    // Read in values for run, subrun, event, and reco_nuvertx for
    // events that pass Mark's selection
    std::ifstream in;
    in.open("STAGE_LIST_BNB");
    if(in.fail() ) {
        cout << "Bad data file" << endl;
        return;
    }
    for(int i = 0; i < numLines; i++) {
        in >> run_bkg[i] >> subrun_bkg[i] >> event_bkg[i] >> reco_nuvertx[i];
        cout << "run_bkg[" << i << "]: " << run_bkg[i] << endl;
        cout << "subrun_bkg[" << i << "]: " << subrun_bkg[i] << endl;
        cout << "event_bkg[" << i << "]: " << event_bkg[i] << endl;
        cout << "reco_nuvertx[" << i << "]: " << reco_nuvertx[i] << endl;
    }
    // NC pi0 background loop
    // Fill background histos; filter so as to get Mark's NC pi0's
    // Make sure it (a) passes single photon code, (b) has one exiting pi0, 
    // and (c) isn't a true delta rad. Should get Mark's NC pi0 backgrounds
    int totalMatched = 0;
    for (int j = 0; j < tbkg->GetEntries(); j++) {
        tbkg->GetEntry(j);
        cout << "run_number_bkg = " << run_number_bkg << endl;
        cout << "subrun_number_bkg = " << subrun_number_bkg << endl;
        cout << "event_number_bkg = " << event_number_bkg << endl;
        cout << "reco_nuvertx_bkg = " << reco_nuvertx_bkg << endl;
        int matchedRun = 0;
        int matchedSubrun = 0;
        int matchedEvent = 0;
        int matchedVertx = 0;
        // Loop over runs, see if this run number matches any of Mark's selection;
        // do the same for subrun, event, and reco_nuvertx
        for (int k = 0; k < numLines; k++) {
            if (run_bkg[k] == run_number_bkg) {
                cout << "Matched run " << run_number_bkg << " to " << run_bkg[k] 
                     << " for entry " << j << endl;
                matchedRun = 1;
            }
        }
        if (matchedRun != 1) continue;

        for (int k = 0; k < numLines; k++) {
            if (subrun_bkg[k] == subrun_number_bkg) {
                cout << "Matched subrun " << subrun_number_bkg << " to " << subrun_bkg[k] 
                     << " for entry " << j << endl;
                matchedSubrun = 1;
            }
        }
        if (matchedSubrun != 1) continue;

        for (int k = 0; k < numLines; k++) {
            if (subrun_bkg[k] == event_number_bkg) {
                cout << "Matched event " << event_number_bkg << " to " << event_bkg[k] 
                     << " for entry " << j << endl;
                matchedEvent = 1;
            }
        }
        if (matchedEvent != 1) continue;

        for (int k = 0; k < numLines; k++) {
            if (reco_nuvertx[k] == reco_nuvertx_bkg) {
                cout << "Matched event " << reco_nuvertx_bkg << " to " << reco_nuvertx[k]
                     << " for entry " << j << endl;
                matchedVertx = 1;
            }
        }
        if (matchedVertx != 1) continue;

        totalMatched++;

        /*
        passed_bkg_pi0_cut++;

        ptot = sqrt( exiting_pi0_px_bkg->at(0)*exiting_pi0_px_bkg->at(0) +
                     exiting_pi0_py_bkg->at(0)*exiting_pi0_py_bkg->at(0) +
                     exiting_pi0_pz_bkg->at(0)*exiting_pi0_pz_bkg->at(0)
               );
        h_PvsPZ_bkg->Fill(ptot, exiting_pi0_pz_bkg->at(0), weight_bkg);

        h_exiting_pi0_px_bkg->Fill(exiting_pi0_px_bkg->at(0), weight_bkg);
        h_exiting_pi0_py_bkg->Fill(exiting_pi0_py_bkg->at(0), weight_bkg);
        h_exiting_pi0_pz_bkg->Fill(exiting_pi0_pz_bkg->at(0), weight_bkg);
        h_exiting_pi0_ptot_bkg->Fill(ptot, weight_bkg);
        */
 
    }
    in.close();
    cout << "Total matched: " << totalMatched << endl;
    cout << "Passes Mark's selection: " << passed_deltaRad << endl;
    cout << "After pi0==1 cut: " << passed_bkg_pi0_cut << endl;
    cout << "----------------------" << endl;
    cout << "Passes my selection: " << passed_ncpi0 << endl;

    fout->Write();
    fout->Close();

}

int main() {
    kinematic_comp();
    return 0;
}

