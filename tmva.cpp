


#include <iostream>
#include <typeinfo>

#include "TFile.h"
#include "TTree.h"

#include "tmva/DataLoader.h"
#include "tmva/Reader.h"



class void_vec {

  std::vector<std::pair<void*, std::string>> pv;
  
public:

  ~void_vec() {
    for(std::pair<void*, std::string> & p : pv) {
      if(p.second == typeid(int).name()) {
	delete (int*)p.first;
      }
      else if(p.second == typeid(double).name()) {
	delete (double*)p.first;
      }
    }
  }

  size_t size() const {return pv.size();}

  void push_back(int * i) {
    pv.push_back(std::pair<void *, std::string>(i, typeid(int).name()));
  }
  
  void push_back(double * d) {
    pv.push_back(std::pair<void *, std::string>(d, typeid(double).name()));
  }

  std::string const & get_type(size_t const s) const {
    return pv.at(s).second;
  }

  template <class T>
  T * at(size_t const s) const {
    if(typeid(T).name() != get_type(s)) {
      std::cout << __LINE__ << " " << __PRETTY_FUNCTION__ << "\n"
		<< "ERROR: wrong requested type\n";
      exit(1);
    }
    return (T*)pv.at(s).first;
  }

};



class tmva_helper {

  TFile * ifile_sp;
  TFile * ifile_bnb;
  TFile * ifile_bnb_cosmic;
  TFile * ifile_cosmic;
  
  TTree * vertex_tree_sp;
  TTree * vertex_tree_bnb;
  TTree * vertex_tree_bnb_cosmic;
  TTree * vertex_tree_cosmic;
  
  double pot_sp;
  double pot_sp_cosmic;
  double pot_bnb;
  double pot_bnb_cosmic;
  double ngenbnbcosmic;
  double ngencosmic;

  double run_pot;
  
  double weight_sp;
  double weight_bnb_cosmic;
  double weight_cosmic;

  std::vector<std::pair<std::string, std::string>> variable_v;
  std::vector<std::string> method_v;

public:

  tmva_helper(std::string dir) :
    ifile_sp(nullptr),
    ifile_bnb(nullptr),
    ifile_bnb_cosmic(nullptr),
    ifile_cosmic(nullptr),
    vertex_tree_sp(nullptr),
    vertex_tree_bnb(nullptr),
    vertex_tree_bnb_cosmic(nullptr),
    vertex_tree_cosmic(nullptr),
    pot_sp(-1),
    pot_sp_cosmic(-1),
    pot_bnb(-1),
    pot_bnb_cosmic(-1),
    ngenbnbcosmic(-1),
    ngencosmic(-1),
    run_pot(-1),
    weight_sp(-1),
    weight_bnb_cosmic(-1),
    weight_cosmic(-1) {



    if(dir != "") dir += "/";
    std::string gdir = "LEEPhoton/";



    ifile_sp = TFile::Open((dir+"runmv_sp.root").c_str());
    if(!ifile_sp) {
      std::cout << "Could not find file\n";
      exit(1);
    }
    vertex_tree_sp = (TTree*)ifile_sp->Get((gdir+"vertex_tree").c_str());  
    TTree * pot_tree_sp = (TTree*)ifile_sp->Get((gdir+"get_pot").c_str());
    pot_tree_sp->SetBranchAddress("pot", &pot_sp);
    pot_tree_sp->GetEntry(0);


  
    ifile_bnb_cosmic = TFile::Open((dir+"runmv_bnb_cosmic.root").c_str());
    if(!ifile_bnb_cosmic) {
      std::cout << "Could not find file\n";
      exit(1);
    }
    vertex_tree_bnb_cosmic = (TTree*)ifile_bnb_cosmic->Get((gdir+"vertex_tree").c_str());
    TTree * pot_tree_bnb_cosmic = (TTree*)ifile_bnb_cosmic->Get((gdir+"get_pot").c_str());
    pot_tree_bnb_cosmic->SetBranchAddress("pot", &pot_bnb_cosmic);
    {
      int temp_ngenbnbcosmic;
      pot_tree_bnb_cosmic->SetBranchAddress("number_of_events", &temp_ngenbnbcosmic);
      pot_tree_bnb_cosmic->GetEntry(0);
      ngenbnbcosmic = temp_ngenbnbcosmic;
    }

    ifile_cosmic = TFile::Open((dir+"runmv_cosmic.root").c_str());
    if(!ifile_cosmic) {
      std::cout << "Could not find file\n";
      exit(1);
    }
    vertex_tree_cosmic = (TTree*)ifile_cosmic->Get((gdir+"vertex_tree").c_str());
    TTree * pot_tree_cosmic = (TTree*)ifile_cosmic->Get((gdir+"get_pot").c_str());
    {
      int temp_ngencosmic;
      pot_tree_cosmic->SetBranchAddress("number_of_events", &temp_ngencosmic);
      pot_tree_cosmic->GetEntry(0);
      ngencosmic = temp_ngencosmic;
    }      

  }



