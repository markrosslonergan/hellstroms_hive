#include "bdt_train.h"

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

    std::cout<<"bdt_train::bdt_train()\t\t||\t\t Finished Training! Going to do some tests and make some plots."<<std::endl;
    TFile * readbackfile = TFile::Open((name+"_training.root").c_str(), "read");
    TTree * trainTree = (TTree*)readbackfile->Get( ("BDTxmls_"+name+"/TrainTree").c_str());
    TTree * testTree = (TTree*)readbackfile->Get( ("BDTxmls_"+name+"/TestTree").c_str());

    std::cout<<"We had "<<trainTree->GetEntries()<<" total training events and "<<testTree->GetEntries()<<" total testing events"<<std::endl;

    TCanvas *c_var = new TCanvas((name+"_training_plots").c_str(), (name+"_training_plots").c_str(),2200,1000);
    c_var->Divide(2,1);
    TPad *p1 = (TPad*)c_var->cd(1);

    int nbins = 50;
    TH1D * train_signal = new TH1D("trainsignal","trainsignal",nbins,0,1);
    TH1D * test_signal = new TH1D("testsignal","testsignal",nbins,0,1);

    TH1D * train_bkg = new TH1D("trainbkg","trainbkg",nbins,0,1);
    TH1D * test_bkg = new TH1D("testbkg","testbkg",nbins,0,1);

    int test_classID =0;
    int train_classID =0;

    float test_response=0;
    float train_response=0;

    testTree->SetBranchAddress("classID",&test_classID);
    trainTree->SetBranchAddress("classID",&train_classID);
    testTree->SetBranchAddress((info.TMVAmethod.bdt_tag).c_str(),&test_response);
    trainTree->SetBranchAddress((info.TMVAmethod.bdt_tag).c_str(),&train_response);

    float min = FLT_MAX;
    float max = FLT_MIN;

    for(int i=0; i< testTree->GetEntries(); i++){
            testTree->GetEntry(i);
            if(test_classID==0){
                test_signal->Fill(test_response);
            }else if(test_classID==1){
                test_bkg->Fill(test_response);
            }
            min = std::min(min,test_response);
            max = std::max(max,test_response);
    }

    for(int i=0; i< trainTree->GetEntries(); i++){
            trainTree->GetEntry(i);
            if(train_classID==0){
                train_signal->Fill(train_response);
            }else if(train_classID==1){
                train_bkg->Fill(train_response);
            }
            min = std::min(min,train_response);
            max = std::max(max,train_response);

    }

    
            test_signal->Scale(1.0/test_signal->Integral());			
            train_signal->Scale(1.0/train_signal->Integral());			
            test_bkg->Scale(1.0/test_bkg->Integral());			
            train_bkg->Scale(1.0/train_bkg->Integral());			

            //Some KS tests. 
            double signal_KS = test_signal->KolmogorovTest(train_signal);
            double bkg_KS = test_bkg->KolmogorovTest(train_bkg);

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

    //
    

	return 0;
}
