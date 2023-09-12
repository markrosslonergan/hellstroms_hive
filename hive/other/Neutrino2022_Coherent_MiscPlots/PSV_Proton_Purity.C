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

TH1D getTH1fromMap(const std::string& FileDir, const std::map<std::string, std::string>& file_def_map, const std::string& cut_variable_def, const std::string& cut_variable_name, const std::string& cut_variable_binning, std::vector<TH1D>& vhists, double POT_scale=1.0){
   TH1D summed_hist;
   int i = 0;
   std::cout<<"Total number of " << file_def_map.size() << " files " << std::endl;

   for(auto& pair:  file_def_map){
	std::string filename = pair.first, file_def = pair.second;
	std::string f = FileDir+filename;
	std::string nam = filename.substr(0, filename.find(".")) + std::to_string(i);

	TFile* fin = new TFile(f.c_str(), "read");
        TTree* tin = (TTree*)fin->Get("singlephoton/vertex_tree");
        tin->AddFriend("singlephoton/simple_tree");
 	tin->AddFriend("singlephoton/unflatten_1g0pCohNeutrino2022PSV");


	//draw histogram and grab from the directory
 	tin->Draw((cut_variable_def+">>+"+nam + cut_variable_binning).c_str() , ( std::to_string(POT_scale) + "* simple_pot_weight * (" + file_def + ")").c_str(), "gooff");
	TH1D* h1 = (TH1D*)gDirectory->Get(nam.c_str());
	h1->SetTitle( (std::to_string(i+1)+"th distribution;"+cut_variable_name+";Entries").c_str());

	if(i == 0){
	   summed_hist = *(TH1D*)h1->Clone("summed_hist");
	   summed_hist.Reset();
	}
	//std::cout << i << "  " << &summed_hist <<  " " << h1 << std::endl;

	summed_hist.Add(h1);
	vhists.push_back(*h1);

	//std::cout << "File: " << filenames[i] << " has total of " << vhists.back().Integral() << " events" << std::endl;
	delete h1;
	h1 = nullptr;
	fin->Close();
	++i;
   }
   summed_hist.SetTitle( ("Overall summed distribution; " +cut_variable_name+";Entries").c_str());
   return summed_hist;
    
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
   //hs->GetYaxis()->SetTitle("Entries");
   hs->GetYaxis()->SetTitle("Proton vs Non-proton Entries");
   gPad->BuildLegend(0.75,0.75,0.95,0.95,"");
   c->SaveAs(pdfname.c_str(), "pdf");
   return;
}