  void runtmva() {
    
    TFile * runtmva_ofile = TFile::Open("runtmva.root", "recreate");

    TMVA::Factory * factory = new TMVA::Factory("runtmva",
					       runtmva_ofile,
					       "!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification" );
    TMVA::DataLoader * dataloader = new TMVA::DataLoader("dataset");

    {
      double const sp_weight = 1;
      double const bnb_cosmic_weight = 1;
      double const cosmic_weight = 1;
      
      dataloader->AddSignalTree(vertex_tree_sp, sp_weight);
      dataloader->AddBackgroundTree(vertex_tree_bnb_cosmic, bnb_cosmic_weight);
      dataloader->AddBackgroundTree(vertex_tree_cosmic, cosmic_weight);
    }

    variable_v.push_back({"closest_asso_shower_dist_to_flashzcenter", "d"});
    variable_v.push_back({"totalpe_ibg_sum", "d"});
    variable_v.push_back({"summed_associated_reco_shower_energy", "d"});
    variable_v.push_back({"reco_nu_vtx_dist_to_closest_tpc_wall", "d"});
    variable_v.push_back({"shortest_asso_shower_to_vert_dist", "d"});
    variable_v.push_back({"most_energetic_shower_reco_thetaxz", "d"});
    variable_v.push_back({"most_energetic_shower_reco_thetayz", "d"});
    variable_v.push_back({"longest_asso_track_thetaxz", "d"});
    variable_v.push_back({"longest_asso_track_thetayz", "d"});
    variable_v.push_back({"reco_asso_tracks", "i"});
    variable_v.push_back({"longest_asso_track_displacement", "d"});
    for(std::pair<std::string, std::string> const & p : variable_v) dataloader->AddVariable(p.first.c_str());

    TCut all_cut = "";
    TCut sig_cut = all_cut + "is_delta_rad == 1 && true_nu_vtx_fid_contained == 1";

    dataloader->PrepareTrainingAndTestTree(sig_cut, all_cut,
					   "nTrain_Signal="+std::to_string(vertex_tree_sp->GetEntries(sig_cut)/2)+":nTrain_Background="+std::to_string((vertex_tree_bnb_cosmic->GetEntries(all_cut)+vertex_tree_cosmic->GetEntries(all_cut))/2)+":SplitMode=Random:NormMode=NumEvents:!V");

    factory->BookMethod(dataloader, TMVA::Types::kBDT, "BDTG",
			"!H:!V:NTrees=1000:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=20:MaxDepth=2");
    method_v.push_back("BDTG");
    /*
    factory->BookMethod(dataloader, TMVA::Types::kBDT, "BDT",
			"!H:!V:NTrees=850:MinNodeSize=2.5%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=20");
    method_v.push_back("BDT");
    factory->BookMethod(dataloader, TMVA::Types::kBDT, "BDTB",
			"!H:!V:NTrees=400:BoostType=Bagging:SeparationType=GiniIndex:nCuts=20");
    method_v.push_back("BDTB");
    factory->BookMethod(dataloader, TMVA::Types::kBDT, "BDTD",
			"!H:!V:NTrees=400:MinNodeSize=5%:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:VarTransform=Decorrelate");
    method_v.push_back("BDTD");
    factory->BookMethod(dataloader, TMVA::Types::kBDT, "BDTF",
			"!H:!V:NTrees=50:MinNodeSize=2.5%:UseFisherCuts:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20");
    method_v.push_back("BDTF");
    factory->BookMethod(dataloader, TMVA::Types::kRuleFit, "RuleFit",
			"H:!V:RuleFitModule=RFTMVA:Model=ModRuleLinear:MinImp=0.001:RuleMinDist=0.001:NTrees=20:fEventsMin=0.01:fEventsMax=0.5:GDTau=-1.0:GDTauPrec=0.01:GDStep=0.01:GDNSteps=10000:GDErrScale=1.02");
    method_v.push_back("RuleFit");
*/

    factory->TrainAllMethods();
    factory->TestAllMethods();
    factory->EvaluateAllMethods();

    runtmva_ofile->Close();

    delete factory;
    delete dataloader;
    
  }



  void update(void_vec const & tvv, std::vector<float> & rvv) {

    if(tvv.size() != rvv.size()) {
      std::cout << __LINE__ << " " << __PRETTY_FUNCTION__ << "\n"
		<< "ERROR - tvv: " << tvv.size() << " and rvv: " << rvv.size() << " different sizes\n";
      exit(1);
    }

    for(size_t i = 0; i < tvv.size(); ++i) {
      if(tvv.get_type(i) == typeid(int).name()) {
	rvv.at(i) = *tvv.at<int>(i);
      }
      else if(tvv.get_type(i) == typeid(double).name()) {
	rvv.at(i) = *tvv.at<double>(i);
      }
    }

  }


  
  void runtmva_app_tree(TTree * tree) {

    TMVA::Reader * reader = new TMVA::Reader("!Color:!Silent");

    void_vec tree_var_v;
    std::vector<float> reader_var_v;
    for(std::pair<std::string, std::string> const & p : variable_v) {
      if(p.second == "d") {
	double * dp = new double(-1);
	tree_var_v.push_back(dp);
	tree->SetBranchAddress(p.first.c_str(), dp);
      }
      else if(p.second == "i") {
	int * ip = new int(-1);
	tree_var_v.push_back(ip);
	tree->SetBranchAddress(p.first.c_str(), ip);
      }
      reader_var_v.push_back(float(-1));
      reader->AddVariable(p.first.c_str(), &reader_var_v.back());
    }

    //edit
    
    for(int i = 0; i < tree->GetEntries(); ++i) {
      tree->GetEntry(i);
      update(tree_var_v, reader_var_v);
      

    }


    
    delete reader;

  }



  void runtmva_app() {

    TFile * runtmva_app_ofile = TFile::Open("runtmva_app.root", "recreate");

    runtmva_app_tree(vertex_tree_sp);
    //runtmva_app_tree(vertex_tree_bnb_cosmic);
    //runtmva_app_tree(vertex_tree_cosmic);

    runtmva_app_ofile->Close();

  }
  


};




int main(int const argc, char const * argv[]) {

  if(argc < 2) {
    std::cout << "Directory with runmv*root files required\n";
    return 1;
  }

  tmva_helper th(argv[1]);
  th.runtmva();
  th.runtmva_app();
  
  return 0;

}
