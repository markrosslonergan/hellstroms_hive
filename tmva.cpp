


#include <iostream>
#include <typeinfo>

#include "TFile.h"
#include "TTree.h"
#include "TTreeFormula.h"

#include "tmva/Types.h"
#include "tmva/DataLoader.h"
#include "tmva/Reader.h"



struct method {

  TMVA::Types::EMVA const menum;
  std::string const method_str;
  std::string const option;

  method(TMVA::Types::EMVA const a, std::string const & b, std::string const & c) :
    menum(a),
    method_str(b),
    option(c) {}

};



class void_vec {

  std::vector<std::pair<void *, std::string>> pv;
  
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

  void print() {
    for(std::pair<void *, std::string> const & p : pv) {
      if(p.second == typeid(int).name()) {
	std::cout << *(int*)p.first << "\n";
      }
      else if(p.second == typeid(double).name()) {
	std::cout << *(double*)p.first << "\n";
      }
    }
  }

};



struct runtmva_app_tree_struct {

  TTree * tree;
  int is_delta_rad;
  int true_nu_vtx_fid_contained;
  int reco_asso_tracks;
  double mva;

  runtmva_app_tree_struct(std::string const & tree_name) {
    tree = new TTree(tree_name.c_str(), "");
    tree->Branch("is_delta_rad", &is_delta_rad, "is_delta_rad/I");
    tree->Branch("true_nu_vtx_fid_contained", &true_nu_vtx_fid_contained, "true_nu_vtx_fid_contained/I");
    tree->Branch("reco_asso_tracks", &reco_asso_tracks, "reco_asso_tracks/I");
    tree->Branch("mva", &mva, "mva/D");
  }

  ~runtmva_app_tree_struct() {
    delete tree;
  }

};



class tmva_helper {

  std::string const method_name;
  std::string const signal_definition;
  std::string const sample_selection;
  std::string and_sample_selection;

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

  std::vector<std::pair<std::string, std::string>> variable_v;
  std::vector<method> method_v;



  void update(void_vec const & tvv, std::vector<float *> & rvv) {

    for(size_t i = 0; i < tvv.size(); ++i) {
      if(tvv.get_type(i) == typeid(int).name()) {
	*rvv.at(i) = *tvv.at<int>(i);
      }
      else if(tvv.get_type(i) == typeid(double).name()) {
	*rvv.at(i) = *tvv.at<double>(i);
      }
    }

  }



  void runtmva_app_tree(TTree * tree, std::string const & otree_name) {

    TMVA::Reader * reader = new TMVA::Reader("!Color:!Silent");

    void_vec tree_var_v;
    std::vector<float *> reader_var_v;
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
      else {
	std::cout << __LINE__ << " " << __PRETTY_FUNCTION__ <<"\n"
		  << "ERROR: invalid type: " << p.second << "\n";
      }
      reader_var_v.push_back(new float(-1));
      reader->AddVariable(p.first.c_str(), reader_var_v.back());
    }
  
    int is_delta_rad;
    int true_nu_vtx_fid_contained;
    int reco_asso_tracks;
    tree->SetBranchAddress("is_delta_rad", &is_delta_rad);
    tree->SetBranchAddress("true_nu_vtx_fid_contained", &true_nu_vtx_fid_contained);
    tree->SetBranchAddress("reco_asso_tracks", &reco_asso_tracks);

    TTreeFormula * tf = new TTreeFormula("tf", sample_selection.c_str(), tree);

    for(method const & m : method_v) {
      reader->BookMVA(m.method_str.c_str(), ("dataset/weights/"+method_name+"_"+m.method_str+".weights.xml").c_str());
      runtmva_app_tree_struct ts(otree_name+"_"+m.method_str);
      for(int i = 0; i < tree->GetEntries(); ++i) {
	tree->GetEntry(i);
	if(!tf->EvalInstance()) continue;
	update(tree_var_v, reader_var_v);
	ts.is_delta_rad = is_delta_rad;
	ts.true_nu_vtx_fid_contained = true_nu_vtx_fid_contained;
	ts.reco_asso_tracks = reco_asso_tracks;
	ts.mva = reader->EvaluateMVA(m.method_str.c_str());
	if(ts.mva != -999) ts.tree->Fill();
      }
      ts.tree->Write();
    }

