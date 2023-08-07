#include "bdt_train.h"

int plot_train(bdt_info &info, bdt_file *signal_train_file, bdt_file *signal_test_file,   bdt_file *background_train_file,  bdt_file *background_test_file){
    std::vector<method_struct> methods = {info.TMVAmethod};
    std::vector<bdt_variable> variables = info.train_vars;

    std::string const name = info.identifier;

    std::cout<<"bdt_train::bdt_train()\t\t||\t\t Finished Training! Going to do some tests and make some plots."<<std::endl;
    TFile * readbackfile = TFile::Open((name+"_training.root").c_str(), "read");
    TTree * trainTree = (TTree*)readbackfile->Get( ("BDTxmls_"+name+"/TrainTree").c_str());
    TTree * testTree = (TTree*)readbackfile->Get( ("BDTxmls_"+name+"/TestTree").c_str());

    std::cout<<"We had "<<trainTree->GetEntries()<<" total training events and "<<testTree->GetEntries()<<" total testing events"<<std::endl;

    TCanvas *c_var = new TCanvas((name+"_training_plots").c_str(), (name+"_training_plots").c_str(),2200,1000);
    c_var->Divide(2,1);
    TPad *p1 = (TPad*)c_var->cd(1);

    //p1->SetLogy();
    //   int nbins = 50;
    int nbins = 75;
    TH1D * train_signal = new TH1D("trainsignal","trainsignal",nbins,0,1);
    TH1D * test_signal = new TH1D("testsignal","testsignal",nbins,0,1);

    TH1D * train_bkg = new TH1D("trainbkg","trainbkg",nbins,0,1);
    TH1D * test_bkg = new TH1D("testbkg","testbkg",nbins,0,1);

    int test_classID =0;
    int train_classID =0;

    float test_wei=0;
    float train_wei=0;

    float test_response=0;
    float train_response=0;

    testTree->SetBranchAddress("classID",&test_classID);
    trainTree->SetBranchAddress("classID",&train_classID);
    testTree->SetBranchAddress((info.TMVAmethod.str).c_str(),&test_response);
    trainTree->SetBranchAddress((info.TMVAmethod.str).c_str(),&train_response);

    //testTree->SetBranchAddress((info.TMVAmethod.bdt_tag).c_str(),&test_response);
    //trainTree->SetBranchAddress((info.TMVAmethod.bdt_tag).c_str(),&train_response);
    //   testTree->SetBranchAddress("BDT",&test_response);
    // trainTree->SetBranchAddress("BDT",&train_response);

    testTree->SetBranchAddress("weight",&test_wei);
    trainTree->SetBranchAddress("weight",&train_wei);


    float min = FLT_MAX;
    float max = FLT_MIN;

    for(int i=0; i< testTree->GetEntries(); i++){
        testTree->GetEntry(i);
        if(test_classID==0){
            test_signal->Fill(test_response,test_wei);
        }else if(test_classID==1){
            test_bkg->Fill(test_response,test_wei);
        }
        min = std::min(min,test_response);
        max = std::max(max,test_response);
    }

    for(int i=0; i< trainTree->GetEntries(); i++){
        trainTree->GetEntry(i);
        if(train_classID==0){
            train_signal->Fill(train_response,train_wei);
        }else if(train_classID==1){
            train_bkg->Fill(train_response,train_wei);
        }
        min = std::min(min,train_response);
        max = std::max(max,train_response);

    }


    test_signal->Scale(1.0/test_signal->Integral());			
    train_signal->Scale(1.0/train_signal->Integral());			
    test_bkg->Scale(1.0/test_bkg->Integral());			
    train_bkg->Scale(1.0/train_bkg->Integral());			

    //Some KS tests. 
    double signal_KS = train_signal->KolmogorovTest(test_signal,"X");
    double bkg_KS = train_bkg->KolmogorovTest(test_bkg,"X");

    std::cout<<"KS Tests! -------"<<std::endl;
    std::cout<<"--- Signal Train V Test Samples : "<<signal_KS<<std::endl;
    std::cout<<"--- Background Train V Test Samples : "<<bkg_KS<<std::endl;


    test_signal->SetLineColor(signal_test_file->col);
    train_signal->SetLineColor(signal_train_file->col);
    train_signal->SetMarkerColor(signal_train_file->col);

    train_bkg->SetLineColor(background_test_file->col);
    train_bkg->SetMarkerColor(background_train_file->col);
    test_bkg->SetLineColor(background_train_file->col);

    train_signal->SetLineWidth(1);
    train_signal->SetLineWidth(1);
    test_signal->SetLineWidth(1);
    test_bkg->SetLineWidth(1);

    train_signal->SetMarkerStyle(20);
    train_bkg->SetMarkerStyle(20);

    p1->cd();
    test_signal->SetStats(0);
    test_signal->Draw("hist");
    test_bkg->Draw("hist same");

    test_signal->SetMinimum(0.0);
    test_signal->GetXaxis()->SetRangeUser(min*0.9,max*1.1);
    //            test_signal->GetXaxis()->SetRangeUser(0.1,1);
    test_signal->GetXaxis()->SetTitle((name+" BDT Response").c_str());
    test_signal->SetTitle("");

    train_signal->Draw("E1P same");
    train_bkg->Draw("E1P same");

    test_signal->GetYaxis()->SetTitle("Events [Area Normalized]");
    test_signal->GetYaxis()->SetTitleOffset(1.5);

    TLegend *l = new TLegend(0.11,0.75,0.89,0.89);
    l->SetLineColor(kWhite);
    l->SetFillStyle(0);
    l->SetNColumns(2);

    l->AddEntry(test_signal, ("Test "+signal_train_file->plot_name).c_str(),"l");	
    l->AddEntry(train_signal, ("Train "+signal_test_file->plot_name).c_str(),"p");	
    l->AddEntry(test_bkg, ("Test "+background_train_file->plot_name).c_str(),"l");	
    l->AddEntry(train_bkg, ("Train "+background_test_file->plot_name).c_str(),"p");	
    l->Draw();

    TLatex pottex;
    pottex.SetTextSize(0.03);
    pottex.SetTextAlign(13);  //align at top
    pottex.SetNDC();
    std::string pot_draw = "Signal KS("+std::to_string(signal_KS)+"), Bkg KS( "+std::to_string(bkg_KS)+")";

    pottex.DrawLatex(.15,.74, pot_draw.c_str());



    double max_height = std::max( train_signal->GetMaximum(), train_bkg->GetMaximum());
    test_signal->SetMaximum(max_height*1.4);


    TPad *p2 = (TPad*)c_var->cd(2);
    p2->SetLogy();

    TH1D* test_signal2 = (TH1D*)test_signal->Clone(("tt"+name).c_str());
    test_signal2->SetMinimum(0.001);

    test_signal2->Draw("hist");
    test_bkg->Draw("hist same");

    //       test_signal->SetMinimum(0);
    train_signal->Draw("E1P same");
    train_bkg->Draw("E1P same");
    //l->Draw();
    //pottex.DrawLatex(.15,.74, pot_draw.c_str());





    c_var->Print((name+"_training_plots.pdf").c_str(),"pdf");



    return 0;

}

int bdt_train(bdt_info& info, bdt_file *signal_train_file, bdt_file * signal_test_file, bdt_file * background_train_file, bdt_file *background_test_file){

    std::vector<method_struct> methods = {info.TMVAmethod};
    std::vector<bdt_variable> variables = info.train_vars;

    return bdt_train(info, signal_train_file,signal_test_file, background_train_file,background_test_file, variables, methods);
}

