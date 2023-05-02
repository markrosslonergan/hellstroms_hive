/* Goal of this macro aims to plot the purity*efficiency and S/sqrt(B) for targeted signal at different cut positions, in order to determine the optimal cut position
 *
 * By Guanqun April 27, 2022
 */

void ReadFile(const std::string& FileDir, const std::vector<std::string>& filenames, const std::string& cut_variable_def, const std::string& cut_variable_name, int nbin, double cut_variable_low, double cut_variable_high, std::vector<TH1D>& vhists, double POT_scale=1.0){

   for(int i = 0; i != filenames.size(); ++i){
	std::string f = FileDir+filenames[i];

	TFile* fin = new TFile(f.c_str(), "read");
        TTree* tin = (TTree*)fin->Get("singlephoton/vertex_tree");
        tin->AddFriend("singlephoton/simple_tree");
 	tin->AddFriend("singlephoton/unflatten_1g0pCohNeutrino2022PSV");

	TTreeFormula* formula = new TTreeFormula(Form("formula_%d", i+1), cut_variable_def.c_str(), tin);
	double eweight;
	tin->SetBranchAddress("simple_pot_weight", &eweight);

	vhists.push_back(TH1D(Form("h_%d", i+1), (std::to_string(i+1)+"th distribution;"+cut_variable_name+";Entries").c_str(), nbin, cut_variable_low, cut_variable_high));
	for(int e = 0 ; e != tin->GetEntries(); ++e){
	    tin->GetEntry(e);
	    formula->GetNdata();
	    double var = formula->EvalInstance();
	    vhists.back().Fill(var, eweight*POT_scale);
	}

	//std::cout << "File: " << filenames[i] << " has total of " << vhists.back().Integral() << " events" << std::endl;
	fin->Close();
   }

   return;
    
}

void AddHists(TH1D* hsum, const std::vector<TH1D>& vhists){
   for(int i = 0 ; i!= vhists.size(); ++i){
	if(i == 0) {
	   *hsum = vhists[0];
	}
   	else hsum->Add(&vhists[i]);
   } 
    
   return;
}

void DrawStack(TCanvas* c, TH1D h_total_sig, TH1D h_total_bkg, const std::string& var_name, const std::string& pdfname){
   c->Clear();
   THStack* hs = new THStack("hs", "");
   h_total_sig.SetTitle("Total Signal");
   h_total_sig.SetFillColor(kOrange);
   h_total_bkg.SetTitle("Total Background");
   h_total_bkg.SetFillColor(kGreen);
   hs->Add(&h_total_sig,"hist");
   hs->Add(&h_total_bkg,"hist");
   hs->Draw();
   hs->GetXaxis()->SetTitle(var_name.c_str());
   hs->GetYaxis()->SetTitle("Entries");
   gPad->BuildLegend(0.75,0.75,0.95,0.95,"");
   c->SaveAs(pdfname.c_str(), "pdf");
   return;
}


