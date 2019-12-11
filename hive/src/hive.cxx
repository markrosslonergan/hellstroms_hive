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



int main (int argc, char *argv[]){

    //This is a standardized location on /pnfs/ that everyone can use. 
    std::string dir = "/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_mcc9_v17/";

    std::string mode_option = "enter_a_mode_please"; 
    std::string xml = "default.xml";
    std::string topo_tag = "this_is_xml_now";

    int number = -1;
    bool response_only = false;
    bool is_combined = false;
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
        {"combined",    no_argument,        0, 'c'},
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
        iarg = getopt_long(argc,argv, "x:o:d:s:f:t:p:b:i:n:v:rch?", longopts, &index);

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
            case 'c':
                is_combined = true;
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
                std::cout<<"\t\t\t\t\t -- N=3 SuperBDT needs to be ran later (experimental) "<<std::endl;
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
    std::vector<bdt_file*> training_bdt_files;

    bdt_file * signal;
    bdt_file * onbeam_data_file;

    std::map<std::string, bdt_file*> tagToFileMap;

    std::map<bdt_file*,bool> plotOnTopMap;


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
            onbeam_data_file = bdt_files.back();
        }

        if(is_combined) bdt_files.back()->addFriend("output_"+bdt_files.back()->tag ,"output_superMVA.root");


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

        if(XMLconfig.bdt_on_top[f]){
            plotOnTopMap[bdt_files.back()] = true;
        }else{
            plotOnTopMap[bdt_files.back()] = false;
        }

        //Lets collate the training files, these are only used for BDT purposes
        if(XMLconfig.bdt_is_training_signal[f]){
            incl_in_stack = false;
            training_bdt_files.push_back(bdt_files.back());
        }


        bdt_files.back()->calcPOT();
        if(incl_in_stack) stack_bdt_files.push_back(bdt_files.back());
    }

    //The "signal" is whichever signal BDT you define first.
    signal = signal_bdt_files[0];

    /*
       if(is_combined){
       for(auto &f: stack_bdt_files){
       std::cout<<"Adding Super Friend Tree"<<std::endl;
       f->addFriend("output_"+f->tag ,"output_superMVA.root");
       }
       onbeam_data_file->addFriend("output_"+onbeam_data_file->tag,"output_superMVA.root");
       }
       */


    //===========================================================================================
    //===========================================================================================
    //		Main flow of the program , using OPTIONS
    //===========================================================================================
    //===========================================================================================
    std::cout<<"--------------------------------------------------------------------------"<<std::endl;
    std::cout<<"--------------------------------------------------------------------------"<<std::endl;

    std::cout<<"--------------------------------------------------------------------------"<<std::endl;
    std::cout<<" Going to add any trained BDT responses  and precal some EntryLists  "<<std::endl;
    std::cout<<" If you see warnings, but havenet yet ran app stage, thats ok!            "<<std::endl;
    std::cout<<"--------------------------------------------------------------------------"<<std::endl;

    for(auto &f: bdt_files){

        if(mode_option != "app" && mode_option !="train" ){
            for(int k=0; k<bdt_infos.size(); k++){
                f->addBDTResponses(bdt_infos[k]);
            }
        }
        if(mode_option != "train"  && mode_option != "sbnfit"){
            f->calcBaseEntryList(analysis_tag);
        }
    }

    std::cout<<"--------------------------------------------------------------------------"<<std::endl;
    std::cout<<"--------------------------------------------------------------------------"<<std::endl;



    //===========================================================================================
    //===========================================================================================
    //		Main flow of the program , using OPTIONS
    //===========================================================================================
    //===========================================================================================


    if(mode_option == "train") {
        //First lets create the bdt_file's* and flows for training

        for(auto &f: bdt_files){
            f->calcBaseEntryList(analysis_tag);
        }

        for(int i=0; i< bdt_infos.size(); i++){

            if(bdt_infos[i].TMVAmethod.str=="XGBoost"){

                if(tagToFileMap.count(bdt_infos[i].TMVAmethod.sig_train_tag)!=1){
                    std::cout<<"ERROR! the signal training tag "<<bdt_infos[i].TMVAmethod.sig_train_tag<<" is not in the bdt_files list!"<<std::endl;
                    exit(EXIT_FAILURE);
                }
                if(tagToFileMap.count(bdt_infos[i].TMVAmethod.sig_test_tag)!=1){
                    std::cout<<"ERROR! the signal testing tag "<<bdt_infos[i].TMVAmethod.sig_test_tag<<" is not in the bdt_files list!"<<std::endl;
                    exit(EXIT_FAILURE);
                }
                if(tagToFileMap.count(bdt_infos[i].TMVAmethod.bkg_train_tag)!=1){
                    std::cout<<"ERROR! the bkg training tag "<<bdt_infos[i].TMVAmethod.bkg_train_tag<<" is not in the bdt_files list!"<<std::endl;
                    exit(EXIT_FAILURE);
                }
                if(tagToFileMap.count(bdt_infos[i].TMVAmethod.bkg_test_tag)!=1){
                    std::cout<<"ERROR! the bkg testing tag "<<bdt_infos[i].TMVAmethod.bkg_test_tag<<" is not in the bdt_files list!"<<std::endl;
                    exit(EXIT_FAILURE);
                }

            }
        }

        for(int i=0; i< bdt_infos.size(); i++){

            if(!(which_bdt==i || which_bdt==-1)) continue;


            bdt_file * training_signal = tagToFileMap[bdt_infos[i].TMVAmethod.sig_train_tag]; 
            bdt_file * testing_signal = tagToFileMap[bdt_infos[i].TMVAmethod.sig_test_tag]; 
            bdt_file * training_background = tagToFileMap[bdt_infos[i].TMVAmethod.bkg_train_tag]; 
            bdt_file * testing_background = tagToFileMap[bdt_infos[i].TMVAmethod.bkg_test_tag]; 



            if(bdt_infos[i].TMVAmethod.str=="XGBoost"){

                //This is NAF, need to save it and not repeat
                convertToLibSVM(bdt_infos[i], training_signal, testing_signal, bdt_infos[i].TMVAmethod.sig_test_cut, training_background, testing_background, bdt_infos[i].TMVAmethod.bkg_test_cut);
                bdt_XGtrain(bdt_infos[i]);

            }else if(bdt_infos[i].TMVAmethod.str=="TMVA"){

                bdt_train(bdt_infos[i], training_signal, testing_signal, training_background, testing_background);
                plot_train(bdt_infos[i], training_signal, testing_signal, training_background, testing_background);
            }

        }
        return 0;

    }else if(mode_option == "app"){

        for(int f=0; f< bdt_files.size();++f){
            for(int i=0; i< bdt_infos.size();++i){

                //By default loop over all bdt's and files, but if specified do just 1
                if(!((which_bdt==i || which_bdt==-1 )&&(which_file==f||which_file==-1))) continue;

                if(bdt_infos[i].TMVAmethod.str=="XGBoost"){
                    bdt_XGapp(bdt_infos[i], bdt_files[f]);

                }else{
                    bdt_app(bdt_infos[i], bdt_files[f]);
                }
            }
        }
        return 0;
    }
    else if(mode_option=="super"){

        //Define what we want to call signal and background here
        const std::vector<std::string> s_tags = {"NCDeltaRadOverlay"};
        const std::vector<std::string> b_tags ={"BNBOverlays","NCPi0","BNBext","Dirt"};

        //OK super preliminarly, need to have run sbnfit with simple_tree option on precut stage before attempting this
        super_bdt_train(analysis_tag, bdt_infos, s_tags, b_tags, "1", "1");

        for(int i=0; i< 6; i++){
            bdt_files[i]->makeSBNfitFile(analysis_tag, bdt_infos, 1, fbdtcuts,"reco_vertex_size");
        }

        //and apply it
        super_bdt_app(analysis_tag, bdt_infos, bdt_files);

        return 0;

    }else if(mode_option=="stack"){
        std::cout<<"Starting stack "<<std::endl;

        if (access("stack",F_OK) == -1){
            mkdir("stack",0777);//Create a folder for pdf.
        }
        else{
            std::cout<<"Overwrite stack/ in 2 seconds, 1 seconds, ..."<<std::endl;
            sleep(2);
        }

        std::cout<<"flag0"<<std::endl;

        TFile * ftest = new TFile(("test+"+analysis_tag+".root").c_str(),"recreate");

        bdt_stack *histogram_stack = new bdt_stack(analysis_tag+"_stack");

        histogram_stack->plot_pot =13.2e20;//4.9e19;

        std::cout<<"flag1"<<std::endl;

        for(size_t f =0; f< stack_bdt_files.size(); ++f){
            if(stack_bdt_files[f]->is_data) continue;

            bool is_signal = false;
            for(auto &sig: signal_bdt_files){
                if(stack_bdt_files[f]== sig)  is_signal=true;
            }
            if(!is_signal) histogram_stack->addToStack(stack_bdt_files[f]);
        }

        std::cout<<"flag2"<<std::endl;

        //signal->fillstyle = 0;
        for(int s = signal_bdt_files.size()-1; s>=0; s--){
            histogram_stack->addToStack(signal_bdt_files[s],true);
        }

        onbeam_data_file->col = kWhite;
        onbeam_data_file->fillstyle = 0;
        int ip=0;
        std::vector<bool> subv = {false,false,true};
        if(!response_only){
            if(number != -1){
                bdt_datamc datamc(onbeam_data_file, histogram_stack, analysis_tag+"_stack");	
                datamc.setPlotStage(which_stage);                
                datamc.setStackMode(13.2e20);

                //datamc.printPassingDataEvents("tmp", 3, fcoscut, fbnbcut);
                //datamc.setSubtractionVector(subv);
                std::vector<bdt_variable> tmp_var = {vars.at(number)};
                datamc.plotStacks(ftest,  tmp_var , fbdtcuts);
            }else{
                std::cout<<"flag5"<<std::endl;


                bdt_datamc real_datamc(onbeam_data_file, histogram_stack, analysis_tag+"_stack");	
                real_datamc.setPlotStage(which_stage);                
                real_datamc.setStackMode(13.2e20);

                //real_datamc.setSubtractionVector(subv);
                // real_datamc.plotStacks(ftest, vars,fcoscut,fbnbcut);
                //real_datamc.plotStacks(ftest, vars,fcoscut,fbnbcut);

                real_datamc.plotStacks(ftest, vars, fbdtcuts);
                //real_datamc.SetSpectator();
                //real_datamc.plotStacks(ftest, plotting_vars,fcoscut,fbnbcut);
            }
        }else{
            bdt_datamc real_datamc(onbeam_data_file, histogram_stack, analysis_tag+"_stack");	

            real_datamc.setStackMode(13.2e20);
            if(which_bdt ==-1){
                for(int k=0; k< bdt_infos.size(); k++){
                    real_datamc.plotBDTStacks(bdt_infos[k] , fbdtcuts);
                }
            }else{
                real_datamc.plotBDTStacks(bdt_infos[which_bdt],fbdtcuts);
            }
        }

    }    else if(mode_option == "datamc"){
        std::cout<<"Starting datamc "<<std::endl;

        if (access("datamc",F_OK) == -1){
            mkdir("datamc",0777);//Create a folder for pdf.
        }
        else{
            std::cout<<"Overwrite datamc/ in 2 seconds, 1 seconds, ..."<<std::endl;
            sleep(2);
        }



        TFile * ftest = new TFile(("test+"+analysis_tag+".root").c_str(),"recreate");

        bdt_stack *histogram_stack = new bdt_stack(analysis_tag+"_datamc");

        histogram_stack->plot_pot = onbeam_data_file->pot;

        if (!response_only){
            for(size_t f =0; f< stack_bdt_files.size(); ++f){
                if(stack_bdt_files[f]->is_data) continue;
                if(!plotOnTopMap[stack_bdt_files[f]] ){
                    histogram_stack->addToStack(stack_bdt_files[f]);
                    std::cout<<"adding to stack ON BOTTOM: "<<stack_bdt_files[f]->tag<<std::endl;
                }
            }

            for(size_t f =0; f< stack_bdt_files.size(); ++f){
                if(stack_bdt_files[f]->is_data) continue;
                if(plotOnTopMap[stack_bdt_files[f]] ){
                    histogram_stack->addToStack(stack_bdt_files[f],true);
                    std::cout<<"adding to stack ON BOTTOM: "<<stack_bdt_files[f]->tag<<std::endl;
                }
            }

        }else{
            //then add SM
            for(size_t f =0; f< stack_bdt_files.size(); ++f){
                if(stack_bdt_files[f]->is_data) continue;
                if( plotOnTopMap[stack_bdt_files[f]] ){
                    histogram_stack->addToStack(stack_bdt_files[f],true);
                    std::cout<<"adding to stack: "<<stack_bdt_files[f]->tag<<std::endl;

                }else{
                    histogram_stack->addToStack(stack_bdt_files[f]);
                    std::cout<<"adding to stack: "<<stack_bdt_files[f]->tag<<std::endl;

                }

            }

        }
        int ip=0;
        std::vector<bool> subv = {false,false,true};
        if(!response_only){
            if(number != -1){
                bdt_datamc datamc(onbeam_data_file, histogram_stack, analysis_tag+"_datamc");	
                datamc.setPlotStage(which_stage);                

                //datamc.printPassingDataEvents("tmp", 4, fbdtcuts);

                //datamc.printPassingDataEvents("tmp", 3, fcoscut, fbnbcut);
                //datamc.setSubtractionVector(subv);
                std::vector<bdt_variable> tmp_var = {vars.at(number)};
                datamc.plotStacks(ftest,  tmp_var , fbdtcuts);
            }else{

                bdt_datamc real_datamc(onbeam_data_file, histogram_stack, analysis_tag+"_datamc");	
                real_datamc.setPlotStage(which_stage);                

                //real_datamc.setSubtractionVector(subv);
                // real_datamc.plotStacks(ftest, vars,fcoscut,fbnbcut);
                //real_datamc.plotStacks(ftest, vars,fcoscut,fbnbcut);

                real_datamc.plotStacks(ftest, vars, fbdtcuts);
                //real_datamc.SetSpectator();
                //real_datamc.plotStacks(ftest, plotting_vars,fcoscut,fbnbcut);
            }
        }else{
            bdt_datamc real_datamc(onbeam_data_file, histogram_stack, analysis_tag+"_datamc");	

            if(which_bdt ==-1){
                for(int k=0; k< bdt_infos.size(); k++){
                    real_datamc.plotBDTStacks(bdt_infos[k] , fbdtcuts);
                }
            }else{
                real_datamc.plotBDTStacks(bdt_infos[which_bdt],fbdtcuts);
            }
        }
    }
    else if(mode_option == "superdatamc"){
        std::cout<<"Starting superdatamc "<<std::endl;

        TFile * ftest = new TFile(("test+"+analysis_tag+".root").c_str(),"recreate");
        TFile * fsuper = new TFile("output_superMVA.root","recreate");

        std::vector<TTree*> super_trees;
        for(size_t f =0; f< stack_bdt_files.size(); ++f){
            std::string stree = "output_"+stack_bdt_files[f]->tag;
            std::cout<<"Getting supertree : "<<stree<<std::endl;
            super_trees.push_back((TTree*)fsuper->Get(stree.c_str()));
        }

        bdt_stack *histogram_stack = new bdt_stack(analysis_tag+"_superdatamc");
        histogram_stack->plot_pot = onbeam_data_file->pot;

        /*
           for(size_t f =0; f< stack_bdt_files.size(); ++f){
           if(stack_bdt_files[f]->is_data) continue;
           if(!plotOnTopMap[stack_bdt_files[f]] ){
           histogram_stack->addToStack(stack_bdt_files[f]);
           std::cout<<"adding to stack ON BOTTOM: "<<stack_bdt_files[f]->tag<<std::endl;
           }
           }

           for(size_t f =0; f< stack_bdt_files.size(); ++f){
           if(stack_bdt_files[f]->is_data) continue;
           if(plotOnTopMap[stack_bdt_files[f]] ){
           histogram_stack->addToStack(stack_bdt_files[f],true);
           std::cout<<"adding to stack ON BOTTOM: "<<stack_bdt_files[f]->tag<<std::endl;
           }
           }

           int ip=0;

           bdt_datamc datamc(onbeam_data_file, histogram_stack, analysis_tag+"_datamc");	
           datamc.setPlotStage(which_stage);                
           std::vector<bdt_variable> tmp_var = {vars.at(number)};
           datamc.plotStacks(ftest,  tmp_var , fbdtcuts);
           */           
    }
    else if(mode_option == "var2D"){
        std::cout<<"Starting var2D "<<std::endl;

        if (access("var2D",F_OK) == -1){
            mkdir("var2D",0777);//Create a folder for pdf.
        }
        else{
            std::cout<<"Overwrite var2D/ in 2 seconds, 1 seconds, ..."<<std::endl;
            sleep(2);
        }


        TFile * ftest = new TFile(("test+"+analysis_tag+".root").c_str(),"recreate");

        bdt_stack *histogram_stack = new bdt_stack(analysis_tag+"_var2D");

        histogram_stack->plot_pot = onbeam_data_file->pot;

        //add MC files to stack
        for(size_t f =0; f< stack_bdt_files.size(); ++f){
            if(bdt_files[f]->is_data) continue;
            histogram_stack->addToStack(stack_bdt_files[f]);
            std::cout<<"adding to stack"<<stack_bdt_files[f]->name<<std::endl;
        }

        //make the datamc object 
        bdt_datamc real_datamc(onbeam_data_file, histogram_stack, analysis_tag+"_var2D");	
        real_datamc.setPlotStage(which_stage);                


        if (vector != ""){//if passed specific variables
            std::vector<bdt_variable> tmp_var =  real_datamc.GetSelectVars(vector, vars);
            real_datamc.plot2D(ftest, tmp_var, fbdtcuts);
        }else{    
            real_datamc.plot2D(ftest, vars, fbdtcuts); //warning this will make a lot of plots
        }//if passed a vector
    }
    else if(mode_option == "test"){

       if(which_bdt==-1)which_bdt = 0;
       bdt_XGBoost_importance(bdt_infos[which_bdt]);

        return 0;
    }else if(mode_option == "sig"){

        switch(number){
            case 0:
                scan_significance(signal_bdt_files , bkg_bdt_files, bdt_infos,what_pot);
                break;
            case 1:
                scan_likelihood(stack_bdt_files, bdt_infos);
                break;
            case 2:
                scan_significance_random(signal_bdt_files, bkg_bdt_files,bdt_infos);
                break;
            case 3:
                super_significance(signal_bdt_files, bkg_bdt_files);
                break;

            default:
                break;
        }
        return 0;
    }else if(mode_option == "scatter"){

        std::cout<<"Starting Scatter "<<std::endl;

        if (access("scatter",F_OK) == -1){
            mkdir("scatter",0777);//Create a folder for pdf.
        }
        else{
            std::cout<<"Overwrite scatter/ in 2 seconds, 1 seconds, ..."<<std::endl;
            sleep(2);
        }

        if(which_file==-1){
            for(auto &f: bdt_files){
                plot_scatter(f, bdt_infos);
            }
        }else{
            plot_scatter(bdt_files[which_file], bdt_infos);
        }


        return 0;

    }else if(mode_option == "sss"){

        /*
           std::vector<std::vector<double>> signal_eff;
           std::vector<std::vector<double>> bkg_eff;
           std::vector<double> impact;

           for(double im =0; im<40.0; im+=0.20){
           impact.push_back(im);
           }

           for(int stage = 1; stage<4; stage++){
           if(stage>1){
           signal->calcBDTEntryList(stage,fbdtcuts);
           bnb->calcBDTEntryList(stage,fbdtcuts);
           }	

           signal->setStageEntryList(stage);
           bnb->setStageEntryList(stage);

           double N_s = signal->GetEntries("1");
           double N_b = bnb->GetEntries("1");

           std::vector<double> ts;
           std::vector<double> tb;

           for(double im =0; im<40.0; im+=0.2){

           std::string s_impact = "((sss_num_candidates==0) || Min$(sss_candidate_impact_parameter)>"+std::to_string(im)+") ";
           tb.push_back(bnb->GetEntries(s_impact)/N_b*100.0);
           ts.push_back(signal->GetEntries(s_impact)/N_s*100.0);
           }
           signal_eff.push_back(ts);
           bkg_eff.push_back(tb);
           }
           TCanvas *cimpact = new TCanvas();
           cimpact->cd();

           std::vector<TGraph*> gb;
           std::vector<TGraph*> gs;

           for(int i=0; i<signal_eff.size(); i++){
           gs.push_back( new TGraph(impact.size(),&impact[0],&signal_eff[i][0]));
           gb.push_back( new TGraph(impact.size(),&impact[0],&bkg_eff[i][0]));
           }

           gs[0]->Draw("al");
           gs[0]->GetXaxis()->SetLimits(0.,40.0);                 // along X
           gs[0]->GetHistogram()->SetMaximum(100.0);   // along          
           gs[0]->GetHistogram()->SetMinimum(0.0);  //   Y     
           gs[0]->SetLineColor(signal->col);

           gs[0]->SetTitle("");
           gs[0]->GetXaxis()->SetTitle("Impact Parameter Cut [cm]");
           gs[0]->GetYaxis()->SetTitle("Second Shower Veto Efficiency [%]");

           TLegend *l = new TLegend(0.59,0.89,0.59,0.89);
           l->SetNColumns(2);
           l->SetLineWidth(0);
           l->SetLineColor(kWhite);

           std::vector<int> ls_i = {1,2,7};
           std::vector<std::string> s_i {"Precuts","Cosmic","BNB"};
           for(int i=0; i<signal_eff.size();i++){
           gs[i]->Draw("l same");
           gb[i]->Draw("l same");

           gs[i]->SetLineColor(signal->col);
           gb[i]->SetLineColor(bnb->col);

           gs[i]->SetLineWidth(3);
           gb[i]->SetLineWidth(3);

        gs[i]->SetLineStyle(ls_i[i]);
    gb[i]->SetLineStyle(ls_i[i]);

    l->AddEntry(gs[i],("Signal "+s_i[i]).c_str(),"l");
    l->AddEntry(gb[i],("BNB "+s_i[i]).c_str(),"l");
}

l->Draw();
cimpact->Update();
cimpact->SaveAs("Impact.pdf","pdf");
*/
}else if(mode_option == "eff"){

    if(which_file == -1)which_file = 1;

    //which_file = 7;//checking ext
    std::vector<std::string> v_denom = XMLconfig.bdt_definitions[which_file];
    std::vector<std::string> v_topo = {TMVAmethods[0].topological_definition};


    if(which_stage==-1)which_stage=0;

    what_pot = 5e19;

    bdt_efficiency(bdt_files[which_file], v_denom, v_topo, vec_precuts, fbdtcuts, what_pot,false,which_stage,analysis_tag);

    //specifically for protond/photons pre-topological
    // bdt_efficiency(bdt_files[which_file], v_denom, v_topo, vec_precuts, fbdtcuts, what_pot,false,which_stage,analysis_tag, true);
    //normally stops here

    //Ok, this runs now for a full cut
    std::string full_cut = "reco_asso_showers==1 && reco_asso_tracks==1";
    bdt_efficiency(bdt_files,full_cut);


}else if(mode_option == "sbnfit"){
    if(which_file==-1) which_file ==0;
    if(which_stage==-1) which_stage ==1;


    return bdt_files[which_file]->makeSBNfitFile(analysis_tag, bdt_infos, which_stage, fbdtcuts,input_string);


}else if(mode_option == "recomc"){
    if (access("recomc",F_OK) == -1){
        mkdir("recomc",0777);//Create a folder for pdf.
    }
    else{
        std::cout<<"Overwrite recomc/ in 2 seconds, 1 seconds, ..."<<std::endl;
        sleep(2);
    }


    std::vector<int> recomc_cols;
    for(auto &c: XMLconfig.recomc_cols){
        recomc_cols.push_back(c->GetNumber());
    }
    recomc_cols.push_back(kGray);

    std::vector<std::string> recomc_names = XMLconfig.recomc_names; 
    recomc_names.push_back("Other");


    std::vector<std::string> recomc_cuts = XMLconfig.recomc_defs;

    std::string other = "1";
    for(auto &c: XMLconfig.recomc_defs){
        other += "&& !("+c+")";
    }
    recomc_cuts.push_back(other);

    bdt_recomc recomc(recomc_names, recomc_cuts, recomc_cols,analysis_tag);
    recomc.setPlotStage(which_stage);                

    TFile * ftest = new TFile(("test+"+analysis_tag+".root").c_str(),"recreate");
    int h=0;
    if(which_file == -1) which_file =0;

    if(number != -1){
        std::vector<bdt_variable> tmp = {vars.at(number)};
        recomc.plot_recomc(ftest, bdt_files[which_file], tmp, fbdtcuts,what_pot);
        return 0;
    }else{
        recomc.plot_recomc(ftest, bdt_files[which_file], vars, fbdtcuts,what_pot);
    }	



    /*
       else if(mode_option == "response"){

       TFile * ftest = new TFile(("test+"+analysis_tag+".root").c_str(),"recreate");
//Ok print out Cosmic BDT
if(run_cosmic){
bdt_response cosmic_response(cosmic_bdt_info, training_signal, OffBeamData);
cosmic_response.plot_bdt_response(ftest);
}

if(run_bnb){
bdt_response bnb_response(bnb_bdt_info, training_signal, training_ncpi0);
bnb_response.plot_bdt_response(ftest);
}
}	

else if(mode_option == "stack"){
bdt_stack histogram_stack(analysis_tag+"_stack");
histogram_stack.plot_pot = what_pot;
histogram_stack.addToStack(signal);
histogram_stack.addToStack(signal_other);
histogram_stack.addToStack(bnb);
//histogram_stack.addToStack(nueintrinsic);
histogram_stack.addToStack(ncpi0);
//Add OffBeamData but change the color and style first
OffBeamData->col;	
OffBeamData->fillstyle = 3333;
histogram_stack.addToStack(dirt);

histogram_stack.addToStack(OffBeamData);
//histogram_stack.addToStack(dirt);

TFile * ftest = new TFile(("test+"+analysis_tag+".root").c_str(),"recreate");
int ip=0;


if(!response_only){
if(number == -1){
histogram_stack.plotStacks(ftest,vars,fcoscut,fbnbcut);
}else{
std::cout<<"Starting to make a stack of : "<<vars.at(number).name<<std::endl;

std::vector<bdt_variable> v_tmp = {vars.at(number)};
histogram_stack.plotStacks(ftest,v_tmp,fcoscut,fbnbcut);
}
}else{
histogram_stack.plotBDTStacks(ftest, bdt_infos[1], fcoscut, fbnbcut);
histogram_stack.plotBDTStacks(ftest, bdt_infos[0], fcoscut, fbnbcut);
return 0;
}
*/
}else if(mode_option == "vars"){

    if (access("vars",F_OK) == -1){
        mkdir("vars",0777);//Create a folder for pdf.
    }
    else{
        std::cout<<"Overwrite vars/ in 2 seconds, 1 seconds, ..."<<std::endl;
        sleep(2);
    }


    std::vector<std::string> title = {"Topological Selection","Pre-Selection Cuts"};

    if(which_stage == -1) which_stage = 1;

    std::vector<bdt_file*> training_background_files;
    for(int i=0; i< bdt_infos.size(); i++){


            if(!(which_bdt==i || which_bdt==-1)) continue;

            bdt_file * training_signal = tagToFileMap[bdt_infos[i].TMVAmethod.sig_train_tag]; 
            bdt_file * training_background = tagToFileMap[bdt_infos[i].TMVAmethod.bkg_train_tag]; 
            plot_bdt_variables(training_signal, training_background, vars, bdt_infos[i], false, which_stage,fbdtcuts);
    }


}else {
    std::cout << "WARNING: " << mode_option << " is an invalid option\n";
}

return 0;

}