int bdt_train(bdt_info &info, bdt_file *signal_train_file, bdt_file * signal_test_file, bdt_file * background_train_file, bdt_file *background_test_file, std::vector<bdt_variable>& variables, std::vector<method_struct> & methods){

    std::string const name = info.identifier;
    TFile * outfile = TFile::Open((name+"_training.root").c_str(), "recreate");

    TMVA::Factory * factory = new TMVA::Factory(name.c_str(), outfile,
            //"!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification");
        "!V:!Silent:Color:DrawProgressBar:AnalysisType=Classification");
    TMVA::DataLoader * dataloader = new TMVA::DataLoader(("BDTxmls_"+name).c_str());

    int bdt_precut_stage = 1;
    TCut sig_traincut =  TCut(signal_train_file->getStageCuts(bdt_precut_stage,-9,-9).c_str());
    TCut back_traincut = TCut(background_train_file->getStageCuts(bdt_precut_stage,-9,-9).c_str());

    TCut sig_testcut =  TCut(("("+signal_test_file->getStageCuts(bdt_precut_stage,-9,-9)+"&&"+info.TMVAmethod.sig_test_cut+")" ).c_str());
    TCut back_testcut =  TCut(("("+background_test_file->getStageCuts(bdt_precut_stage,-9,-9)+"&&"+info.TMVAmethod.bkg_test_cut+")" ).c_str());

    TTree * background_traintree_prefiltered = (TTree*)background_train_file->tvertex->CopyTree(back_traincut);
    TTree * signal_traintree_prefiltered = (TTree*)signal_train_file->tvertex->CopyTree(sig_traincut);

    TTree * background_testtree_prefiltered = (TTree*)background_test_file->tvertex->CopyTree(back_testcut);
    TTree * signal_testtree_prefiltered = (TTree*)signal_test_file->tvertex->CopyTree(sig_testcut);

    dataloader->AddSignalTree(signal_traintree_prefiltered,1.0,TMVA::Types::ETreeType::kTraining);
    dataloader->AddSignalTree(signal_testtree_prefiltered,1.0,TMVA::Types::ETreeType::kTesting);

    int signal_train_entries = signal_train_file->tvertex->GetEntries(sig_traincut);
    int signal_test_entries = signal_test_file->tvertex->GetEntries(sig_testcut);

    dataloader->AddBackgroundTree(background_traintree_prefiltered,1.0,TMVA::Types::ETreeType::kTraining);
    dataloader->AddBackgroundTree(background_testtree_prefiltered,1.0,TMVA::Types::ETreeType::kTesting);

    int background_train_entries = background_train_file->tvertex->GetEntries(back_traincut);
    int background_test_entries  = background_test_file->tvertex->GetEntries(back_testcut);

    dataloader->SetSignalWeightExpression((signal_train_file->weight_branch+"*"+signal_train_file->run_weight_string).c_str());
    dataloader->SetBackgroundWeightExpression((background_train_file->weight_branch+"*"+background_train_file->run_weight_string).c_str());

    for(bdt_variable &var: variables) dataloader->AddVariable(var.name.c_str());

    std::cout<<"signal_train_entries: "<<signal_train_entries<<" background_train_entries: "<<background_train_entries<<std::endl;
    std::cout<<"signal_test_entries: "<<signal_test_entries<<" background_test_entries: "<<background_test_entries<<std::endl;

    dataloader->PrepareTrainingAndTestTree("1","1","NormMode=NumEvents:!V");

    for(method_struct const & method : methods) factory->BookMethod(dataloader, method.type, method.str, method.option);

    factory->TrainAllMethods();
    factory->TestAllMethods();
    factory->EvaluateAllMethods();

    outfile->Close();

    delete factory;
    delete dataloader;

    return 0;
}


int convertToLibSVM(bdt_info& info, bdt_file *file){

    std::vector<bdt_variable> variables = info.train_vars;
    std::string const name = info.identifier;
    std::cout<<"Beginninng to convert files into a libSVM format for XGBoost"<<std::endl;

    std::ofstream sslibSVM;
    sslibSVM.open (name+"_"+file->tag+".libSVM.dat");
    TFile * outfile = TFile::Open((name+"libSVM_test.root").c_str(), "recreate");
    TTreeFormula* weight = new TTreeFormula("sig_w",(file->weight_branch+"*"+file->run_weight_string).c_str(),file->tvertex);

    std::vector<TTreeFormula*> tree_formulas_v;
    std::vector<int> id_v;
    for(bdt_variable &var : variables) {
        tree_formulas_v.push_back(new TTreeFormula(var.safe_name.c_str(), var.name.c_str(),file->tvertex));
        id_v.push_back(var.id);
    }

    size_t next_entry = file->precut_list->GetEntry(0);
    size_t num = 0;
    while(num<file->precut_list->GetN()){

        file->tvertex->GetEntry(next_entry);

        weight->GetNdata();
        double wei = weight->EvalInstance();

        if(wei<0 || wei!=wei || isinf(wei) ){
            std::cout<<"WARNING WARNING, the weight here is "<<wei<<std::endl;
            std::cout<<"Setting to 1 for now, investigate!"<<std::endl;
            wei = 1.0;
        }

        sslibSVM<<"0:"<<wei<<" ";

        for(int t=0; t< tree_formulas_v.size();++t){
            tree_formulas_v[t]->GetNdata();
            double val = tree_formulas_v[t]->EvalInstance();

            if(!(val!=val || val == -999 || val == -9999 || val == -99999 || val == -99 || val == -9)){
                int id = id_v[t];
                sslibSVM<<id<<":"<<val<<" ";
            }else{
                int id = id_v[t];
                sslibSVM<<id<<":"<<-999<<" ";

            }
        }

        sslibSVM<<std::endl;

        next_entry = file->precut_list->Next();
        num++;
    } 


    sslibSVM.close();
    outfile->Close();
    return 0;
}

