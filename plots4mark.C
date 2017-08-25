

//root 'plots4mark.C("plot_runtmva.root")'


TFile * ifile = nullptr;



TH1F * get_hist(char const * hname) {

  return (TH1F*)ifile->Get(hname);

}



void plots4mark(char const * fname) {

  ifile = TFile::Open(fname);
  
  TH1F * hsp_noweight = get_hist("summed_associated_reco_shower_energy_hsp");
  TH1F * hsp_nuweight = get_hist("summed_associated_reco_shower_energy_nuweighted_hsp");
  TH1F * hsp_pweight = get_hist("summed_associated_reco_shower_energy_pweighted_hsp");

  TCanvas * c1 = new TCanvas("c1");
  TLegend * l = new TLegend(0.6, 0.9, 0.9, 0.6);
  hsp_nuweight->Draw("hist");
  l->AddEntry(hsp_nuweight, "Nu Energy Weight");
  hsp_pweight->Draw("hist same");
  l->AddEntry(hsp_pweight, "#gamma Energy Weight");
  hsp_noweight->Draw("hist same");
  l->AddEntry(hsp_noweight, "No Weight");
  l->Draw();
  

  hsp_nuweight->SetStats(0);
  hsp_nuweight->SetTitle("");
  hsp_nuweight->GetXaxis()->SetTitle("Summed Associated Reco Shower Energy [GeV]");
  hsp_nuweight->GetXaxis()->CenterTitle(); 
  hsp_nuweight->GetYaxis()->SetTitle("Number of Events");
  hsp_nuweight->GetYaxis()->CenterTitle();

  hsp_nuweight->SetFillColor(0);
  hsp_nuweight->SetLineColor(1);

  hsp_pweight->SetFillColor(0);
  hsp_pweight->SetLineColor(2);

  hsp_noweight->SetFillColor(0);
  hsp_noweight->SetLineColor(3);


}
