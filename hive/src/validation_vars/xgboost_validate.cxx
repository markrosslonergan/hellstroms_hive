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

	//close files
    odd_file->Close();
	even_file->Close();
	
	outfile->Close();
	return 0;

}
