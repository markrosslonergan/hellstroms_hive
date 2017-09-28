

void runtmva(std::string dir) {

  double const pot_sp = 2.23455e+22;
  double const pot_bnb = 1.0097e+19;
  double const pot_bnb_cosmic = 1.06793e+19;
  double const ngenbnbcosmic = 10450;
  double const ngencosmic = 10348;
  
  double const run_pot = 6.6e20;

  dir += "/";
  std::string const gDir = "LEEPhoton/";

  TFile * ifile_sp = TFile::Open((dir+"runmv_sp.root").c_str());
  TTree * vertex_tree_sp = (TTree*)ifile_sp->Get((gDir+"vertex_tree").c_str());  

  TFile * ifile_bnb_cosmic = TFile::Open((dir+"runmv_bnb_cosmic.root").c_str());
  TTree * vertex_tree_bnb_cosmic = (TTree*)ifile_bnb_cosmic->Get((gDir+"vertex_tree").c_str());

  TFile * ifile_cosmic = TFile::Open((dir+"runmv_cosmic.root").c_str());
  TTree * vertex_tree_cosmic = (TTree*)ifile_cosmic->Get((gDir+"vertex_tree").c_str());

  TFile * ofile = TFile::Open("tmva.root", "recreate");

  double const sp_weight = run_pot / pot_sp;
  double const bnb_cosmic_weight = run_pot / pot_bnb_cosmic;
  double const cosmic_weight = ngenbnbcosmic/ngencosmic*10.729 / pot_bnb_cosmic * run_pot;

  TMVA::Factory *factory = new TMVA::Factory("TMVAClassification",
					     ofile,
					     "!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification" );
  TMVA::DataLoader *dataloader=new TMVA::DataLoader("dataset");

  dataloader->AddSignalTree(vertex_tree_sp, sp_weight);
  dataloader->AddBackgroundTree(vertex_tree_bnb_cosmic, bnb_cosmic_weight);
  dataloader->AddBackgroundTree(vertex_tree_cosmic, cosmic_weight);

  dataloader->AddVariable("closest_asso_shower_dist_to_flashzcenter");
  dataloader->AddVariable("totalpe_ibg_sum"); 
  dataloader->AddVariable("summed_associated_reco_shower_energy");
  dataloader->AddVariable("longest_asso_track_displacement");

  dataloader->AddVariable("reco_nu_vtx_dist_to_closest_tpc_wall");
  dataloader->AddVariable("shortest_asso_shower_to_vert_dist");
  dataloader->AddVariable("most_energetic_shower_reco_thetaxz");
  dataloader->AddVariable("most_energetic_shower_reco_thetayz");
  dataloader->AddVariable("longest_asso_track_thetaxz");
  dataloader->AddVariable("longest_asso_track_thetayz");  

  //TCut back_cut = "passed_swtrigger == 1 && reco_nu_vtx_fid_contained == 1";
  //TCut sig_cut = back_cut + "is_delta_rad == 1 && true_nu_vtx_fid_contained == 1";

  TCut back_cut = "passed_swtrigger == 1";
  TCut sig_cut = back_cut + "is_delta_rad == 1 && true_nu_vtx_fid_contained == 1";

  dataloader->PrepareTrainingAndTestTree(sig_cut, back_cut,
					 "nTrain_Signal=1700:nTrain_Background=13000:SplitMode=Random:NormMode=NumEvents:!V");

  /*
  factory->BookMethod( dataloader, TMVA::Types::kCuts, "Cuts",
		       "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart" );
  factory->BookMethod( dataloader, TMVA::Types::kCuts, "CutsD",
		       "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart:VarTransform=Decorrelate" );
  factory->BookMethod( dataloader, TMVA::Types::kCuts, "CutsPCA",
		       "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart:VarTransform=PCA" );
  factory->BookMethod( dataloader, TMVA::Types::kCuts, "CutsGA",
		       "H:!V:FitMethod=GA:CutRangeMin[0]=-10:CutRangeMax[0]=10:VarProp[1]=FMax:EffSel:Steps=30:Cycles=3:PopSize=400:SC_steps=10:SC_rate=5:SC_factor=0.95" );
  factory->BookMethod( dataloader, TMVA::Types::kCuts, "CutsSA",
		       "!H:!V:FitMethod=SA:EffSel:MaxCalls=150000:KernelTemp=IncAdaptive:InitialTemp=1e+6:MinTemp=1e-6:Eps=1e-10:UseDefaultScale" );
  */

  factory->BookMethod( dataloader, TMVA::Types::kBDT, "BDTG",
		       "!H:!V:NTrees=1000:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=20:MaxDepth=2" );
		       
  /*
    factory->BookMethod( dataloader, TMVA::Types::kBDT, "BDT",
    "!H:!V:NTrees=850:MinNodeSize=2.5%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=20" );
  factory->BookMethod( dataloader, TMVA::Types::kBDT, "BDTB",
		       "!H:!V:NTrees=400:BoostType=Bagging:SeparationType=GiniIndex:nCuts=20" );
  factory->BookMethod( dataloader, TMVA::Types::kBDT, "BDTD",
		       "!H:!V:NTrees=400:MinNodeSize=5%:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:VarTransform=Decorrelate" );
  factory->BookMethod( dataloader, TMVA::Types::kBDT, "BDTF",
		       "!H:!V:NTrees=50:MinNodeSize=2.5%:UseFisherCuts:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20" );
  factory->BookMethod( dataloader, TMVA::Types::kRuleFit, "RuleFit",
		       "H:!V:RuleFitModule=RFTMVA:Model=ModRuleLinear:MinImp=0.001:RuleMinDist=0.001:NTrees=20:fEventsMin=0.01:fEventsMax=0.5:GDTau=-1.0:GDTauPrec=0.01:GDStep=0.01:GDNSteps=10000:GDErrScale=1.02" );
  */

  factory->TrainAllMethods();
  factory->TestAllMethods();
  factory->EvaluateAllMethods();

  ofile->Close();

  //if(!gROOT->IsBatch()) TMVA::TMVAGui(ofile->GetName());

}
