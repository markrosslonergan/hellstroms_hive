TH1 * getTH1(TTree * tin, std::string var, std::string cuts, std::string nam, std::string binning){

    tin->Draw((var+">>"+nam+ binning).c_str() , ("("+cuts+ ")").c_str(),"goff");
    TH1* th1 = (TH1*)gDirectory->Get(nam.c_str()) ;
      th1->Sumw2();
    th1->SetLineWidth(1);
    th1->SetStats(0);
    th1->SetDirectory(0);

    return th1;
}


void quick_h_division(){

    TFile* f1 = new TFile("/pnfs/uboone/persistent/users/markross/Jan2022_gLEE_files/UniqDir/Precut2Topo/TestTrainSplit_Jan24_2022/preTopo_vertex_NCSingleCoherent_Run123_TestSplit_v50.0.uniq.root", "read");
    TTree* t1 = (TTree*)f1->Get("singlephotonana/vertex_tree");

    TH1D* h1 = (TH1D*)getTH1(t1, "mctruth_daughters_E[1]", "1", "preTopo", "(100, 0, 7)");

    TFile* f2 = new TFile("/uboone/app/users/gge/hellstroms_hive/hive/working_directory/Jan2022_1g0p/new_coherent_sp/TestOptimization/sbnfit_1g0pCohNeutrino2022_stage_5_NCSingleCoherentTestTextGen.root", "read");
    TTree* t2 = (TTree*)f2->Get("singlephoton/vertex_tree");

    TH1D* h2 = (TH1D*)getTH1(t2, "mctruth_daughters_E[1]", "1", "final", "(100, 0, 7)");

    TCanvas* c = new TCanvas("c", "c");
    h1->SetTitle("Number of NC coherent 1#gamma events; True #nu Energy [GeV]; Entries");
    h1->Draw("hist");
    h2->Draw("same");
    c->SaveAs("QuickOverlay.pdf", "pdf");
    h2->Divide(h1);
    h2->Scale(100);
    h2->SetTitle("Selection Efficiency as function of true #nu energy; True #nu Energy [GeV]; Efficiency [%]");
    c->Clear();
    h2->Draw("hist");
    c->SaveAs("QuickDivision.pdf", "pdf");
   return 0;
}