int convertToLibSVMTT(bdt_info &info, bdt_file *signal_file_train, bdt_file *signal_file_test, std::string signal_test_cut, bdt_file *background_file_train, bdt_file *background_file_test, std::string background_test_cut){
    //This is the new-new one that splits based individual test/training and uses a more standard precut evaluation. 

    std::vector<bdt_variable> variables = info.train_vars;
    std::string const name = info.identifier;
    std::cout<<"Beginninng to convert training/testing files into a libSVM format for XGBoost on BDT: "<<name<<std::endl;

    std::ofstream sslibSVMtrain,sslibSVMtest;
    sslibSVMtest.open (name+".libSVM.test.dat");
    sslibSVMtrain.open (name+".libSVM.train.dat");

    TFile * outfile = TFile::Open((name+"libSVM_test.root").c_str(), "recreate");

    int bdt_precut_stage = 1;
    //train samples
    TCut sig_tcut_train =  TCut(signal_file_train->getStageCuts(bdt_precut_stage,-9,-9).c_str());
    TCut back_tcut_train = TCut(background_file_train->getStageCuts(bdt_precut_stage,-9,-9).c_str());

    //and for test
    TCut sig_tcut_test =  TCut((signal_file_test->getStageCuts(bdt_precut_stage,-9,-9)).c_str());
    TCut back_tcut_test = TCut((background_file_test->getStageCuts(bdt_precut_stage,-9,-9)).c_str());

    int signal_entries_train = signal_file_train->tvertex->GetEntries(sig_tcut_train);
    int background_entries_train = background_file_train->tvertex->GetEntries(back_tcut_train);

    int signal_entries_test = signal_file_test->tvertex->GetEntries(sig_tcut_test);
    int background_entries_test = background_file_test->tvertex->GetEntries(back_tcut_test);

    std::cout<<"Train signal_entries: "<<signal_entries_train<<" background_entries_train: "<<background_entries_train<<std::endl;
    std::cout<<"Test signal_entries: "<<signal_entries_test<<" background_entries_test: "<<background_entries_test<<std::endl;

    TTreeFormula* sig_weight_train = new TTreeFormula("sig_w",(signal_file_train->weight_branch+"*"+signal_file_train->run_weight_string).c_str(),signal_file_train->tvertex);
    TTreeFormula* bkg_weight_train = new TTreeFormula("bkg_w",(background_file_train->weight_branch+"*"+background_file_train->run_weight_string).c_str(),background_file_train->tvertex);
    TTreeFormula* sig_weight_test = new TTreeFormula("sig_w",(signal_file_test->weight_branch+"*"+signal_file_test->run_weight_string).c_str(),signal_file_test->tvertex);
    TTreeFormula* bkg_weight_test = new TTreeFormula("bkg_w",(background_file_test->weight_branch+"*"+background_file_test->run_weight_string).c_str(),background_file_test->tvertex);

    std::vector<TTreeFormula*> sig_tree_formulas_v_train;
    std::vector<TTreeFormula*> bkg_tree_formulas_v_train;
    std::vector<TTreeFormula*> sig_tree_formulas_v_test;
    std::vector<TTreeFormula*> bkg_tree_formulas_v_test;

    std::vector<int> id_v;
    for(bdt_variable &var : variables){
        std::cout<<"TTreeFormula: "<<var.id<<std::endl;
        sig_tree_formulas_v_train.push_back(new TTreeFormula(var.safe_name.c_str(), var.name.c_str(),signal_file_train->tvertex));
        bkg_tree_formulas_v_train.push_back(new TTreeFormula(var.safe_name.c_str(), var.name.c_str(),background_file_train->tvertex));
        sig_tree_formulas_v_test.push_back(new TTreeFormula(var.safe_name.c_str(), var.name.c_str(),signal_file_test->tvertex));
        bkg_tree_formulas_v_test.push_back(new TTreeFormula(var.safe_name.c_str(), var.name.c_str(),background_file_test->tvertex));
        id_v.push_back(var.id);
    }

    //Signal training events
    size_t next_entry_sig_train = signal_file_train->precut_list->GetEntry(0);
    size_t num_sig_train = 0;
    while(num_sig_train < signal_file_train->precut_list->GetN()){

        signal_file_train->tvertex->GetEntry(next_entry_sig_train);
        sig_weight_train->GetNdata();
        double wei = sig_weight_train->EvalInstance();

        if(wei<0 || wei!=wei || isinf(wei) ){
            std::cout<<"WARNING WARNING, the weight here is "<<wei<<std::endl;
            std::cout<<"Setting to 1 for now, investigate!"<<std::endl;
            wei = 1.0;
        }

        sslibSVMtrain<<"1:"<<wei<<" "; //guanqun: write the weight into file 'sslibSVMtrain'

        for(int t=0; t< sig_tree_formulas_v_train.size();++t){
            sig_tree_formulas_v_train[t]->GetNdata();
            double val = sig_tree_formulas_v_train[t]->EvalInstance();
            int id = id_v[t];

            //if nan, lets do something
            if(!(val!=val || val == -999 || val == -9999 || val == -99999 || val == -99 || val == -9)){
                sslibSVMtrain<<id<<":"<<val<<" ";
            }else{
                sslibSVMtrain<<id<<":"<<-999<<" ";
            
            }
        }
        sslibSVMtrain<<std::endl;  
        next_entry_sig_train = signal_file_train->precut_list->Next();
        num_sig_train++;
    } 
    std::cout<<"We wrote "<<num_sig_train<<" Signal Training Events"<<std::endl;

    
    //Signal Testing events
    size_t next_entry_sig_test = signal_file_test->precut_list->GetEntry(0);
    size_t num_sig_test = 0;
    while(num_sig_test < signal_file_test->precut_list->GetN()){

        signal_file_test->tvertex->GetEntry(next_entry_sig_test);
        sig_weight_test->GetNdata();
        double wei = sig_weight_test->EvalInstance();

        if(wei<0 || wei!=wei || isinf(wei) ){
             std::cout<<"WARNING WARNING, the weight here is "<<wei<<std::endl;
            std::cout<<"Setting to 1 for now, investigate!"<<std::endl;

            wei = 1.0;
        }

        sslibSVMtest<<"1:"<<wei<<" ";

        for(int t=0; t< sig_tree_formulas_v_test.size();++t){
            sig_tree_formulas_v_test[t]->GetNdata();
            double val = sig_tree_formulas_v_test[t]->EvalInstance();
            int id = id_v[t];

            //if nan, lets do something
            if(!(val!=val || val == -999 || val == -9999 || val == -99999 || val == -99 || val == -9)){
                sslibSVMtest<<id<<":"<<val<<" ";
            }else{
                sslibSVMtest<<id<<":"<<-999<<" ";
            }
        }
        sslibSVMtest<<std::endl;  
        next_entry_sig_test = signal_file_test->precut_list->Next();
        num_sig_test++;
    } 
    std::cout<<"We wrote "<<num_sig_test<<" Signal Testing Events"<<std::endl;

    //Background Train
    size_t next_entry_bkg_train = background_file_train->precut_list->GetEntry(0);
    size_t num_bkg_train = 0;
    std::cout<<"Starting on Bkg Train with "<<background_file_train->precut_list->GetN()<<" in precut list "<<std::endl;
    while(num_bkg_train < background_file_train->precut_list->GetN()){

        background_file_train->tvertex->GetEntry(next_entry_bkg_train);
        bkg_weight_train->GetNdata();
        double wei = bkg_weight_train->EvalInstance();

        if(wei<0 || wei!=wei || isinf(wei) ){
             std::cout<<"WARNING WARNING, the weight here is "<<wei<<std::endl;
            std::cout<<"Setting to 1 for now, investigate!"<<std::endl;

            wei = 1.0;
        }

        sslibSVMtrain<<"0:"<<wei<<" ";

        for(int t=0; t< bkg_tree_formulas_v_train.size();++t){
            bkg_tree_formulas_v_train[t]->GetNdata();
            double val = bkg_tree_formulas_v_train[t]->EvalInstance();
            int id = id_v[t];

            //if nan, lets do something
            if(!(val!=val || val == -999 || val == -9999 || val == -99999 || val == -99 || val == -9)){
                sslibSVMtrain<<id<<":"<<val<<" ";
            }else{
                sslibSVMtrain<<id<<":"<<-999<<" ";

            }
        }
        sslibSVMtrain<<std::endl;  
        next_entry_bkg_train = background_file_train->precut_list->Next();
        num_bkg_train++;
    } 
    std::cout<<"We wrote "<<num_bkg_train<<" Background Training Events"<<std::endl;

    
    //Background test
    size_t next_entry_bkg_test = background_file_test->precut_list->GetEntry(0);
    size_t num_bkg_test = 0;
    while(num_bkg_test < background_file_test->precut_list->GetN()){

        background_file_test->tvertex->GetEntry(next_entry_bkg_test);
        bkg_weight_test->GetNdata();
        double wei = bkg_weight_test->EvalInstance();

        if(wei<0 || wei!=wei || isinf(wei) ){
                std::cout<<"WARNING WARNING, the weight here is "<<wei<<std::endl;
            std::cout<<"Setting to 1 for now, investigate!"<<std::endl;

            wei = 1.0;
        }

        sslibSVMtest<<"0:"<<wei<<" ";

        for(int t=0; t< bkg_tree_formulas_v_test.size();++t){
            bkg_tree_formulas_v_test[t]->GetNdata();
            double val = bkg_tree_formulas_v_test[t]->EvalInstance();
            int id = id_v[t];

            //if nan, lets do something
            if(!(val!=val || val == -999 || val == -9999 || val == -99999 || val == -99 || val == -9)){
                sslibSVMtest<<id<<":"<<val<<" ";
            }else{
                sslibSVMtest<<id<<":"<<-999<<" ";
            }
        }
        sslibSVMtest<<std::endl;  
        next_entry_bkg_test = background_file_test->precut_list->Next();
        num_bkg_test++;
    } 
    std::cout<<"We wrote "<<num_bkg_test<<" Background Testing Events"<<std::endl;

    sslibSVMtest.close();
    sslibSVMtrain.close();
    outfile->Close();
    return 0;
}

