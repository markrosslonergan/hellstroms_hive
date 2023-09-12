
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

  TFile *f = new TFile("/pnfs/uboone/persistent/users/markross/Jan2022_gLEE_files/UniqDir/vertex_NCSingleCoherent_Run123_BatchA_v50.0.uniq.root","read");
 // TFile *f = new TFile("/home/mark/work/uBooNE/photon/Jan2022_MasterFolder/FuchsiaWorldOrder/CoherentRoot/vertex_NCSingleCoherent_Run123_BatchAB_v50.0.uniq.root","read");
  TTree*t = (TTree*)f->Get("singlephotonana/vertex_tree"); 
  
 //directly get TH2 from TTree 
 if(false){
	 TH2D* h_en = (TH2D*)getTH2(t, "mctruth_daughters_E[0]:mctruth_daughters_pz[0]/sqrt(pow(mctruth_daughters_pz[0],2)+pow(mctruth_daughters_px[0],2)+pow(mctruth_daughters_py[0],2))","1","e","(80,-1,1.0,80,0.14,1.0)");

	 TCanvas *c = new TCanvas();
	 c->SetRightMargin(0.13);  //allow more space for x,y axis labels
	 c->SetBottomMargin(0.15);
	 c->SetLeftMargin(0.1);


	 h_en->Scale(1/h_en->Integral());
	 h_en->Draw("colz");
	 h_en->GetXaxis()->SetTitleSize(0.043);
	 h_en->GetYaxis()->SetTitleSize(0.043);
	 h_en->SetTitle("");
	 h_en->GetYaxis()->SetTitle("True Photon Energy [GeV]");
	 h_en->GetXaxis()->SetTitle("True Photon Angle cos(#theta) Relative to Neutrino Beam");

	 TLegend *l = new TLegend(0.19,0.29,0.49,0.59);
	 l->AddEntry(h_en, "NC 1 #gamma Coherent","l");
	 l->SetLineColor(kWhite);
	 //l->Draw();

	 //draw MicroBooNE on plot
	 double x_pos=(h_en->GetXaxis()->GetXmax() + 5*h_en->GetXaxis()->GetXmin())/5.0;
	 TText* uboone_txt = new TText(x_pos, h_en->GetYaxis()->GetXmax()*0.8, "MicroBooNE Simulation");
	 uboone_txt->SetTextFont(63);
	 uboone_txt->SetTextSize(22);
	 uboone_txt->Draw();
	 uboone_txt->DrawText(x_pos, h_en->GetYaxis()->GetXmax()*0.7, "Preliminary");

	 c->SaveAs("NC_COH_gamma_AngEn2D.pdf","pdf");
 }

 //get topology distribution 
 if(true){
	std::vector<std::string> xlabels = {"0", "1", "2", "3+"};
     	TH2D* htopo = new TH2D("htopo", "Topology Distribution", 4, 0,4, 4, 0,4);
        int num_shower, num_track;
	t->SetBranchAddress("reco_asso_showers", &num_shower);
   	t->SetBranchAddress("reco_asso_tracks", &num_track);

        for(int i = 0 ;i!= t->GetEntries(); ++i){
	    t->GetEntry(i);
	    if(num_shower <= 3 && num_track <= 3)
		htopo->Fill(num_shower, num_track);
	    else if(num_track <= 3)
		htopo->SetBinContent(4, num_track+1, htopo->GetBinContent(4, num_track+1) + 1);
	    else if(num_shower <= 3)
		htopo->SetBinContent(num_shower+1, 4, htopo->GetBinContent(num_shower+1, 4) + 1);
	    else
		htopo->SetBinContent(4, 4, htopo->GetBinContent(4,4)+1);
	}

	htopo->Scale(1/htopo->Integral());
	htopo->SetTitle("Reco Topology Distribution; Number of Reco Showers; Number of Reco Tracks");
	htopo->GetXaxis()->SetLabelSize(0.043);
	htopo->GetYaxis()->SetLabelSize(0.043);
	for(int i = 0; i != xlabels.size(); ++i){
	    htopo->GetXaxis()->SetBinLabel(i+1, xlabels[i].c_str());
	    htopo->GetYaxis()->SetBinLabel(i+1, xlabels[i].c_str());
	}


	TCanvas *c = new TCanvas();
	c->SetRightMargin(0.13);  //allow more space for x,y axis labels
	c->SetBottomMargin(0.15);
	c->SetLeftMargin(0.1);
	gStyle->SetOptStat(0);
      	htopo->SetMarkerColor(kRed);
      	htopo->SetMarkerSize(1.5);
	htopo->Draw("COLZ TEXT");
	c->SaveAs("NC_COH_gamma_Topo.pdf","pdf");
 }

 f->Close();
 return;
}
