
TH1 * getTH1(TTree * tin, std::string var, std::string cuts, std::string nam, std::string binning){

    tin->Draw((var+">>"+nam+ binning).c_str() , ("("+cuts+ ")").c_str(),"goff");
    TH1* th1 = (TH1*)gDirectory->Get(nam.c_str()) ;
    th1->Sumw2();
    th1->SetLineWidth(1);
    th1->SetStats(0);
    th1->SetDirectory(0);

    //delete ctmp;
    return th1;
}


void makeP(){


  TFile *f = new TFile("/home/mark/work/uBooNE/photon/Jan2022_MasterFolder/FuchsiaWorldOrder/CoherentRoot/vertex_NCSingleCoherent_Run123_BatchAB_v50.0.uniq.root","read");
  TTree*t = (TTree*)f->Get("singlephotonana/vertex_tree"); 
  
 TH1D* h_en = (TH1D*)getTH1(t, "mctruth_daughters_E[0]","1","e","(100,0,1.0)");

 TCanvas *c = new TCanvas();
 h_en->SetLineColor(kRed-7);
 h_en->SetLineWidth(2);
 h_en->Draw("hist");
 h_en->SetTitle("");
 h_en->GetXaxis()->SetTitle("True Photon Energy [GeV]");
 h_en->GetYaxis()->SetTitle("Events");

 TLegend *l = new TLegend(0.59,0.89,0.59,0.89);
 l->AddEntry(h_en, "NC 1 #gamma Coherent","l");
 l->SetLineColor(kWhite);
 l->Draw();

 c->SaveAs("En.pdf","pdf");
}