int convertToLibSVM(bdt_info &info, bdt_file *signal_file_train, bdt_file *signal_file_test, std::string signal_test_cut, bdt_file *background_file_train, bdt_file *background_file_test, std::string background_test_cut){
    //This is the new one that splits based individual test/training -- WARNING DEPRECIATED
   std::cout<<"WARNING WARNING DEPRCIATED! use convertToLibSVMTT instead"<<std::endl;

    std::vector<bdt_variable> variables = info.train_vars;
    std::string const name = info.identifier;
    std::cout<<"Beginninng to convert training/testing files into a libSVM format for XGBoost on BDT: "<<name<<std::endl;

    std::ofstream sslibSVMtrain,sslibSVMtest;
    sslibSVMtest.open (name+".libSVM.test.dat");
    sslibSVMtrain.open (name+".libSVM.train.dat");

    TFile * outfile = TFile::Open((name+"libSVM_test.root").c_str(), "recreate");

    int bdt_precut_stage = 1;
    //train samples
    TCut sig_tcut_train =  TCut(signal_file_train->getStageCuts(bdt_precut_stage,-9,-9).c_str());
    TCut back_tcut_train = TCut(background_file_train->getStageCuts(bdt_precut_stage,-9,-9).c_str());

    //and for test
    TCut sig_tcut_test =  TCut((signal_file_test->getStageCuts(bdt_precut_stage,-9,-9)+"&&"+signal_test_cut).c_str());
    TCut back_tcut_test = TCut((background_file_test->getStageCuts(bdt_precut_stage,-9,-9)+"&&"+background_test_cut).c_str());


    std::cout<<"Prefiltering out all events that fail precuts + topocuts"<<std::endl;
    TTree * background_ttree_prefiltered_train = (TTree*)background_file_train->tvertex->CopyTree(back_tcut_train);
    TTree * signal_ttree_prefiltered_train = (TTree*)signal_file_train->tvertex->CopyTree(sig_tcut_train);

    TTree * background_ttree_prefiltered_test = (TTree*)background_file_test->tvertex->CopyTree(back_tcut_test);
    TTree * signal_ttree_prefiltered_test = (TTree*)signal_file_test->tvertex->CopyTree(sig_tcut_test);


    int signal_entries_train = signal_file_train->tvertex->GetEntries(sig_tcut_train);
    int background_entries_train = background_file_train->tvertex->GetEntries(back_tcut_train);

    int signal_entries_test = signal_file_test->tvertex->GetEntries(sig_tcut_test);
    int background_entries_test = background_file_test->tvertex->GetEntries(back_tcut_test);


    std::cout<<"Train signal_entries: "<<signal_entries_train<<" background_entries_train: "<<background_entries_train<<std::endl;
    std::cout<<"TRAIN PREFILTERED signal_entries: "<<signal_ttree_prefiltered_train->GetEntries()<<" background_entries_train: "<<background_ttree_prefiltered_train->GetEntries()<<std::endl;
    std::cout<<"Test signal_entries: "<<signal_entries_test<<" background_entries_test: "<<background_entries_test<<std::endl;
    std::cout<<"TEST PREFILTERED signal_entries: "<<signal_ttree_prefiltered_test->GetEntries()<<" background_entries_test: "<<background_ttree_prefiltered_test->GetEntries()<<std::endl;


    TTreeFormula* sig_weight_train = new TTreeFormula("sig_w",(signal_file_train->weight_branch+"*"+signal_file_train->run_weight_string).c_str(),signal_ttree_prefiltered_train);
    TTreeFormula* bkg_weight_train = new TTreeFormula("bkg_w",(background_file_train->weight_branch+"*"+background_file_train->run_weight_string).c_str(),background_ttree_prefiltered_train);
    TTreeFormula* sig_weight_test = new TTreeFormula("sig_w",(signal_file_test->weight_branch+"*"+signal_file_test->run_weight_string).c_str(),signal_ttree_prefiltered_test);
    TTreeFormula* bkg_weight_test = new TTreeFormula("bkg_w",(background_file_test->weight_branch+"*"+background_file_test->run_weight_string).c_str(),background_ttree_prefiltered_test);


    std::vector<TTreeFormula*> sig_tree_formulas_v_train;
    std::vector<TTreeFormula*> bkg_tree_formulas_v_train;
    std::vector<TTreeFormula*> sig_tree_formulas_v_test;
    std::vector<TTreeFormula*> bkg_tree_formulas_v_test;

    std::vector<int> id_v;
    for(bdt_variable &var : variables){
        sig_tree_formulas_v_train.push_back(new TTreeFormula(var.safe_name.c_str(), var.name.c_str(),signal_ttree_prefiltered_train));
        bkg_tree_formulas_v_train.push_back(new TTreeFormula(var.safe_name.c_str(), var.name.c_str(),background_ttree_prefiltered_train));
        sig_tree_formulas_v_test.push_back(new TTreeFormula(var.safe_name.c_str(), var.name.c_str(),signal_ttree_prefiltered_test));
        bkg_tree_formulas_v_test.push_back(new TTreeFormula(var.safe_name.c_str(), var.name.c_str(),background_ttree_prefiltered_test));
        id_v.push_back(var.id);
    }

    for(int i = 0; i < signal_entries_train; ++i) {
        signal_ttree_prefiltered_train->GetEntry(i);

        sig_weight_train->GetNdata();
        double wei = sig_weight_train->EvalInstance();
        sslibSVMtrain<<"1:"<<wei<<" ";

        for(int t=0; t< sig_tree_formulas_v_train.size();++t){
            sig_tree_formulas_v_train[t]->GetNdata();
            double val = sig_tree_formulas_v_train[t]->EvalInstance();
            int id = id_v[t];

            //if nan, lets do something
            if(!(val!=val || val == -999 || val == -9999 || val == -99999 || val == -99 || val == -9)){

                sslibSVMtrain<<id<<":"<<val<<" ";
            }else{

               sslibSVMtrain<<id<<":"<<-999<<" ";
            }
        }
        sslibSVMtrain<<std::endl;
    } 

    for(int i = 0; i < signal_entries_test; ++i) {
        signal_ttree_prefiltered_test->GetEntry(i);

        sig_weight_test->GetNdata();
        double wei = sig_weight_test->EvalInstance();
        sslibSVMtest<<"1:"<<wei<<" ";

        for(int t=0; t< sig_tree_formulas_v_test.size();++t){
            sig_tree_formulas_v_test[t]->GetNdata();
            double val = sig_tree_formulas_v_test[t]->EvalInstance();
            int id = id_v[t];

            //if nan, lets do something
            if(!(val!=val || val == -999 || val == -9999 || val == -99999 || val == -99|| val ==-9)){

                sslibSVMtest<<id<<":"<<val<<" ";
            }else{
                sslibSVMtest<<id<<":"<<-999<<" ";

            }
        }
        sslibSVMtest<<std::endl;
    } 


    for(int i = 0; i < background_entries_train; ++i) {
        background_ttree_prefiltered_train->GetEntry(i);

        bkg_weight_train->GetNdata();
        double wei = bkg_weight_train->EvalInstance();
        sslibSVMtrain<<"0:"<<wei<<" ";

        for(int t=0; t< bkg_tree_formulas_v_train.size();++t){
            bkg_tree_formulas_v_train[t]->GetNdata();
            double val = bkg_tree_formulas_v_train[t]->EvalInstance();
            int id = id_v[t];

            //if nan, lets do something
            if(!(val!=val || val == -999 || val == -9999 || val == -99999 || val == -99 || val == -9)){

                sslibSVMtrain<<id<<":"<<val<<" ";
            }else{
                sslibSVMtrain<<id<<":"<<-999<<" ";

            }
        }
        sslibSVMtrain<<std::endl;
    } 

    for(int i = 0; i < background_entries_test; ++i) {
        background_ttree_prefiltered_test->GetEntry(i);

        bkg_weight_test->GetNdata();
        double wei = bkg_weight_test->EvalInstance();
        sslibSVMtest<<"0:"<<wei<<" ";

        for(int t=0; t< bkg_tree_formulas_v_test.size();++t){
            bkg_tree_formulas_v_test[t]->GetNdata();
            double val = bkg_tree_formulas_v_test[t]->EvalInstance();
            int id = id_v[t];

            //if nan, lets do something
            if(!(val!=val || val == -999 || val == -9999 || val == -99999 || val == -99 || val == -9)){
                sslibSVMtest<<id<<":"<<val<<" ";
            }else{
                sslibSVMtest<<id<<":"<<-999<<" ";
            }
        }
        sslibSVMtest<<std::endl;
    } 


    sslibSVMtest.close();
    sslibSVMtrain.close();
    outfile->Close();
    return 0;
}


