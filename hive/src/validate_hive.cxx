#include <getopt.h>
#include <sys/stat.h> 

#include "TFriendElement.h"
#include "TList.h"

#include "variable_list.h"
#include "bdt_file.h"
#include "bdt_datamc.h"
#include "bdt_var.h"
#include "bdt_varplot.h"
#include "bdt_precalc.h"
#include "bdt_info.h"
#include "bdt_train.h"
#include "bdt_app.h"
#include "bdt_response.h"
#include "bdt_recomc.h"
#include "bdt_sig.h"
#include "bdt_boxcut.h"
#include "bdt_spec.h"
#include "bdt_eff.h"
#include "bdt_test.h"
#include "bdt_scatter.h"
#include "load_mva_param.h"
#include "tinyxml.h"

int compareQuick(bdt_variable var, std::vector<bdt_file*> files, std::vector<std::string> cuts, std::string name);
int compareQuick(bdt_variable var, std::vector<bdt_file*> files, std::vector<std::string> cuts, std::string name,bool shape_only);


int main (int argc, char *argv[]){

    //This is a standardized location on /pnfs/ that everyone can use. 
    std::string dir = "/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_mcc9_v17/";

    std::string mode_option = "enter_a_mode_please"; 
    std::string xml = "default.xml";
    std::string topo_tag = "this_is_xml_now";

    int number = -1;
    bool response_only = false;
    double what_pot = 13.2e20;
    int which_file = -1;
    int which_bdt = -1;
    int which_stage = -1;
    std::string vector = "";
    std::string input_string = "";

    //All of this is just to load in command-line arguments, its not that important
    const struct option longopts[] = 
    {
        {"dir", 		required_argument, 	0, 'd'},
        {"option",		required_argument,	0, 'o'},
        {"input",		required_argument,	0, 'i'},
        {"xml"	,		required_argument,	0, 'x'},
        {"topo_tag",	required_argument,	0, 't'},
        {"bdt",		    required_argument,	0, 'b'},
        {"stage",		required_argument,	0, 's'},
        {"help",		required_argument,	0, 'h'},
        {"pot",		    required_argument,	0, 'p'},
        {"number",		required_argument,	0, 'n'},
        {"response",	no_argument,	    0, 'r'},
        {"file",		required_argument,	0, 'f'},
        {"vector",      required_argument,  0, 'v'},
        {0,			    no_argument, 		0,  0},
    };

    int iarg = 0; opterr=1; int index;
    while(iarg != -1)
    {
        iarg = getopt_long(argc,argv, "x:o:d:s:f:t:p:b:i:n:v:rh?", longopts, &index);

        switch(iarg)
        {
            case 'r':
                response_only = true;;
                break;
            case 'f':
                which_file = (int)strtof(optarg,NULL);
                break;
            case 'n':
                number = strtof(optarg,NULL);
                break;
            case 'x':
                xml = optarg;
                break;
            case 'b':
                which_bdt = (int)strtof(optarg,NULL);
                break;
            case 'o':
                mode_option = optarg;
                break;
            case 'd':
                dir = optarg;
                break;
            case 'p':
                what_pot = strtod(optarg,NULL);
                std::cout<<"Setting POT to : "<<what_pot<<std::endl;
                break;
            case 's':
                which_stage = strtod(optarg,NULL);
                break;
            case 't':
                topo_tag = optarg;
                break;
            case 'v':
                vector = optarg;
                break;
            case 'i':
                input_string = optarg;
                break;
            case '?':
            case 'h':
                std::cout<<"Allowed arguments:"<<std::endl;
                std::cout<<"\t-x\t--xml\t\tInput .xml file for configuring what MVA/BDT & param"<<std::endl;
                std::cout<<"\t-o\t--option\t\tOptional mode to run, train, app..etc.."<<std::endl;
                std::cout<<"\t\t\t\t --Options are:"<<std::endl;
                std::cout<<"\t\t\t\t train: Train all BDT (Or just BDT number B defined with -b/--bdt B)"<<std::endl;
                std::cout<<"\t\t\t\t app: Apply the BDT to all files in bdt_files, (Or just file F --f/--file on BDB -b/--bdt B)"<<std::endl;
                std::cout<<"\t\t\t\t response: Make a training BDT response [bit obsolete, -o train makes it anyway] "<<std::endl;
                std::cout<<"\t\t\t\t sig: Significance scan, combined with -n/--number N "<<std::endl;
                std::cout<<"\t\t\t\t\t -- N=0 (default) Do a simple box scan between XML defined values "<<std::endl;
                std::cout<<"\t\t\t\t\t -- N=1 Combine BDT's into a likelihood (experimental) "<<std::endl;
                std::cout<<"\t\t\t\t\t -- N=2 Random points within XML defined regions (experimental) "<<std::endl;
                std::cout<<"\t\t\t\t stack: Produce a Stacked PDF for BDT variable -n/--number N, at stage -s/--stage S for POT -p/--pot P"<<std::endl;
                std::cout<<"\t\t\t\t datamc: Produce a Stacked MV V data PDF for BDT variable -n/--number N, at stage -s/--stage S for POT -p/--pot P"<<std::endl;
                std::cout<<"\t\t\t\t recomc:"<<std::endl;
                std::cout<<"\t\t\t\t eff: Produce an efficiency plot for 1g1p for stage -s/--stage S"<<std::endl;
                std::cout<<"\t\t\t\t tplot: Produces a test/training for BDT -b/--bdt B (auto ran at train mode too)"<<std::endl;
                std::cout<<"\t\t\t\t sbnfit: Makes a file at stage S (set with --stage S) for file set with --file. Can also make a flattened simpletree based on a variable passed in with argument `i` "<<std::endl;
                std::cout<<"\t\t\t\t vars: Prints training variables"<<std::endl;
                std::cout<<"\t-i\t--input\t\t An input generic input_string, used in a variety of places, notably sbnfit mode"<<std::endl;
                std::cout<<"\t-b\t--bdt\t\t Run only N BDT training/app, or BDT specific option"<<std::endl;
                std::cout<<"\t-f\t--file\t\t Which file in bdt_files you want to run over, for file specifc options."<<std::endl;
                std::cout<<"\t-p\t--pot\t\tSet POT for plots"<<std::endl;
                std::cout<<"\t-s\t--stage\t\tSet what stage to do things at."<<std::endl;
                std::cout<<"\t-r\t--response\t\t Run only BDT response plots for datamc/recomc"<<std::endl;
                std::cout<<"\t-t\t--topo_tag\t\tTopological Tag [Superseeded by XML defined tag]"<<std::endl;
                std::cout<<"\t-d\t--dir\t\tDirectory for file inputs [Superseeded by XML]"<<std::endl;
                std::cout<<"\t-v\t--vector\t\tA list of variable numbers, currently only works for var2D. Should be comma separated and enclosed in quotes like \"1,2,3\" "<<std::endl;
                std::cout<<"\t-h\t--help\t\tThis help menu"<<std::endl;
                return 0;
        }
    }

    //===========================================================================================
    //===========================================================================================
    //			Begininning of main program here!
    //===========================================================================================
    //===========================================================================================

    std::cout<<"================================================================================"<<std::endl;
    std::cout<<"=================  Beginning HIVE run (version Blue World Order)================"<<std::endl;
    std::cout<<"================================================================================"<<std::endl;
    std::cout<<"Welcome."<<std::endl;get_joy();
    //Most TMVA arguments are loaded in here via XML
    std::cout<<"Getting xml variables"<<std::endl;
    MVALoader XMLconfig(xml);
    std::vector<method_struct> TMVAmethods  = XMLconfig.GetMethods(); 
    std::string analysis_tag = XMLconfig.analysis_tag;

    std::vector<double> fbdtcuts = XMLconfig.bdt_cuts;
    if(fbdtcuts.size()==0){
        std::cout<<"No BDT cuts set, so setting all to 0 for now"<<std::endl;
        fbdtcuts.resize(TMVAmethods.size(),0);
    }else{
        std::cout<<"BDT cuts have been loaded and set as: "<<std::endl;
        for(auto &c: fbdtcuts)std::cout<<c<<" ";
        std::cout<<std::endl;
    }


    dir = XMLconfig.filedir;
    std::cout<<"Core File dir set as "<<dir<<std::endl;

    std::cout<<"We have "<<TMVAmethods.size()<<" Different BDT's we are going to train today"<<std::endl;
    std::vector<bdt_info> bdt_infos;
    for(int k=0; k< TMVAmethods.size(); k++){
        bdt_infos.push_back( bdt_info(analysis_tag, TMVAmethods[k]));
        std::cout<<"BDT Number: "<<k<<" is on "<<bdt_infos.back().name<<std::endl;
        std::cout<<"We have "<<bdt_infos.back().train_vars.size()<<" Training variables"<<std::endl;
    }

    //This is a vector each containing a precut, they are all added together to make the whole "precut"
    std::vector<std::string> vec_precuts = TMVAmethods[0].precuts;
    //Get all the variables you want to use	
    std::vector<bdt_variable> vars = TMVAmethods[0].bdt_all_vars;
    std::string postcuts = "1";  //We dont currently use postcuts


    std::string topological_cuts = TMVAmethods[0].topological_definition;
    //**** Setting up bdt_files NWO style

    std::vector<bdt_file*> bdt_files;
    std::vector<bdt_file*> stack_bdt_files;
    std::vector<bdt_file*> signal_bdt_files;
    std::vector<bdt_file*> bkg_bdt_files;

    bdt_file * signal;
    bdt_file * training_signal;

    std::map<std::string, bdt_file*> tagToFileMap;

    std::cout<<"================================================================================"<<std::endl;
    std::cout<<"=============== Loading all BDT files for this analysis ========================"<<std::endl;
    std::cout<<"================================================================================"<<std::endl;

    for(size_t f = 0; f < XMLconfig.GetNFiles(); ++f){

        std::cout<<"============= Starting bdt_file number "<<f<<"  with tag -- "<<XMLconfig.bdt_tags[f]<<"==========="<<std::endl;
        //First build a bdt_flow for this file.
        std::string def = "1";  
        for(int i=0; i< XMLconfig.bdt_definitions[f].size(); ++i){
            def += "&&" + XMLconfig.bdt_definitions[f][i];
        }
        std::cout<<def<<std::endl;
        bdt_flow analysis_flow(topological_cuts, def, 	vec_precuts,	postcuts,	bdt_infos);

        std::cout<<" -- Filename "<<XMLconfig.bdt_filenames[f]<<" subdir "<<XMLconfig.bdt_dirs[f]<<std::endl;
        std::cout<<" -- Color ";XMLconfig.bdt_cols[f]->Print();std::cout<<" and hist style "<<XMLconfig.bdt_hist_styles[f]<<" fillstyle "<<XMLconfig.bdt_fillstyles[f]<<std::endl;
        std::cout<<" -- With the following Definition Cuts: "<<std::endl;
        for(int i=0; i< XMLconfig.bdt_definitions[f].size(); ++i){
            std::cout<<" -- ---> "<<XMLconfig.bdt_definitions[f][i]<<std::endl;
        }

        bdt_files.push_back(new bdt_file(dir, XMLconfig.bdt_filenames[f].c_str(),	XMLconfig.bdt_tags[f].c_str(), XMLconfig.bdt_hist_styles[f].c_str(),XMLconfig.bdt_dirs[f].c_str(), XMLconfig.bdt_cols[f]->GetNumber() , XMLconfig.bdt_fillstyles[f] , analysis_flow));
        bdt_files.back()->addPlotName(XMLconfig.bdt_plotnames[f]);
        tagToFileMap[XMLconfig.bdt_tags[f]] = bdt_files.back();

        bool incl_in_stack = true;

        if(XMLconfig.bdt_scales[f] != 1.0){
            std::cout<<" -- Scaling "<<XMLconfig.bdt_tags[f]<<" file by a factor of "<<XMLconfig.bdt_scales[f]<<std::endl;
            bdt_files.back()->scale_data = XMLconfig.bdt_scales[f];
        }


        if(XMLconfig.bdt_is_onbeam_data[f]){
            std::cout<<" -- Setting as ON beam data with "<<XMLconfig.bdt_onbeam_pot[f]/1e19<<" e19 POT equivalent"<<std::endl;
            bdt_files.back()->setAsOnBeamData(XMLconfig.bdt_onbeam_pot[f]); //tor860_wc
            incl_in_stack = false;
        }
        if(XMLconfig.bdt_is_offbeam_data[f]){
            std::cout<<" -- Setting as Off beam data with "<<XMLconfig.bdt_offbeam_spills[f]<<" EXT spills being normalized to "<<XMLconfig.bdt_onbeam_spills[f]<<" BNB spills at a "<<XMLconfig.bdt_onbeam_pot[f]/1e19<<" e19 POT equivalent"<<std::endl;
            bdt_files.back()->setAsOffBeamData( XMLconfig.bdt_onbeam_pot[f], XMLconfig.bdt_onbeam_spills[f], XMLconfig.bdt_offbeam_spills[f]);  //onbeam tor860_wcut, on beam spills E1DCNT_wcut, off beam spills EXT)

            bkg_bdt_files.push_back(bdt_files.back());
        }

        if(!bdt_files.back()->is_data && !XMLconfig.bdt_is_training_signal[f] ){
            if(XMLconfig.bdt_is_signal[f]){
                std::cout<<" -- For the purposes of calculting a significance, this is a signal file"<<std::endl;
                signal_bdt_files.push_back(bdt_files.back());
            }else{
                std::cout<<" -- For the purposes of calculting a significance, this is a BKG file"<<std::endl;
                bkg_bdt_files.push_back(bdt_files.back());
            }
        }

        if(XMLconfig.bdt_is_training_signal[f]){
            training_signal = bdt_files.back();
            incl_in_stack = false;
        }


        bdt_files.back()->calcPOT();
        if(incl_in_stack) stack_bdt_files.push_back(bdt_files.back());
    }

    //The "signal" is whichever signal BDT you define first.

    //===========================================================================================
    //===========================================================================================
    //		Main flow of the program , using OPTIONS
    //===========================================================================================
    //===========================================================================================
    std::cout<<"--------------------------------------------------------------------------"<<std::endl;
    std::cout<<"--------------------------------------------------------------------------"<<std::endl;

    if(true){

        std::cout<<"--------------------------------------------------------------------------"<<std::endl;
        std::cout<<" Going to add any trained BDT responses  and precal some EntryLists  "<<std::endl;
        std::cout<<" If you see warnings, but havenet yet ran app stage, thats ok!            "<<std::endl;
        std::cout<<"--------------------------------------------------------------------------"<<std::endl;

        for(auto &f: bdt_files){

            if(mode_option != "app" ){
                for(int k=0; k<bdt_infos.size(); k++){
                    f->addBDTResponses(bdt_infos[k]);
                }
            }
            if(mode_option != "train"  && mode_option != "sbnfit"){
                f->calcBaseEntryList(analysis_tag);
            }
        }
    }

    std::cout<<"--------------------------------------------------------------------------"<<std::endl;
    std::cout<<"--------------------------------------------------------------------------"<<std::endl;



    //===========================================================================================
    //===========================================================================================
    //		Main flow of the program , using OPTIONS
    //===========================================================================================
    //===========================================================================================



    for(auto &f: bdt_files){
            std::cout<<"Calculating any necessary EntryLists for "<<f->tag<<" On stage "<<which_stage<<"."<<std::endl;
            if(which_stage>1) f->calcBDTEntryList(which_stage,fbdtcuts);
            std::cout<<"Setting up EntryLists for "<<f->tag<<" On stage "<<which_stage<<"."<<std::endl;
            f->setStageEntryList(which_stage);
    }	


    for(auto &var: vars){
        std::vector<std::string> cuts(bdt_files.size(),"1");
        compareQuick(var,bdt_files,cuts,"VALID_"+var.safe_unit,true);
    }











return 0;

}



