#include "bdt_train.h"

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

  // If you want to plot something, but don't want to train on it, add it as a spectator
  /*
  std::string shower_index1 = "(reco_shower_ordered_energy_index[0])";
  std::string shower_index2 = "(reco_shower_ordered_energy_index[1])";
  std::string E1 = "reco_shower_energy["+shower_index1+"]";
  std::string E2 = "reco_shower_energy["+shower_index2+"]";
  std::string two_shower_opening_angle = "(reco_shower_dirx[0]*reco_shower_dirx[1] + reco_shower_diry[0]*reco_shower_diry[1] + reco_shower_dirz[0]*reco_shower_dirz[1])";
  std::string invariant_mass = "sqrt(2.0*"+E1+"*"+E2+"*(1.0-"+two_shower_opening_angle+"))/1000";

  std::string p_pi_x = "(reco_shower_energy["+shower_index1+"]*reco_shower_dirx["+shower_index1+"] + reco_shower_energy["+shower_index2+"]*reco_shower_dirx["+shower_index2+"])";
  std::string p_pi_y = "(reco_shower_energy["+shower_index1+"]*reco_shower_diry["+shower_index1+"] + reco_shower_energy["+shower_index2+"]*reco_shower_diry["+shower_index2+"])";
  std::string p_pi_z = "(reco_shower_energy["+shower_index1+"]*reco_shower_dirz["+shower_index1+"] + reco_shower_energy["+shower_index2+"]*reco_shower_dirz["+shower_index2+"])";
  std::string p_pi = "sqrt("+p_pi_x+"*"+p_pi_x+" + "+p_pi_y+"*"+p_pi_y+" + "+p_pi_z+"*"+p_pi_z+")";

  std::string cm_angle = "(fabs("+E1+" - "+E2+")/("+p_pi+"))";
  dataloader->AddSpectator(cm_angle); // Should be CM angle
  */

	for(bdt_variable &var: variables) dataloader->AddVariable(var.name.c_str());

	std::cout<<"signal_entries: "<<signal_entries<<" background_entries: "<<background_entries<<std::endl;
	std::cout<<"PREFILTERED signal_entries: "<<signal_ttree_prefiltered->GetEntries()<<" background_entries: "<<background_ttree_prefiltered->GetEntries()<<std::endl;


	dataloader->PrepareTrainingAndTestTree(sig_tcut, back_tcut,
	  "nTrain_Signal="+std::to_string(floor(signal_entries*0.75))+":nTrain_Background="+std::to_string(floor(background_entries*0.75))+":SplitMode=Random:NormMode=NumEvents:!V");
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
