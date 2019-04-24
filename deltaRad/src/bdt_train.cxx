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

	//TTree * background_ttree_prefiltered = (TTree*)background_file->tvertex->CopyTree(back_tcut);
	//TTree * signal_ttree_prefiltered = (TTree*)signal_file->tvertex->CopyTree(sig_tcut);
	//dataloader->AddSignalTree(signal_ttree_prefiltered);
	dataloader->AddSignalTree(signal_file->tvertex);
	int signal_entries = signal_file->tvertex->GetEntries(sig_tcut);

	dataloader->AddBackgroundTree(background_file->tvertex); 
	//dataloader->AddBackgroundTree(background_ttree_prefiltered);
	int background_entries = background_file->tvertex->GetEntries(back_tcut);


	dataloader->SetSignalWeightExpression(signal_file->weight_branch.c_str());
	dataloader->SetBackgroundWeightExpression(background_file->weight_branch.c_str());

	for(bdt_variable &var: variables) dataloader->AddVariable(var.name.c_str());

	std::cout<<"signal_entries: "<<signal_entries<<" background_entries: "<<background_entries<<std::endl;
	//std::cout<<"PREFILTERED signal_entries: "<<signal_ttree_prefiltered->GetEntries()<<" background_entries: "<<background_ttree_prefiltered->GetEntries()<<std::endl;


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

	//TTree * background_ttree_prefiltered = (TTree*)background_file->tvertex->CopyTree(back_tcut);
	//TTree * signal_ttree_prefiltered = (TTree*)signal_file->tvertex->CopyTree(sig_tcut);


	//dataloader->AddSignalTree(signal_ttree_prefiltered);
	dataloader->AddSignalTree(signal_file->tvertex);
	int signal_entries = signal_file->tvertex->GetEntries(sig_tcut);

	//dataloader->AddBackgroundTree(background_ttree_prefiltered);
	dataloader->AddBackgroundTree(background_file->tvertex);
	int background_entries = background_file->tvertex->GetEntries(back_tcut);


	dataloader->SetSignalWeightExpression(signal_file->weight_branch.c_str());
	dataloader->SetBackgroundWeightExpression(background_file->weight_branch.c_str());

//  for (bdt_variable &var: plot_variables) dataloader->AddSpectator(var.name.c_str() );

	for(bdt_variable &var: train_variables) dataloader->AddVariable(var.name.c_str());

	std::cout<<"signal_entries: "<<signal_entries<<" background_entries: "<<background_entries<<std::endl;
	//std::cout<<"PREFILTERED signal_entries: "<<signal_ttree_prefiltered->GetEntries()<<" background_entries: "<<background_ttree_prefiltered->GetEntries()<<std::endl;


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
