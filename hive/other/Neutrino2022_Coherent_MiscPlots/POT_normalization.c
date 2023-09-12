/* The goal of this macro is to calculate exact POT normalization for simulated NC coherent gamma events, 
 * given microboone flux histogram and genie splines
 *
 * Note: this macro only considers interactions from muon-neutrino contributoin
 *
 * by Guanqun, April 14 2022
 */

//---------- HELPER function -----------------

TH1D ConvolveSplineFlux(TGraph* spline, TH1D* flux, const std::string& process){
// this function generates a histogram representing spline*flux (or, cross section* flux) 

     if(false){
     	std::cout << "Number of bins in flux historgram: " << flux->GetNbinsX() << std::endl;
     	std::cout << "Low edge : " << flux->GetXaxis()->GetXmin() << ", upper edge: " << flux->GetXaxis()->GetXmax() << std::endl;
     	for(int i = 0 ; i != flux->GetNbinsX() ; ++i)
	    std::cout << "Bin: " << i << ", Low edge: "  << flux->GetXaxis()->GetBinLowEdge(i) << std::endl;
     	std::cout << "Last bin upper edge: " << flux->GetXaxis()->GetBinUpEdge(flux->GetNbinsX()) << std::endl;

     	std::cout << "Number of points in spline: " << spline->GetN() << std::endl;
     } 


     double x, y;
     int last_bin = 0, bin_max = flux->GetNbinsX(); // for BNB, flux histogram is only 0-10 GeV, while the spline goes from 0 -100 GeV  

     //histogram related
     std::vector<double> bin_edges = {0}; 
     std::vector<double> bin_content = {0.0};  //initialize for 0th bin
 
     //calculate cross-section averaged flux
     for(int i = 0; i <= spline->GetN(); ++i){

	spline->GetPoint(i, x, y);


	// only calculate integral up to flux's upper limit
	//if(x > flux->GetXaxis()->GetXmax())
	//    break;

	int curr_bin = flux->FindBin(x);
	
	// count number of neutrinos expected from last bin to current bin [in units of neutrinos/POT/cm2]
	double count = 0;
	while(last_bin <= std::min(bin_max, curr_bin)){
	    //count += flux->GetBinContent(last_bin)*h_numu->GetBinWidth(last_bin);     //in case of flux hisotgram shown in per bin width instead of per bin
	    count += flux->GetBinContent(last_bin);
	    ++last_bin;
 	}	

	// add this part of integral to total count
	bin_edges.push_back(flux->GetXaxis()->GetBinUpEdge(last_bin - 1));
	bin_content.push_back(count * y * 1e-38);
	std::cout << "Energy: " << x << "GeV, spline: " << y << "e-38 cm2, counts in Flux histogram in this energy region: " << count <<"nu/POT/cm2 " << std::endl;
	
	if(last_bin > bin_max)
	    break;
     }

     TH1D h_product(Form("h_%s", process.c_str()), Form("%s Flux * Spline", process.c_str()), bin_edges.size()-1, &bin_edges[0]);
     h_product.SetContent(&bin_content[0]);
     h_product.GetXaxis()->SetTitle("#nu Energy [GeV]");
     h_product.GetYaxis()->SetTitle("nu/POT");
     std::cout << "Integral of absolutely normalized flux histogram and genie spline is " << h_product.Integral() << " neutrinos/POT" << std::endl;

     return h_product;
}

//------------ MAIN MACRO ------------
/*
void POT_normalization(){


     //--- Input file -----
     TFile* f_flux = new TFile("/pnfs/uboone/persistent/uboonebeam/bnb_gsimple/bnb_gsimple_fluxes_01.09.2019_463_hist/MCC9_FluxHist_volTPCActive.root", "read");
     TH1D* h_numu = (TH1D*)f_flux->Get("hEnumu_cv");

     TFile* f_spline = new TFile("/uboone/data/users/gge/SinglePhotonAnalysis/CoherentPhotonSample/COHGAMMA/numu_uBooNE_Flux/500k/numu-argon-coh.root", "read");
     // get per-atom cross section from the spline, and corresponding neutrino energy
     // cross-section (i.e. y) is in units of [1e-38 cm^2]
     TGraph* g_spline = (TGraph*)f_spline->Get("nu_mu_Ar40/coh_gamma_nc");

     //To normalize flux historgram to have unit  [nu/POT/bin/cm2] 
     double scale_factor=1/(4997.*5e8)/(256.35*233.);
     h_numu->Scale(scale_factor);

     // MicroBooNE TPC volume & Number of target 
     double dx = 256.35, dy = 116.5*2, dz = 1036.8; // in unit of cm
     double density = 1.3954, mol_argon = 39.948;
     double N_mol = dx*dy*dz*density/mol_argon; //mol of argon in active TPC
     double N_target = N_mol*6.022e23;        //number of argon atom in active TPC
     std::cout << "Number of argon atoms in active TPC : " << N_target << std::endl;

   
     TH1D h = ConvolveSplineFlux(g_spline, h_numu, "numu-argon");  
     for(int i  =0; i <= h.GetNbinsX(); ++i)
     std::cout << "Bin " << i << " "  << h.GetBinContent(i) << std::endl;

     double integral = h.Integral(); 
     double N = 34083.598; 
     std::cout << "Corresponding POT for " << N << " events are : " << N/integral/N_target<< "POT" << std::endl;
     std::cout << "Number of events expected for 6.8e20 POT is: " << integral*N_target*6.8*1e20 << std::endl;

     f_flux->Close();
     f_spline->Close();


     return;
}
*/