    delete reader;
    delete tf;
    for(float * f : reader_var_v) delete f;

  }



public:



  tmva_helper(std::string const & name, std::string dir, std::string const & selection = "") :
    method_name(name),
    signal_definition("is_delta_rad == 1 && true_nu_vtx_fid_contained == 1"),
    sample_selection(selection),
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
    ngencosmic(-1) {


    
    if(sample_selection != "") and_sample_selection = "&&"+sample_selection;
    else and_sample_selection = "";



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



  void add_variable(std::string const & var, std::string const & type) {
    variable_v.push_back({var, type});
  }


  
  void add_method(TMVA::Types::EMVA const emva, std::string const & method, std::string const & option) {
    method_v.push_back({emva, method, option});
  }



  void runtmva() {
    
    TFile * runtmva_ofile = TFile::Open((method_name+".root").c_str(), "recreate");

    TMVA::Factory * factory = new TMVA::Factory(method_name.c_str(),
						runtmva_ofile,
						"!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification");
    TMVA::DataLoader * dataloader = new TMVA::DataLoader("dataset");

    {
      double const sp_weight = 1;
      double const bnb_cosmic_weight = 1;
      double const cosmic_weight = 1;
      
      dataloader->AddSignalTree(vertex_tree_sp, sp_weight);
      dataloader->AddBackgroundTree(vertex_tree_bnb_cosmic, bnb_cosmic_weight);
      dataloader->AddBackgroundTree(vertex_tree_cosmic, cosmic_weight);
    }

    for(std::pair<std::string, std::string> const & p : variable_v) dataloader->AddVariable(p.first.c_str());

    TCut all_cut = sample_selection.c_str();
    TCut sig_cut = all_cut + TCut(signal_definition.c_str());

    dataloader->PrepareTrainingAndTestTree(sig_cut, all_cut,
					   "nTrain_Signal="+std::to_string(vertex_tree_sp->GetEntries(sig_cut)/2)+":nTrain_Background="+std::to_string((vertex_tree_bnb_cosmic->GetEntries(all_cut)+vertex_tree_cosmic->GetEntries(all_cut))/2)+":SplitMode=Random:NormMode=NumEvents:!V");

    for(method const & m : method_v) {
      factory->BookMethod(dataloader, m.menum, m.method_str, m.option);
    }

    factory->TrainAllMethods();
    factory->TestAllMethods();
    factory->EvaluateAllMethods();

    runtmva_ofile->Close();

    delete factory;
    delete dataloader;
    
  }



  void runtmva_app() {

    TFile * runtmva_app_ofile = TFile::Open((method_name+"_app.root").c_str(), "recreate");

    runtmva_app_tree(vertex_tree_sp, "sp");
    runtmva_app_tree(vertex_tree_bnb_cosmic, "bnbcosmic");
    runtmva_app_tree(vertex_tree_cosmic, "cosmic");

    runtmva_app_ofile->Close();

  }



  void runtmva_sig(std::string const & method_str, double const run_pot) {

    TFile * file = TFile::Open((method_name+"_app.root").c_str());

    TTree * tree_sp = (TTree*)file->Get(("sp_"+method_str).c_str()); 
    TTree * tree_bnb_cosmic = (TTree*)file->Get(("bnbcosmic_"+method_str).c_str());
    TTree * tree_cosmic = (TTree*)file->Get(("cosmic_"+method_str).c_str());

    int total_sp = vertex_tree_sp->GetEntries((signal_definition).c_str());
    int total_bnb_cosmic = vertex_tree_bnb_cosmic->GetEntries();
    int total_cosmic = vertex_tree_cosmic->GetEntries();
    double total_scaled_sp = total_sp * run_pot / pot_sp;
    double total_scaled_bnb_cosmic = total_bnb_cosmic * run_pot / pot_bnb_cosmic;
    double total_scaled_cosmic = total_cosmic * run_pot * ngenbnbcosmic / ngencosmic * 10.729 / pot_bnb_cosmic;
    
    double largest_significance = 0;
    int isp = 0;
    int ibc = 0;
    int ic = 0;
    double lsp = 0;
    double lbc = 0;
    double lc = 0;
    
    std::cout << method_str << "\n";

    for(double d = -1; d < 1; d += 0.05) {
    
      int sp = tree_sp->GetEntries(("mva > "+std::to_string(d)+"&&"+signal_definition).c_str());
      int bnb_cosmic = tree_bnb_cosmic->GetEntries(("mva > "+std::to_string(d)).c_str());
      int cosmic = tree_cosmic->GetEntries(("mva > "+std::to_string(d)).c_str());
      double scaled_sp = sp * run_pot / pot_sp;
      double scaled_bnb_cosmic =  bnb_cosmic * run_pot / pot_bnb_cosmic;
      double scaled_cosmic = cosmic * run_pot * ngenbnbcosmic / ngencosmic * 10.729 / pot_bnb_cosmic;
      
      if(scaled_bnb_cosmic+scaled_cosmic) {
	if(scaled_sp / sqrt(scaled_bnb_cosmic+scaled_cosmic) > largest_significance) {
	  largest_significance = scaled_sp / sqrt(scaled_bnb_cosmic+scaled_cosmic);
	  isp = sp;
	  lsp = scaled_sp;
	  ibc = bnb_cosmic;
	  lbc = scaled_bnb_cosmic;
	  ic = cosmic;
	  lc = scaled_cosmic;
	} 
      }
      else if(scaled_sp) {
	std::cout << "Background eliminated scaled signal: " << scaled_sp << " efficiency: " << scaled_sp / total_scaled_sp * 100 << " %\n";
      }
      
    }
    
    std::cout << "total - sp: " << total_sp << " scaled: " << total_scaled_sp << "\n"
	      << "        bnb_cosmic: " << total_bnb_cosmic << " scaled: " << total_scaled_bnb_cosmic << "\n"
	      << "        cosmic: " << total_cosmic << " scaled: " << total_scaled_cosmic << "\n"
	      << "after - sp: " << isp << " scaled: " << lsp << "\n"
	      << "        bnb_cosmic: " << ibc << " scaled: " << lbc << "\n"
	      << "        cosmic: " << ic << " scaled: " << lc << "\n"
	      << "largest significance: " << largest_significance << "\n"
	      << "seff: " << lsp / total_scaled_sp * 100 << " % beff: " << (lbc + lc) / (total_scaled_bnb_cosmic + total_scaled_cosmic) * 100 << " %\n\n";
    
    file->Close();

  }
  


  void runtmva_sig_all(double const run_pot) {

    for(method const & m : method_v) {
      runtmva_sig(m.method_str, run_pot);
    }

  }



  void runtmva_sig(std::string const & fname1, std::string const & cut1, std::string const & fname2, std::string const & cut2, std::string const & method_str, double const run_pot) {

    std::string andcut1 = "";
    if(cut1 != "") andcut1 = "&&"+cut1;
    std::string andcut2 = "";
    if(cut2 != "") andcut2 = "&&"+cut2;

    TFile * file1 = TFile::Open(fname1.c_str());
    TFile * file2 = TFile::Open(fname2.c_str());

    TTree * tree_sp1 = (TTree*)file1->Get(("sp_"+method_str).c_str());
    TTree * tree_bnb_cosmic1 = (TTree*)file1->Get(("bnbcosmic_"+method_str).c_str());
    TTree * tree_cosmic1 = (TTree*)file1->Get(("cosmic_"+method_str).c_str());

    TTree * tree_sp2 = (TTree*)file2->Get(("sp_"+method_str).c_str()); 
    TTree * tree_bnb_cosmic2 = (TTree*)file2->Get(("bnbcosmic_"+method_str).c_str());
    TTree * tree_cosmic2 = (TTree*)file2->Get(("cosmic_"+method_str).c_str());

    int total_sp1 = vertex_tree_sp->GetEntries((signal_definition+andcut1).c_str());
    int total_bnb_cosmic1 = vertex_tree_bnb_cosmic->GetEntries(cut1.c_str());
    int total_cosmic1 = vertex_tree_cosmic->GetEntries(cut1.c_str());
    double total_scaled_sp1 = total_sp1 * run_pot / pot_sp;
    double total_scaled_bnb_cosmic1 = total_bnb_cosmic1 * run_pot / pot_bnb_cosmic;
    double total_scaled_cosmic1 = total_cosmic1 * run_pot * ngenbnbcosmic / ngencosmic * 10.729 / pot_bnb_cosmic;

    int total_sp2 = vertex_tree_sp->GetEntries((signal_definition+andcut2).c_str());
    int total_bnb_cosmic2 = vertex_tree_bnb_cosmic->GetEntries(cut2.c_str());
    int total_cosmic2 = vertex_tree_cosmic->GetEntries(cut2.c_str());
    double total_scaled_sp2 = total_sp2 * run_pot / pot_sp;
    double total_scaled_bnb_cosmic2 = total_bnb_cosmic2 * run_pot / pot_bnb_cosmic;
    double total_scaled_cosmic2 = total_cosmic2 * run_pot * ngenbnbcosmic / ngencosmic * 10.729 / pot_bnb_cosmic;
    
    double largest_significance1 = 0;
    int isp1 = 0;
    int ibc1 = 0;
    int ic1 = 0;
    double lsp1 = 0;
    double lbc1 = 0;
    double lc1 = 0;

    double largest_significance2 = 0;
    int isp2 = 0;
    int ibc2 = 0;
    int ic2 = 0;
    double lsp2 = 0;
    double lbc2 = 0;
    double lc2 = 0;
    
    std::cout << method_str << "\n";
    
    for(double d = -1; d < 1; d += 0.05) {
    
      int sp = tree_sp1->GetEntries(("mva > "+std::to_string(d)+"&&"+signal_definition).c_str());
      int bnb_cosmic = tree_bnb_cosmic1->GetEntries(("mva > "+std::to_string(d)).c_str());
      int cosmic = tree_cosmic1->GetEntries(("mva > "+std::to_string(d)).c_str());
      double scaled_sp = sp * run_pot / pot_sp;
      double scaled_bnb_cosmic =  bnb_cosmic * run_pot / pot_bnb_cosmic;
      double scaled_cosmic = cosmic * run_pot * ngenbnbcosmic / ngencosmic * 10.729 / pot_bnb_cosmic;
      
      if(scaled_bnb_cosmic+scaled_cosmic) {
	if(scaled_sp / sqrt(scaled_bnb_cosmic+scaled_cosmic) > largest_significance1) {
	  largest_significance1 = scaled_sp / sqrt(scaled_bnb_cosmic+scaled_cosmic);
	  isp1 = sp;
	  lsp1 = scaled_sp;
	  ibc1 = bnb_cosmic;
	  lbc1 = scaled_bnb_cosmic;
	  ic1 = cosmic;
	  lc1 = scaled_cosmic;
	} 
      }
      else if(scaled_sp) {
	std::cout << "Background eliminated scaled signal: " << scaled_sp << " efficiency: " << scaled_sp / total_scaled_sp1 * 100 << " %\n";
      }
      
    }

    for(double d = -1; d < 1; d += 0.05) {
    
      int sp = tree_sp2->GetEntries(("mva > "+std::to_string(d)+"&&"+signal_definition).c_str());
      int bnb_cosmic = tree_bnb_cosmic2->GetEntries(("mva > "+std::to_string(d)).c_str());
      int cosmic = tree_cosmic2->GetEntries(("mva > "+std::to_string(d)).c_str());
      double scaled_sp = sp * run_pot / pot_sp;
      double scaled_bnb_cosmic =  bnb_cosmic * run_pot / pot_bnb_cosmic;
      double scaled_cosmic = cosmic * run_pot * ngenbnbcosmic / ngencosmic * 10.729 / pot_bnb_cosmic;
      
      if(scaled_bnb_cosmic+scaled_cosmic) {
	if(scaled_sp / sqrt(scaled_bnb_cosmic+scaled_cosmic) > largest_significance2) {
	  largest_significance2 = scaled_sp / sqrt(scaled_bnb_cosmic+scaled_cosmic);
	  isp2 = sp;
	  lsp2 = scaled_sp;
	  ibc2 = bnb_cosmic;
	  lbc2 = scaled_bnb_cosmic;
	  ic2 = cosmic;
	  lc2 = scaled_cosmic;
	} 
      }
      else if(scaled_sp) {
	std::cout << "Background eliminated scaled signal: " << scaled_sp << " efficiency: " << scaled_sp / total_scaled_sp2 * 100 << " %\n";
      }
      
    }
    
    std::cout << "total - sp: " << total_sp1 + total_sp2 << " scaled: " << total_scaled_sp1 + total_scaled_sp2 << "\n"
	      << "        bnb_cosmic: " << total_bnb_cosmic1 + total_bnb_cosmic2 << " scaled: " << total_scaled_bnb_cosmic1 + total_scaled_bnb_cosmic2 << "\n"
	      << "        cosmic: " << total_cosmic1 + total_cosmic2 << " scaled: " << total_scaled_cosmic1 + total_scaled_cosmic2 << "\n"
	      << "after - sp: " << isp1 + isp2 << " scaled: " << lsp1 + lsp2 << "\n"
	      << "        bnb_cosmic: " << ibc1 + ibc2 << " scaled: " << lbc1 + lbc2 << "\n"
	      << "        cosmic: " << ic1 + ic2 << " scaled: " << lc1 + lc2 << "\n"
	      << "largest significance: " << (lsp1+lsp2) / sqrt(lbc1+lbc2+lc1+lc2) << "\n"
	      << "seff: " << (lsp1+lsp2) / (total_scaled_sp1+total_scaled_sp2) * 100 << " % beff: " << (lbc1 + lbc2 + lc1 + lc2) / (total_scaled_bnb_cosmic1 + total_scaled_bnb_cosmic2 + total_scaled_cosmic1 + total_scaled_cosmic2) * 100 << " %\n\n";
 
    file1->Close();
    file2->Close();

  }


  
  void runtmva_sig_all_twofiles(std::string const & fname1, std::string const & cut1, std::string const & fname2, std::string const & cut2, double const run_pot) {
    
    for(method const & m : method_v) {
      runtmva_sig(fname1, cut1, fname2, cut2, m.method_str, run_pot);
    }

  }



};