int compareQuick(bdt_variable var, std::vector<bdt_file*> files, std::vector<std::string> cuts, std::string name){
    return compareQuick(var,files,cuts,name,true); 
}

int compareQuick(bdt_variable var, std::vector<bdt_file*> files, std::vector<std::string> cuts, std::string name, bool shape_only){

    TCanvas *c = new TCanvas();
 
    TH1* rat_CV; 


    std::vector<int> cols = {kRed-7,  kBlue-7, kGreen+1 , kMagenta,kOrange,kGray};
    std::vector<TH1*> tvec;
    TLegend* leg=new TLegend(0.55,0.55,0.9,0.9);
    double max = -99;

    std::string testcut = "1";


       
        c->cd();
        TPad *pad0top = new TPad(("pad0top_"+var.safe_unit).c_str(), ("pad0top_"+var.safe_unit).c_str(), 0, 0.35, 1, 1.0);

        pad0top->SetBottomMargin(0); // Upper and lower plot are joined
            pad0top->Draw();             // Draw the upper pad: pad2top
            pad0top->cd();               // pad2top becomes the current pad



        c->cd();
        TPad *pad0bot = new TPad(("padbot_"+var.safe_unit).c_str(),("padbot_"+var.safe_unit).c_str(), 0, 0.05, 1, 0.35);
            pad0bot->SetTopMargin(0);
            pad0bot->SetBottomMargin(0.351);
            pad0bot->SetGridx(); // vertical grid
            pad0bot->Draw();

    double rmin  = 1.1;
    double rmax = 1.1;

    for(int i=0; i< files.size();i++){

        pad0top->cd();
        TH1* th1 =  (TH1*) files[i]->getTH1(var, testcut+"&&"+cuts[i], "photon_truth_overlay"+std::to_string(i), 6.6e20, 1);
        tvec.push_back(th1);
        std::cout<<"Int "<<th1->Integral()<<std::endl;
        pad0top->cd();

            
        th1->SetLineColor(files[i]->col);
        th1->SetLineWidth(2);

        if(shape_only){
            double norm = th1->Integral();
            th1->Scale(1.0/norm);
        }
        std::cout<<files[i]->tag<<" mean: "<<th1->GetMean()<<std::endl;
        th1->Draw("hist same");

        max = std::max(max, th1->GetMaximum());
        th1->SetMaximum(th1->GetMaximum()*1.5);
               th1->SetTitle(var.unit.c_str());
    
        leg->AddEntry(th1,files[i]->tag.c_str(),"l");
            pad0bot->cd();       // pad0bot becomes the current pad

            if(i==0) rat_CV =  (TH1*)th1->Clone("ratio_CV");


            TH1* rat_denom = (TH1*)th1->Clone(("ratio_denom_"+std::to_string(i)).c_str());
            for(int j=0; j<rat_denom->GetNbinsX(); j++){
                rat_denom->SetBinError(j,0.0);
            }	

            rat_denom->Divide(rat_CV);
            rat_denom->Draw("same hist");

            rat_denom->GetXaxis()->SetTitle(var.unit.c_str());
            rat_denom->SetMinimum(rmin);	
            rat_denom->SetMaximum(rmax);//ratunit->GetMaximum()*1.1);



    }

    tvec[0]->SetMaximum(max*1.5);

    leg->SetFillStyle(0);
    leg->SetLineColor(kWhite);
    leg->SetLineWidth(0);
    leg->Draw();
    c->SaveAs((name+".pdf").c_str(),"pdf");

    return 0;
};


