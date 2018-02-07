#include "bdt_train.h"

int bdt_train(bdt_cuts cuts, bdt_file *signal_file, bdt_file *background_file, std::vector<bdt_variable> variables, std::vector<method_struct> & methods){

	std::string const name = cuts.identifier;
	TFile * outfile = TFile::Open((name+"_training.root").c_str(), "recreate");


	TMVA::Factory * factory = new TMVA::Factory(name.c_str(), outfile,
				"!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification");
	TMVA::DataLoader * dataloader = new TMVA::DataLoader(("BDTxmls_"+name).c_str());

	TCut all_tcut = cuts.base_cuts.c_str();
	TCut sig_tcut = all_tcut + TCut(cuts.signal_definition.c_str());
	TCut back_tcut = all_tcut + TCut(cuts.background_definition.c_str());

	double signal_entries = 0;
	dataloader->AddSignalTree(signal_file->tvertex);
	signal_entries += signal_file->tvertex->GetEntries(sig_tcut);

	double background_entries = 0;
	dataloader->AddBackgroundTree(background_file->tvertex);
	background_entries += background_file->tvertex->GetEntries(back_tcut);

	for(bdt_variable &var: variables) dataloader->AddVariable(var.name.c_str());

	std::cout<<"signal_entries: "<<signal_entries<<" background_entries: "<<background_entries<<std::endl;

	dataloader->PrepareTrainingAndTestTree(sig_tcut, back_tcut,
			"nTrain_Signal="+std::to_string(int(signal_entries/2))+":nTrain_Background="+std::to_string(int(background_entries/2))+":SplitMode=Random:NormMode=NumEvents:!V");

	for(method_struct const & method : methods) factory->BookMethod(dataloader, method.type, method.str, method.option);

	factory->TrainAllMethods();
	factory->TestAllMethods();
	factory->EvaluateAllMethods();

	outfile->Close();

	delete factory;
	delete dataloader;

	return 0;
}
