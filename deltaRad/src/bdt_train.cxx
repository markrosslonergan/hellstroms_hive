#include "bdt_train.h"

int bdt_train(bdt_info info, bdt_file *signal_file, bdt_file *background_file, std::vector<bdt_variable> variables, std::vector<method_struct> & methods){

	std::string const name = info.identifier;
	TFile * outfile = TFile::Open((name+"_training.root").c_str(), "recreate");


	TMVA::Factory * factory = new TMVA::Factory(name.c_str(), outfile,
			//	"!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification");
				"!V:!Silent:Color:DrawProgressBar:AnalysisType=Classification");
	TMVA::DataLoader * dataloader = new TMVA::DataLoader(("BDTxmls_"+name).c_str());

	int bdt_precut_stage = 1;
	TCut sig_tcut =  TCut(signal_file->getStageCuts(bdt_precut_stage,-9,-9).c_str());
	TCut back_tcut = TCut(background_file->getStageCuts(bdt_precut_stage,-9,-9).c_str());

	dataloader->AddSignalTree(signal_file->tvertex);
	int signal_entries = signal_file->tvertex->GetEntries(sig_tcut);

	dataloader->AddBackgroundTree(background_file->tvertex);
	int background_entries = background_file->tvertex->GetEntries(back_tcut);


	dataloader->SetSignalWeightExpression(signal_file->weight_branch.c_str());
	dataloader->SetBackgroundWeightExpression(background_file->weight_branch.c_str());

	for(bdt_variable &var: variables) dataloader->AddVariable(var.name.c_str());

	std::cout<<"signal_entries: "<<signal_entries<<" background_entries: "<<background_entries<<std::endl;

	std::cout<<"SIGNAL CUTS : "<<sig_tcut<<std::endl;
	std::cout<<"BKG CUTS : "<<back_tcut<<std::endl;

	std::string splotopts = "nTrain_Signal="+std::to_string(floor(signal_entries))+":nTrain_Background="+std::to_string(floor(background_entries))+":SplitMode=Random:NormMode=NumEvents:!V";
	const TString s = splotopts;

	
 
        dataloader->PrepareTrainingAndTestTree(sig_tcut, back_tcut, "V:NormMode=NumEvents:SplitMode=Random:nTrain_Signal=10000:nTest_Signal=10:nTrain_Background=10000:nTest_Background=10");
		
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
