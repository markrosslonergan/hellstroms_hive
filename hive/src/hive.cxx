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
    double what_pot = 13.2e20;
    int which_file = -1;
    int which_bdt = -1;
    int which_stage = -1;

    //All of this is just to load in command-line arguments, its not that important
    const struct option longopts[] = 
    {
        {"dir", 		required_argument, 	0, 'd'},
        {"option",		required_argument,	0, 'o'},
        {"xml"	,		required_argument,	0, 'x'},
        {"topo_tag",	required_argument,	0, 't'},
        {"bdt",		    required_argument,	0, 'b'},
        {"stage",		required_argument,	0, 's'},
        {"help",		required_argument,	0, 'h'},
        {"pot",		    required_argument,	0, 'p'},
        {"number",		required_argument,	0, 'n'},
        {"response",	no_argument,	0, 'r'},
        {"file",		required_argument,	0, 'f'},
        {0,			    no_argument, 		0,  0},
    };

    int iarg = 0; opterr=1; int index;
    while(iarg != -1)
    {
        iarg = getopt_long(argc,argv, "x:o:d:s:f:t:p:b:n:rh?", longopts, &index);

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
                std::cout<<"\t\t\t\t sbnfit: Makes a file at stage S (set with --stage S) for file set with --file"<<std::endl;
                std::cout<<"\t\t\t\t vars: Prints training variables"<<std::endl;
                std::cout<<"\t-b\t--bdt\t\t Run only N BDT training/app, or BDT specific option"<<std::endl;
                std::cout<<"\t-f\t--file\t\t Which file in bdt_files you want to run over, for file specifc options."<<std::endl;
                std::cout<<"\t-p\t--pot\t\tSet POT for plots"<<std::endl;
                std::cout<<"\t-s\t--stage\t\tSet what stage to do things at."<<std::endl;
                std::cout<<"\t-r\t--response\t\t Run only BDT response plots for datamc/recomc"<<std::endl;
                std::cout<<"\t-t\t--topo_tag\t\tTopological Tag [Superseeded by XML defined tag]"<<std::endl;
                std::cout<<"\t-d\t--dir\t\tDirectory for file inputs [Superseeded by XML]"<<std::endl;
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



        if(XMLconfig.bdt_scales[f] != 1.0){
            std::cout<<" -- Scaling "<<XMLconfig.bdt_tags[f]<<" file by a factor of "<<XMLconfig.bdt_scales[f]<<std::endl;
            bdt_files.back()->scale_data = XMLconfig.bdt_scales[f];
        }


        if(XMLconfig.bdt_is_onbeam_data[f]){
            std::cout<<" -- Setting as ON beam data with "<<XMLconfig.bdt_onbeam_pot[f]/1e19<<" e19 POT equivalent"<<std::endl;
            bdt_files.back()->setAsOnBeamData(XMLconfig.bdt_onbeam_pot[f]); //tor860_wc
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
        }


        bdt_files.back()->calcPOT();
    }

    //BAD practice! 
    //signal = tagToFileMap["NCPi0"];
    signal = tagToFileMap["NCPi0eltaRadOverlay"];


    std::vector<bdt_file*> stack_bdt_files = signal_bdt_files;
    stack_bdt_files.insert(stack_bdt_files.end(), bkg_bdt_files.begin(), bkg_bdt_files.end());

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

            if(mode_option != "app" && mode_option != "train" ){
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


    if(mode_option == "train") {
        //First lets create the bdt_file's* and flows for training
        std::vector<bdt_file*> training_background_files;
        for(int i=0; i< bdt_infos.size(); i++){
            std::cout<<"Starting to make a Training BDT_FILE for BDT number "<<i<<" "<<bdt_infos[i].identifier<<std::endl;
            bdt_flow tmp_flow(topological_cuts, bdt_infos[i].TMVAmethod.training_cut ,	vec_precuts, postcuts,	bdt_infos);
            training_background_files.push_back( new bdt_file("/",bdt_infos[i].TMVAmethod.filename, "BDT_background_"+bdt_infos[i].identifier+"_"+std::to_string(i),"hist", bdt_infos[i].TMVAmethod.foldername, kBlack,tmp_flow)); 
            training_background_files.back()->calcPOT();
            if(bdt_infos[i].TMVAmethod.str=="XGBoost"){
                convertToLibSVM(bdt_infos[i], training_signal, training_background_files[i]);
            }

        }

        //Then we train!
        if(which_bdt == -1){
            for(int i=0; i< bdt_infos.size(); i++){

                if(bdt_infos[i].TMVAmethod.str=="XGBoost"){
                    bdt_XGtrain(bdt_infos[i]);
                }else{
                    bdt_train(bdt_infos[i], training_signal, training_background_files[i]);
                    plot_train(bdt_infos[i], training_signal, training_background_files[i]);
                }

            }
        }else{

            if(bdt_infos[which_bdt].TMVAmethod.str=="XGBoost"){
                bdt_XGtrain(bdt_infos[which_bdt]);
            }else{
                bdt_train(bdt_infos[which_bdt], training_signal, training_background_files[which_bdt]);
                plot_train(bdt_infos[which_bdt], training_signal, training_background_files[which_bdt]);
            }
        }
        return 0;



    }else if (mode_option=="tplot"){
        std::vector<bdt_file*> training_background_files;
        for(int i=0; i< bdt_infos.size(); i++){
            std::cout<<"Starting to make a Training BDT_FILE for BDT number "<<i<<" "<<bdt_infos[i].identifier<<std::endl;
            bdt_flow tmp_flow(topological_cuts, bdt_infos[i].TMVAmethod.training_cut ,	vec_precuts, postcuts,	bdt_infos);
            training_background_files.push_back( new bdt_file("/",bdt_infos[i].TMVAmethod.filename, "BDT_background_"+bdt_infos[i].identifier+"_"+std::to_string(i),"hist", bdt_infos[i].TMVAmethod.foldername, kBlack,tmp_flow)); 
            training_background_files.back()->calcPOT();
        }

        for(int i=0; i< bdt_infos.size(); i++){
        }

        //Then we train!
        if(which_bdt == -1){
            for(int i=0; i< bdt_infos.size(); i++){
                plot_train(bdt_infos[i], training_signal, training_background_files[i]);
            }
        }else{
            plot_train(bdt_infos[which_bdt], training_signal, training_background_files[which_bdt]);
        }

        return 0;





    }else if(mode_option == "app"){

        std::vector<bdt_file*> tf;
        if(which_file==-1){
            tf = bdt_files;
        }else{
            tf = {bdt_files[which_file]};
        }
        if(which_bdt == -1){
            for(int i=0; i< bdt_infos.size();i++){

                if(bdt_infos[i].TMVAmethod.str=="XGBoost"){
                    bdt_XGapp(bdt_infos[i], tf);

                }else{
                    bdt_app(bdt_infos[i], tf);
                }
            }
        }else{

            if(bdt_infos[which_bdt].TMVAmethod.str=="XGBoost"){
                bdt_XGapp(bdt_infos[which_bdt], tf);
            }else{
                bdt_app(bdt_infos[which_bdt], tf);
            }

        }
        return 0;
    }


    else if(mode_option == "datamc"){
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

        histogram_stack->plot_pot = tagToFileMap["Data5e19"]->pot;

        for(size_t f =0; f< stack_bdt_files.size(); ++f){
            if(bdt_files[f]->is_data) continue;
            histogram_stack->addToStack(stack_bdt_files[f]);
        }

        int ip=0;
        std::vector<bool> subv = {false,false,true};
        if(!response_only){
            if(number != -1){
                bdt_datamc datamc(tagToFileMap["Data5e19"], histogram_stack, analysis_tag+"_datamc");	
                datamc.setPlotStage(which_stage);                

                //datamc.printPassingDataEvents("tmp", 3, fcoscut, fbnbcut);
                //datamc.setSubtractionVector(subv);
                std::vector<bdt_variable> tmp_var = {vars.at(number)};
                datamc.plotStacks(ftest,  tmp_var , fbdtcuts);
            }else{

                bdt_datamc real_datamc(tagToFileMap["Data5e19"], histogram_stack, analysis_tag+"_datamc");	
                real_datamc.setPlotStage(which_stage);                

                //real_datamc.setSubtractionVector(subv);
                // real_datamc.plotStacks(ftest, vars,fcoscut,fbnbcut);
                //real_datamc.plotStacks(ftest, vars,fcoscut,fbnbcut);

                real_datamc.plotStacks(ftest, vars, fbdtcuts);
                //real_datamc.SetSpectator();
                //real_datamc.plotStacks(ftest, plotting_vars,fcoscut,fbnbcut);
            }
        }else{
            bdt_datamc real_datamc(tagToFileMap["Data5e19"], histogram_stack, analysis_tag+"_datamc");	

            if(which_bdt ==-1){
                for(int k=0; k< bdt_infos.size(); k++){
                    real_datamc.plotBDTStacks(bdt_infos[k] , fbdtcuts);
                }
            }else{
                real_datamc.plotBDTStacks(bdt_infos[which_bdt],fbdtcuts);
            }
        }
    }else if(mode_option == "test"){

        //First lets create the bdt_file's* and flows for training

        if(true){
            std::vector<bdt_file*> training_background_files;
            for(int i=0; i< bdt_infos.size(); i++){
                std::cout<<"Starting to make a Training BDT_FILE for BDT number "<<i<<" "<<bdt_infos[i].identifier<<std::endl;
                bdt_flow tmp_flow(topological_cuts, bdt_infos[i].TMVAmethod.training_cut ,	vec_precuts, postcuts,	bdt_infos);
                training_background_files.push_back( new bdt_file("/",bdt_infos[i].TMVAmethod.filename, "BDT_background_"+bdt_infos[i].identifier+"_"+std::to_string(i),"hist", bdt_infos[i].TMVAmethod.foldername, kBlack,tmp_flow)); 
                training_background_files.back()->calcPOT();
                convertToLibSVM(bdt_infos[i], training_signal, training_background_files[i]);
            }
        }

        for(int i=2; i< bdt_infos.size(); i++){
            bdt_XGtrain(bdt_infos[i]);
            bdt_XGapp(bdt_infos[i], bdt_files);
            break;
        }

        //plot_bdt_variables(signal, ncpi0, vars, fbdtcuts, 5);

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
            default:
                break;
        }
        return 0;
    }else if(mode_option == "scatter"){

        plot_scatter(bdt_files[0], bdt_files[5], bdt_infos[0], bdt_infos[1]);

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

    if(which_file == -1)which_file = 0;

    std::vector<std::string> v_denom = XMLconfig.bdt_definitions[which_file];
    std::vector<std::string> v_topo = {TMVAmethods[which_file].topological_definition};

    if(which_stage==-1)which_stage=0;

    bdt_efficiency(bdt_files[which_file], v_denom, v_topo, vec_precuts, fbdtcuts, what_pot,false,which_stage);
    //normally stops here

    //Ok, this runs now for a full cut
    std::string full_cut = "reco_asso_showers==1 && reco_asso_tracks==1";
    bdt_efficiency(bdt_files,full_cut);


}else if(mode_option == "sbnfit"){
    if(which_file==-1) which_file ==0;
    if(which_stage==-1) which_stage ==1;

    bdt_file * file = bdt_files.at(which_file);

    //have to first add the vertex tree as a friend to the eventweight tree, you will see why later.. if i get to those comments
    file->teventweight->AddFriend(file->tvertex);

    std::string output_file_name = "sbnfit_"+analysis_tag+"_stage_"+std::to_string(which_stage)+"_"+file->tag+".root";

    std::cout<<"Starting to make SBNFit output file named: "<<output_file_name<<std::endl;
    TFile* f_sbnfit = new TFile(output_file_name.c_str(),"recreate");


    std::cout<<"Creating directory structure"<<std::endl;
    TDirectory *cdtof = f_sbnfit->mkdir("singlephoton");
    cdtof->cd();    


    std::string sbnfit_cuts = file->getStageCuts(which_stage,fbdtcuts);
    //std::string sbnfit_cuts = "mctruth_cc_or_nc==1 && mctruth_num_exiting_pi0 >0"; //file->getStageCuts(which_stage,fcoscut,fbnbcut);

    std::cout<<"Copying vertex tree"<<std::endl;
    TTree * t_sbnfit_tree = (TTree*)file->tvertex->CopyTree(sbnfit_cuts.c_str());
    std::cout<<"Copying POT tree"<<std::endl;
    TTree * t_sbnfit_pot_tree = (TTree*)file->tpot->CopyTree("1");
    std::cout<<"Copying eventweight tree (via friends)"<<std::endl;
    TTree * t_sbnfit_eventweight_tree = (TTree*)file->teventweight->CopyTree(sbnfit_cuts.c_str());
    std::cout<<"Copying Slice tree "<<std::endl;
    TTree * t_sbnfit_slice_tree = (TTree*)file->tslice->CopyTree("1");


    TList * lf1 = (TList*)t_sbnfit_tree->GetListOfFriends();
    for(const auto&& obj: *lf1) t_sbnfit_tree->GetListOfFriends()->Remove(obj);

    TList * lf2 = (TList*)t_sbnfit_eventweight_tree->GetListOfFriends();
    for(const auto&& obj: *lf2) t_sbnfit_eventweight_tree->GetListOfFriends()->Remove(obj);

    std::cout<<"Writing to file"<<std::endl;
    cdtof->cd();
    t_sbnfit_tree->Write();
    t_sbnfit_pot_tree->Write();
    t_sbnfit_eventweight_tree->Write(); 
    t_sbnfit_slice_tree->Write();
    f_sbnfit->Close();
    std::cout<<"Done!"<<std::endl;


    return 0;


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

    std::vector<bdt_file*> training_background_files;
    for(int i=0; i< bdt_infos.size(); i++){
        std::cout<<"Starting to make a Training BDT_FILE for BDT number "<<i<<" "<<bdt_infos[i].identifier<<std::endl;
        bdt_flow tmp_flow(topological_cuts, bdt_infos[i].TMVAmethod.training_cut ,	vec_precuts, postcuts,	bdt_infos);
        training_background_files.push_back( new bdt_file("/",bdt_infos[i].TMVAmethod.filename, "BDT_background_"+bdt_infos[i].identifier+"_"+std::to_string(i),"hist", bdt_infos[i].TMVAmethod.foldername, kBlack,tmp_flow)); 
        training_background_files.back()->calcPOT();
        training_background_files.back()->calcBaseEntryList(analysis_tag);


    }


    if(which_bdt==-1){
        for(int k=0; k< bdt_infos.size(); k++){
            if(number != -1){
                plot_bdt_variable(training_signal, training_background_files[k], vars.at(number), bdt_infos[k], false);
            }else{
                plot_bdt_variables(training_signal, training_background_files[k], vars, bdt_infos[k], false);
            }
        }
    }else{
        if(number != -1){
            plot_bdt_variable(training_signal, training_background_files[which_bdt], vars.at(number), bdt_infos[which_bdt], false);
        }else{
            plot_bdt_variables(training_signal, training_background_files[which_bdt], vars, bdt_infos[which_bdt], false);
        }
    }

}else {
    std::cout << "WARNING: " << mode_option << " is an invalid option\n";
}

return 0;

}




