#include "bdt_train.h"

int plot_train(bdt_info info, bdt_file *signal_file, bdt_file *background_file){
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
   int nbins = 60;
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
    testTree->SetBranchAddress((info.TMVAmethod.bdt_tag).c_str(),&test_response);
    trainTree->SetBranchAddress((info.TMVAmethod.bdt_tag).c_str(),&train_response);
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

            
            test_signal->SetLineColor(signal_file->col);
            train_signal->SetLineColor(signal_file->col);
            train_signal->SetMarkerColor(signal_file->col);

            train_bkg->SetLineColor(background_file->col);
            train_bkg->SetMarkerColor(background_file->col);
            test_bkg->SetLineColor(background_file->col);

            train_signal->SetLineWidth(2);
            train_signal->SetLineWidth(2);
            test_signal->SetLineWidth(2);
            test_bkg->SetLineWidth(2);

            train_signal->SetMarkerStyle(20);
            train_bkg->SetMarkerStyle(20);

            p1->cd();
            test_signal->SetStats(0);
            test_signal->Draw("hist");
            test_bkg->Draw("hist same");

            test_signal->SetMinimum(0.0);
            test_signal->GetXaxis()->SetRangeUser(min*0.9,max*1.1);
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

            l->AddEntry(test_signal, ("Test "+signal_file->plot_name).c_str(),"l");	
            l->AddEntry(train_signal, ("Train "+signal_file->plot_name).c_str(),"p");	
            l->AddEntry(test_bkg, ("Test "+background_file->plot_name).c_str(),"l");	
            l->AddEntry(train_bkg, ("Train "+background_file->plot_name).c_str(),"p");	
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

int bdt_train(bdt_info info, bdt_file *signal_file, bdt_file *background_file){

    std::vector<method_struct> methods = {info.TMVAmethod};
    std::vector<bdt_variable> variables = info.train_vars;

    return bdt_train(info, signal_file, background_file, variables, methods);
}

int bdt_train(bdt_info info, bdt_file *signal_file, bdt_file *background_file, std::vector<bdt_variable> variables, std::vector<method_struct> & methods){

	std::string const name = info.identifier;
	TFile * outfile = TFile::Open((name+"_training.root").c_str(), "recreate");


	TMVA::Factory * factory = new TMVA::Factory(name.c_str(), outfile,
				//"!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification");
				"!V:!Silent:Color:DrawProgressBar:AnalysisType=Classification");
	TMVA::DataLoader * dataloader = new TMVA::DataLoader(("BDTxmls_"+name).c_str());

	int bdt_precut_stage = 1;
	TCut sig_tcut =  TCut(signal_file->getStageCuts(bdt_precut_stage,-9,-9).c_str());
	TCut back_tcut = TCut(background_file->getStageCuts(bdt_precut_stage,-9,-9).c_str());

    TTree * background_ttree_prefiltered = (TTree*)background_file->tvertex->CopyTree(back_tcut);
    TTree * signal_ttree_prefiltered = (TTree*)signal_file->tvertex->CopyTree(sig_tcut);

	dataloader->AddSignalTree(signal_ttree_prefiltered);
	int signal_entries = signal_file->tvertex->GetEntries(sig_tcut);

	dataloader->AddBackgroundTree(background_ttree_prefiltered);
	int background_entries = background_file->tvertex->GetEntries(back_tcut);


	dataloader->SetSignalWeightExpression(signal_file->weight_branch.c_str());
	dataloader->SetBackgroundWeightExpression(background_file->weight_branch.c_str());

	for(bdt_variable &var: variables) dataloader->AddVariable(var.name.c_str());

	std::cout<<"signal_entries: "<<signal_entries<<" background_entries: "<<background_entries<<std::endl;
	std::cout<<"PREFILTERED signal_entries: "<<signal_ttree_prefiltered->GetEntries()<<" background_entries: "<<background_ttree_prefiltered->GetEntries()<<std::endl;


	dataloader->PrepareTrainingAndTestTree(sig_tcut, back_tcut,
	  "nTrain_Signal="+std::to_string(floor(signal_entries*info.TMVAmethod.training_fraction))+":nTrain_Background="+std::to_string(floor(background_entries*info.TMVAmethod.training_fraction))+":SplitMode=Random:NormMode=NumEvents:!V");
  // "nTrain_Signal=66629:nTrain_Background=49472:nTest_Signal=15578:nTest_Background=11885:SplitMode=Random:NormMode=NumEvents:!V"
			//"SplitMode=Random:NormMode=NumEvents:!V");

    //factory.PrepareTrainingAndTestTree(ROOT.TCut(),"NormMode=EqualNumEvents:SplitMode=Block:nTrain_Signal=%s:nTest_Signal=%s:nTrain_Background=%s:nTest_Background=%s"%(nTrain_Signal,nTest_Signal,nTrain_Background,nTest_Background))


	for(method_struct const & method : methods) factory->BookMethod(dataloader, method.type, method.str, method.option);

	factory->TrainAllMethods();
	factory->TestAllMethods();
	factory->EvaluateAllMethods();

	outfile->Close();

	delete factory;
	delete dataloader;

	return 0;
}

// Separate training and plotting variables
int bdt_train(bdt_info info, bdt_file *signal_file, bdt_file *background_file, std::vector<bdt_variable> train_variables, std::vector<bdt_variable> plot_variables, std::vector<method_struct> & methods){

	std::string const name = info.identifier;
	TFile * outfile = TFile::Open((name+"_training.root").c_str(), "recreate");


	TMVA::Factory * factory = new TMVA::Factory(name.c_str(), outfile,
				//"!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification");
				"!V:!Silent:Color:DrawProgressBar:AnalysisType=Classification");
	TMVA::DataLoader * dataloader = new TMVA::DataLoader(("BDTxmls_"+name).c_str());

	int bdt_precut_stage = 1;
	TCut sig_tcut =  TCut(signal_file->getStageCuts(bdt_precut_stage,-9,-9).c_str());
	TCut back_tcut = TCut(background_file->getStageCuts(bdt_precut_stage,-9,-9).c_str());

    TTree * background_ttree_prefiltered = (TTree*)background_file->tvertex->CopyTree(back_tcut);
    TTree * signal_ttree_prefiltered = (TTree*)signal_file->tvertex->CopyTree(sig_tcut);

	dataloader->AddSignalTree(signal_ttree_prefiltered);
	int signal_entries = signal_file->tvertex->GetEntries(sig_tcut);

	dataloader->AddBackgroundTree(background_ttree_prefiltered);
	int background_entries = background_file->tvertex->GetEntries(back_tcut);


	dataloader->SetSignalWeightExpression(signal_file->weight_branch.c_str());
	dataloader->SetBackgroundWeightExpression(background_file->weight_branch.c_str());

//  for (bdt_variable &var: plot_variables) dataloader->AddSpectator(var.name.c_str() );

	for(bdt_variable &var: train_variables) dataloader->AddVariable(var.name.c_str());

	std::cout<<"signal_entries: "<<signal_entries<<" background_entries: "<<background_entries<<std::endl;
	std::cout<<"PREFILTERED signal_entries: "<<signal_ttree_prefiltered->GetEntries()<<" background_entries: "<<background_ttree_prefiltered->GetEntries()<<std::endl;


	dataloader->PrepareTrainingAndTestTree(sig_tcut, back_tcut,
	  "nTrain_Signal="+std::to_string(floor(signal_entries*0.65))+":nTrain_Background="+std::to_string(floor(background_entries*0.65))+":SplitMode=Random:NormMode=NumEvents:!V");
  // "nTrain_Signal=66629:nTrain_Background=49472:nTest_Signal=15578:nTest_Background=11885:SplitMode=Random:NormMode=NumEvents:!V"
			//"SplitMode=Random:NormMode=NumEvents:!V");

    //factory.PrepareTrainingAndTestTree(ROOT.TCut(),"NormMode=EqualNumEvents:SplitMode=Block:nTrain_Signal=%s:nTest_Signal=%s:nTrain_Background=%s:nTest_Background=%s"%(nTrain_Signal,nTest_Signal,nTrain_Background,nTest_Background))


	for(method_struct const & method : methods) factory->BookMethod(dataloader, method.type, method.str, method.option);

	factory->TrainAllMethods();
	factory->TestAllMethods();
	factory->EvaluateAllMethods();

	outfile->Close();

	delete factory;
	delete dataloader;







	return 0;
}

int convertToLibSVM(bdt_info info, bdt_file *file){

    std::vector<bdt_variable> variables = info.train_vars;
    std::string const name = info.identifier;
    std::cout<<"Beginninng to convert files into a libSVM format for XGBoost"<<std::endl;

    std::ofstream sslibSVM;
    sslibSVM.open (name+"_"+file->tag+".libSVM.dat");
    TFile * outfile = TFile::Open((name+"libSVM_test.root").c_str(), "recreate");

    TTreeFormula* weight = new TTreeFormula("sig_w",file->weight_branch.c_str(),file->tvertex);
       
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

        sslibSVM<<"0:"<<wei<<" ";

        for(int t=0; t< tree_formulas_v.size();++t){
            tree_formulas_v[t]->GetNdata();
            double val = tree_formulas_v[t]->EvalInstance();
            int id = id_v[t];
                sslibSVM<<id<<":"<<val<<" ";

        }

        sslibSVM<<std::endl;

       next_entry = file->precut_list->Next();
       num++;
    } 


    sslibSVM.close();
    outfile->Close();
	return 0;
}