void run(std::string const & dir, std::string const option) {

  tmva_helper th("runtmva", dir, "passed_swtrigger == 1");
  
  th.add_variable("closest_asso_shower_dist_to_flashzcenter", "d");
  th.add_variable("totalpe_ibg_sum", "d");
  th.add_variable("summed_associated_reco_shower_energy", "d");
  th.add_variable("reco_nu_vtx_dist_to_closest_tpc_wall", "d");
  th.add_variable("shortest_asso_shower_to_vert_dist", "d");
  th.add_variable("most_energetic_shower_reco_thetaxz", "d");
  th.add_variable("most_energetic_shower_reco_thetayz", "d");
  th.add_variable("most_energetic_shower_bp_dist_to_tpc", "d");
  th.add_variable("longest_asso_track_thetaxz", "d");
  th.add_variable("longest_asso_track_thetayz", "d");
  th.add_variable("reco_asso_tracks", "i");
  th.add_variable("longest_asso_track_displacement", "d");
  
  th.add_method(TMVA::Types::kBDT, "BDTG",
		"!H:!V:NTrees=1000:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=20:MaxDepth=2");
  th.add_method(TMVA::Types::kBDT, "BDT",
		"!H:!V:NTrees=850:MinNodeSize=2.5%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=20");
  th.add_method(TMVA::Types::kBDT, "BDTB",
		"!H:!V:NTrees=400:BoostType=Bagging:SeparationType=GiniIndex:nCuts=20");
  th.add_method(TMVA::Types::kBDT, "BDTD",
		"!H:!V:NTrees=400:MinNodeSize=5%:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:VarTransform=Decorrelate");
  th.add_method(TMVA::Types::kBDT, "BDTF",
		"!H:!V:NTrees=50:MinNodeSize=2.5%:UseFisherCuts:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20");
  th.add_method(TMVA::Types::kRuleFit, "RuleFit",
		"H:!V:RuleFitModule=RFTMVA:Model=ModRuleLinear:MinImp=0.001:RuleMinDist=0.001:NTrees=20:fEventsMin=0.01:fEventsMax=0.5:GDTau=-1.0:GDTauPrec=0.01:GDStep=0.01:GDNSteps=10000:GDErrScale=1.02");

  double const run_pot = 6.6e20;
    
  if(option == "all") {
    th.runtmva();
    th.runtmva_app();
    th.runtmva_sig_all(run_pot);
  }
  else if(option == "app") {
    th.runtmva_app();
    th.runtmva_sig_all(run_pot);
  }
  else if(option == "sig") {
    th.runtmva_sig_all(run_pot);
  }
  else {
    std::cout << "Invalid option\n";
    return;
  }

}



