void makehist(){

TFile*f=new TFile("delta_BDT_new.root","recreate");
f->cd();
std::vector<double> sig = {0.00616044,0.0936386,0.740484,2.09701,2.41243,2.29168,2.08839,1.87277,1.49575,1.08547,0.845212,0.623436,0.481746,0.332664+0.162636+0.105959+0.0221776+.0172492};
std::vector<double> bkg = {1.21532,19.4451,36.1557,38.2825,36.7634,30.6868,26.1293,23.3949,17.0144,9.41871,10.3302,6.68425,3.94978+3.03829+0.607659+0.303829+0};

std::cout<<sig.size()<<" "<<bkg.size()<<std::endl;

TH1D *hsig = new TH1D("nu_uBooNE_singlephoton_signal","nu_uBooNE_singlephoton_signal",12,0,12); 
TH1D *hbkg = new TH1D("nu_uBooNE_singlephoton_bkg","nu_uBooNE_singlephoton_bkg",12,0,12); 

//double modb = 263.42/255.521;
//double mods = 16.79/15.6524;


double modb = 857.81/255.521;
double mods = 39.485/15.6524;
for(int i=0; i< 12; i++){
	hsig->SetBinContent(i+1, sig.at(i)*mods);
	hbkg->SetBinContent(i+1, bkg.at(i)*modb);
}

hbkg->Rebin();
hsig->Rebin();

hbkg->Draw();
hsig->Draw("same");

std::cout<<hbkg->Integral()<<" "<<hsig->Integral()<<std::endl;
hbkg->Write();
hsig->Write();



f->Close();
}
