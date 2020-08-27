/*#include "object_helper.h"
#include "get_pot_info.h"
#include "train.h"
#include "app.h"
#include "merge.h"
#include "significance.h"
#include "get_mva_response_hists.h"
#include "plot_mva_response_hists.h"
#include "gen_tlimits.h"
#include "plotstack.h"*/
//#include "data_mc_testing_suite.h"
//#include "efficiency.h"

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




int main (int argc, char *argv[]){

    // Just some simple argument things
    //===========================================================================================

    //input files
    std::string odd_file_name = "/uboone/app/users/ksutton/hellstroms_hive_2.5_sl7/hellstroms_hive/hive/build/src/training_tests_8_17_20_odd/XGBoost_Validation_1g1pMar2020_v4NCPi0.root";

    std::string even_file_name = "/uboone/app/users/ksutton/hellstroms_hive_2.5_sl7/hellstroms_hive/hive/build/src/training_tests_8_17_20_even/XGBoost_Validation_1g1pMar2020_v4NCPi0.root";

    std::string full_file_name = "/uboone/app/users/ksutton/hellstroms_hive_2.5_sl7/hellstroms_hive/hive/build/src/training_tests_8_17_20/XGBoost_Validation_1g1pMar2020_v4NCPi0.root";

    TFile *odd_file =new TFile(odd_file_name.c_str());
    TFile *even_file =new TFile(even_file_name.c_str());
    TFile *full_file =new TFile(full_file_name.c_str());

    bool plot_full= false;


    //output files
    //  TFile * outfile = new TFile("NCpi0_Validation_split_train.root","recreate");
    //===========================================================================================
    //===========================================================================================
    //===========================================================================================
    //===========================================================================================

    std::cout<<"starting xgboost validaton comparison"<<std::endl;

    //get output canvases
    //TCanvas *c_err_odd = (TCanvas*)odd_file->Get("c_error");
    //TCanvas *c_err_even = (TCanvas*)even_file->Get("c_error");

    //get log loss plots
    TGraph * even_test = (TGraph*)even_file->Get("g_test");
    TGraph * even_train = (TGraph*)even_file->Get("g_train");
    TGraph * even_min = (TGraph*)even_file->Get("g_min");
    TGraph * odd_test = (TGraph*)odd_file->Get("g_test");
    TGraph * odd_train = (TGraph*)odd_file->Get("g_train");
    TGraph * odd_min = (TGraph*)odd_file->Get("g_min");

    TGraph * full_test = (TGraph*)full_file->Get("g_test");
    TGraph * full_train = (TGraph*)full_file->Get("g_train");
    TGraph * full_min = (TGraph*)full_file->Get("g_min");

    //TGraph optimized_cut = new TGraph();
    TLine *line = new TLine(650,0,650,0.8);

    //close files  
    odd_file->Close();
    even_file->Close();
    full_file->Close();


    // outfile->cd();


    //start plotting
    TCanvas *c_error = new TCanvas("c_error","",900,900);

    c_error->cd();

    TLegend *lgr = new TLegend(0.2,0.8,0.8,0.9);

    even_test->Draw("AL");
    // even_test->SetTitle("Negative Log-Likelihood");

    even_train->Draw("same AL");
    even_train->SetTitle("Negative Log-Likelihood");
    even_train->SetLineColor(kRed);
    even_train->SetLineWidth(2);
    even_test->Draw("same CL");
    even_test->SetLineColor(kBlue);
    even_test->SetLineWidth(2);

    odd_test->Draw("same CL");
    odd_test->SetLineColor(kGreen);
    odd_test->SetLineWidth(2);

    odd_train->Draw("same CL");
    odd_train->SetLineColor(kOrange);
    odd_train->SetLineWidth(2);

    if(plot_full){
        full_test->Draw("same CL");
        full_test->SetLineColor(kViolet);
        full_test->SetLineWidth(2);

        full_train->Draw("same CL");
        full_train->SetLineColor(kYellow);
        full_train->SetLineWidth(2);
    }

    even_min->SetLineColor(kCyan);
    even_min->SetMarkerStyle(29);
    even_min->SetMarkerSize(3);
    even_min->Draw("p");

    odd_min->SetLineColor(kCyan);
    odd_min->SetMarkerStyle(29);
    odd_min->SetMarkerSize(3);
    odd_min->Draw("p");

    if(plot_full){
        full_min->SetLineColor(kCyan);
        full_min->SetMarkerStyle(29);
        full_min->SetMarkerSize(3);
        full_min->Draw("p");
    }

    line->SetLineWidth(3);
   // line->Draw();

    lgr->AddEntry(even_train,"Even Train","f");
    lgr->AddEntry(odd_train,"Odd Train","f");

    if(plot_full) lgr->AddEntry(full_train,"Full Train","f");

    lgr->AddEntry(even_test,"Even Test","f");
    lgr->AddEntry(odd_test,"Odd Test","f");
    if(plot_full)  lgr->AddEntry(full_test,"Full Test","f");

    lgr->SetLineWidth(0);
    //   lgr->SetLineColor(kWhite);
    lgr->SetFillStyle(0);
    if(plot_full){
        lgr->SetNColumns(3);
    }else{
        lgr->SetNColumns(2);
    }
    lgr->Draw();


    c_error->Update();
    //   c_error->Write();
    c_error->SaveAs("NCpi0_Validation_split_train.pdf","pdf");


    std::cout<<"writing to file NCpi0_Validation_split_train.pdf"<<std::endl;
    //  outfile->Close();
    return 0;

}