void run_split_track(std::string const & dir, std::string const option) {
  
  std::string const thg0_cut = "reco_asso_tracks > 0 && passed_swtrigger";
  tmva_helper thg0("runtmva_g0track", dir, thg0_cut);
  
  thg0.add_variable("closest_asso_shower_dist_to_flashzcenter", "d");
  thg0.add_variable("totalpe_ibg_sum", "d");
  thg0.add_variable("summed_associated_reco_shower_energy", "d");
  thg0.add_variable("reco_nu_vtx_dist_to_closest_tpc_wall", "d");
  thg0.add_variable("shortest_asso_shower_to_vert_dist", "d");
  thg0.add_variable("most_energetic_shower_reco_thetaxz", "d");
  thg0.add_variable("most_energetic_shower_reco_thetayz", "d");
  thg0.add_variable("most_energetic_shower_bp_dist_to_tpc", "d");
  thg0.add_variable("longest_asso_track_thetaxz", "d");
  thg0.add_variable("longest_asso_track_thetayz", "d");
  thg0.add_variable("reco_asso_tracks", "i");
  thg0.add_variable("longest_asso_track_displacement", "d");
  
  thg0.add_method(TMVA::Types::kBDT, "BDTG",
		"!H:!V:NTrees=1000:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=20:MaxDepth=2");
  thg0.add_method(TMVA::Types::kBDT, "BDT",
		"!H:!V:NTrees=850:MinNodeSize=2.5%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=20");
  thg0.add_method(TMVA::Types::kBDT, "BDTB",
		"!H:!V:NTrees=400:BoostType=Bagging:SeparationType=GiniIndex:nCuts=20");
  thg0.add_method(TMVA::Types::kBDT, "BDTD",
		"!H:!V:NTrees=400:MinNodeSize=5%:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:VarTransform=Decorrelate");
  thg0.add_method(TMVA::Types::kBDT, "BDTF",
		"!H:!V:NTrees=50:MinNodeSize=2.5%:UseFisherCuts:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20");
  thg0.add_method(TMVA::Types::kRuleFit, "RuleFit",
		"H:!V:RuleFitModule=RFTMVA:Model=ModRuleLinear:MinImp=0.001:RuleMinDist=0.001:NTrees=20:fEventsMin=0.01:fEventsMax=0.5:GDTau=-1.0:GDTauPrec=0.01:GDStep=0.01:GDNSteps=10000:GDErrScale=1.02");

  std::string const th0_cut = "reco_asso_tracks == 0 && passed_swtrigger";
  tmva_helper th0("runtmva_0track", dir, th0_cut);
  
  th0.add_variable("closest_asso_shower_dist_to_flashzcenter", "d");
  th0.add_variable("totalpe_ibg_sum", "d");
  th0.add_variable("summed_associated_reco_shower_energy", "d");
  th0.add_variable("reco_nu_vtx_dist_to_closest_tpc_wall", "d");
  th0.add_variable("shortest_asso_shower_to_vert_dist", "d");
  th0.add_variable("most_energetic_shower_reco_thetaxz", "d");
  th0.add_variable("most_energetic_shower_reco_thetayz", "d");
  th0.add_variable("most_energetic_shower_bp_dist_to_tpc", "d");
  
  th0.add_method(TMVA::Types::kBDT, "BDTG",
		"!H:!V:NTrees=1000:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=20:MaxDepth=2");
  th0.add_method(TMVA::Types::kBDT, "BDT",
		"!H:!V:NTrees=850:MinNodeSize=2.5%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=20");
  th0.add_method(TMVA::Types::kBDT, "BDTB",
		"!H:!V:NTrees=400:BoostType=Bagging:SeparationType=GiniIndex:nCuts=20");
  th0.add_method(TMVA::Types::kBDT, "BDTD",
		"!H:!V:NTrees=400:MinNodeSize=5%:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:VarTransform=Decorrelate");
  th0.add_method(TMVA::Types::kBDT, "BDTF",
		"!H:!V:NTrees=50:MinNodeSize=2.5%:UseFisherCuts:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20");
  th0.add_method(TMVA::Types::kRuleFit, "RuleFit",
		"H:!V:RuleFitModule=RFTMVA:Model=ModRuleLinear:MinImp=0.001:RuleMinDist=0.001:NTrees=20:fEventsMin=0.01:fEventsMax=0.5:GDTau=-1.0:GDTauPrec=0.01:GDStep=0.01:GDNSteps=10000:GDErrScale=1.02");

  double const run_pot = 6.6e20;
    
  if(option == "all") {
    thg0.runtmva();
    thg0.runtmva_app();
    th0.runtmva();
    th0.runtmva_app();
    thg0.runtmva_sig_all_twofiles("runtmva_g0track_app.root", thg0_cut, "runtmva_0track_app.root", th0_cut, run_pot);
  }
  else if(option == "app") {
    thg0.runtmva_app();
    th0.runtmva_app();
    thg0.runtmva_sig_all_twofiles("runtmva_g0track_app.root", thg0_cut, "runtmva_0track_app.root", th0_cut, run_pot);
  }
  else if(option == "sig") {
    thg0.runtmva_sig_all_twofiles("runtmva_g0track_app.root", thg0_cut, "runtmva_0track_app.root", th0_cut, run_pot);
  }
  else {
    std::cout << "Invalid option\n";
    return;
  }

}



int main(int const argc, char const * argv[]) {

  if(argc < 3) {
    std::cout << "Usage: option (\"all\", \"app\", \"sig\"), directory with runmv*root files\n";
    return 1;
  }

  run(argv[2], argv[1]);
  run_split_track(argv[2], argv[1]);

  return 0;

}
