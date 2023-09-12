template <typename T>
std::string to_string_prec(const T a_value, const int n = 6)
{
    std::ostringstream out;
    out <<std::fixed<< std::setprecision(n) << a_value;
    return out.str();
}

TH1D *getTH1(TTree *tin, std::string var, std::string cuts, std::string nam, std::vector<double>  bins){
  
      TH1D* th1 = new TH1D(nam.c_str(),nam.c_str(), bins.size()-1, &(bins)[0]);
      tin->Draw((var+">>+"+nam).c_str() , ("("+cuts+")").c_str(),"goff");
      th1->Sumw2();
      th1->SetLineWidth(1);
      th1->SetStats(0);
      th1->SetDirectory(0);
  
      return th1;
}

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



void effPlot_single(){
    // ********************** NCDelta photons **************************
    TFile *fgen = new TFile("/pnfs/uboone/persistent/users/markross/Jan2022_gLEE_files/UniqDir/Precut2Topo/TestTrainSplit_Jan24_2022/preTopo_vertex_NCPi0_Run123_TestSplit_v50.0.uniq.root","read");
    TTree *tgen = (TTree*)fgen->Get("singlephotonana/vertex_tree");
    TTree *te = (TTree*)fgen->Get("singlephotonana/eventweight_tree");
    tgen->AddFriend(te);
    tgen->AddFriend("unflatten_1g0pCohNeutrino2022PSV", "/pnfs/uboone/persistent/users/markross/Jan2022_gLEE_files/UniqDir/Precut2Topo/Flatten_Neutrino2022_v5/UNFLATTEN_1g0pCohNeutrino2022PSV_NCPi0NotCohFLAT.root");

    std::string run1cut = "run_number >= 4952 && run_number <= 7770";
    std::string truth_cut="mctruth_num_exiting_protons > 0 && GTruth_Gscatter!=5 && mctruth_cc_or_nc==1 &&  mctruth_num_exiting_pi0==1 && m_flash_optfltr_pe_beam > 20 && m_flash_optfltr_pe_veto <  20 && MCFlux_NuPosX > (0.0-1.55) &&MCFlux_NuPosX <  (256.35-1.55) && MCFlux_NuPosY > (-116.5+0.97) && MCFlux_NuPosY < (116.5+0.97) && MCFlux_NuPosZ > (0.0+0.1) && MCFlux_NuPosZ <  (1036.8+0.1)";
    std::string topo_cut = "reco_vertex_size==1 && reco_asso_showers==1  && reco_asso_tracks==0";
    std::string bdt_cut = "Max$(1g0pCohNeutrino2022PSV_score) < 0.5";


    //std::string ebinn = "(10,-1,0.999)";
    //std::string ebinn = "(100,0,0.5)";
    std::vector<double> ebinn = {0, 0.005, 0.01, 0.015, 0.020,0.03, 0.04, 0.05,  0.08, 0.11, 0.15, 0.20, 0.50};
//    for(double b = 0; b <= 0.2; b += 0.005 )
//	ebinn.push_back(b);
//    ebinn.push_back(0.5);

    std::string name_v = "Maximum Kinetic Energy of Protons Leaving Nucleus [GeV]";
    std::string del_v = "mctruth_leading_exiting_proton_energy - 0.938272";
    TH1D *bdt_topo = (TH1D*)getTH1(tgen, "Max$(1g0pCohNeutrino2022PSV_score)", truth_cut + "&&" + topo_cut, "bdt_topo", "(100, 0, 1.0)");
    TH1D *del_topo = (TH1D*)getTH1(tgen, del_v,truth_cut + "&&" + topo_cut,"del_topo",ebinn);
    TH1D *del_bdt_pass = (TH1D*)getTH1(tgen, del_v,truth_cut + "&& " + topo_cut + "&&" +bdt_cut,"del_bdt",ebinn);
    TH1D* del_bdt = (TH1D*)del_topo->Clone("bdt_fail");
    del_bdt->Add(del_bdt_pass, -1.0);

    TCanvas *c = new TCanvas();
    c->cd();
    double x_pos=0.5;
    TText* uboone_txt = new TText();
    uboone_txt->SetTextFont(63);
    uboone_txt->SetTextSize(22);

    
    bdt_topo->SetTitle("BDT score distribution for NC non-coherent 1#pi^{0} background; Max PSV BDT cluster score; Entries");    
    bdt_topo->Draw("hist");
    uboone_txt->DrawText(x_pos, bdt_topo->GetMaximum()*0.8, "MicroBooNE Simulation");
    uboone_txt->DrawText(x_pos, bdt_topo->GetMaximum()*0.7, "Preliminary");
    
    c->SaveAs("PSV_Score_NCpi0_distribution.pdf", "pdf");
   
    c->Clear(); 
    del_bdt->Divide(del_bdt,del_topo,1,1);
    del_bdt->Scale(100.);
    //del_bdt->Draw("E2");
    del_bdt->Draw("hist");
    
    //del_bdt->GetXaxis()->SetTitle("True Photon Cos#theta");
    del_bdt->SetTitle("Rejection efficiency for NC non-coherent 1#pi^{0} background");
    del_bdt->GetXaxis()->SetTitle(name_v.c_str());
    del_bdt->GetYaxis()->SetTitle("Rejection Efficiency[%]");
    del_bdt->SetMinimum(0);

    del_bdt->SetLineColor(kRed-7);
    //del_bdt->SetLineStyle(9);
    //del_bdt->SetFillColorAlpha(kRed-7,0.7);
    del_bdt->SetLineWidth(3);
    TLegend *l1= new TLegend(0.11,0.7,0.5,0.89);
    l1->SetLineWidth(0);
    l1->SetLineColor(kWhite);
    //l1->AddEntry(del_bdt,"#Delta #rightarrow N#gamma (0 proton > 50 MeV KE)","lf");
    //l1->Draw();
	
    x_pos=0.25;
    uboone_txt->DrawText(x_pos, del_bdt->GetMaximum()*0.8, "MicroBooNE Simulation");
    uboone_txt->DrawText(x_pos, del_bdt->GetMaximum()*0.7, "Preliminary");

    c->SaveAs("PSV_rejection_eff_NCpi0.pdf","pdf");
    
}