int convertToLibSVM(bdt_info &info, bdt_file *signal_file, bdt_file *background_file){
    //This is the older one that just splits based on training fraction

    std::vector<bdt_variable> variables = info.train_vars;
    std::string const name = info.identifier;
    std::cout<<"Beginninng to convert training/testing files into a libSVM format for XGBoost"<<std::endl;

    std::ofstream sslibSVMtrain,sslibSVMtest;
    sslibSVMtest.open (name+".libSVM.test.dat");
    sslibSVMtrain.open (name+".libSVM.train.dat");

    TFile * outfile = TFile::Open((name+"libSVM_test.root").c_str(), "recreate");

    int bdt_precut_stage = 1;
    TCut sig_tcut =  TCut(signal_file->getStageCuts(bdt_precut_stage,-9,-9).c_str());
    TCut back_tcut = TCut(background_file->getStageCuts(bdt_precut_stage,-9,-9).c_str());

    std::cout<<"Prefiltering out all events that fail precuts + topocuts"<<std::endl;
    TTree * background_ttree_prefiltered = (TTree*)background_file->tvertex->CopyTree(back_tcut);
    TTree * signal_ttree_prefiltered = (TTree*)signal_file->tvertex->CopyTree(sig_tcut);

    int signal_entries = signal_file->tvertex->GetEntries(sig_tcut);
    int background_entries = background_file->tvertex->GetEntries(back_tcut);

    std::cout<<"signal_entries: "<<signal_entries<<" background_entries: "<<background_entries<<std::endl;
    std::cout<<"PREFILTERED signal_entries: "<<signal_ttree_prefiltered->GetEntries()<<" background_entries: "<<background_ttree_prefiltered->GetEntries()<<std::endl;

    TTreeFormula* sig_weight = new TTreeFormula("sig_w",(signal_file->weight_branch+"*"+signal_file->run_weight_string).c_str(),signal_ttree_prefiltered);
    TTreeFormula* bkg_weight = new TTreeFormula("bkg_w",(background_file->weight_branch+"*"+background_file->run_weight_string).c_str(),background_ttree_prefiltered);

    std::vector<TTreeFormula*> sig_tree_formulas_v;
    std::vector<TTreeFormula*> bkg_tree_formulas_v;
    std::vector<int> id_v;
    for(bdt_variable &var : variables) {
        sig_tree_formulas_v.push_back(new TTreeFormula(var.safe_name.c_str(), var.name.c_str(),signal_ttree_prefiltered));
        bkg_tree_formulas_v.push_back(new TTreeFormula(var.safe_name.c_str(), var.name.c_str(),background_ttree_prefiltered));
        id_v.push_back(var.id);
    }

    int sig_train_num = info.TMVAmethod.training_fraction*signal_entries;
    int bkg_train_num = info.TMVAmethod.training_fraction*background_entries;

    for(int i = 0; i < signal_entries; ++i) {
        signal_ttree_prefiltered->GetEntry(i);

        sig_weight->GetNdata();
        double wei =sig_weight->EvalInstance();

        if(i < sig_train_num){
            sslibSVMtrain<<"1:"<<wei<<" ";
        }else{
            sslibSVMtest<<"1:"<<wei<<" ";
        }

        for(int t=0; t< sig_tree_formulas_v.size();++t){
            sig_tree_formulas_v[t]->GetNdata();
            double val = sig_tree_formulas_v[t]->EvalInstance();
            int id = id_v[t];

            //if nan, lets do something
            if(!(val!=val || val == -999 || val == -9999 || val == -99999 || val == -99 || val == -9)){
                if(i < sig_train_num){
                    sslibSVMtrain<<id<<":"<<val<<" ";
                }else{
                    sslibSVMtest<<id<<":"<<val<<" ";
                }
            }else{
                 if(i < sig_train_num){
                    sslibSVMtrain<<id<<":"<<-999<<" ";
                }else{
                    sslibSVMtest<<id<<":"<<-999<<" ";
                }
            }

        }

        if(i < sig_train_num){
            sslibSVMtrain<<std::endl;
        }else{
            sslibSVMtest<<std::endl;
        }

    } 


    for(int i = 0; i < background_entries; ++i) {
        background_ttree_prefiltered->GetEntry(i);

        bkg_weight->GetNdata();
        double wei = bkg_weight->EvalInstance();

        if(i < bkg_train_num){
            sslibSVMtrain<<"0:"<<wei<<" ";
        }else{
            sslibSVMtest<<"0:"<<wei<<" ";
        }

        for(int t=0; t< bkg_tree_formulas_v.size();++t){
            bkg_tree_formulas_v[t]->GetNdata();
            double val = bkg_tree_formulas_v[t]->EvalInstance();
            int id = id_v[t];

            //if its a missing value, lets ignore it, lets do something
            if(!(val!=val || val == -999 || val == -9999 || val == -99999 || val == -99 || val == -9)){

                if(i < bkg_train_num){
                    sslibSVMtrain<<id<<":"<<val<<" ";
                }else{
                    sslibSVMtest<<id<<":"<<val<<" ";
                }
            }else{

                if(i < bkg_train_num){
                    sslibSVMtrain<<id<<":"<<-999<<" ";
                }else{
                    sslibSVMtest<<id<<":"<<-999<<" ";
                }

            }

        }

        if(i < bkg_train_num){
            sslibSVMtrain<<std::endl;
        }else{
            sslibSVMtest<<std::endl;
        }

    }


    sslibSVMtest.close();
    sslibSVMtrain.close();
    outfile->Close();
    return 0;
}



