#include "load_mva_param.h"
#include "tinyxml.h"

#include <getopt.h>
#include <sys/stat.h> 

#include "TFriendElement.h"

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

int compareQuick(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, std::string name);

int main (int argc, char *argv[]){

    //This is a standardized location on /pnfs/ that everyone can use. 
    std::string dir = "/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_mcc9_v17/";

    std::string mode_option = "fake"; 
    std::string xml = "default.xml";
    std::string topo_tag = "track";

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

    std::string analysis_tag = topo_tag;
    std::cout<<"================================================================================"<<std::endl;
    std::cout<<"=================  Beginning New World Order gLEE run =========================="<<std::endl;
    std::cout<<"================================================================================"<<std::endl;
    std::cout<<"Working with analysis tag: "<<analysis_tag<<std::endl;

    //Most TMVA arguments are loaded in here via XML
    std::cout<<"Getting xml variables"<<std::endl;
    MVALoader XMLconfig(xml);
    std::vector<method_struct> TMVAmethods  = XMLconfig.GetMethods(); 
    
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
        bdt_flow analysis_flow(topological_cuts, def, 	vec_precuts,	postcuts,	bdt_infos);
   
        bdt_files.push_back(new bdt_file(dir, XMLconfig.bdt_filenames[f].c_str(),	XMLconfig.bdt_tags[f].c_str(), XMLconfig.bdt_hist_styles[f].c_str(),XMLconfig.bdt_dirs[f].c_str(), XMLconfig.bdt_cols[f]->GetNumber() , XMLconfig.bdt_fillstyles[f] , analysis_flow));
        bdt_files.back()->addPlotName(XMLconfig.bdt_plotnames[f]);
        tagToFileMap[XMLconfig.bdt_tags[f]] = bdt_files.back();

        std::cout<<" -- Filename "<<XMLconfig.bdt_filenames[f]<<" subdir "<<XMLconfig.bdt_dirs[f]<<std::endl;
        std::cout<<" -- Color ";XMLconfig.bdt_cols[f]->Print();std::cout<<" and hist style "<<XMLconfig.bdt_hist_styles[f]<<" fillstyle "<<XMLconfig.bdt_fillstyles[f]<<std::endl;
        std::cout<<" -- With the following Definition Cuts: "<<std::endl;
        for(int i=0; i< XMLconfig.bdt_definitions[f].size(); ++i){
            std::cout<<" -- ---> "<<XMLconfig.bdt_definitions[f][i]<<std::endl;
        }

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
        }
        
        bdt_files.back()->calcPOT();
    }

    bdt_file * training_signal = tagToFileMap["NCDeltaRadOverlayTrain"];
    bdt_file * signal = tagToFileMap["NCDeltaRadOverlay"];

    
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
            if(mode_option != "train" && mode_option != "app" && mode_option != "sbnfit"){
                f->calcBaseEntryList(analysis_tag);
            }
        }
    }

    std::cout<<"--------------------------------------------------------------------------"<<std::endl;
    std::cout<<"--------------------------------------------------------------------------"<<std::endl;
    
    
    std::vector<double> fcuts = {0,0}; 
    
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
        }

        //Then we train!
        if(which_bdt == -1){
            for(int i=0; i< bdt_infos.size(); i++){
                bdt_train(bdt_infos[i], training_signal, training_background_files[i]);
                plot_train(bdt_infos[i], training_signal, training_background_files[i]);
            }
        }else{
            bdt_train(bdt_infos[which_bdt], training_signal, training_background_files[which_bdt]);
            plot_train(bdt_infos[which_bdt], training_signal, training_background_files[which_bdt]);
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
                bdt_app(bdt_infos[i], tf);
            }
        }else{
            bdt_app(bdt_infos[which_bdt], tf);
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

        for(size_t f =0; f< bdt_files.size(); ++f){
            if(bdt_files[f]->is_data) continue;
            histogram_stack->addToStack(bdt_files[f]);
        }
    
        //OffBeamData->fillstyle = 3333;

        int ip=0;
        std::vector<bool> subv = {false,false,true};
        if(!response_only){
            if(number != -1){
                bdt_datamc datamc(tagToFileMap["Data5e19"], histogram_stack, analysis_tag+"_datamc");	
                datamc.setPlotStage(which_stage);                
                
                //datamc.printPassingDataEvents("tmp", 3, fcoscut, fbnbcut);
                //datamc.setSubtractionVector(subv);
                std::vector<bdt_variable> tmp_var = {vars.at(number)};
                datamc.plotStacks(ftest,  tmp_var , fcuts);
            }else{

                bdt_datamc real_datamc(tagToFileMap["Data5e19"], histogram_stack, analysis_tag+"_datamc");	
                real_datamc.setPlotStage(which_stage);                
                
                //real_datamc.setSubtractionVector(subv);
                // real_datamc.plotStacks(ftest, vars,fcoscut,fbnbcut);
                //real_datamc.plotStacks(ftest, vars,fcoscut,fbnbcut);

                real_datamc.plotStacks(ftest, vars, fcuts);
                //real_datamc.SetSpectator();
                //real_datamc.plotStacks(ftest, plotting_vars,fcoscut,fbnbcut);
            }
        }else{
            bdt_datamc real_datamc(tagToFileMap["Data5e19"], histogram_stack, analysis_tag+"_datamc");	

               
            if(which_bdt ==-1){
                for(int k=0; k< bdt_infos.size(); k++){
                    real_datamc.plotBDTStacks(bdt_infos[k] , fcuts);
                }
            }else{
                real_datamc.plotBDTStacks(bdt_infos[which_bdt],fcuts);
            }
        }
    }else if(mode_option == "test"){

        //plot_bdt_variables(signal, ncpi0, vars, fcuts, 5);

        return 0;
    }else if(mode_option == "sig"){

        switch(number){
            case 0:
             //   scan_significance({signal, signal_SM, signal_other} , {bnb, OffBeamData, dirt,ncpi0}, bdt_infos,what_pot);
                break;
            case 1:
             //   scan_likelihood({signal,signal_SM, signal_other, bnb, OffBeamData,dirt,ncpi0}, bdt_infos);
                break;
            case 2:
             //   scan_significance_random({signal,signal_SM,signal_other},{bnb,OffBeamData,dirt,ncpi0},bdt_infos);
                break;
            default:
                break;
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
                signal->calcBDTEntryList(stage,fcuts);
                bnb->calcBDTEntryList(stage,fcuts);
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
    }
    else if(mode_option == "eff"){

        std::vector<std::string> v_denom;


        std::vector<std::string> v_topo;
        if (topo_tag == "notrack"){
            v_topo =  {"reco_vertex_size>0","reco_asso_showers==1","reco_asso_tracks==0"};

        } else{
            v_topo =  {"reco_vertex_size>0","reco_asso_showers==1","reco_asso_tracks>0"};
        }

        if(which_stage==-1)which_stage=0;

        bdt_efficiency(signal, v_denom, v_topo, vec_precuts, fcuts, what_pot,false,which_stage);



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


        std::string sbnfit_cuts = file->getStageCuts(which_stage,fcuts);
        //std::string sbnfit_cuts = "mctruth_cc_or_nc==1 && mctruth_num_exiting_pi0 >0"; //file->getStageCuts(which_stage,fcoscut,fbnbcut);

        std::cout<<"Copying vertex tree"<<std::endl;
        TTree * t_sbnfit_tree = (TTree*)file->tvertex->CopyTree(sbnfit_cuts.c_str());
        std::cout<<"Copying POT tree"<<std::endl;
        TTree * t_sbnfit_pot_tree = (TTree*)file->tpot->CopyTree("1");
        std::cout<<"Copying eventweight tree (via friends)"<<std::endl;
        TTree * t_sbnfit_eventweight_tree = (TTree*)file->teventweight->CopyTree(sbnfit_cuts.c_str());
        std::cout<<"Copying Slice tree "<<std::endl;
        TTree * t_sbnfit_slice_tree = (TTree*)file->tslice->CopyTree("1");



        // TFriendElement *fr0 = (TFriendElement*)t_sbnfit_eventweight_tree->GetListOfFriends()->FindObject((file->tag +"_"+cosmic_bdt_info.identifier).c_str());
        // TFriendElement *fr1 = (TFriendElement*)t_sbnfit_eventweight_tree->GetListOfFriends()->FindObject((file->tag +"_"+bnb_bdt_info.identifier).c_str());
        // t_sbnfit_eventweight_tree->GetListOfFriends()->Remove(fr0);
        // t_sbnfit_eventweight_tree->GetListOfFriends()->Remove(fr1);


        // TFriendElement *fr2 = (TFriendElement*)t_sbnfit_tree->GetListOfFriends()->FindObject((file->tag +"_"+cosmic_bdt_info.identifier).c_str() );
        // TFriendElement *fr3 = (TFriendElement*)t_sbnfit_tree->GetListOfFriends()->FindObject((file->tag +"_"+bnb_bdt_info.identifier).c_str());
        //t_sbnfit_tree->GetListOfFriends()->Remove(fr2);
        //t_sbnfit_tree->GetListOfFriends()->Remove(fr3);


        std::cout<<"Writing to file"<<std::endl;
        cdtof->cd();
        t_sbnfit_tree->Write();
        t_sbnfit_pot_tree->Write();
        t_sbnfit_eventweight_tree->Write(); 
        t_sbnfit_slice_tree->Write();
        f_sbnfit->Close();
        std::cout<<"Done!"<<std::endl;


        return 0;


    
/*
    else if(mode_option == "recomc"){
        if (access("recomc",F_OK) == -1){
            mkdir("recomc",0777);//Create a folder for pdf.
        }
        else{
            std::cout<<"Overwrite recomc/ in 2 seconds, 1 seconds, ..."<<std::endl;
            sleep(2);

            std::vector<int> recomc_cols = {kRed-7, kBlue+3, kBlue, kBlue-7, kMagenta-3, kYellow-7,kOrange-3, kGreen+1 , kGray};
            //        std::vector<std::string> recomc_names = {"NC #Delta Radiative #gamma", "CC #pi^{0}", "NC #pi^{0}","Non #pi^{0} #gamma","Intrinsic #nu_{e} electron","BNB Michel e^{#pm}","BNB Other Non #gamma",  "Overlay","Other"};
            //         std::vector<int> recomc_cols = { kYellow-7,kOrange-3, kGreen+1 ,kGray};
            std::vector<std::string> recomc_names = {"NC #Delta Radiative #gamma", "CC #pi^{0}", "NC #pi^{0}","Non #pi^{0} #gamma","Intrinsic #nu_{e} electron","BNB Michel e^{#pm}", "Other NC", "Other CC", "Cosmic (Overlay)"};

            std::string overlay = "sim_shower_overlay_fraction[0] >= 0.8";

            std::string ncdelta = "sim_shower_pdg[0] == 22 && sim_shower_parent_pdg[0] != 111 && mctruth_is_delta_radiative ==1 && !("+overlay+")";
            std::string ccpi0 = "sim_shower_pdg[0] == 22 && sim_shower_parent_pdg[0] == 111 && mctruth_cc_or_nc==0 && !("+overlay+")";
            std::string ncpi0 = "sim_shower_pdg[0] == 22 && sim_shower_parent_pdg[0] == 111 && mctruth_cc_or_nc==1 && !("+overlay+")";
            std::string othergamma =  "sim_shower_pdg[0] == 22 && sim_shower_parent_pdg[0] != 111 && mctruth_is_delta_radiative!=1 && !("+overlay+")";
            std::string  nue = "abs(mctruth_lepton_pdg[0])==11 && abs(sim_shower_pdg[0]) ==11  && !("+overlay+")"; // && (exiting_electron_number==1 || exiting_antielectron_number==1)";
            std::string  michel = "abs(sim_shower_pdg[0]) ==11 && abs(sim_shower_parent_pdg[0])==13 && !("+overlay+")";
            //std::string bnbother =  "sim_shower_pdg[0]!=22 && !("+nue+") && !("+michel+")  && !("+overlay+")";
            // std::string overlay = "sim_shower_overlay_fraction[0] == 1";
            //std::vector<std::string> recomc_cuts = {ncdelta,ccpi0,ncpi0,othergamma,nue,michel,bnbother};
            std::vector<std::string> recomc_cuts = {ncdelta,ccpi0,ncpi0,othergamma,nue,michel};


            std::string othercc = "!("+overlay+") && ! mctruth_cc_or_nc==0";
            for(auto s: recomc_cuts){
                othercc += "&& !("+s+")";
            }

            std::string othernc = "!("+overlay+") && ! mctruth_cc_or_nc==1";
            for(auto s: recomc_cuts){
                othernc += "&& !("+s+")";
            }

            recomc_cuts.push_back(othernc);
            recomc_cuts.push_back(othercc);

            std::cout<<"other nc = "<<othernc<<std::endl;

            std::string other = "!("+overlay+")";
            for(auto s: recomc_cuts){
                othernc += "&& !("+s+")";
            }

            // recomc_cuts.push_back(other);
            recomc_cuts.push_back(overlay);


            bdt_recomc recomc(recomc_names, recomc_cuts, recomc_cols,analysis_tag);

            TFile * ftest = new TFile(("test+"+analysis_tag+".root").c_str(),"recreate");
            if(!response_only){
                int h=0;

                if(number !=-1){
                    std::vector<bdt_variable> tmp = {vars.at(number)};
                    recomc.plot_recomc(ftest, bnb, tmp, fcoscut, fbnbcut);
                    return 0;

                }else{
                    recomc.plot_recomc(ftest, bnb, vars, fcoscut, fbnbcut);
                }	
            }

            if(response_only){
                recomc.is_log = true;
                // recomc.plot_recomc(ftest, signal, (std::vector<bdt_variable>){signal->getBDTVariable(cosmic_bdt_info)} , fcoscut,fbnbcut);
                // recomc.plot_recomc(ftest, bnb, (std::vector<bdt_variable>){bnb->getBDTVariable(cosmic_bdt_info)} , fcoscut,fbnbcut);
                //  recomc.plot_recomc(ftest, bnb, (std::vector<bdt_variable>){bnb->getBDTVariable(bnb_bdt_info)} , fcoscut,fbnbcut);
                //  recomc.plot_recomc(ftest, signal, (std::vector<bdt_variable>){signal->getBDTVariable(bnb_bdt_info)} , fcoscut,fbnbcut);

                recomc.is_log = false;
            }

        }

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

    else if(mode_option == "sig"){


    TFile *fsig = new TFile(("significance_"+analysis_tag+".root").c_str(),"recreate");
    std::vector<double> ans = scan_significance(fsig, {signal} , {bnb, ncpi0, nueintrinsic, OffBeamData, dirt}, cosmic_bdt_info, bnb_bdt_info);
    //std::vector<double> ans = lin_scan({signal}, {bnb, OffBeamData}, cosmic_bdt_info, bnb_bdt_info,fcoscut,fbnbcut);

    std::cout<<"Best Fit Significance: "<<ans.at(0)<<" "<<ans.at(1)<<" "<<ans.at(2)<<std::endl;
    fsig->Close();


}else if(mode_option == "stack"){
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
    
       }else if(mode_option == "sbnfit"){
       if(number==-1) number ==0;

       bdt_file * file = bdt_files.at(number);

//have to first add the vertex tree as a friend to the eventweight tree, you will see why later.. if i get to those comments
file->teventweight->AddFriend(file->tvertex);

std::string output_file_name = "sbnfit_"+analysis_tag+"_stage_"+std::to_string(sbnfit_stage)+"_"+file->tag+".root";

std::cout<<"Starting to make SBNFit output file named: "<<output_file_name<<std::endl;
TFile* f_sbnfit = new TFile(output_file_name.c_str(),"recreate");


std::cout<<"Creating directory structure"<<std::endl;
TDirectory *cdtof = f_sbnfit->mkdir("singlephoton");
cdtof->cd();    


std::string sbnfit_cuts = file->getStageCuts(sbnfit_stage,fcoscut,fbnbcut);

std::cout<<"Copying vertex tree"<<std::endl;
TTree * t_sbnfit_tree = (TTree*)file->tvertex->CopyTree(sbnfit_cuts.c_str());
std::cout<<"Copying POT tree"<<std::endl;
TTree * t_sbnfit_pot_tree = (TTree*)file->tpot->CopyTree("1");
std::cout<<"Copying eventweight tree (via friends)"<<std::endl;
TTree * t_sbnfit_eventweight_tree = (TTree*)file->teventweight->CopyTree(sbnfit_cuts.c_str());


TFriendElement *fr0 = (TFriendElement*)t_sbnfit_eventweight_tree->GetListOfFriends()->FindObject((file->tag +"_"+cosmic_bdt_info.identifier).c_str());
TFriendElement *fr1 = (TFriendElement*)t_sbnfit_eventweight_tree->GetListOfFriends()->FindObject((file->tag +"_"+bnb_bdt_info.identifier).c_str());
t_sbnfit_eventweight_tree->GetListOfFriends()->Remove(fr0);
t_sbnfit_eventweight_tree->GetListOfFriends()->Remove(fr1);


TFriendElement *fr2 = (TFriendElement*)t_sbnfit_tree->GetListOfFriends()->FindObject((file->tag +"_"+cosmic_bdt_info.identifier).c_str() );
TFriendElement *fr3 = (TFriendElement*)t_sbnfit_tree->GetListOfFriends()->FindObject((file->tag +"_"+bnb_bdt_info.identifier).c_str());
t_sbnfit_tree->GetListOfFriends()->Remove(fr2);
t_sbnfit_tree->GetListOfFriends()->Remove(fr3);


std::cout<<"Writing to file"<<std::endl;
cdtof->cd();
t_sbnfit_tree->Write();
t_sbnfit_pot_tree->Write();
t_sbnfit_eventweight_tree->Write(); 

f_sbnfit->Close();
std::cout<<"Done!"<<std::endl;


return 0;


}*/
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


int compareQuick(std::vector<bdt_variable> vars, std::vector<bdt_file*> files, std::vector<std::string> cuts, std::string name){

    TCanvas *c = new TCanvas();
    c->cd(); 



    std::vector<int> cols = {kRed-7,  kBlue-7, kGreen+1 ,kGray};
    TLegend* leg=new TLegend(0.55,0.55,0.9,0.9);


    std::string testcut = "1";
    for(int i=0; i< files.size();i++){

        c->cd();
        TH1* th1 =  (TH1*) files[i]->getTH1(vars[i], testcut+"&&"+cuts[i], "photon_truth_overlay"+std::to_string(i), 13.2e20, 1);
        c->cd();

        th1->SetLineColor(cols[i]);
        th1->SetLineWidth(2);

        double norm = th1->Integral();
        th1->Scale(1.0/norm);


        th1->Draw("hist same");

        th1->SetMaximum(th1->GetMaximum()*1.5);
        th1->GetXaxis()->SetTitle(vars[i].unit.c_str());
        th1->SetTitle(vars[i].unit.c_str());

        leg->AddEntry(th1,files[i]->tag.c_str(),"l");


    }

    leg->Draw();
    c->SaveAs((name+".pdf").c_str(),"pdf");

    return 0;
};