void POT_normalization(){


     //--- Input file -----
     TFile* f_flux = new TFile("/pnfs/uboone/persistent/uboonebeam/bnb_gsimple/bnb_gsimple_fluxes_01.09.2019_463_hist/MCC9_FluxHist_volTPCActive.root", "read");
     std::vector<std::string> FNames = {"hEnumu_cv", "hEnumubar_cv", "hEnue_cv", "hEnuebar_cv"};

     std::string SplineLoc = "/uboone/data/users/gge/SinglePhotonAnalysis/CoherentPhotonSample/COHGAMMA/numu_uBooNE_Flux/Splines/";
     std::vector<std::string> Files = {"numu-argon-coh.root", "antinumu-argon-coh.root", "nue-argon-coh.root", "antinue-argon-coh.root"};
     std::vector<std::string> GNames = {"nu_mu_Ar40/coh_gamma_nc", "nu_mu_bar_Ar40/coh_gamma_nc", "nu_e_Ar40/coh_gamma_nc", "nu_e_bar_Ar40/coh_gamma_nc"};
     std::vector<std::string> Labels = {"numu_argon", "antinumu_argon", "nue_argon", "antinue_argon"};

     //----- END of INPUT ------

     std::vector<TFile*> spline_file_vec;
     std::vector<TGraph*> spline_vec;
     std::vector<TH1D*> flux_vec;
     std::vector<TH1D> flux_averaged_spline_vec;

     //To normalize flux historgram to have unit  [nu/POT/bin/cm2] 
     double scale_factor=1/(4997.*5e8)/(256.35*233.);

     for(int i = 0; i != Files.size(); ++i){
     	 spline_file_vec.push_back(new TFile((SplineLoc+Files[i]).c_str(), "read"));

         // get per-atom cross section from the spline, and corresponding neutrino energy
         // cross-section (i.e. y) is in units of [1e-38 cm^2]
     	 spline_vec.push_back((TGraph*)spline_file_vec.back()->Get(GNames[i].c_str()));
         flux_vec.push_back((TH1D*)f_flux->Get(FNames[i].c_str()));
     	 flux_vec.back()->Scale(scale_factor);
     }


     double total_integral = 0;
     for(int i =0; i != Files.size(); ++i){
     	 flux_averaged_spline_vec.push_back(ConvolveSplineFlux(spline_vec[i], flux_vec[i], Labels[i]));  
	 total_integral += flux_averaged_spline_vec.back().Integral();
     }
   
     TFile* fout = new TFile("Flux_averaged_cross_section.root", "recreate");
     TH1D hsum;
     for(int i = 0; i != Files.size(); ++i){
	 flux_averaged_spline_vec[i].Write();
	 if(i == 0){
	    hsum = *((TH1D*)flux_averaged_spline_vec[i].Clone("hsum"));
	    hsum.Reset();
	    hsum.SetTitle("All Flux*Spline"); 
	 }

	 hsum.Add(&flux_averaged_spline_vec[i]);
     }
     hsum.Write();
     TH1D hdiv = *((TH1D*)hsum.Clone("hratio"));
     //hdiv.Add(&flux_averaged_spline_vec[0], -1);
     hdiv.Scale(flux_averaged_spline_vec[0].Integral()/hdiv.Integral());
     hdiv.Divide(&flux_averaged_spline_vec[0]);
     hdiv.SetTitle("Flux*Spline Scaling over numu-argon [after area normalization]; #nu Energy[GeV]; ratio");
     hdiv.Write();
     fout->Close();

     // MicroBooNE TPC volume & Number of target 
     double dx = 256.35, dy = 116.5*2, dz = 1036.8; // in unit of cm
     double density = 1.3954, mol_argon = 39.948;
     double N_mol = dx*dy*dz*density/mol_argon; //mol of argon in active TPC
     double N_target = N_mol*6.022e23;        //number of argon atom in active TPC
     std::cout << "Number of argon atoms in active TPC : " << N_target << std::endl;

     double num_events = 34083.598; 
     std::cout << "Corresponding POT for " << num_events << " events are : " << num_events/total_integral/N_target<< "POT" << std::endl;
     std::cout << "Number of events expected for 6.8e20 POT is: " << total_integral*N_target*6.8*1e20 << std::endl;

     f_flux->Close();
     for(auto& f : spline_file_vec)
	f->Close();


     return;
}