void PSV_Proton_Purity(){

    //----- INput Files ---------------
    // Manually define signal and background
    std::string plot_tag = "DEFAULT";
    int threshold = 0;
    std::string proton_count = "Sum$(mctruth_exiting_proton_energy-0.93827 >" + std::to_string(threshold/1000.) + ")";
    std::string proton_def = proton_count + ">0", signal_def = proton_count + " == 0";
    std::string FileDir = "/uboone/app/users/gge/Hive/2023_hive/hive/working_directory/BDToptimization/";


    //Count number of proton event
    std::map<std::string, std::string> sig_def_map = {{"sbnfit_1g0pCohNeutrino2022_stage_5_BNBOtherExtra.root", proton_def}, 
					  {"sbnfit_1g0pCohNeutrino2022_stage_5_CC1Pi0.root", proton_def},
				          {"sbnfit_1g0pCohNeutrino2022_stage_5_Dirt.root", proton_def},
					  {"sbnfit_1g0pCohNeutrino2022_stage_5_NCDeltaRadOverlaySMTrueN.root", proton_def}, 
					  {"sbnfit_1g0pCohNeutrino2022_stage_5_NCDeltaRadOverlaySMTrueP.root", "1"},
					  {"sbnfit_1g0pCohNeutrino2022_stage_5_NCPi0NotCoh.root", proton_def},
					  {"sbnfit_1g0pCohNeutrino2022_stage_5_NueOverlays.root", proton_def},
					  {"sbnfit_1g0pCohNeutrino2022_stage_5_OTPCExtra.root", proton_def}};
    std::map<std::string, std::string> bkg_def_map = {{"sbnfit_1g0pCohNeutrino2022_stage_5_NCSingleCoherentTestTextGen.root", "1"},
					  {"sbnfit_1g0pCohNeutrino2022_stage_5_NCPi0Coh.root", "1"},
					  {"sbnfit_1g0pCohNeutrino2022_stage_5_BNBext.root", "1"},
					  {"sbnfit_1g0pCohNeutrino2022_stage_5_BNBOtherExtra.root", signal_def}, 
					  {"sbnfit_1g0pCohNeutrino2022_stage_5_CC1Pi0.root", signal_def},
				          {"sbnfit_1g0pCohNeutrino2022_stage_5_Dirt.root", signal_def},
					  {"sbnfit_1g0pCohNeutrino2022_stage_5_NCDeltaRadOverlaySMTrueN.root", signal_def}, 
					  {"sbnfit_1g0pCohNeutrino2022_stage_5_NCPi0NotCoh.root", signal_def},
					  {"sbnfit_1g0pCohNeutrino2022_stage_5_NueOverlays.root", signal_def},
					  {"sbnfit_1g0pCohNeutrino2022_stage_5_OTPCExtra.root", signal_def}};

    //Coherent and Delta 0p as the only signal 
    /*
    std::map<std::string, std::string> sig_def_map = {{"sbnfit_1g0pCohNeutrino2022_stage_5_NCSingleCoherentTestTextGen.root", "1"},
					  {"sbnfit_1g0pCohNeutrino2022_stage_5_NCPi0Coh.root", "1"},
					  {"sbnfit_1g0pCohNeutrino2022_stage_5_NCDeltaRadOverlaySMTrueN.root", signal_def}}; 
    std::map<std::string, std::string> bkg_def_map = {{"sbnfit_1g0pCohNeutrino2022_stage_5_BNBOtherExtra.root", "1"}, 
					  {"sbnfit_1g0pCohNeutrino2022_stage_5_CC1Pi0.root", "1"},
				          {"sbnfit_1g0pCohNeutrino2022_stage_5_Dirt.root", "1"},
					  {"sbnfit_1g0pCohNeutrino2022_stage_5_NCDeltaRadOverlaySMTrueN.root", proton_def}, 
					  {"sbnfit_1g0pCohNeutrino2022_stage_5_NCDeltaRadOverlaySMTrueP.root", "1"},
					  {"sbnfit_1g0pCohNeutrino2022_stage_5_NCPi0NotCoh.root", "1"},
					  {"sbnfit_1g0pCohNeutrino2022_stage_5_NueOverlays.root", "1"},
					  {"sbnfit_1g0pCohNeutrino2022_stage_5_OTPCExtra.root", "1"},
					  {"sbnfit_1g0pCohNeutrino2022_stage_5_BNBext.root", "1"}};
    */


    //Use any non-proton events as signal 
    /*
    std::map<std::string, std::string> sig_def_map = {{"sbnfit_1g0pCohNeutrino2022_stage_5_NCSingleCoherentTestTextGen.root", "1"},
					  {"sbnfit_1g0pCohNeutrino2022_stage_5_BNBOtherExtra.root", signal_def }, 
					  {"sbnfit_1g0pCohNeutrino2022_stage_5_CC1Pi0.root", signal_def },
				          {"sbnfit_1g0pCohNeutrino2022_stage_5_Dirt.root", signal_def },
					  {"sbnfit_1g0pCohNeutrino2022_stage_5_NCDeltaRadOverlaySMTrueN.root", "1"}, 
					  {"sbnfit_1g0pCohNeutrino2022_stage_5_NCPi0Coh.root", "1"},
					  {"sbnfit_1g0pCohNeutrino2022_stage_5_NCPi0NotCoh.root", signal_def },
					  {"sbnfit_1g0pCohNeutrino2022_stage_5_NueOverlays.root", signal_def },
					  {"sbnfit_1g0pCohNeutrino2022_stage_5_OTPCExtra.root", signal_def }};
    std::map<std::string, std::string> bkg_def_map = {{"sbnfit_1g0pCohNeutrino2022_stage_5_BNBOtherExtra.root", proton_def}, 
					  {"sbnfit_1g0pCohNeutrino2022_stage_5_CC1Pi0.root", proton_def},
				          {"sbnfit_1g0pCohNeutrino2022_stage_5_Dirt.root", proton_def},
					  {"sbnfit_1g0pCohNeutrino2022_stage_5_NCDeltaRadOverlaySMTrueP.root", "1"},
					  {"sbnfit_1g0pCohNeutrino2022_stage_5_NCPi0NotCoh.root", proton_def},
					  {"sbnfit_1g0pCohNeutrino2022_stage_5_NueOverlays.root", proton_def},
					  {"sbnfit_1g0pCohNeutrino2022_stage_5_OTPCExtra.root", proton_def},
					  {"sbnfit_1g0pCohNeutrino2022_stage_5_BNBext.root", "1"}};
   */


   // Defnition and range of cut variable
   std::string cut_variable_def = "Max$((trackstub_candidate_plane==0)*1g0pCohNeutrino2022PSV_score + (trackstub_candidate_plane==2)*1g0pCohNeutrino2022PSV_score)";
   std::string cut_variable_name= "Max PSV score for clusters on P0 and P2 (" + std::to_string(threshold) + "MeV Threshold)";
   double cut_variable_low = 0, cut_variable_high = 1;
   //std::vector<double> bin_edges = {0, 0.3, 0.6, 1.0};
   int Nbins = 50;
   std::string binning = "(" + std::to_string(Nbins) + "," + std::to_string(cut_variable_low) + "," + std::to_string(cut_variable_high) + ")"; 
   std::cout << "Cut on variable : " << cut_variable_name <<" with def: " << cut_variable_def << " and binning: " << binning << std::endl; 

   // POT scaling to 6.8E20
   double POT_scale = 6.8e20/4.566e+19;

   std::string temp = cut_variable_name;
   std::replace(temp.begin(), temp.end(), ' ', '_');
   std::string output_name = plot_tag + "_sig_"+ temp + "_"+std::to_string(Nbins)+"Bin"; 
   //----- End of INput Files ---------------


   TH1D h_total_sig, h_total_bkg; //sum of all signal or background
   std::vector<TH1D> v_sigs, v_bkgs;
  
   h_total_sig = getTH1fromMap(FileDir, sig_def_map, cut_variable_def, cut_variable_name, binning, v_sigs, POT_scale); 
   h_total_bkg = getTH1fromMap(FileDir, bkg_def_map, cut_variable_def, cut_variable_name, binning, v_bkgs, POT_scale); 

   std::cout << "Total signal: " << h_total_sig.Integral() << ", Total background: " << h_total_bkg.Integral() << std::endl;

   TCanvas* c = new TCanvas("c", "c");

   double Nsig = 0, Nbkg = 0;
   TH1D heff("heff", (";"+cut_variable_name+";").c_str(), Nbins, cut_variable_low, cut_variable_high);
   TH1D heff_bkg("heff_bkg", (";"+cut_variable_name+";").c_str(), Nbins, cut_variable_low, cut_variable_high);
   TH1D hpurity("hpurity", (";"+cut_variable_name+";").c_str(), Nbins, cut_variable_low, cut_variable_high);
   TH1D heff_purity("heff_purity", (";"+cut_variable_name+";").c_str(), Nbins, cut_variable_low, cut_variable_high);
   TH1D hsig("hsig", (";"+cut_variable_name+";").c_str(), Nbins, cut_variable_low, cut_variable_high);

   std::ofstream file(FileDir+output_name+".txt", std::ofstream::out);
   
   //for(int i = Nbins; i >= 1; --i){
   for(int i = 1; i <= Nbins; ++i){
       double var = h_total_sig.GetBinCenter(i);;
       //double var = cut_variable_low + (i-1)*(cut_variable_high-cut_variable_low)/Nbins;
       Nsig += h_total_sig.GetBinContent(i);
       Nbkg += h_total_bkg.GetBinContent(i);
       double total = Nsig + Nbkg;

       double eff = Nsig/h_total_sig.Integral();
       double bkg_eff = Nbkg/h_total_bkg.Integral();
       double purity = (total == 0 ? 0 : Nsig/total);
       //double purity = (h_total_sig.GetBinContent(i) + h_total_bkg.GetBinContent(i) == 0 ? 0 : h_total_sig.GetBinContent(i)/(h_total_sig.GetBinContent(i) + h_total_bkg.GetBinContent(i)));
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
   DrawStack(c, h_total_sig, h_total_bkg, cut_variable_name, FileDir+plot_tag+"_Proton_distribution_"+temp+"_"+std::to_string(Nbins)+"Bin.pdf");
 
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
   leg->AddEntry(&hpurity, "Signal Purity (in arbitray unit)", "L");
   leg->AddEntry(&heff, "Signal Efficiency (in arbitray unit)", "L");
   leg->AddEntry(&heff_bkg, "Bkg Eff (in arbitray unit)", "L");
   leg->AddEntry(&heff_purity, "Signal Efficiency*Purity (in arbitray unit)", "L");
   leg->AddEntry(&hsig, "Significance S/#sqrt{B}", "L");
   leg->Draw();

   c->SaveAs((FileDir+output_name+".pdf").c_str(), "pdf");

   return;   

}
