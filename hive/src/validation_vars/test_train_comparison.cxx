#include "load_mva_param.h"
#include "tinyxml.h"

#include <getopt.h>


#include "bdt_file.h"
#include "bdt_datamc.h"
#include "bdt_var.h"
#include "bdt_precalc.h"
#include "bdt_info.h"
#include "bdt_train.h"
#include "bdt_app.h"
#include "bdt_response.h"
#include "bdt_recomc.h"
#include "bdt_sig.h"
#include "bdt_spec.h"

int main(){
    /*
       TFile *f1 = new TFile("1g1pMar2020_v4COSMIC_BNBextTest_app.root","read");
       TFile *f2 = new TFile("1g1pMar2020_v4COSMIC_BNBextTrain_app.root","read");
       TTree *t1 = (TTree*)f1->Get("BNBextTest_1g1pMar2020_v4COSMIC");
       TTree *t2 = (TTree*)f2->Get("BNBextTrain_1g1pMar2020_v4COSMIC");
       */
  std::vector<std::string> nam = {"COSMIC","BNB","NCPi0","Nue","SSV"};
    //std::vector<std::string> nam = {"COSMIC","BNB"};
    TCanvas *c=new TCanvas();
    c->Divide(2,3);
   

     for(int i=0; i< nam.size(); i++){
        TFile *f1 = new TFile(("XGBoost_train_output_1g1pNov2020"+nam[i]+".root").c_str(),"read");
        TH1D *h1 = (TH1D*)f1->Get("stest");
        TH1D *h2 = (TH1D*)f1->Get("strain");
        TH1D *b1 = (TH1D*)f1->Get("btest");
        TH1D *b2 = (TH1D*)f1->Get("btrain");
        gStyle->SetOptStat(0);
        TPad*p = (TPad*)c->cd(i+1);
        p->SetLogy();
        TLegend *leg = new TLegend(0.59,0.89,0.59,0.89);
       

         //TH1D *h1 = new TH1D("h1","h1",100,0,1);
        //TH1D *h2 = new TH1D("h2","h2",100,0,1);
        //t1->Draw("mva>>h1","mva>0");
        //t2->Draw("mva>>h2","mva>0");
        

        b1->Scale(1.0/b1->Integral());
        b2->Scale(1.0/b2->Integral());
        b1->SetLineColor(kGreen-3);
        b2->SetLineColor(kMagenta);
        b1->SetLineWidth(2);
        b2->SetLineWidth(2);
        h1->Scale(1.0/h1->Integral());
        h2->Scale(1.0/h2->Integral());
        h1->SetLineColor(kRed-7);
        h2->SetLineColor(kBlue-7);
        h1->SetLineWidth(2);
        h2->SetLineWidth(2);
        h1->Draw("hist");
        h2->Draw("hist same");
        b1->Draw("hist same");
        b2->Draw("hist same");
    

        double mmax = std::max(  std::max(b1->GetMaximum(),b2->GetMaximum()),  std::max(h1->GetMaximum(),h2->GetMaximum()) );
        double mmin = std::min(  std::min(b1->GetMinimum(),b2->GetMinimum()),  std::min(h1->GetMinimum(),h2->GetMinimum()) );
        h1->SetMaximum(mmax*1.2);
        //h1->SetMinimum(mmin*0.9);
        h1->GetXaxis()->SetTitle((nam[i]+" BDT Score").c_str());
        h1->GetYaxis()->SetTitle("Area Normalized Events");
        h1->SetTitle((nam[i]+" BDT Test/Train").c_str());
      
        leg->SetLineWidth(0);
        leg->SetLineColor(kWhite);
        leg->SetNColumns(2);
        leg->SetFillStyle(0);
        leg->AddEntry(h1,"SigTest","l");
        leg->AddEntry(h2,"SigTrain","l");
        leg->AddEntry(b1,"BkgTest","l");
        leg->AddEntry(b2,"BkgTrain","l");
        leg->Draw();
    }
    c->SaveAs("Hope.pdf","pdf");

    return 0;
}
