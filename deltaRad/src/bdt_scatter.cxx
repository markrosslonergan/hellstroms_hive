#include "bdt_scatter.h"

int getBDTScatter(TFile *fout, std::vector<bdt_file*> files, bdt_info cosmic_bdt_info, bdt_info bnb_bdt_info) {

  // Binnings set in your xml file
  std::string bin_cos = cosmic_bdt_info.binning; 
  std::string bin_bnb = bnb_bdt_info.binning; 

  // Delete parentheses from strings so they can be read in later
  bin_cos.erase(std::remove(bin_cos.begin(), bin_cos.end(), '(' ), bin_cos.end());
  bin_cos.erase(std::remove(bin_cos.begin(), bin_cos.end(), ')' ), bin_cos.end());
  bin_bnb.erase(std::remove(bin_bnb.begin(), bin_bnb.end(), '(' ), bin_bnb.end());
  bin_bnb.erase(std::remove(bin_bnb.begin(), bin_bnb.end(), ')' ), bin_bnb.end());

  // Convert binning from comma-delimited string to ints and doubles
  // This is dumb and convoluted but I don't know how else to do it
  int nBins_cos;
  int nBins_bnb;
  std::vector<double> cos_range;
  std::vector<double> bnb_range;
  int count = 0;

  std::stringstream ss_cos(bin_cos);
  while( ss_cos.good() ) {
    std::string substr;
    std::getline( ss_cos, substr, ',' );
    if (count == 0) {
      nBins_cos = atoi(substr.c_str() );
    }
    else {
      cos_range.push_back( atof(substr.c_str()) );
    }
    count++;
  }
  // Same thing for BNB binning
  count = 0;
  std::stringstream ss_bnb(bin_bnb);
  while( ss_bnb.good() ) {
    std::string substr;
    std::getline( ss_bnb, substr, ',' );
    if (count == 0) {
      nBins_bnb = atoi(substr.c_str() );
    }
    else {
      bnb_range.push_back( atof(substr.c_str()) );
    }
    count++;
  }

  std::vector<TH2D*> vec_th2;
  vec_th2.resize(files.size() );
  std::vector<double> cosmic_responses;
  std::vector<double> bnb_responses;

  // Set up histogram and tree branches
  for(int i=0; i < files.size(); i++){
    TString scat_title = "sc_"+files[i]->tag;
    TH2D *sc = new TH2D("sc","", nBins_cos, cos_range.at(0), cos_range.at(1), 
                                 nBins_bnb, bnb_range.at(0), bnb_range.at(1) );
    int col = files[i]->col;
    sc->SetMarkerColor(col);
    sc->SetFillColor(col);
    sc->SetMarkerStyle(8);
    sc->SetMarkerSize(0.5);
    vec_th2.at(i) = sc; // Davio says don't use push_back. He wouldn't lie, would he?

    cosmic_responses.push_back(0);
    files.at(i)->tvertex->SetBranchAddress( (files[i]->tag+"_"+cosmic_bdt_info.identifier+".mva").c_str(), &cosmic_responses.back() );

    bnb_responses.push_back(0);
    files.at(i)->tvertex->SetBranchAddress( (files[i]->tag+"_"+bnb_bdt_info.identifier+".mva").c_str(), &bnb_responses.back() );
  //}

  // Loop over files, get response values for each event
  //for(int i=0; i < files.size(); i++){

    for(int j=0; j < files[i]->tvertex->GetEntries(); ++j){
      files[i]->tvertex->GetEntry(j);

      double cosmic_response  = cosmic_responses[i];
      double bnb_response = bnb_responses[i];

      // Make sure response values aren't garbage, fill histograms
      if(cosmic_response > 0 && bnb_response > 0){
        vec_th2[i]->Fill(cosmic_response, bnb_response);
      }

    }

  }

  TCanvas *c = new TCanvas("c", "", 1000, 800);
  c->cd();
  gStyle->SetOptStat(0);
  gStyle->SetLegendTextSize(0.04);
  TLegend *l = new TLegend(0.1, 0.7, 0.4, 0.9);

  vec_th2[0]->GetXaxis()->SetTitle("Cosmic BDT Response");
  vec_th2[0]->GetYaxis()->SetTitle("BNB BDT Response");
  vec_th2[0]->SetTitle("");
  vec_th2[0]->Draw();
  l->AddEntry(vec_th2[0], "Signal", "f");

  for(int i = 1; i < files.size(); i++){
    vec_th2[i]->Draw("same");
    l->AddEntry(vec_th2[i], "Background", "f");
  }

  c->SetTitle("");
  l->Draw("same");
   
  c->SaveAs("scatterplot_response.pdf", "PDF");
  c->SaveAs("scatterplot_response.png", "PNG");

  return 0;
 
}
