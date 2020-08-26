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

    //expecting input --out <output file name with friend tree> --old <oldfile without optical info> --new <newfile with optical info>

    std::string old_file = "";
    std::string new_file = "";
    std::string out_name = "optical_match.root";

    const struct option longopts[] =
    {   
        {"old",         required_argument,  0, 'o'},
        {"new",         required_argument,  0, 'n'},
        {"out",        no_argument,  0, 'w'},
        {0,             no_argument,        0,  0},
    };


    int iarg = 0; opterr=1; int index;
    while(iarg != -1)
    {
        iarg = getopt_long(argc,argv, "o:n:w?", longopts, &index);

        switch(iarg)
        {
            case 'o':
                old_file = optarg;             
                break;
            case 'n':
                new_file = optarg;
                break;
            case '2':
                out_name = optarg;
                break;
                return 0;
        }
    }

    if (old_file == "" || new_file == ""){
        std::cout<<"ERROR: expecting input --old <oldfile without optical info> --new <newfile with optical info>"<<std::endl;
        return 0;
    }else{
        std::cout<<"matching old file "<<old_file<<" with new file "<<new_file<<" to get optical filter info"<<std::endl;
    }

    //open input files
    std::cout<<"opening input files "<<std::endl;
    TFile *f_old =new TFile(old_file.c_str());
    TFile *f_new =new TFile(new_file.c_str());

    //output files
    TFile * outfile = new TFile(out_name.c_str(),"recreate");//need to get better name for this
    //===========================================================================================
    //===========================================================================================
    //===========================================================================================
    //===========================================================================================

    //get vertex tree from original files
    std::cout<<"loading vertex trees"<<std::endl;

    TTree * old_vertex_tree = (TTree*)f_old->Get("singlephotonana/vertex_tree");
    TTree * new_vertex_tree = (TTree*)f_new->Get("singlephotonana/vertex_tree");

    //only load the run/subrun/event for both trees
    std::cout<<"the old file has "<< old_vertex_tree->GetEntries()<<" events, the new file has "<<new_vertex_tree->GetEntries() <<" events"<<std::endl;

    old_vertex_tree-> SetBranchStatus("*",0);
    old_vertex_tree-> SetBranchStatus("run_number",1);
    old_vertex_tree-> SetBranchStatus("subrun_number",1);
    old_vertex_tree-> SetBranchStatus("event_number",1);

    int old_run_number;
    int old_subrun_number;
    int old_event_number;
    old_vertex_tree->SetBranchAddress("run_number",    &old_run_number);
    old_vertex_tree->SetBranchAddress("subrun_number", &old_subrun_number);
    old_vertex_tree->SetBranchAddress("event_number",  &old_event_number);


    new_vertex_tree-> SetBranchStatus("*",0);
    new_vertex_tree-> SetBranchStatus("run_number",1);
    new_vertex_tree-> SetBranchStatus("subrun_number",1);
    new_vertex_tree-> SetBranchStatus("event_number",1);

    new_vertex_tree-> SetBranchStatus("flash_optfltr_pe_beam",1);


    int new_run_number;
    int new_subrun_number;
    int new_event_number;
    double new_flash_optfltr_pe_beam;
    new_vertex_tree->SetBranchAddress("run_number",    &new_run_number);
    new_vertex_tree->SetBranchAddress("subrun_number", &new_subrun_number);
    new_vertex_tree->SetBranchAddress("event_number",  &new_event_number);

    new_vertex_tree->SetBranchAddress("flash_optfltr_pe_beam",  &new_flash_optfltr_pe_beam);


    //also keeping track of duplicates



    //blank ttree for corresponding optical filter info

    //loop over old file
    //for each entry in vertex tree
    bool matched = false;
    //    for (int i=0;i < old_vertex_tree->GetEntries(); i++){
    for (int i=0;i <5; i++){
        old_vertex_tree->GetEntry(i);
        matched = false;
        //   if (i<5){ 
        //     std::cout<<"run/subrun/event = "<<old_run_number<<"/"<<old_subrun_number<<"/"<<old_event_number<<std::endl;
        // }

        //check for corresponding run/subrun/event in the new file   
        for (int j=0;j <new_vertex_tree->GetEntries(); j++){
            new_vertex_tree->GetEntry(j);
            //if there's match get the corresponding optical info
            if (old_run_number == new_run_number && old_subrun_number == new_subrun_number && old_event_number == new_event_number){
                if (i<5){ 
                    std::cout<<"found match for run/subrun/event = "<<old_run_number<<"/"<<old_subrun_number<<"/"<<old_event_number<<" at entry "<< i<<" in the old file and entry "<<j<<" in the new file"<<std::endl;
                    std::cout<<"------ new_flash_optfltr_pe_beam = "<<new_flash_optfltr_pe_beam<<std::endl;
                }
                if (matched== false){
                    matched = true;
                }else{
                    std::cout<<"error, duplicated match"<<std::endl;


                }
            }

        }
        if (matched == false)  std::cout<<"no match for run/subrun/event = "<<old_run_number<<"/"<<old_subrun_number<<"/"<<old_event_number<<" at entry "<<i<<std::endl; 

    }


    //if there's a match, add branch to tree
    //if no match, fill rando values
    //write ttree to outfile

    f_old->Close();
    f_new->Close();

    std::cout<<"Writing output to "<<out_name<<std::endl;
    outfile->Close();
    return 0;

}
