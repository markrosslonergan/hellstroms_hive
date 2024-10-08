
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


void makeC(){

  TFile *f = new TFile("/pnfs/uboone/persistent/users/markross/Jan2022_gLEE_files/UniqDir/vertex_NCSingleCoherent_Run123_BatchA_v50.0.uniq.root","read");
  //TFile *f = new TFile("/home/mark/work/uBooNE/photon/Jan2022_MasterFolder/FuchsiaWorldOrder/CoherentRoot/vertex_NCSingleCoherent_Run123_BatchAB_v50.0.uniq.root","read");
  TTree*t = (TTree*)f->Get("singlephotonana/vertex_tree"); 
  
 TH1D* h_en = (TH1D*)getTH1(t, "mctruth_daughters_pz[0]/sqrt(pow(mctruth_daughters_pz[0],2)+pow(mctruth_daughters_px[0],2)+pow(mctruth_daughters_py[0],2))","1","e","(100,-1,1)");

 TCanvas *c = new TCanvas();
 c->SetRightMargin(0.09);  //allow more space for x,y axis labels
 c->SetBottomMargin(0.15);
 c->SetLeftMargin(0.15);

 h_en->Scale(1/h_en->Integral());
 h_en->SetLineColor(kBlue-7);
 h_en->SetLineWidth(2);
 h_en->GetXaxis()->SetTitleSize(0.043);
 h_en->GetYaxis()->SetTitleSize(0.043);
 h_en->Draw("hist");
 h_en->SetTitle("");
 h_en->GetXaxis()->SetTitle("True Photon Angle cos(#theta)");
 h_en->GetYaxis()->SetTitle("Area-normalized");
 //h_en->GetYaxis()->SetTitle("Events");

 TLegend *l = new TLegend(0.2,0.59,0.59,0.89);
 l->AddEntry(h_en, "NC 1 #gamma Coherent","l");
 l->SetLineColor(kWhite);
 l->Draw();

 //draw MicroBooNE Simulation
 double x_pos=(h_en->GetXaxis()->GetXmax() + 5*h_en->GetXaxis()->GetXmin())/5.0;
 TText* uboone_txt = new TText(x_pos, h_en->GetMaximum()*0.5, "MicroBooNE Simulation");
 uboone_txt->SetTextFont(63);
 uboone_txt->SetTextSize(22);
 uboone_txt->Draw();
 uboone_txt->DrawText(x_pos, h_en->GetMaximum()*0.4, "Preliminary");

 c->SaveAs("NC_COH_gamma_Cos.pdf","pdf");
}