void PSV_optimization(){

    //----- INput Files ---------------
    // Manually define signal and background
    std::string FileDir = "/uboone/app/users/gge/hellstroms_hive/hive/working_directory/Jan2022_1g0p/new_coherent_sp/EplusEminus/BenchA/";
    std::vector<std::string> Signal_File = {"sbnfit_1g0pCohNeutrino2022_stage_5_DarkNueEpEmBenchA.root"};
    std::vector<std::string> Bkgd_File = {"sbnfit_1g0pCohNeutrino2022_stage_5_NCSingleCoherentTestTextGen.root",
					  "sbnfit_1g0pCohNeutrino2022_stage_5_BNBext.root", 
					  "sbnfit_1g0pCohNeutrino2022_stage_5_BNBOtherExtra.root", 
					  "sbnfit_1g0pCohNeutrino2022_stage_5_CC1Pi0.root", 
				          "sbnfit_1g0pCohNeutrino2022_stage_5_Dirt.root", 
					  "sbnfit_1g0pCohNeutrino2022_stage_5_NCDeltaRadOverlaySMTrueN.root", 
					  "sbnfit_1g0pCohNeutrino2022_stage_5_NCDeltaRadOverlaySMTrueP.root", 
					  "sbnfit_1g0pCohNeutrino2022_stage_5_NCPi0Coh.root", 
					  "sbnfit_1g0pCohNeutrino2022_stage_5_NCPi0NotCoh.root", 
					  "sbnfit_1g0pCohNeutrino2022_stage_5_NueOverlays.root", 
					  "sbnfit_1g0pCohNeutrino2022_stage_5_OTPCExtra.root"};

   // Defnition and range of cut variable
   std::string cut_variable_def = "(Max$(1g0pCohNeutrino2022PSV_score))";
   std::string cut_variable_name= "Max PSV score in event";
   double cut_variable_low = 0, cut_variable_high = 1;
   int Nbins = 100;
   
   // POT scaling to 6.8E20
   double POT_scale = 6.8e20/4.566e+19;

   //----- End of INput Files ---------------


   TH1D h_total_sig, h_total_bkg; //sum of all signal or background
   std::vector<TH1D> v_sigs, v_bkgs;
   
   ReadFile(FileDir, Signal_File, cut_variable_def, cut_variable_name, Nbins, cut_variable_low, cut_variable_high, v_sigs, POT_scale*0.0231195);
   ReadFile(FileDir, Bkgd_File, cut_variable_def, cut_variable_name, Nbins, cut_variable_low, cut_variable_high, v_bkgs, POT_scale);

   AddHists(&h_total_sig, v_sigs);
   AddHists(&h_total_bkg, v_bkgs);
   std::cout << "Total signal: " << h_total_sig.Integral() << ", Total background: " << h_total_bkg.Integral() << std::endl;

   TCanvas* c = new TCanvas("c", "c");

   double Nsig = 0, Nbkg = 0;
   TH1D heff("heff", (";"+cut_variable_name+";").c_str(), Nbins, cut_variable_low, cut_variable_high);
   TH1D heff_bkg("heff_bkg", (";"+cut_variable_name+";").c_str(), Nbins, cut_variable_low, cut_variable_high);
   TH1D hpurity("hpurity", (";"+cut_variable_name+";").c_str(), Nbins, cut_variable_low, cut_variable_high);
   TH1D heff_purity("heff_purity", (";"+cut_variable_name+";").c_str(), Nbins, cut_variable_low, cut_variable_high);
   TH1D hsig("hsig", (";"+cut_variable_name+";").c_str(), Nbins, cut_variable_low, cut_variable_high);

   std::string temp = cut_variable_name;
   std::replace(temp.begin(), temp.end(), ' ', '_');
   std::string output_name = "PSV_metric_"+ temp + "_"+std::to_string(Nbins)+"Bin"; 
   std::ofstream file(FileDir+output_name+".txt", std::ofstream::out);
   
   for(int i = 1; i <= Nbins; ++i){
       double var = h_total_sig.GetBinCenter(i);;
       //double var = cut_variable_low + (i-1)*(cut_variable_high-cut_variable_low)/Nbins;
       Nsig += h_total_sig.GetBinContent(i);
       Nbkg += h_total_bkg.GetBinContent(i);
       double total = Nsig + Nbkg;

       double eff = Nsig/h_total_sig.Integral();
       double bkg_eff = Nbkg/h_total_bkg.Integral();
       double purity = (total == 0 ? 0 : Nsig/total);
       double sig_bkg_ratio = (Nbkg == 0 ? 0 : Nsig/sqrt(Nbkg));
     
       heff.Fill(var, eff); 
       heff_bkg.Fill(var, bkg_eff);
       hpurity.Fill(var, purity); 
       heff_purity.Fill(var, eff*purity);
       hsig.Fill(var, sig_bkg_ratio);
       file << " Cut: " << var << ", efficiency: " << eff << ", purity: " << purity << ", BKG eff: " << bkg_eff << " e*p: "<< eff*purity <<  ", significance: " << sig_bkg_ratio << std::endl;
   }
   file << "For efficiency*purity, the highest e*p is " << heff_purity.GetMaximum() << ", at " << heff_purity.GetMaximumBin()*(cut_variable_high-cut_variable_low)/Nbins+cut_variable_low << std::endl;
   file << "For significance, the highest significance is " << hsig.GetMaximum() << " " << hsig.GetBinCenter(hsig.GetMaximumBin()) << ", at " << hsig.GetMaximumBin()*(cut_variable_high-cut_variable_low)/(double)Nbins+cut_variable_low << std::endl;
   file.close();


   //plotting
   DrawStack(c, h_total_sig, h_total_bkg, cut_variable_name, FileDir+"PSV_distribution_"+temp+".pdf");
 
   //scale to abitrary unit
   heff.Scale(hsig.Integral()/heff.Integral());
   heff_bkg.Scale(hsig.Integral()/heff_bkg.Integral());
   hpurity.Scale(hsig.Integral()/hpurity.Integral());
   heff_purity.Scale(hsig.Integral()/heff_purity.Integral());

   c->Clear();
   gStyle->SetOptStat(0);
   heff.SetLineColor(kOrange);
   heff_bkg.SetLineColor(kBlue);
   hpurity.SetLineColor(kGreen);
   heff_purity.SetLineColor(kMagenta);
   hsig.SetLineColor(kViolet);
   hsig.SetMaximum(1.5*hsig.GetMaximum());;
   hsig.SetMinimum(0);;
   hsig.Draw("hist");
    
   heff.Draw("hist same");
   heff_bkg.Draw("hist same");
   hpurity.Draw("hist same");
   heff_purity.Draw("hist same");
   TLegend* leg = new TLegend(0.6,0.7,0.95,0.95);
   leg->AddEntry(&hpurity, "Purity (in arbitray unit)", "L");
   leg->AddEntry(&heff, "Efficiency (in arbitray unit)", "L");
   leg->AddEntry(&heff_bkg, "Bkg Eff (in arbitray unit)", "L");
   leg->AddEntry(&heff_purity, "Efficiency*Purity (in arbitray unit)", "L");
   leg->AddEntry(&hsig, "Significance S/#sqrt{B}", "L");
   leg->Draw();

   c->SaveAs((FileDir+output_name+".pdf").c_str(), "pdf");
   return;   
}
