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


    TFile *odd_file =new TFile(odd_file_name.c_str());
    TFile *even_file =new TFile(even_file_name.c_str());


    //output files
    TFile * outfile = new TFile("NCpi0_Validation_split_train.root","recreate");
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

    //close files  
    odd_file->Close();
    even_file->Close();

    outfile->cd();


    //start plotting
    TCanvas *c_error = new TCanvas("c_error","",900,900);

    c_error->cd();

    TLegend *lgr = new TLegend(0.8,0.8,0.6,0.6);

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

    even_min->SetLineColor(kCyan);
    even_min->SetMarkerStyle(29);
    even_min->SetMarkerSize(3);
    even_min->Draw("p");

    odd_min->SetLineColor(kCyan);
    odd_min->SetMarkerStyle(29);
    odd_min->SetMarkerSize(3);
    odd_min->Draw("p");

    lgr->AddEntry(even_train,"Even Train","f");
    lgr->AddEntry(even_test,"Even Test","f");
    lgr->AddEntry(odd_train,"Odd Train","f");
    lgr->AddEntry(odd_test,"Odd Test","f");
    lgr->SetLineWidth(0);
    //   lgr->SetLineColor(kWhite);
    lgr->SetFillStyle(0);
    lgr->Draw();


    c_error->Update();
    c_error->Write();


    std::cout<<"writing to file NCpi0_Validation_split_train.root"<<std::endl;
    outfile->Close();
    return 0;

}