int bdt_XGtrain(bdt_info &info){

    std::string const name = info.identifier;

    DMatrixHandle dtrain, dtest;
    int silent = 0;
    int use_gpu = 0;  // set to 1 to use the GPU for training

    safe_xgboost(XGDMatrixCreateFromFile((info.identifier+".libSVM.train.dat").c_str(), silent, &dtrain));
    safe_xgboost(XGDMatrixCreateFromFile((info.identifier+".libSVM.test.dat").c_str(), silent, &dtest));

    // create the booster
    BoosterHandle booster;
    DMatrixHandle eval_dmats[2] = {dtrain, dtest};
    safe_xgboost(XGBoosterCreate(eval_dmats, 2, &booster));



    TFile *f = new TFile(("XGBoost_train_output_"+name+".root").c_str(),"recreate");
    f->cd();
    TH1* btest = new TH1D("btest","btest",100,0,1);
    TH1* btrain = new TH1D("btrain","btrain",100,0,1);
    TH1* stest = new TH1D("stest","stest",100,0,1);
    TH1* strain = new TH1D("strain","strain",100,0,1);

    TTree * t_strain = new TTree("t_strain","t_strain");
    double v_strain = 0;
    t_strain->Branch("sig_train",&v_strain);

    TTree * t_btrain = new TTree("t_btrain","t_btrain");
    double v_btrain = 0;
    t_btrain->Branch("bkg_train",&v_btrain);

    TTree * t_stest = new TTree("t_stest","t_stest");
    double v_stest = 0;
    t_stest->Branch("sig_test",&v_stest);

    TTree * t_btest = new TTree("t_btest","t_btest");
    double v_btest = 0;
    t_btest->Branch("bkg_test",&v_btest);

        // configure the training
    // available parameters are described here:
    // https://xgboost.readthedocs.io/en/latest/parameter.html


    int n_trees = 250;

    std::cout<<"XGBoost being setup with "<<std::endl;
    for(auto &pairs: info.TMVAmethod.xg_config){
        if(pairs.first == "n_trees"){
            n_trees = (int)std::stod(pairs.second);
        }else if (pairs.first == "eval_metric"){
            std::cout<<"Nah mate, we dont need to pass "<<pairs.first<<" anymore, automatic. No harm done tho"<<std::endl;
        }else{
            safe_xgboost(XGBoosterSetParam(booster,pairs.first.c_str(),pairs.second.c_str()));
            std::cout<<"--"<<pairs.first<<" with a value of "<<pairs.second<<std::endl;
        }
    }

        std::vector<std::string> s_english = {"Negative Log-Liklihood","1 - Area Under the Curve (AUC)","Error (\% of wrong cases)","Root Mean Square Error"};
        std::vector<std::string> s_metrics  = {"logloss","auc","error","rmsle"};
        std::vector<std::string> s_types = {"train","test"};
        for(auto&m:s_metrics){
            safe_xgboost(XGBoosterSetParam(booster, "eval_metric",m.c_str()));
        }

        /*
           safe_xgboost(XGBoosterSetParam(booster, "tree_method", "exact"));
           safe_xgboost(XGBoosterSetParam(booster, "n_gpus", "0"));
           safe_xgboost(XGBoosterSetParam(booster, "objective", "binary:logistic")); //
           safe_xgboost(XGBoosterSetParam(booster, "eval_metric", "logloss")); //auc , error, rmsle , ams@0.15i
        //safe_xgboost(XGBoosterSetParam(booster, "eval_metric", "auc")); //auc , error, rmsle , ams@0.15i
        safe_xgboost(XGBoosterSetParam(booster, "min_child_weight", "1"));
        safe_xgboost(XGBoosterSetParam(booster, "gamma", "1.0")); //regular
        safe_xgboost(XGBoosterSetParam(booster, "max_depth", "5"));
        safe_xgboost(XGBoosterSetParam(booster, "verbosity", "1"));
        safe_xgboost(XGBoosterSetParam(booster, "eta", "0.02"));
        safe_xgboost(XGBoosterSetParam(booster, "subsample", "0.9"));
        */

        std::vector<double> iteration;

        std::vector<std::vector<double>> train_metric_res(s_metrics.size(),iteration);
        std::vector<std::vector<double>> test_metric_res(s_metrics.size(),iteration);

        std::vector<std::pair<double,int>> test_min_vals(s_metrics.size(),std::make_pair(99999,-1));

        const char* eval_names[2] = {"train", "test"};
        const char* eval_result = NULL;
        int cotr = 0;
        for (int i = 0; i < n_trees; ++i){
            safe_xgboost(XGBoosterUpdateOneIter(booster, i, dtrain));
            safe_xgboost(XGBoosterEvalOneIter(booster, i, eval_dmats, eval_names, 2, &eval_result));
            std::string res = eval_result;

            std::cout<<"RES "<<res<<std::endl; 


            for(int t =0; t<s_types.size();++t){
                for(int m = 0; m<s_metrics.size();m++){
                    std::string nam = s_types[t]+"-"+s_metrics[m]; 
                    int loc = res.find(nam,0);
                    std::string val = res.substr(loc+nam.size()+1,8); 
                    double d_val = std::stod(val);
                    if(m==1)d_val = 1.0-d_val;
                    //std::cout<<"Wripped "<<nam<<" "<<val<<std::endl;           
                    if(s_types[t]=="train") train_metric_res[m].push_back(d_val);
                    if(s_types[t]=="test"){
                        test_metric_res[m].push_back(d_val);
                        if(d_val < test_min_vals[m].first){
                            test_min_vals[m].first = d_val;
                            test_min_vals[m].second = i;
                        }
                    }
                }
            }

            iteration.push_back(i);

            printf("%s\n", eval_result);

            //not a stopping conditin
            if(i>2){
                if(test_metric_res[0][i-1] > test_metric_res[0][i-2] ){
                    std::cout<<"Yes"<<" "<<cotr<<std::endl;
                    cotr++;
                }else{
                    cotr=0;
                }
            }
            //if(cotr>10) break;
        }


        // predict
        bst_ulong out_len = 0;
        const float* out_result = NULL;

        //safe_xgboost(XGBoosterLoadModel(booster,"test.mod"));


        /*std::cout<<"TEST"<<std::endl;
          const float* tester = NULL;
          safe_xgboost(XGDMatrixGetFloatInfo(dtest, "logloss", &out_len, &tester));
          for (int i = 0; i < out_len; ++i) {
          std::cout<<tester[i]<<std::endl;
          }
          std::cout<<"TEST"<<std::endl;
          */

        const float* test_score = NULL;
        const float* test_label = NULL;
        safe_xgboost(XGDMatrixGetFloatInfo(dtest, "label", &out_len, &test_label));
        safe_xgboost(XGBoosterPredict(booster, dtest, 0, 0, &out_len, &test_score));
        for (int i = 0; i < out_len; ++i) {
            //i    printf("%1.4f ", out_result[i]);
            if(test_label[i]){
                stest->Fill(test_score[i]);  
                v_stest =  test_score[i];
                t_stest->Fill();

            }else{
                btest->Fill(test_score[i]);  
                v_btest =  test_score[i];
                t_btest->Fill();

            }
        }

        const float* train_score = NULL;
        const float* train_label = NULL;
        safe_xgboost(XGDMatrixGetFloatInfo(dtrain, "label", &out_len, &train_label));
        safe_xgboost(XGBoosterPredict(booster, dtrain, 0, 0, &out_len, &train_score));
        for (int i = 0; i < out_len; ++i) {
            if(train_label[i]){
                strain->Fill(train_score[i]);  
                v_strain =  train_score[i];
                t_strain->Fill();
            }else{
                btrain->Fill(train_score[i]);  
                v_btrain =  train_score[i];
                t_btrain->Fill();
            }
        }


        safe_xgboost(XGBoosterSaveModel(booster,(name+".XGBoost.mod").c_str() ));
        safe_xgboost(XGDMatrixFree(dtrain));
        safe_xgboost(XGDMatrixFree(dtest));





        //Some print out of metrics
        for(int m = 0; m<s_metrics.size();m++){
                    std::string nam = s_types[1]+" "+s_metrics[m]; 
                    std::cout<<"FIN: "<<nam<<" minimum at iteration "<<test_min_vals[m].second<<" with val "<<test_min_vals[m].first<<std::endl;
        }


        f->cd();

        TCanvas *c_error = new TCanvas("c_error","",1800,1800);
        TFile *hfile = new TFile(("XGBoost_Validation_"+name+".root").c_str(),"RECREATE");
        c_error->Divide(2,2);
        c_error->cd(1);

        TLegend *lgr = new TLegend(0.59,0.89,0.59,0.89);
        for(int i=0; i< s_metrics.size(); i++){
            TPad *p = (TPad*)c_error->cd(i+1);
            if(false&&i==1){
                p->SetLogy();
                p->SetLogx();
            }
        
            
            TGraph *g_test= new TGraph(iteration.size(),&iteration[0],&(test_metric_res[i])[0]);
            TGraph *g_train = new TGraph(iteration.size(),&iteration[0],&(train_metric_res[i])[0]);
         /*   TGraph *g_test2;
               if (split_test){
                g_test2 = new TGraph(iteration.size(),&iteration[0],&(train_metric_res[i])[0]);
         
            }*/

            double mmax1 = std::max(test_metric_res[i].front(),test_metric_res[i].back());
            double mmax2 = std::max(train_metric_res[i].front(),train_metric_res[i].back());
            double mmax = std::max(mmax1,mmax2)*1.2;
		if(i==0)mmax= std::max(mmax,1.0);

            g_test->Draw("AL");
 
	   g_test->SetMinimum(0);
            g_test->SetMaximum(mmax);
            g_test->GetHistogram()->SetMaximum(mmax);
            g_test->GetHistogram()->SetMinimum(0.0);
           
            g_test->Draw("AL");


	   g_test->SetTitle(s_english[i].c_str());

            g_train->Draw("same AL");
            g_train->SetTitle(s_english[i].c_str());
            g_train->SetLineColor(kRed);
            g_train->SetLineWidth(2);
            g_test->Draw("same CL");
            g_test->SetLineColor(kBlue);
            g_test->SetLineWidth(2);

            /*
            g_test2->Draw("same CL");
            g_test2->SetLineColor(kBlue);
            g_test2->SetLineWidth(2);
*/
            g_test->SetMinimum(0);
            g_test->SetMaximum(mmax);
            g_train->SetMaximum(mmax);
            g_train->SetMinimum(0.0);
            g_test->GetHistogram()->SetMaximum(mmax);
            g_test->GetHistogram()->SetMinimum(0.0);


            TGraph *g_min = new TGraph(1);
            g_min->SetPoint(0,test_min_vals[i].second,test_min_vals[i].first);
            g_min->SetLineColor(kCyan);
            g_min->SetMarkerStyle(29);
            g_min->SetMarkerSize(3);
            g_min->Draw("p");

            if(i==0){
                lgr->AddEntry(g_train,"Train","f"); 
                lgr->AddEntry(g_test,"Test","f"); 
                lgr->SetLineWidth(0);
                lgr->SetLineColor(kWhite);
                lgr->SetFillStyle(0);
                lgr->Draw();
                g_test->Write("g_test");
                g_train->Write("g_train");
                g_min->Write("g_min");
            }
            

            c_error->Update();
        }
     //   c_error->SaveAs(("XGBoost_Validation_"+name+".root").c_str(),"root");
        c_error->SaveAs(("XGBoost_Validation_"+name+".pdf").c_str(),"pdf");
        c_error->Write(); 
        hfile ->Close();

        f->cd();

        std::vector<double> pos;
        std::vector<double> st,st2;
        std::vector<double> bt,bt2;
        for(int ip =0; ip<=1000;ip++){
	        double p = (double)ip*0.001;
            pos.push_back(p);
            st.push_back((double)t_strain->GetEntries(("sig_train > "+std::to_string(p)).c_str())/(double)t_strain->GetEntries());
            bt.push_back(1.0-(double)t_btrain->GetEntries(("bkg_train > "+std::to_string(p)).c_str())/(double)t_btrain->GetEntries());

            st2.push_back((double)t_stest->GetEntries(("sig_test > "+std::to_string(p)).c_str())/(double)t_stest->GetEntries());
            bt2.push_back(1.0-(double)t_btest->GetEntries(("bkg_test > "+std::to_string(p)).c_str())/(double)t_btest->GetEntries());
        }

        TCanvas *c_eff = new TCanvas();
        c_eff->cd();
        TGraph *gc = new TGraph(pos.size(),&bt[0],&st[0]);
        TGraph *gc2 = new TGraph(pos.size(),&bt2[0],&st2[0]);
        TGraph *gst = new TGraph(pos.size(),&pos[0],&st2[0]);
        TGraph *gbt = new TGraph(pos.size(),&pos[0],&bt2[0]);
        gst->Draw("AL");
        gst->SetMaximum(1);
        gst->SetMinimum(0);
        gst->SetLineColor(kRed-7);
        gst->SetLineWidth(3);
        gbt->Draw("same CL");
        gbt->SetLineColor(kBlue-7);
        gbt->SetLineWidth(3);
        TLegend *leff = new TLegend(0.1,0.3,0.3,0.6);
        leff->AddEntry(gst,"Signal (Test) Acceptance Eff","l");
        leff->AddEntry(gbt,"BKG (Test) Rejection Eff","l");
        leff->Draw();
        gst->GetXaxis()->SetTitle("BDT score cut position");
        gst->GetYaxis()->SetTitle("Efficiency");


        c_eff->Write("Test Efficiency"); 


        TCanvas *c_eff3 = new TCanvas();//same for training
        c_eff3->cd();
        TGraph *gst3 = new TGraph(pos.size(),&pos[0],&st[0]);
        TGraph *gbt3 = new TGraph(pos.size(),&pos[0],&bt[0]);

        gst3->Draw("AL");
        gst3->SetMaximum(1);
        gst3->SetMinimum(0);
        gst3->SetLineColor(kRed-7);
        gst3->SetLineWidth(3);
        gbt3->Draw("same CL");
        gbt3->SetLineColor(kBlue-7);
        gbt3->SetLineWidth(3);
        TLegend *leff3 = new TLegend(0.1,0.3,0.3,0.6);
        leff3->AddEntry(gst,"Signal (Train) Acceptance Eff","l");
        leff3->AddEntry(gbt,"BKG (Train) Rejection Eff","l");
        leff3->Draw();
        gst3->GetXaxis()->SetTitle("BDT score cut position");
        gst3->GetYaxis()->SetTitle("Efficiency");


        c_eff3->Write("Train Efficiency"); 


        TCanvas *c_eff2 = new TCanvas();
        c_eff2->cd();
        gc->Draw("AL");
        gc2->Draw("same L");
        gc2->SetLineColor(kRed);
        c_eff2->Write(); 

        t_strain->Write();
        t_btrain->Write();
        t_stest->Write();
        t_btest->Write();
        btest->Write();
        stest->Write();
        strain->Write();
        btrain->Write();


        TCanvas*cPP = new TCanvas();
  
        TPad *pPP = (TPad*)cPP->cd();

    pPP->SetLogy();

    btest->Scale(1.0/btest->Integral());
    stest->Scale(1.0/stest->Integral());
    btrain->Scale(1.0/btrain->Integral());
    strain->Scale(1.0/strain->Integral());


   btest->Draw("hist");
   btrain->Draw("hist same");
   stest->Draw("hist same");
   strain->Draw("hist same");

   btest->SetTitle("");
   btest->GetXaxis()->SetTitle("BDT Score");

   btest->SetLineColor(kBlue-7);
   btrain->SetLineStyle(9);
   btrain->SetLineColor(kBlue-7);

   stest->SetLineColor(kRed-7);
   strain->SetLineStyle(9);
   strain->SetLineColor(kRed-7);

 
   btest->SetLineWidth(2);
   btrain->SetLineWidth(2);
   stest->SetLineWidth(2);
   strain->SetLineWidth(2);

   TLegend *lP = new TLegend(0.59,0.89,0.59,0.89);
   lP->AddEntry(btest,"Bkg Test","l");
   lP->AddEntry(btrain,"Bkg Train","l");
   lP->AddEntry(stest,"Sig Test","l");
   lP->AddEntry(strain,"Sig Train","l");
   lP->Draw();

   //gStyle->SetOptStat(0);
    cPP->Write();




        f->Close(); 


        bdt_XGBoost_importance(info, booster);
      



        return 0;

    }



    int super_bdt_train(std::string &analysis_tag, const std::vector<bdt_info> & bdt_infos, const std::vector<std::string> & s_tags, const std::vector<std::string> & b_tags, const std::string & additional_sig_cut, const std::string & additional_bkg_cut){

        std::string name_template = "sbnfit_"+analysis_tag+"_stage_"+std::to_string(1)+"_";
        std::string end_template = ".root";

        std::vector<std::string> bdt_score_vars;
        for(auto &info: bdt_infos){
            bdt_score_vars.push_back("simple_"+info.identifier+"_mva");
        }

        //Form a TChain for signal and background like bdt_files
        std::cout<<"Starting to build background chain: "<<std::endl;
        TChain b_chain("singlephoton/simple_tree");  

        for(auto &b: b_tags ){
            std::cout<<"--on tag "<<b<<std::endl;
            b_chain.Add((name_template+b+end_template).c_str());
        }

        TChain b_vertex_chain("singlephoton/vertex_tree");  
        for(auto &b: b_tags ){
            b_vertex_chain.Add((name_template+b+end_template).c_str());
        }


        std::cout<<"Starting to build signal _chain"<<std::endl;
        TChain s_chain("singlephoton/simple_tree");  
        for(auto &s: s_tags ){
            std::cout<<"on tag "<<s<<std::endl;
            s_chain.Add((name_template+s+end_template).c_str());
        }
        TChain s_vertex_chain("singlephoton/vertex_tree");  
        for(auto &s: s_tags ){
            s_vertex_chain.Add((name_template+s+end_template).c_str());
        }

        s_chain.AddFriend(&s_vertex_chain,"super");
        b_chain.AddFriend(&b_vertex_chain,"super");


        std::string name = "SUPERBDT_"+analysis_tag;
        TFile * outfile = TFile::Open((name+"_training.root").c_str(), "recreate");


        TMVA::Factory * factory = new TMVA::Factory(name.c_str(), outfile,"!V:!Silent:Color:DrawProgressBar:AnalysisType=Classification");
        TMVA::DataLoader * dataloader = new TMVA::DataLoader(("BDTxmls_"+name).c_str());

        TCut sig_tcut = TCut(additional_sig_cut.c_str());  
        dataloader->AddSignalTree(&s_chain);
        dataloader->AddCut(sig_tcut,"Signal");
        int signal_entries = s_chain.GetEntries();


        TCut bkg_tcut = TCut(additional_bkg_cut.c_str());  
        dataloader->AddBackgroundTree(&b_chain);
        dataloader->AddCut(bkg_tcut,"Background");
        int background_entries = b_chain.GetEntries();

        dataloader->SetSignalWeightExpression("simple_pot_weight");
        dataloader->SetBackgroundWeightExpression("simple_pot_weight");

        for(auto &var: bdt_score_vars) dataloader->AddVariable(var.c_str());
        std::cout<<"signal_entries: "<<signal_entries<<" background_entries: "<<background_entries<<std::endl;

        dataloader->PrepareTrainingAndTestTree("1","1", "nTrain_Signal="+std::to_string(floor(signal_entries*0.5))+":nTrain_Background="+std::to_string(floor(background_entries*0.5))+":SplitMode=Random:NormMode=NumEvents:!V");

//        factory->BookMethod(dataloader, "BDT" , "TMVA", "!H:!V:NTrees=400:BoostType=RealAdaBoost:nCuts=-1:MaxDepth=2");
        factory->BookMethod(dataloader, "Likelihood" , "TMVA", "!H:!V");

        factory->TrainAllMethods();
        factory->TestAllMethods();
        factory->EvaluateAllMethods();

        outfile->Close();

        delete factory;
        delete dataloader;

    }


    int bdt_XGBoost_importance(bdt_info &info){
        BoosterHandle booster;
        safe_xgboost(XGBoosterCreate(0, 0, &booster));
        safe_xgboost(XGBoosterLoadModel(booster,(info.identifier+".XGBoost.mod").c_str()));

        return bdt_XGBoost_importance(info, booster);
    }

    int bdt_XGBoost_importance(bdt_info &info, BoosterHandle &booster){

        //some vectors to save info on each variable
        int t_vars = info.train_vars.size()+info.spec_vars.size();

        std::vector<int> variable_uses(t_vars,0); 
        std::vector<double> total_gain(t_vars,0.0); 
        std::vector<double> mean_gain(t_vars,0.0); 
        std::vector<int> train_var_id(t_vars,0);

        std::cout<<"We have "<<t_vars<<" variables of which "<<info.train_vars.size()<<" are training variables"<<std::endl;

        std::vector<bool> is_training(t_vars,false);

        for(int k=0; k< info.train_vars.size(); k++){
            is_training.at(info.train_vars[k].id) = true;  
            train_var_id[info.train_vars[k].id] = k;
        }


        //Some storage for the info
        int with_stats = 1;
        bst_ulong out_len = 0;
        const char ** out_dump_array = NULL;
        safe_xgboost(XGBoosterDumpModel(booster, "", with_stats,  &out_len, &out_dump_array));

        std::cout<<"XGBoost importance : "<<out_len<<std::endl;

        for(size_t i=0; i<out_len ; ++i){
            //std::cout<<i<<std::endl;
            if((out_dump_array)[i]!=NULL){
               // std::cout<<i<<" "<<(out_dump_array)[i]<<std::endl;  
                const std::string cconvert((out_dump_array)[i]);

                //Look for the feature, starts with in form "[f10<1.2" ... sp we want to find the number between the [f and the <
                std::string stag = "[f";
                std::string del = "<";

                //when we find the feature, grab the number after the next time gain= appears
                std::string sgain = "gain=";
                size_t found = cconvert.find(stag);

                while (found!=std::string::npos){
                    size_t ndel = cconvert.find(del,found);
                    std::string s_int = cconvert.substr(found+stag.size(),ndel-found-stag.size());
                    int which_var = std::stoi(s_int);
                    variable_uses[which_var]++;
                    std::string s_gain   = "";
                    //ok if we found a variable, add the gain, first locate it.
                    //std::cout<<"here"<<std::endl; 
                    size_t ngain = cconvert.find(sgain,ndel);
                    if(ngain!=std::string::npos){
                        size_t ngainend = cconvert.find(",",ngain);
                        if (sgain.size() <1) continue;
                        if( ngainend !=std::string::npos)   s_gain = cconvert.substr(ngain+sgain.size(), ngainend-ngain-sgain.size()); 
                        size_t offset = 0;
                        //std::cout<<"ngainend,ngain, sgain.size(),sgain.length() , s_gain = " <<ngainend<<", "<<ngain<<", "<<sgain.size()<<", "<<sgain.length()<<", "<<s_gain<<" "<<s_gain.length()<<" "<<s_gain.size()<<std::endl;
                        double d_gain =  std::stod(s_gain,&offset);
                        total_gain[which_var] +=  d_gain;
                    }        
                 //                   std::cout<<" var "<<s_int<<" gain "<<s_gain<<std::endl;
                    found = cconvert.find(stag,ndel);
                }
            }
        }

        double total_gain_allvars = 0;
        for(int i=0; i<total_gain.size() ; ++i){
            total_gain_allvars+=  total_gain[i];
            mean_gain[i] = total_gain[i]/(double)variable_uses[i];
        }

        //Sort them in different ways
        std::vector<size_t> sorted_by_total_gain = sort_indexes<double>(total_gain);
        std::vector<size_t> sorted_by_mean_gain = sort_indexes<double>(mean_gain);
        std::vector<size_t> sorted_by_uses = sort_indexes<int>(variable_uses);

        /* 
           for(int i = 0; i < sorted_by_uses.size(); i++){
           std::cout<<"sorted_by_uses["<<i<<"] = "<<sorted_by_uses[i]<<std::endl;
           }
           for(int i = 0; i < info.train_vars.size(); i++){
           std::cout<<i<<":  "<<info.train_vars[i].unit<<std::endl;
           }
           */
         std::cout<<"----------- Sort By Input Order: "<<info.identifier<<" ----------------------"<<std::endl;
       for(int i=0; i< is_training.size();i++){
               if( is_training[i]){ 
               std::cout<<i<<" Variable: "<<info.train_vars[train_var_id[i]].unit<<"-- -- uses: "<<variable_uses[i]<<" gain: "<<total_gain[i]<<"  <gain>: "<<total_gain[i]/(double)variable_uses[i]<<"   relative gain: "<<total_gain[i]/total_gain_allvars<<std::endl;      
     
               } else{
                std::cout<<i<<" Variable: "<<info.train_vars[train_var_id[i]].unit<<"  -"<<std::endl;
               }
       }
               
       

        TCanvas cuses("","",3000,1200);
        cuses.cd();
        TH1D htuses("tuses","tuses",info.train_vars.size(),0,info.train_vars.size());
        std::cout<<"----------- Sort By Uses: "<<info.identifier<<" ----------------------"<<std::endl;
        std::cout<<"sorted_by_uses size = " <<sorted_by_uses.size()<<", variable_uses size = "<<variable_uses.size()<<std::endl;
        for(int i=0; i< variable_uses.size();i++){
            size_t is = sorted_by_uses[sorted_by_uses.size()-1-i];
            if(is_training[is]){
                std::cout<<i<<"  "<<    " "<<is<<" Variable: "<<info.train_vars[train_var_id[is]].unit<<"-- -- uses: "<<variable_uses[is]<<" gain: "<<total_gain[is]<<"  <gain>: "<<total_gain[is]/(double)variable_uses[is]<<std::endl;
                htuses.SetBinContent(i+1, variable_uses[is]);
                htuses.GetXaxis()->SetBinLabel(i+1,  (std::to_string(train_var_id[is])+" : "+info.train_vars[train_var_id[is]].unit).c_str()); // Find out which bin on the x-axis the point corresponds to and set the
                }
        }
        htuses.Draw("hist");
        htuses.SetTitle("Variable importance by total uses");
        cuses.SetBottomMargin(0.5);
        cuses.SaveAs(("XGBoost_"+info.identifier+"_var_import_total_uses.pdf").c_str(),"pdf");


        TCanvas cgain("","",3000,1200);
        cgain.cd();

        TH1D htgain("tgain","tgain",info.train_vars.size(),0,info.train_vars.size());
        std::cout<<"----------- Sort By Total Gain: "<<info.identifier<<" ----------------------"<<std::endl;
        for(int i=0; i< variable_uses.size();i++){
            size_t is = sorted_by_total_gain[sorted_by_total_gain.size()-1-i];
            if(is_training[is]){
                std::cout<<i<<"  "<<    " "<<is<<" Variable: "<<info.train_vars[train_var_id[is]].unit<<"-- -- uses: "<<variable_uses[is]<<" gain: "<<total_gain[is]<<"  <gain>: "<<total_gain[is]/(double)variable_uses[is]<<std::endl;
                htgain.SetBinContent(i+1, total_gain[is]);
                htgain.GetXaxis()->SetBinLabel(i+1,  (std::to_string(train_var_id[is])+" : "+info.train_vars[train_var_id[is]].unit).c_str()); // Find out which bin on the x-axis the point corresponds to and set the
            }
        }
        htgain.Draw("hist");
        htgain.SetTitle("Variable importance by total gain");
        cgain.SetBottomMargin(0.5);
        cgain.SaveAs(("XGBoost_"+info.identifier+"_var_import_total_gain.pdf").c_str(),"pdf");

        std::cout<<"----------- Sort By Mean Gain: "<<info.identifier<<" ----------------------"<<std::endl;
        for(int i=0; i< variable_uses.size();i++){
            size_t is = sorted_by_mean_gain[sorted_by_mean_gain.size()-1-i];
            if(is_training[is])   std::cout<<i<<"  "<<    " "<<is<<" Variable: "<<info.train_vars[train_var_id[is]].unit<<"-- -- uses: "<<variable_uses[is]<<" gain: "<<total_gain[is]<<"  <gain>: "<<total_gain[is]/(double)variable_uses[is]<<std::endl;
        }






        std::cout<<"done!"<<std::endl;
        return 0;

    }
