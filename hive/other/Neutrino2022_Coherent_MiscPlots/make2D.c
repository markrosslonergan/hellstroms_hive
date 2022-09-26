
TH2 * getTH2(TTree * tin, std::string var, std::string cuts, std::string nam, std::string binning){

    tin->Draw((var+">>"+nam+ binning).c_str() , ("("+cuts+ ")").c_str(),"goff");
    TH2* th2 = (TH2*)gDirectory->Get(nam.c_str()) ;
    th2->Sumw2();
    th2->SetStats(0);
    th2->SetDirectory(0);

    //delete ctmp;
    return th2;
}


void make2D(){


  TFile *f = new TFile("/home/mark/work/uBooNE/photon/Jan2022_MasterFolder/FuchsiaWorldOrder/CoherentRoot/vertex_NCSingleCoherent_Run123_BatchAB_v50.0.uniq.root","read");
  TTree*t = (TTree*)f->Get("singlephotonana/vertex_tree"); 
  
 TH2D* h_en = (TH2D*)getTH2(t, "mctruth_daughters_E[0]:mctruth_daughters_pz[0]/sqrt(pow(mctruth_daughters_pz[0],2)+pow(mctruth_daughters_px[0],2)+pow(mctruth_daughters_py[0],2))","1","e","(80,-1,1.0,80,0.14,1.0)");

 TCanvas *c = new TCanvas();
 h_en->Draw("colz");
 h_en->SetTitle("");
 h_en->GetYaxis()->SetTitle("True Photon Energy [GeV]");
 h_en->GetXaxis()->SetTitle("True Photon Cos #theta");

 TLegend *l = new TLegend(0.19,0.19,0.59,0.59);
 l->AddEntry(h_en, "NC 1 #gamma Coherent","l");
 l->SetLineColor(kWhite);
 l->Draw();



 c->SaveAs("AngEn.pdf","pdf");
}
