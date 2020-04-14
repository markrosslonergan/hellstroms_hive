#include <getopt.h>
#include <sys/stat.h> 

#include "TFriendElement.h"
#include "TList.h"

#include "TROOT.h"
#include "TSystem.h"


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
    std::string mydir = "/uboone/app/users/amogan/wes_ncpi0_filter/workdir/";
    std::string dir2g0p = "/uboone/app/users/amogan/singlePhotonFilterMCC9.1_2g0p/workdir/";

    std::string mode_option = "enter_a_mode_please"; 
    std::string xml = "default.xml";
    std::string topo_tag = "this_is_xml_now";

    int number = -1;
    bool response_only = false;
    bool is_combined = false;
    // Added by A. Mogan 1/13/20 for doing normalization fits
    bool scale_mode = true;
    //double what_pot = 13.2e20;
    double what_pot = 1.8e20;
    int which_file = -1;
    int which_bdt = -1;
    int which_stage = -1;
    std::string vector = "";
    std::string input_string = "";
    int which_group = -1;

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
        {"group",       required_argument, 0, 'g'},
        {"number",		required_argument,	0, 'n'},
        {"response",	no_argument,	    0, 'r'},
        {"file",		required_argument,	0, 'f'},
        {"vector",      required_argument,  0, 'v'},
        {0,			    no_argument, 		0,  0},
    };

    int iarg = 0; opterr=1; int index;
    while(iarg != -1)
    {
        iarg = getopt_long(argc,argv, "x:o:d:s:f:t:p:b:i:n:g:v:rch?", longopts, &index);

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
            case 'g':
                which_group = (int)strtof(optarg,NULL);
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
                std::cout<<"\t\t\t\t\t -- N=3 Similar to N=0, but cut values maximized for efficiency*purity "<<std::endl;
                std::cout<<"\t\t\t\t\t -- N=4 Similar to N=0, but cut values maximized efficiency"<<std::endl;
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
                std::cout<<"\t-g\t--group\t\tSet a group for variable plotting"<<std::endl;
                std::cout<<"\t-s\t--stage\t\tSet what stage to do things at."<<std::endl;
                std::cout<<"\t-r\t--response\t\t Run only BDT response plots for datamc/recomc"<<std::endl;
                std::cout<<"\t-t\t--topo_tag\t\tTopological Tag [Superseeded by XML defined tag]"<<std::endl;
                std::cout<<"\t-d\t--dir\t\tDirectory for file inputs [Superseeded by XML]"<<std::endl;
                std::cout<<"\t-v\t--vector\t\tA list of variable numbers, currently only works for var2D. Should be comma separated and enclosed in quotes like \"1,2,3\" "<<std::endl;
                std::cout<<"\t-h\t--help\t\tThis help menu"<<std::endl;
                return 0;
        }
    }

        
    gSystem->Load("/uboone/app/users/markrl/Hive_v3.0/hellstroms_hive/hive/root_linkdefs/loc/denan_cxx.so");
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
    std::vector<bdt_file*> validate_files;

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

        //If its a training file we are working with, add the training definitions 
        if(XMLconfig.bdt_is_training_signal[f]){
            for(int i=0; i< XMLconfig.bdt_training_cuts[f].size(); ++i){
                def += "&&" + XMLconfig.bdt_training_cuts[f][i];
            }
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

        if(is_combined) bdt_files.back()->addFriend("output_"+bdt_files.back()->tag ,analysis_tag+"_superMVA.root");


        if(XMLconfig.bdt_is_offbeam_data[f]){
            std::cout<<" -- Setting as Off beam data with "<<XMLconfig.bdt_offbeam_spills[f]<<" EXT spills being normalized to "<<XMLconfig.bdt_onbeam_spills[f]<<" BNB spills at a "<<XMLconfig.bdt_onbeam_pot[f]/1e19<<" e19 POT equivalent"<<std::endl;
            bdt_files.back()->setAsOffBeamData( XMLconfig.bdt_onbeam_pot[f], XMLconfig.bdt_onbeam_spills[f], XMLconfig.bdt_offbeam_spills[f]);  //onbeam tor860_wcut, on beam spills E1DCNT_wcut, off beam spills EXT)

           if(!XMLconfig.bdt_is_training_signal[f])  bkg_bdt_files.push_back(bdt_files.back());
        }

        if(!bdt_files.back()->is_data && !XMLconfig.bdt_is_training_signal[f]  && !XMLconfig.bdt_is_validate_file[f] && !XMLconfig.bdt_is_offbeam_data[f]){
            if(XMLconfig.bdt_is_signal[f]){
                std::cout<<" -- For the purposes of calculting a significance, this is a signal file"<<std::endl;
                signal_bdt_files.push_back(bdt_files.back());
            }else{
                std::cout<<" -- For the purposes of calculting a significance, this is a BKG file "<<std::endl;
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


        bdt_files.back()->calcPOT(XMLconfig.run_names, XMLconfig.run_cuts, XMLconfig.run_fractions);

        //std::string r1 = "run_number>=5121 && run_number <=5946";
        //bdt_files.back()->scale( bdt_files.back()->tvertex->GetEntries(r1.c_str())/(double)bdt_files.back()->tvertex->GetEntries() );


        if(incl_in_stack) stack_bdt_files.push_back(bdt_files.back());

        if(XMLconfig.bdt_is_validate_file[f]) validate_files.push_back(bdt_files.back());

    }

    //The "signal" is whichever signal BDT you define first.
    signal = signal_bdt_files[0];



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


        //These are old and redundant.
        f->addFriend("sss_precalc",analysis_tag+"_"+f->tag+"_SSSprecalc.root");
        //f->addFriend("track_tree",analysis_tag+"_"+f->tag+"_simtrack.root");

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
                convertToLibSVMTT(bdt_infos[i], training_signal, testing_signal, bdt_infos[i].TMVAmethod.sig_test_cut, training_background, testing_background, bdt_infos[i].TMVAmethod.bkg_test_cut);
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
        const std::vector<std::string> s_tags = {"NCDeltaRadOverlay","NCDeltaRadOverlaySM"};
        const std::vector<std::string> b_tags ={"BNBOverlays","NCPi0","CCPi0","NueOverlays","BNBext","Dirt"};

        for(int i=0; i< bdt_files.size(); i++){
            //   bdt_files[i]->makeSBNfitFile(analysis_tag, bdt_infos, 1, fbdtcuts,"reco_vertex_size",vars);
        }

        //OK super preliminarly, need to have run sbnfit with simple_tree option on precut stage before attempting this
        super_bdt_train(analysis_tag, bdt_infos, s_tags, b_tags, "1", "1");

        //and apply it
        std::vector<bdt_file*> tempt;
        for(int i=0; i<9; ++i){tempt.push_back(bdt_files[i]);}
        super_bdt_app(analysis_tag, bdt_infos, tempt);

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

        histogram_stack->plot_pot =10.115e20;//4.9e19;

        std::cout<<"flag1"<<std::endl;

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

        std::cout<<"flag2"<<std::endl;

        //signal->fillstyle = 0;
        //histogram_stack->addToStack(signal,true);

        onbeam_data_file->col = kWhite;
        onbeam_data_file->fillstyle = 0;
        int ip=0;
        std::vector<bool> subv = {false,false,true};
        if(true){
            if(number != -1){
                bdt_datamc datamc(onbeam_data_file, histogram_stack, analysis_tag+"_stack");	
                datamc.setPlotStage(which_stage);                
                datamc.setStackMode(histogram_stack->plot_pot);

                std::vector<bdt_variable> tmp_var = {vars.at(number)};
                datamc.plotStacks(ftest,  tmp_var , fbdtcuts);
            }else{
                std::vector<bdt_variable> tmp_vars;
                for(auto &v: vars){
                    if(which_group == -1 || which_group == v.cat){
                        tmp_vars.push_back(v);
                    }
                }



                bdt_datamc real_datamc(onbeam_data_file, histogram_stack, analysis_tag+"_stack");	
                real_datamc.setPlotStage(which_stage);                
                real_datamc.setStackMode( histogram_stack->plot_pot);

                real_datamc.plotStacks(ftest, tmp_vars, fbdtcuts);
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
        std::vector<bool> subv = {false,false,true};
        if(true){
            if(number != -1){
                bdt_datamc datamc(onbeam_data_file, histogram_stack, analysis_tag+"_datamc");	
                datamc.setPlotStage(which_stage);                

                //datamc.printPassingDataEvents("tmp", 4, fbdtcuts);

                //datamc.printPassingDataEvents("tmp", 3, fbdtcuts);
                //datamc.printPassingPi0DataEvents("tmp", 2, fbdtcuts);
                //datamc.setSubtractionVector(subv);
                std::vector<bdt_variable> tmp_var = {vars.at(number)};
                datamc.plotStacks(ftest,  tmp_var , fbdtcuts);
                // TODO: Commented out for time's sake. Put back in later
                //datamc.plotEfficiency(tmp_var,fbdtcuts,1,(which_stage>1 ? which_stage : 2));
            }else{

                std::vector<bdt_variable> tmp_vars;
                for(auto &v: vars){
                    if(which_group == -1 || which_group == v.cat){
                        tmp_vars.push_back(v);
                    }
                }

                bdt_datamc real_datamc(onbeam_data_file, histogram_stack, analysis_tag+"_datamc");	
                real_datamc.setPlotStage(which_stage);                

                if(which_bdt==-1){
                    real_datamc.plotStacks(ftest, tmp_vars, fbdtcuts);
                    // TODO: Commented out for time's sake. Put back in later
                    //real_datamc.plotEfficiency(tmp_vars,fbdtcuts,1, (which_stage > 1? which_stage : 2 ) );
                }else{
                    real_datamc.plotStacks(ftest, bdt_infos[which_bdt].train_vars, fbdtcuts);
                    // TODO: Commented out for time's sake. Put back in later
                    //real_datamc.plotEfficiency(bdt_infos[which_bdt].train_vars,fbdtcuts,1,  (which_stage >1 ? which_stage :2 ));
                }
            }

        }
    }
    else if(mode_option == "superdatamc"){
        std::cout<<"Starting superdatamc "<<std::endl;

        TFile * ftest = new TFile(("test+"+analysis_tag+".root").c_str(),"recreate");
        TFile * fsuper = new TFile((analysis_tag+"_superMVA.root").c_str(),"read");

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
    // Added by A. Mogan 1/13/20 for normalization fits
    // Similar to datamc, but iteratively scales signal histogram and
    // re-runs plotStacks to re-calculate chi^2 for each scaling
    else if (mode_option == "scalenorm") {
        std::cout<<"Starting scalenorm "<<std::endl;
        if (response_only || number==-1) {
            std::cout << "[SCALENORM]: ERROR: This mode is meant to be run with response_only set to false and -n set to a number" << std::endl;
            return 1;
        }

        if (access("scalenorm",F_OK) == -1){
            mkdir("scalenorm",0777);//Create a folder for pdf.
        }
        else{
            std::cout<<"Overwrite scalenorm/ in 2 seconds, 1 seconds, ..."<<std::endl;
            sleep(2);
        }

        TFile * ftest = new TFile(("test+"+analysis_tag+".root").c_str(),"recreate");

        // Set scale factor range and step size
        double scaleLow = 0.8;
        double scaleHigh = 1.2;
        double scaleStep = 0.1;

        std::vector<bdt_variable> tmp_var = {vars.at(number)};

        bdt_stack *histogram_stack = new bdt_stack(analysis_tag+"_scalenorm");
        histogram_stack->plot_pot = onbeam_data_file->pot;

        for(size_t f =0; f< stack_bdt_files.size(); ++f){
            if(stack_bdt_files[f]->is_data) continue;
            // NOTE: Make sure signal is the first element of the stack vector
            histogram_stack->addToStack(stack_bdt_files[f]);
            std::cout<<"adding to stack: "<<stack_bdt_files[f]->tag<<std::endl;
        }

        bdt_datamc datamc(onbeam_data_file, histogram_stack, analysis_tag+"_datamc");	
        datamc.scaleNorm(tmp_var, stack_bdt_files, scaleLow, scaleHigh, scaleStep, which_stage, fbdtcuts, analysis_tag);

        // Make stack with scale factor on signal; loop over
        // scale factors, recalculating chi^2 each time in datamc.cxx
        /*
        for (double s = scaleLow; s<scaleHigh; s+=scaleStep) {

            bdt_stack *histogram_stack = new bdt_stack(analysis_tag+"_scalenorm"+std::to_string(s));
            histogram_stack->plot_pot = onbeam_data_file->pot;

            stack_bdt_files[0]->scale(s);
            std::cout << "[SCALENORM]: Scale factor = " << s << std::endl;
            for(size_t f =0; f< stack_bdt_files.size(); ++f){
                if(stack_bdt_files[f]->is_data) continue;
                // NOTE: Make sure signal is the first element of the stack vector
                histogram_stack->addToStack(stack_bdt_files[f]);
                std::cout<<"adding to stack: "<<stack_bdt_files[f]->tag<<std::endl;
            }

            //int ip=0;
            //std::vector<bool> subv = {false,false,true};
            bdt_datamc datamc(onbeam_data_file, histogram_stack, analysis_tag+"_datamc"+std::to_string(s));	
            datamc.setPlotStage(which_stage);                

            //datamc.printPassingDataEvents("tmp", 4, fbdtcuts);

            //datamc.printPassingDataEvents("tmp", 3, fbdtcuts);
            //datamc.printPassingPi0DataEvents("tmp", 3, fbdtcuts);
            //datamc.setSubtractionVector(subv);
            //datamc.plotStacks(ftest,  tmp_var , fbdtcuts);
            datamc.plotStacks(ftest,  tmp_var , fbdtcuts);

            //bdt_datamc real_datamc(onbeam_data_file, histogram_stack, analysis_tag+"_datamc");	
            //real_datamc.setPlotStage(which_stage);                

            // Reset stack and scaling
            //histogram_stack->clearStack();
            //stack_bdt_files[0]->scale(1./s);
        }
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
    else if(mode_option == "precalc"){ 
        std::cout<<"looking at "<<bdt_files.size()<<"bdt files"<<std::endl;
        for(int f=0; f< bdt_files.size();++f){
            bool is_train = false;
            for(auto & t: training_bdt_files){
                if(t==bdt_files[f]) is_train=true;
            }
            if(which_file == f || which_file <0 ){
                if(which_file<0 && is_train) continue;
                ncpi0_sss_precalc(bdt_files[f], analysis_tag);
                //sim_track_precalc(bdt_files[f], analysis_tag);
            }
        }
    }
    else if(mode_option == "test"){

        signal_bdt_files[0]->tvertex->Scan("reco_shower_kalman_dEdx_plane2_median[0]:DeNan(reco_shower_kalman_dEdx_plane2_median[0],12.0):reco_shower_dEdx_amalgamated[0]","reco_asso_showers==1");

        return 0;

        bdt_stack *pphistogram_stack = new bdt_stack(analysis_tag+"_stack");
        bdt_datamc ppdatamc(onbeam_data_file, pphistogram_stack, analysis_tag+"_stack");	
        ppdatamc.printPassingDataEvents("tmp", which_stage, fbdtcuts);

        return 0;
        return 0;
        tagToFileMap["NueOverlays"]->scanStage(6, fbdtcuts,"run_number:subrun_number:event_number");

        return 0;

        bdt_stack *histogram_stack = new bdt_stack(analysis_tag+"_stack");
        bdt_datamc datamc(onbeam_data_file, histogram_stack, analysis_tag+"_stack");	
        datamc.printPassingDataEvents("tmp", 6, fbdtcuts);

        return 0;


        return 0;
        if(which_bdt==-1)which_bdt = 0;
        bdt_XGBoost_importance(bdt_infos[which_bdt]);

        return 0;
    }else if(mode_option == "sig"){

        // sig_type tells the function which cut metric to maximize for
        //    sig_type == 3: efficiency times purity
        //    sig_Type == 4: efficinecy
        //    sig_type == 5: purity 
        int sig_type = number; 

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
                scan_significance(signal_bdt_files , bkg_bdt_files, bdt_infos,what_pot, sig_type);
                break;
            case 4:
                //What is this?
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

}else if (mode_option == "valid"){

    std::cout<<"Running validate mode: "<<validate_files.size()<<std::endl;

        if (access("valid",F_OK) == -1){
            mkdir("valid",0777);//Create a folder for pdf.
        }
        else{
            std::cout<<"Overwrite valid/ in 2 seconds, 1 seconds, ..."<<std::endl;
            sleep(2);
        }


    std::vector<bdt_variable> quick_vars;

    if(which_bdt == -1){
        quick_vars = vars;
    }else{ 
        quick_vars = bdt_infos[which_bdt].train_vars;
    }   
    for(auto &var: quick_vars){

        if(which_group == -1 || which_group == var.cat){

            std::vector<std::string> cuts;
            int v = 0;
            std::vector<bdt_file*> compare_files;

            for(auto &f: validate_files){
                v++;
                if(which_stage>1){
                    std::string cu = f->getStageCuts(which_stage, fbdtcuts);
                    cuts.push_back(cu); 
                }else{
                    cuts.push_back("1");
                }
                compare_files.push_back(f);
            }

            compareQuick(var,compare_files,cuts,"VALID_"+var.safe_unit+"_stage_"+std::to_string(which_stage),true);
        }
    }





}else if(mode_option == "fancy"){

    
    if(which_file<0) which_file = 0; //default to first file.
    if(number<0) number = 0; //default to first variable

    if(XMLconfig.v_eff_denom_stage.size()!= 1){
        std::cout<<"Warning your running Eff Fancy with no XML inputs, breaking."<<std::endl;
        return 0;
    }

    for(auto &var: vars){
        if(which_group == -1 || which_group == var.cat){
            fancyFiciency(bdt_files[which_file], XMLconfig.v_eff_denom_cut[0], XMLconfig.v_eff_numer_cut[0], var, analysis_tag, XMLconfig.v_eff_denom_stage[0], XMLconfig.v_eff_numer_stage[0], fbdtcuts);
        }
     }

    return 0;

}else if(mode_option == "eff"){

    if(which_file == -1)which_file = 1;

    //which_file = 7;//checking ext
    std::vector<std::string> v_denom = XMLconfig.bdt_definitions[which_file];
    std::vector<std::string> v_topo = {TMVAmethods[0].topological_definition};//,"sim_shower_pdg==22","sim_track_pdg==2212","sim_shower_overlay_fraction<0.9","sim_track_overlay_fraction<0.9"};

if(which_stage==-1)which_stage=0;

    //what_pot = 10.1e20;
    // Needs to be 13.2 for filter studies
    what_pot = 13.2e20;

//added 1g0p case but need to use -t option
bool is0p = false;
if (topo_tag == "notrack"){
    is0p = true;
}

//bdt_efficiency(bdt_files[which_file], v_denom, v_topo, vec_precuts, fbdtcuts, what_pot,false,which_stage,analysis_tag, false, is0p);


//specifically for protond/photons pre-topological
 bdt_efficiency(bdt_files[which_file], v_denom, v_topo, vec_precuts, fbdtcuts, what_pot,false,which_stage,analysis_tag, true);
//normally stops here

//Ok, this runs now for a full cut
// Cut for NC pi0 filter
//std::string full_cut = "reco_asso_showers==2 && reco_asso_tracks==1 && reco_vertex_size>0 && (reco_vertex_x > 5.0 && reco_vertex_x < 251 && reco_vertex_y > -112 && reco_vertex_y < 112 && reco_vertex_z > 5 && reco_vertex_z < 1031) && reco_shower_conversion_distance[0]>1 && reco_shower_conversion_distance[1]>1";
//bdt_efficiency(bdt_files,full_cut);


}

// Last-minute garbage function I added for NC pi0 filter studies. Ignore
else if(mode_option == "eff2"){

    // Fiducial volume definition
    //std::string ZMIN = "0.0"; std::string ZMAX = "1036.8"; 	std::string XMIN = "0.0"; std::string XMAX = "256.35"; std::string YMIN = "-116.5"; std::string YMAX = "116.5";
    std::string ZMIN = "0.0"; std::string ZMAX = "1036.8"; 	std::string XMIN = "0.0"; std::string XMAX = "256.35"; std::string YMIN = "-116.5"; std::string YMAX = "116.5";
    std::string pmass = "0.938272";
    std::string fid_cut = "(mctruth_nu_vertex_x >"+XMIN+"+5 && mctruth_nu_vertex_x < "+XMAX+"-5 && mctruth_nu_vertex_y >"+ YMIN+"+5 && mctruth_nu_vertex_y <"+ YMAX+"-5 && mctruth_nu_vertex_z >"+ ZMIN +" +5 && mctruth_nu_vertex_z < "+ZMAX+"-5)";
    //std::string fid_cut = "(mctruth_nu_vertex_x >"+XMIN+"+10 && mctruth_nu_vertex_x < "+XMAX+"-10 && mctruth_nu_vertex_y >"+ YMIN+"+20 && mctruth_nu_vertex_y <"+ YMAX+"-20 && mctruth_nu_vertex_z >"+ ZMIN +" +10 && mctruth_nu_vertex_z < "+ZMAX+"-10)";

    // Signal NC pi0
    std::vector<std::string> v_denom_signal_ncpi0 = {"mctruth_cc_or_nc == 1",
        "mctruth_num_exiting_pi0==1", 
        "mctruth_pi0_leading_photon_energy > 0.02", 
        "mctruth_pi0_subleading_photon_energy > 0.02", 
        "Sum$(mctruth_exiting_proton_energy-0.93827>0.04)==0",
        fid_cut
    };
    std::string signal_ncpi0_def = v_denom_signal_ncpi0[0];
    for(int i=1; i< v_denom_signal_ncpi0.size();i++){
        signal_ncpi0_def += "&&" + v_denom_signal_ncpi0[i];
    }

    // Intrinsic nue's
    std::vector<std::string> v_denom_nue_intrinsic = {"mctruth_cc_or_nc == 0",
        "abs(mctruth_nu_pdg)==12", 
        "mctruth_nu_E > 0.05",
        "mctruth_nu_E < 1.5"
    };
    std::string nue_intrinsic_def = v_denom_nue_intrinsic[0];
    for(int i=1; i< v_denom_nue_intrinsic.size();i++){
        nue_intrinsic_def += "&&" + v_denom_nue_intrinsic[i];
    }

    // Deltarad 1g1p
    std::vector<std::string> v_denom_deltarad_1g1p = {"mctruth_cc_or_nc == 1", 
        "mctruth_is_delta_radiative==1",
        "mctruth_num_exiting_pi0==0",
        "mctruth_num_exiting_photons==1",
        "mctruth_exiting_photon_energy > 0.02", 
        //"Sum$(mctruth_exiting_proton_energy-0.93827>0.04)==1",
        fid_cut
    };
    std::string deltarad_1g1p_def = v_denom_deltarad_1g1p[0];
    for(int i=1; i< v_denom_deltarad_1g1p.size();i++){
        deltarad_1g1p_def += "&&" + v_denom_deltarad_1g1p[i];
    }

    std::string topological_cuts = "(reco_vertex_size>0 && reco_asso_showers == 2 && reco_asso_tracks==0)";

    //***************************************************************************************************/
    //***********	The bdt_flows define the "flow" of the analysis, i.e what cuts at what stage  *******/
    //***************************************************************************************************/
    std::cout<<"[EFF] Defining all bdt_flows, old-school style!"<<std::endl;
    bdt_flow signal_flow(topological_cuts, 	signal_ncpi0_def,	vec_precuts,	postcuts,	bdt_infos[0],	bdt_infos[0]);
    bdt_flow nue_bkg_flow(topological_cuts,	nue_intrinsic_def,	vec_precuts,	postcuts,	bdt_infos[0],	bdt_infos[0]);
    bdt_flow deltarad_1g1p_flow(topological_cuts,	deltarad_1g1p_def, vec_precuts,	postcuts,	bdt_infos[0],	bdt_infos[0]);

    bdt_file *ncpi0 = new bdt_file(dir2g0p, "vertexed_ncpi0_run1_overlay_2g0p.root", "NCPi0Overlay", "hist","singlephotonana/", kRed-7, signal_flow);
    bdt_file *nue = new bdt_file(dir2g0p,"vertexed_intrinsic_nue_run1_overlay.root","IntrinsicNuE","hist","singlephotonana/", kGreen+1, nue_bkg_flow);
    bdt_file *deltarad_1g1p = new bdt_file(dir2g0p,"vertexed_ncdeltarad_run1_overlay.root","NCDeltaRad","hist","singlephotonana/", kAzure+1, deltarad_1g1p_flow);

    ncpi0->calcPOT();
    nue->calcPOT();
    deltarad_1g1p->calcPOT();

    std::cout << "[EFF] Done with old-school BDT files" << std::endl;

    std::vector<bdt_file*> filtered_files = {ncpi0, nue, deltarad_1g1p};

    if(which_file == -1)which_file = 0;

    //which_file = 7;//checking ext
    std::vector<std::string> v_denom = XMLconfig.bdt_definitions[which_file];
    std::vector<std::string> v_topo = {TMVAmethods[0].topological_definition};

    if(which_stage==-1)which_stage=0;

    //bdt_efficiency(bdt_files[which_file], v_denom, v_topo, vec_precuts, fbdtcuts, what_pot,false,which_stage);
    //normally stops here

    //Ok, this runs now for a full cut
    // Cut for NC pi0 filter
    //std::string full_cut = "reco_asso_showers==2 && reco_asso_tracks==1 && reco_vertex_size>0 && (reco_vertex_x > 5.0 && reco_vertex_x < 251 && reco_vertex_y > -112 && reco_vertex_y < 112 && reco_vertex_z > 5 && reco_vertex_z < 1031) && reco_shower_conversion_distance[0]>1 && reco_shower_conversion_distance[1]>1";
    //bdt_efficiency(bdt_files,full_cut);

    std::vector<bdt_file*> bdt_eff_files = {bdt_files.at(0), bdt_files.at(1), bdt_files.at(2) };
    bdt_efficiency(bdt_eff_files, filtered_files);


}else if(mode_option == "sbnfit"){


    double splot_pot =   onbeam_data_file->pot;
    
    std::cout<<"Starting SBNfit with "<<splot_pot<<" POT"<<std::endl;

    if(which_stage==-1) which_stage ==1;
    if(which_file==-1){
        for(size_t f =0; f< bdt_files.size(); f++){
            std::cout<<"on bdt file "<<f<<std::endl;
            bdt_files[f]->makeSBNfitFile(analysis_tag, bdt_infos, which_stage, fbdtcuts,input_string,vars,splot_pot);
        }
    }else{
        bdt_files[which_file]->makeSBNfitFile(analysis_tag, bdt_infos, which_stage, fbdtcuts,input_string,vars,splot_pot);

    }
    return 0;

}else if(mode_option == "ssssbnfit"){
    if(which_stage==-1) which_stage ==1;
    if(which_file==-1){
        for(size_t f =0; f< bdt_files.size(); ++f){
            std::cout<<"on bdt file "<<f<<std::endl;
            bdt_files[f]->makePrecalcSBNfitFile(analysis_tag, which_stage, fbdtcuts);
        }
    }else{
        bdt_files[which_file]->makePrecalcSBNfitFile(analysis_tag,which_stage, fbdtcuts);

    }
    return 0;


}else if(mode_option == "export"){


    std::cout << "EXPORT starting export function" << std::endl;
    for(auto &v :vars){
        //std::cout<<v.edges[0]<<" "<<v.edges[1]<<" "<<v.edges[2]<<std::endl;
      /*
        bool is_train = false;
        for(auto &in: bdt_infos){
            for(auto &tv: in.train_vars){
                if(tv.id == v.id){ is_train=true; break;}
            }
            if(is_train)break;
        }
        */
        //if(is_train){
        if(true){
            std::cout<<"EXPORT|NAM|VID"<<v.id<<"|\""<<v.name<<"\""<<"|\""<<v.safe_name<<"\" | "<<v.n_bins<<" | "<<v.edges[1]<<" | "<<v.edges[2]<<" | \"";

            for(double k = 0; k<=v.n_bins; k++){
                double b = v.edges[1]+k*fabs(v.edges[1]-v.edges[2])/(double)v.n_bins;
                std::cout<<" "<<b;
            }
            std::cout<<"\""<<std::endl;
        }
    }



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
    std::cout<<"Starting to print vars. which_bdt is set to "<<which_bdt<<std::endl;
    for(int i=0; i< bdt_infos.size(); i++){


        std::vector<bdt_variable> t_vars;
        if(which_bdt>=0){
            t_vars = bdt_infos[which_bdt].train_vars;
        }else{
            t_vars = vars;
        }

        if((which_bdt==i || which_bdt==-1)){
            bdt_file * training_signal = tagToFileMap[bdt_infos[i].TMVAmethod.sig_train_tag]; 
            bdt_file * training_background = tagToFileMap[bdt_infos[i].TMVAmethod.bkg_train_tag]; 
            plot_bdt_variables(training_signal, training_background, t_vars, bdt_infos[i], false, which_stage,fbdtcuts);
       }
        
    }


}else {
    std::cout << "WARNING: " << mode_option << " is an invalid option\n";
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

    double title_size_ratio=0.1;
    double label_size_ratio=0.1;
    double title_offset_ratioY = 0.3 ;
    double title_offset_ratioX = 1.1;

    double title_size_upper=0.15;
    double label_size_upper=0.05;
    double title_offset_upper = 1.45;




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

    double rmin  = 0.5;
    double rmax = 1.5;

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
        th1->Draw("E1hist same");

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
        rat_denom->Draw("E1same hist");

        rat_denom->GetXaxis()->SetTitle(var.unit.c_str());
        rat_denom->SetMinimum(rmin);	
        rat_denom->SetMaximum(rmax);//ratunit->GetMaximum()*1.1);
        rat_denom->GetXaxis()->SetTitleOffset(title_offset_ratioX);
        rat_denom->GetYaxis()->SetTitleOffset(title_offset_ratioY);
        rat_denom->SetMinimum(rmin);	
        rat_denom->SetMaximum(rmax);//rat_denom->GetMaximum()*1.1);
        rat_denom->GetYaxis()->SetTitleSize(title_size_ratio);
        rat_denom->GetXaxis()->SetTitleSize(title_size_ratio);
        rat_denom->GetYaxis()->SetLabelSize(label_size_ratio);
        rat_denom->GetXaxis()->SetLabelSize(label_size_ratio);
    }

    tvec[0]->SetMaximum(max*1.5);

    pad0top->cd();
    leg->SetFillStyle(0);
    leg->SetLineColor(kWhite);
    leg->SetLineWidth(0);
    leg->Draw();
    std::string namer = "valid/"+name;
    c->SaveAs((namer+".pdf").c_str(),"pdf");

    return 0;
};



