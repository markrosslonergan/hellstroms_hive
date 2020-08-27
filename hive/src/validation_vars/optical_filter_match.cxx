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

    //expecting input --out <output file name with friend tree> --old <oldfile without optical info> --new <newfile with optical info> [--out <name for new file with friend tree>]

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
    old_vertex_tree-> SetBranchStatus("mctruth_nu_E",1);


    int old_run_number;
    int old_subrun_number;
    int old_event_number;
    double old_true_nu_e;
    old_vertex_tree->SetBranchAddress("run_number",    &old_run_number);
    old_vertex_tree->SetBranchAddress("subrun_number", &old_subrun_number);
    old_vertex_tree->SetBranchAddress("event_number",  &old_event_number);
    old_vertex_tree->SetBranchAddress("mctruth_nu_E",  &old_true_nu_e);



    new_vertex_tree-> SetBranchStatus("*",0);
    new_vertex_tree-> SetBranchStatus("run_number",1);
    new_vertex_tree-> SetBranchStatus("subrun_number",1);
    new_vertex_tree-> SetBranchStatus("event_number",1);
    new_vertex_tree-> SetBranchStatus("mctruth_nu_E",1);
    new_vertex_tree-> SetBranchStatus("m_flash_optfltr_pe_beam",1);
    new_vertex_tree-> SetBranchStatus("m_flash_optfltr_pe_veto",1);
    new_vertex_tree-> SetBranchStatus("m_flash_optfltr_pe_veto_tot",1);
    new_vertex_tree-> SetBranchStatus("m_flash_optfltr_pe_beam_tot",1);

    int new_run_number;
    int new_subrun_number;
    int new_event_number;
    double new_flash_optfltr_pe_beam;
    double new_flash_optfltr_pe_veto;
    double new_flash_optfltr_pe_veto_tot;
    double new_flash_optfltr_pe_beam_tot;
    double new_true_nu_e;

    new_vertex_tree->SetBranchAddress("run_number",    &new_run_number);
    new_vertex_tree->SetBranchAddress("subrun_number", &new_subrun_number);
    new_vertex_tree->SetBranchAddress("event_number",  &new_event_number);
    new_vertex_tree->SetBranchAddress("mctruth_nu_E",  &new_true_nu_e);
    new_vertex_tree->SetBranchAddress("m_flash_optfltr_pe_beam",  &new_flash_optfltr_pe_beam);
    new_vertex_tree->SetBranchAddress("m_flash_optfltr_pe_veto",  &new_flash_optfltr_pe_veto);
    new_vertex_tree->SetBranchAddress("m_flash_optfltr_pe_beam_tot",  &new_flash_optfltr_pe_beam_tot);
    new_vertex_tree->SetBranchAddress("m_flash_optfltr_pe_veto_tot",  &new_flash_optfltr_pe_veto_tot);


    //blank ttree for corresponding optical filter info
    TTree* friend_tree = new TTree("opt_tree", "opt_tree");

    double m_flash_optfltr_pe_beam;
    double m_flash_optfltr_pe_veto;
    double m_flash_optfltr_pe_veto_tot;
    double m_flash_optfltr_pe_beam_tot;

    friend_tree->Branch("m_flash_optfltr_pe_beam",&m_flash_optfltr_pe_beam,"flash_optfltr_pe_beam/D");
    friend_tree->Branch("m_flash_optfltr_pe_veto",&m_flash_optfltr_pe_veto,"flash_optfltr_pe_veto/D");
    friend_tree->Branch("m_flash_optfltr_pe_beam_tot",&m_flash_optfltr_pe_beam_tot,"flash_optfltr_pe_beam_tot/D");
    friend_tree->Branch("m_flash_optfltr_pe_veto_tot",&m_flash_optfltr_pe_veto_tot,"flash_optfltr_pe_veto_tot/D");


    //loop over old file
    bool matched = false;
    int total_matched = 0;
    // int size = old_vertex_tree->GetEntries();
   int size = 2000;

   int check = 1;
   int increment = size/20;
   int current = 5;

    //for each entry in vertex tree
    for (int i=0;i <size; i++){
        old_vertex_tree->GetEntry(i);
        
        if (check * increment == i){
            std::cout<<"------------- ["<<current<<"% complete] -------------"<<std::endl;
            current += 5;
            check++;
        }

        //for each entry reset match
        matched = false;
        m_flash_optfltr_pe_beam = -999;
        m_flash_optfltr_pe_veto = -999;
        m_flash_optfltr_pe_veto_tot = -999;
        m_flash_optfltr_pe_beam_tot = -999;


        //   if (i<5){ 
        //     std::cout<<"run/subrun/event = "<<old_run_number<<"/"<<old_subrun_number<<"/"<<old_event_number<<std::endl;
        // }

        //check for corresponding run/subrun/event in the new file   
        for (int j=0;j <new_vertex_tree->GetEntries(); j++){
            new_vertex_tree->GetEntry(j);

            //if there's match get the corresponding optical info
            if (old_run_number == new_run_number && old_subrun_number == new_subrun_number && old_event_number == new_event_number && fabs(new_true_nu_e -old_true_nu_e)< 0.001){

                //print out info               
/*                if (new_flash_optfltr_pe_beam != 0){ 
                    std::cout<<"found match for run/subrun/event = "<<old_run_number<<"/"<<old_subrun_number<<"/"<<old_event_number<<" at entry "<< i<<" in the old file and entry "<<j<<" in the new file with run/subrun/event = "<< new_run_number<<"/"<<new_subrun_number<<"/"<<new_event_number<<std::endl;
                    std::cout<<"------ new_flash_optfltr_pe_beam = "<<new_flash_optfltr_pe_beam<<std::endl;
                    std::cout<<"------ old true nu energy = "<< old_true_nu_e <<", new true nu energy = "<< new_true_nu_e<<std::endl;
                }
*/
                //match found, if there's already a match it's a duplicate
                if (matched== false){
                    matched = true;

                    //if no existing match, add branch to tree
                    m_flash_optfltr_pe_beam = new_flash_optfltr_pe_beam;
                    m_flash_optfltr_pe_veto = new_flash_optfltr_pe_veto;
                    m_flash_optfltr_pe_veto_tot = new_flash_optfltr_pe_veto_tot;
                    m_flash_optfltr_pe_beam_tot = new_flash_optfltr_pe_beam_tot;
                    total_matched++;

                }else{
                    //it's a duplicate, tree already filled
                    //do the values agree?
                    std::cout<<"error, duplicated match"<<std::endl;


                }
            }

        }
        //if no match to old file event found
        if (matched == false)  std::cout<<"no match for run/subrun/event = "<<old_run_number<<"/"<<old_subrun_number<<"/"<<old_event_number<<" at entry "<<i<<std::endl; 
        //if no match, fill -999 for now. will update.
        friend_tree->Fill();

    }

     std::cout<<"Created friend tree with "<<friend_tree->GetEntries()<<" entries. "<< total_matched<<" were matched to an event in the new file."<<std::endl;

    //write ttree to outfile

    f_old->Close();
    f_new->Close();

    std::cout<<"Writing output to "<<out_name<<std::endl;
    friend_tree->Write();
    outfile->Close();
    return 0;

}