int convertToLibSVM(bdt_info info, bdt_file *signal_file, bdt_file *background_file){

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
   
    TTreeFormula* sig_weight = new TTreeFormula("sig_w",signal_file->weight_branch.c_str(),signal_ttree_prefiltered);
    TTreeFormula* bkg_weight = new TTreeFormula("bkg_w",background_file->weight_branch.c_str(),background_ttree_prefiltered);
        
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
        double wei = sig_weight->EvalInstance();

        if(i < sig_train_num){
            sslibSVMtrain<<"1:"<<wei<<" ";
        }else{
            sslibSVMtest<<"1:"<<wei<<" ";
        }

        for(int t=0; t< sig_tree_formulas_v.size();++t){
            sig_tree_formulas_v[t]->GetNdata();
            double val = sig_tree_formulas_v[t]->EvalInstance();
            int id = id_v[t];

            if(i < sig_train_num){
                sslibSVMtrain<<id<<":"<<val<<" ";
            }else{
                sslibSVMtest<<id<<":"<<val<<" ";
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

            if(i < bkg_train_num){
                sslibSVMtrain<<id<<":"<<val<<" ";
            }else{
                sslibSVMtest<<id<<":"<<val<<" ";
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



int bdt_XGtrain(bdt_info info){

    std::string const name = info.identifier;

    TFile *f = new TFile(("XGBoost_train_output_"+name+".root").c_str(),"recreate");
    
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

    DMatrixHandle dtrain, dtest;
    int silent = 0;
    int use_gpu = 0;  // set to 1 to use the GPU for training
 
    safe_xgboost(XGDMatrixCreateFromFile((info.identifier+".libSVM.train.dat").c_str(), silent, &dtrain));
    safe_xgboost(XGDMatrixCreateFromFile((info.identifier+".libSVM.test.dat").c_str(), silent, &dtest));
  
  // create the booster
  BoosterHandle booster;
  DMatrixHandle eval_dmats[2] = {dtrain, dtest};
  safe_xgboost(XGBoosterCreate(eval_dmats, 2, &booster));

  
  // configure the training
  // available parameters are described here:
  // https://xgboost.readthedocs.io/en/latest/parameter.html
  
  
 int n_trees = 250;

 std::cout<<"XGBoost being setup with "<<std::endl;
 for(auto &pairs: info.TMVAmethod.xg_config){
      if(pairs.first == "n_trees"){
            n_trees = (int)std::stod(pairs.second);
      }else{
          safe_xgboost(XGBoosterSetParam(booster,pairs.first.c_str(),pairs.second.c_str()));
          std::cout<<"--"<<pairs.first<<" with a value of "<<pairs.second<<std::endl;
      }
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
  std::vector<double> test_error;
  std::vector<double> train_error;
  
  const char* eval_names[2] = {"train", "test"};
  const char* eval_result = NULL;
  int cotr = 0;
  for (int i = 0; i < n_trees; ++i){
    safe_xgboost(XGBoosterUpdateOneIter(booster, i, dtrain));
    safe_xgboost(XGBoosterEvalOneIter(booster, i, eval_dmats, eval_names, 2, &eval_result));
    std::string res = eval_result;
    int i1 = res.find(":",0); 
    std::string s1 = res.substr(i1+1,8);
    int i2 = res.find(":",i1+2);
    std::string s2 = res.substr(i2+1,8);
    std::cout<<i<<" TrainError "<<s1<<" TestError "<<s2<<std::endl;
    train_error.push_back(std::stod(s1));
    test_error.push_back(std::stod(s2));
    iteration.push_back(i);

    printf("%s\n", eval_result);
    if(i>2){
    if(test_error[i-1] > test_error[i-2] ){
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

  f->cd();

  TCanvas *c_error = new TCanvas();
  c_error->cd();
  TGraph *g_test = new TGraph(iteration.size(),&iteration[0],&test_error[0]);
  TGraph *g_train = new TGraph(iteration.size(),&iteration[0],&train_error[0]);
  g_test->Draw("AL");
  g_test->SetLineColor(kRed);
  g_train->Draw("same CL");
  g_train->SetLineColor(kBlue);
  c_error->Write(); 

  std::vector<double> pos;
  std::vector<double> st,st2;
  std::vector<double> bt,bt2;
  for(double p =0; p<=1;p+=0.01){
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
  TGraph *gst = new TGraph(pos.size(),&pos[0],&st[0]);
  TGraph *gbt = new TGraph(pos.size(),&pos[0],&bt[0]);
  gst->Draw("AL");
  gst->SetMaximum(1);
  gst->SetMinimum(0);
  gst->SetLineColor(kRed);
  gbt->Draw("same CL");
  gbt->SetLineColor(kBlue);
  c_eff->Write(); 

  TCanvas *c_eff2 = new TCanvas();
  c_eff2->cd();
  gc->Draw("AL");
  gc2->Draw("same L");
  gc2->SetLineColor(kRed);
  c_eff2->Write(); 


 t_strain->Write();
 t_btrain->Write();
 btest->Write();
 stest->Write();
 strain->Write();
 btrain->Write();
    return 0;

}

