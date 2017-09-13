


#include <iostream>
#include <typeinfo>

#include "TFile.h"
#include "TTree.h"
#include "TTreeFormula.h"
#include "TFriendElement.h"

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

  runtmva_app_tree_struct(std::string const & tree_name, bool const extra = true) {
    tree = new TTree(tree_name.c_str(), "");
    if(extra) {
      tree->Branch("is_delta_rad", &is_delta_rad, "is_delta_rad/I");
      tree->Branch("true_nu_vtx_fid_contained", &true_nu_vtx_fid_contained, "true_nu_vtx_fid_contained/I");
      tree->Branch("reco_asso_tracks", &reco_asso_tracks, "reco_asso_tracks/I");
    }
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
  bool separate_backgrounds;
  bool single_background;
  bool verbose;

  TFile * ifile_sp;
  TFile * ifile_sp_cosmic;
  TFile * ifile_bnb_cosmic;
  TFile * ifile_cosmic;
  
  TTree * vertex_tree_sp;
  TTree * vertex_tree_sp_cosmic;
  TTree * vertex_tree_bnb_cosmic;
  TTree * vertex_tree_cosmic;
  
  double pot_sp;
  double pot_sp_cosmic;
  double pot_bnb;
  double pot_bnb_cosmic;
  double ngenbnbcosmic;
  double ngencosmic;

  double const run_pot;

  std::vector<std::pair<std::string, std::string>> variable_v;
  std::vector<method> method_v;

  std::vector<double> test_v;

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



  void runtmva_app_tree(TTree * tree, std::string const & otree_name, std::string background = "") {

    if(background != "") background += "_";
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

    TTreeFormula * tf = new TTreeFormula("tf", sample_selection.c_str(), tree);

    for(method const & m : method_v) {
      reader->BookMVA(m.method_str.c_str(), ("dataset/weights/"+method_name+"_"+background+m.method_str+".weights.xml").c_str());
      runtmva_app_tree_struct ts(otree_name+"_"+background+m.method_str, false);
      for(int i = 0; i < tree->GetEntries(); ++i) {
	tree->GetEntry(i);
	update(tree_var_v, reader_var_v);
	ts.mva = -999;
	if(tf->EvalInstance()) ts.mva = reader->EvaluateMVA(m.method_str.c_str());
	test_v.push_back(ts.mva);
	ts.tree->Fill();
      }
      ts.tree->Write();
    }

    delete reader;
    delete tf;
    for(float * f : reader_var_v) delete f;

  }



public:



  tmva_helper(std::string const & name, std::string dir, std::string const & selection = "", double const pot = 1) :
    method_name(name),
    signal_definition("is_delta_rad == 1 && true_nu_vtx_fid_contained == 1"),
    sample_selection(selection),
    separate_backgrounds(false),
    single_background(false),
    verbose(true),
    ifile_sp(nullptr),
    ifile_sp_cosmic(nullptr),
    ifile_bnb_cosmic(nullptr),
    ifile_cosmic(nullptr),
    vertex_tree_sp(nullptr),
    vertex_tree_sp_cosmic(nullptr),
    vertex_tree_bnb_cosmic(nullptr),
    vertex_tree_cosmic(nullptr),
    pot_sp(-1),
    pot_sp_cosmic(-1),
    pot_bnb(-1),
    pot_bnb_cosmic(-1),
    run_pot(pot),
    ngenbnbcosmic(-1),
    ngencosmic(-1) {



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



    ifile_sp_cosmic = TFile::Open((dir+"runmv_sp_cosmic.root").c_str());
    if(!ifile_sp_cosmic) {
      std::cout << "Could not find file\n";
      exit(1);
    }
    vertex_tree_sp_cosmic = (TTree*)ifile_sp_cosmic->Get((gdir+"vertex_tree").c_str());
    TTree * pot_tree_sp_cosmic = (TTree*)ifile_sp_cosmic->Get((gdir+"get_pot").c_str());
    pot_tree_sp_cosmic->SetBranchAddress("pot", &pot_sp_cosmic);
    pot_tree_sp_cosmic->GetEntry(0);


  
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
      std::cout << "No in-time cosmic file\n";
    }
    else {
      vertex_tree_cosmic = (TTree*)ifile_cosmic->Get((gdir+"vertex_tree").c_str());
      TTree * pot_tree_cosmic = (TTree*)ifile_cosmic->Get((gdir+"get_pot").c_str());
      {
	int temp_ngencosmic;
	pot_tree_cosmic->SetBranchAddress("number_of_events", &temp_ngencosmic);
	pot_tree_cosmic->GetEntry(0);
	ngencosmic = temp_ngencosmic;
      }      
    }

  }



  void set_separate_backgrounds(bool const b = true) {separate_backgrounds = b;}
  void set_single_background(bool const b = true) {single_background = b;}



  void add_variable(std::string const & var, std::string const & type) {
    variable_v.push_back({var, type});
  }


  
  void add_method(TMVA::Types::EMVA const emva, std::string const & method, std::string const & option) {
    method_v.push_back({emva, method, option});
  }



  void add_method(method const & m) {
    method_v.push_back(m);
  }



  void runtmva(bool const weight, std::string const background = "") {
    
    std::string name = method_name;
    if(background != "") name = method_name+"_"+background;
    TFile * runtmva_ofile = TFile::Open((name+".root").c_str(), "recreate");

    TMVA::Factory * factory = new TMVA::Factory(name.c_str(),
						runtmva_ofile,
						"!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification");
    TMVA::DataLoader * dataloader = new TMVA::DataLoader("dataset");

    {

      double sp_weight = 1;
      double bnb_cosmic_weight = 1;
      double cosmic_weight = 1;

      if(weight) {
	sp_weight = run_pot / pot_sp;
	bnb_cosmic_weight = run_pot / pot_bnb_cosmic;
	cosmic_weight = run_pot * ngenbnbcosmic / ngencosmic * 10.729 / pot_bnb_cosmic;
      }

      dataloader->AddSignalTree(vertex_tree_sp, sp_weight);
      if(background == "" || background == "bnb_cosmic") dataloader->AddBackgroundTree(vertex_tree_bnb_cosmic, bnb_cosmic_weight);
      if(background == "" || background == "cosmic") dataloader->AddBackgroundTree(vertex_tree_cosmic, cosmic_weight);

    }

    TCut all_cut = sample_selection.c_str();
    TCut sig_cut = all_cut + TCut(signal_definition.c_str());

    double background_entries = -1;
    if(background == "bnb_cosmic") background_entries = vertex_tree_bnb_cosmic->GetEntries(all_cut);
    else if(background == "cosmic") background_entries = vertex_tree_cosmic->GetEntries(all_cut);
    else if(background == "") background_entries = vertex_tree_bnb_cosmic->GetEntries(all_cut)+vertex_tree_cosmic->GetEntries(all_cut);
    background_entries /= 2;

    for(std::pair<std::string, std::string> const & p : variable_v) dataloader->AddVariable(p.first.c_str());

    dataloader->PrepareTrainingAndTestTree(sig_cut, all_cut,
					   "nTrain_Signal="+std::to_string(vertex_tree_sp->GetEntries(sig_cut)/2)+":nTrain_Background="+std::to_string(int(background_entries))+":SplitMode=Random:NormMode=NumEvents:!V");

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

    if(separate_backgrounds) {
      runtmva_app_tree(vertex_tree_sp, method_name+"_sp", "bnb_cosmic");
      runtmva_app_tree(vertex_tree_sp, method_name+"_sp", "cosmic");
      runtmva_app_tree(vertex_tree_sp_cosmic, method_name+"_sp_cosmic", "bnb_cosmic");
      runtmva_app_tree(vertex_tree_sp_cosmic, method_name+"_sp_cosmic", "cosmic");
      runtmva_app_tree(vertex_tree_bnb_cosmic, method_name+"_bnb_cosmic", "bnb_cosmic");
      runtmva_app_tree(vertex_tree_bnb_cosmic, method_name+"_bnb_cosmic", "cosmic");
      runtmva_app_tree(vertex_tree_cosmic, method_name+"_cosmic", "bnb_cosmic");
      runtmva_app_tree(vertex_tree_cosmic, method_name+"_cosmic", "cosmic");
    }
    else if(single_background) {
      runtmva_app_tree(vertex_tree_sp, method_name+"_sp", "bnb_cosmic");
      runtmva_app_tree(vertex_tree_sp_cosmic, method_name+"_sp_cosmic", "bnb_cosmic");
      runtmva_app_tree(vertex_tree_bnb_cosmic, method_name+"_bnb_cosmic", "bnb_cosmic");
    }
    else {
      runtmva_app_tree(vertex_tree_sp, method_name+"_sp");
      runtmva_app_tree(vertex_tree_sp_cosmic, method_name+"_sp_cosmic");
      runtmva_app_tree(vertex_tree_bnb_cosmic, method_name+"_bnb_cosmic");
      runtmva_app_tree(vertex_tree_cosmic, method_name+"_cosmic");
    } 

    runtmva_app_ofile->Close();

  }



  double runtmva_sig(std::string const & method_str, std::string background = "") {
    
    if(background != "") background += "_";

    std::cout << "---------------------------------------------\n";

    std::string const name_sp = method_name+"_sp_"+background+method_str;
    std::string const name_bnb_cosmic = method_name+"_bnb_cosmic_"+background+method_str;
    std::string const name_cosmic = method_name+"_cosmic_"+background+method_str;

    vertex_tree_sp->AddFriend(name_sp.c_str());
    vertex_tree_bnb_cosmic->AddFriend(name_bnb_cosmic.c_str());
    vertex_tree_cosmic->AddFriend(name_cosmic.c_str());

    int total_sp = vertex_tree_sp->GetEntries((signal_definition).c_str());
    int total_bnb_cosmic = vertex_tree_bnb_cosmic->GetEntries();
    int total_cosmic = vertex_tree_cosmic->GetEntries();
    double total_scaled_sp = total_sp * run_pot / pot_sp;
    double total_scaled_bnb_cosmic = total_bnb_cosmic * run_pot / pot_bnb_cosmic;
    double total_scaled_cosmic = total_cosmic * run_pot * ngenbnbcosmic / ngencosmic * 10.729 / pot_bnb_cosmic;

    double mva = -2;    
    double largest_significance = 0;
    int isp = 0;
    int ibc = 0;
    int ic = 0;
    double lsp = 0;
    double lbc = 0;
    double lc = 0;
    
    std::cout << method_str << "\n";

    double br_sig = 0;
    for(double d = -1; d <= 1; d += 0.05) {

      int sp = vertex_tree_sp->GetEntries((name_sp+".mva > "+std::to_string(d)+"&&"+signal_definition).c_str());
      int bnb_cosmic = vertex_tree_bnb_cosmic->GetEntries((name_bnb_cosmic+".mva > "+std::to_string(d)).c_str());
      int cosmic = vertex_tree_cosmic->GetEntries((name_cosmic+".mva > "+std::to_string(d)).c_str());

      double scaled_sp = sp * run_pot / pot_sp;
      double scaled_bnb_cosmic =  bnb_cosmic * run_pot / pot_bnb_cosmic;
      double scaled_cosmic = cosmic * run_pot * ngenbnbcosmic / ngencosmic * 10.729 / pot_bnb_cosmic;
      
      if(scaled_bnb_cosmic+scaled_cosmic) {
	if(scaled_sp / sqrt(scaled_bnb_cosmic+scaled_cosmic) > largest_significance) {
	  mva = d;
	  largest_significance = scaled_sp / sqrt(scaled_bnb_cosmic+scaled_cosmic);
	  isp = sp;
	  lsp = scaled_sp;
	  ibc = bnb_cosmic;
	  lbc = scaled_bnb_cosmic;
	  ic = cosmic;
	  lc = scaled_cosmic;
	} 
      }
      else if(scaled_sp > br_sig) {
	br_sig = scaled_sp;
      }
      
    }

    if(verbose) {
      std::cout << "total - sp: " << total_sp << " scaled: " << total_scaled_sp << "\n"
		<< "        bnb_cosmic: " << total_bnb_cosmic << " scaled: " << total_scaled_bnb_cosmic << "\n"
		<< "        cosmic: " << total_cosmic << " scaled: " << total_scaled_cosmic << "\n"
		<< "after - sp: " << isp << " scaled: " << lsp << "\n"
		<< "        bnb_cosmic: " << ibc << " scaled: " << lbc << "\n"
		<< "        cosmic: " << ic << " scaled: " << lc << "\n"
		<< "seff: " << lsp / total_scaled_sp * 100 << " % beff: " << (lbc + lc) / (total_scaled_bnb_cosmic + total_scaled_cosmic) * 100 << " %\n";
    }
    
    if(br_sig) std::cout << "Largest background eliminated signal: " << br_sig << " efficiency: " << br_sig / total_scaled_sp * 100 << " %\n";
    std::cout << "largest significance: " << largest_significance << " at mva > " << mva << "\n\n";

    vertex_tree_sp->GetListOfFriends()->Delete();
    vertex_tree_bnb_cosmic->GetListOfFriends()->Delete();
    vertex_tree_cosmic->GetListOfFriends()->Delete();

    return largest_significance;

  }
  


  void runtmva_sig_all(std::string const & background = "") {

    std::cout << "runtmva_sig_all\n=============================================\n";

    TFile * file = TFile::Open((method_name+"_app.root").c_str());

    double largest_sig = 0;
    std::string method_str;
    for(method const & m : method_v) {
      double sig = runtmva_sig(m.method_str, background);
      if(sig > largest_sig) {
	largest_sig = sig;
	method_str = m.method_str;
      }
    }

    file->Close();

    std::cout << "Best significance method: " << method_str << " value: " << largest_sig << "\n\n";

  }



  void GetVerticesPerEvent(TTree * vertex_tree, TTree * mva_tree1, std::string const & cut1, double const mva1, TTree * mva_tree2, std::string const & cut2, double const mva2, std::string other_cut = "") {
    
    if(other_cut != "") other_cut = " && " + other_cut;

    TTreeFormula * vertex_tf1 = new TTreeFormula("vertex_tf1", (cut1 + other_cut).c_str(), vertex_tree);
    TTreeFormula * vertex_tf2 = new TTreeFormula("vertex_tf2", (cut2 + other_cut).c_str(), vertex_tree);

    TTreeFormula * mva_tf1 = new TTreeFormula("mva_tf1", ("mva > " + std::to_string(mva1)).c_str(), mva_tree1);
    TTreeFormula * mva_tf2 = new TTreeFormula("mva_tf2", ("mva > " + std::to_string(mva2)).c_str(), mva_tree2);    

    int number_of_events = 0;
    int number_of_vertices = 0;
    
    int event;
    vertex_tree->SetBranchAddress("event_number", &event);
    int previous_event = -1;

    for(int i = 0; i < vertex_tree->GetEntries(); ++i) {

      vertex_tree->GetEntry(i);

      if(vertex_tf1->EvalInstance()) {
	mva_tree1->GetEntry(i);
	if(mva_tf1->EvalInstance()) {
	  ++number_of_vertices;
	  if(event != previous_event) {
	    ++number_of_events;
	    previous_event = event;
	  }
	}
      }

      else if(vertex_tf2->EvalInstance()) {
	mva_tree2->GetEntry(i);
	if(mva_tf2->EvalInstance()) {
	  ++number_of_vertices;
	  if(event != previous_event) {
	    ++number_of_events;
	    previous_event = event;
	  }
	}
      }      

    }

    std::cout << "Events: " << number_of_events << " Vertices: " << number_of_vertices << " Vertices per Event: " << double(number_of_vertices) / number_of_events << "\n";

    delete vertex_tf1;
    delete vertex_tf2;

    delete mva_tf1;
    delete mva_tf2;

  }



  double runtmva_sig(std::string const & all_cut, std::string const & name1, std::string cut1, std::string const & name2, std::string cut2, std::string const & method_str, std::string background = "") {

    if(background != "") background += "_";

    std::cout << "---------------------------------------------\n";

    std::string const fname1 = name1+"_app.root";
    std::string const fname2 = name2+"_app.root";

    std::string const name_sp1 = name1+"_sp_"+background+method_str;
    std::string const name_sp_cosmic1 = name1+"_sp_cosmic_"+background+method_str;
    std::string const name_bnb_cosmic1 = name1+"_bnb_cosmic_"+background+method_str;
    std::string const name_cosmic1 = name1+"_cosmic_"+background+method_str;

    std::string const name_sp2 = name2+"_sp_"+background+method_str;
    std::string const name_sp_cosmic2 = name2+"_sp_cosmic_"+background+method_str;
    std::string const name_bnb_cosmic2 = name2+"_bnb_cosmic_"+background+method_str;
    std::string const name_cosmic2 = name2+"_cosmic_"+background+method_str;

    TFriendElement * tfe_sp1 = vertex_tree_sp->AddFriend(name_sp1.c_str(), fname1.c_str());
    TFriendElement * tfe_sp_cosmic1 = vertex_tree_sp_cosmic->AddFriend(name_sp_cosmic1.c_str(), fname1.c_str());
    TFriendElement * tfe_bnb_cosmic1 =  vertex_tree_bnb_cosmic->AddFriend(name_bnb_cosmic1.c_str(), fname1.c_str());

    TFriendElement * tfe_sp2 = vertex_tree_sp->AddFriend(name_sp2.c_str(), fname2.c_str());
    TFriendElement * tfe_sp_cosmic2 = vertex_tree_sp_cosmic->AddFriend(name_sp_cosmic2.c_str(), fname2.c_str());
    TFriendElement * tfe_bnb_cosmic2 = vertex_tree_bnb_cosmic->AddFriend(name_bnb_cosmic2.c_str(), fname2.c_str());

    int total_sp1 = vertex_tree_sp->GetEntries((signal_definition+"&&"+cut1).c_str());
    int total_sp_cosmic1 = vertex_tree_sp_cosmic->GetEntries((signal_definition+"&&"+cut1).c_str());
    int total_bnb_cosmic1 = vertex_tree_bnb_cosmic->GetEntries(cut1.c_str());
    int total_cosmic1 = 0;

    double total_scaled_sp1 = total_sp1 * run_pot / pot_sp;
    double total_scaled_sp_cosmic1 = total_sp_cosmic1 * run_pot / pot_sp_cosmic;
    double total_scaled_bnb_cosmic1 = total_bnb_cosmic1 * run_pot / pot_bnb_cosmic;
    double total_scaled_cosmic1 = 0;

    int total_sp2 = vertex_tree_sp->GetEntries((signal_definition+"&&"+cut2).c_str());
    int total_sp_cosmic2 = vertex_tree_sp_cosmic->GetEntries((signal_definition+"&&"+cut2).c_str());
    int total_bnb_cosmic2 = vertex_tree_bnb_cosmic->GetEntries(cut2.c_str());
    int total_cosmic2 = 0;

    double total_scaled_sp2 = total_sp2 * run_pot / pot_sp;
    double total_scaled_sp_cosmic2 = total_sp_cosmic2 * run_pot / pot_sp_cosmic;
    double total_scaled_bnb_cosmic2 = total_bnb_cosmic2 * run_pot / pot_bnb_cosmic;
    double total_scaled_cosmic2 = 0;

    TFriendElement * tfe_cosmic1 = nullptr;
    TFriendElement * tfe_cosmic2 = nullptr;

    if(!single_background) {
      tfe_cosmic1 = vertex_tree_cosmic->AddFriend(name_cosmic1.c_str(), fname1.c_str());
      tfe_cosmic2 = vertex_tree_cosmic->AddFriend(name_cosmic2.c_str(), fname2.c_str());    
      total_cosmic1 = vertex_tree_cosmic->GetEntries(cut1.c_str());
      total_scaled_cosmic1 = total_cosmic1 * run_pot * ngenbnbcosmic / ngencosmic * 10.729 / pot_bnb_cosmic;
      total_cosmic2 = vertex_tree_cosmic->GetEntries(cut2.c_str());
      total_scaled_cosmic2 = total_cosmic2 * run_pot * ngenbnbcosmic / ngencosmic * 10.729 / pot_bnb_cosmic;    
    }
    
    cut1 += all_cut;
    cut2 += all_cut;

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

    /*
    double br_sig1 = 0;
    for(double d = -1; d <= 1; d += 0.05) {
    
      int sp = vertex_tree_sp->GetEntries((name_sp1+".mva > "+std::to_string(d)+"&&"+cut1+"&&"+signal_definition).c_str());
      int bnb_cosmic = vertex_tree_bnb_cosmic->GetEntries((name_bnb_cosmic1+".mva > "+std::to_string(d)+"&&"+cut1).c_str());
      int cosmic = 0;
      double scaled_sp = sp * run_pot / pot_sp;
      double scaled_bnb_cosmic =  bnb_cosmic * run_pot / pot_bnb_cosmic;
      double scaled_cosmic = 0;

      if(!single_background) {
	cosmic = vertex_tree_cosmic->GetEntries((name_cosmic1+".mva > "+std::to_string(d)+"&&"+cut1).c_str());
	scaled_cosmic = cosmic * run_pot * ngenbnbcosmic / ngencosmic * 10.729 / pot_bnb_cosmic;	
      }

      if(!(scaled_bnb_cosmic+scaled_cosmic) && scaled_sp > br_sig1) {
	br_sig1 = scaled_sp;
      }
      
    }

    double br_sig2 = 0;
    for(double d = -1; d <= 1; d += 0.05) {
    
      int sp = vertex_tree_sp->GetEntries((name_sp2+".mva > "+std::to_string(d)+"&&"+cut2+"&&"+signal_definition).c_str());
      int bnb_cosmic = vertex_tree_bnb_cosmic->GetEntries((name_bnb_cosmic2+".mva > "+std::to_string(d)+"&&"+cut2).c_str());
      int cosmic = 0;
      double scaled_sp = sp * run_pot / pot_sp;
      double scaled_bnb_cosmic =  bnb_cosmic * run_pot / pot_bnb_cosmic;
      double scaled_cosmic = 0;

      if(!single_background) {
	cosmic = vertex_tree_cosmic->GetEntries((name_cosmic1+".mva > "+std::to_string(d)+"&&"+cut1).c_str());
	scaled_cosmic = cosmic * run_pot * ngenbnbcosmic / ngencosmic * 10.729 / pot_bnb_cosmic;	
      }

      if(!(scaled_bnb_cosmic+scaled_cosmic) && scaled_sp > br_sig2) {
	br_sig2 = scaled_sp;
      }
            
    }
    */

    double mva1 = -2;
    double mva2 = -2;
    double largest_sig_nested = 0;
    for(double d = -1; d <= 1; d += 0.05) {
    
      int sp1 = vertex_tree_sp->GetEntries((name_sp1+".mva > "+std::to_string(d)+"&&"+cut1+"&&"+signal_definition).c_str());
      int bnb_cosmic1 = vertex_tree_bnb_cosmic->GetEntries((name_bnb_cosmic1+".mva > "+std::to_string(d)+"&&"+cut1).c_str());
      int cosmic1 = 0;
      double scaled_sp1 = sp1 * run_pot / pot_sp;
      double scaled_sp_cosmic1 = sp1 * run_pot / pot_sp_cosmic;
      double scaled_bnb_cosmic1 =  bnb_cosmic1 * run_pot / pot_bnb_cosmic;
      double scaled_cosmic1 = 0;

      if(!single_background) {
	cosmic1 = vertex_tree_cosmic->GetEntries((name_cosmic1+".mva > "+std::to_string(d)+"&&"+cut1).c_str());
	scaled_cosmic1 = cosmic1 * run_pot * ngenbnbcosmic / ngencosmic * 10.729 / pot_bnb_cosmic;	
      }

      for(double d2 = -1; d2 <= 1; d2 += 0.05) {
	
	int sp2 = vertex_tree_sp->GetEntries((name_sp2+".mva > "+std::to_string(d2)+"&&"+cut2+"&&"+signal_definition).c_str());
	int bnb_cosmic2 = vertex_tree_bnb_cosmic->GetEntries((name_bnb_cosmic2+".mva > "+std::to_string(d2)+"&&"+cut2).c_str());
	int cosmic2 = 0;
	double scaled_sp2 = sp2 * run_pot / pot_sp;
	double scaled_sp_cosmic2 = sp2 * run_pot / pot_sp_cosmic;
	double scaled_bnb_cosmic2 =  bnb_cosmic2 * run_pot / pot_bnb_cosmic;
	double scaled_cosmic2 = 0;

	if(!single_background) {
	  cosmic2 = vertex_tree_cosmic->GetEntries((name_cosmic2+".mva > "+std::to_string(d2)+"&&"+cut2).c_str());
	  scaled_cosmic2 =  cosmic2 * run_pot * ngenbnbcosmic / ngencosmic * 10.729 / pot_bnb_cosmic;
	}

	double sig = (scaled_sp1+scaled_sp2) / sqrt(scaled_bnb_cosmic1+scaled_cosmic1+scaled_bnb_cosmic2+scaled_cosmic2);

	if(scaled_bnb_cosmic1+scaled_cosmic1+scaled_bnb_cosmic2+scaled_cosmic2) {
	  if(sig > largest_sig_nested) {

	    mva1 = d;
	    mva2 = d2;
	    
	    largest_sig_nested = sig;

	    isp1 = sp1;
	    lsp1 = scaled_sp1;
	    ibc1 = bnb_cosmic1;
	    lbc1 = scaled_bnb_cosmic1;
	    ic1 = cosmic1;
	    lc1 = scaled_cosmic1;

	    isp2 = sp2;
	    lsp2 = scaled_sp2;
	    ibc2 = bnb_cosmic2;
	    lbc2 = scaled_bnb_cosmic2;
	    ic2 = cosmic2;
	    lc2 = scaled_cosmic2;
	  } 
	}
      }
    }

    int ispc1 = vertex_tree_sp_cosmic->GetEntries((name_sp_cosmic1+".mva > "+std::to_string(mva1)+"&&"+cut1+"&&"+signal_definition).c_str());
    int ispc2 = vertex_tree_sp_cosmic->GetEntries((name_sp_cosmic2+".mva > "+std::to_string(mva2)+"&&"+cut2+"&&"+signal_definition).c_str());
    double lspc1 = double(ispc1) * run_pot / pot_sp_cosmic;
    double lspc2 = double(ispc2) * run_pot / pot_sp_cosmic;

    if(verbose) {
      std::cout << "total - sp: " << total_sp1 + total_sp2 << " scaled: " << total_scaled_sp1 + total_scaled_sp2 << "\n"
		<< "        sp_cosmic: " << total_sp_cosmic1 + total_sp_cosmic2 << " scaled: " << total_scaled_sp_cosmic1 + total_scaled_sp_cosmic2 << "\n"
		<< "        bnb_cosmic: " << total_bnb_cosmic1 + total_bnb_cosmic2 << " scaled: " << total_scaled_bnb_cosmic1 + total_scaled_bnb_cosmic2 << "\n";
      if(!single_background) 
	std::cout << "        cosmic: " << total_cosmic1 + total_cosmic2 << " scaled: " << total_scaled_cosmic1 + total_scaled_cosmic2 << "\n";
      if(all_cut != "") {
	int total_sp1c = vertex_tree_sp->GetEntries((signal_definition+"&&"+cut1+all_cut).c_str());
	int total_sp_cosmic1c = vertex_tree_sp_cosmic->GetEntries((signal_definition+"&&"+cut1+all_cut).c_str());
	int total_bnb_cosmic1c = vertex_tree_bnb_cosmic->GetEntries((cut1+all_cut).c_str());
	double total_scaled_sp1c = total_sp1c * run_pot / pot_sp;
	double total_scaled_sp_cosmic1c = total_sp_cosmic1c * run_pot / pot_sp_cosmic;
	double total_scaled_bnb_cosmic1c = total_bnb_cosmic1c * run_pot / pot_bnb_cosmic;
	int total_sp2c = vertex_tree_sp->GetEntries((signal_definition+"&&"+cut2+all_cut).c_str());
	int total_sp_cosmic2c = vertex_tree_sp_cosmic->GetEntries((signal_definition+"&&"+cut2+all_cut).c_str());
	int total_bnb_cosmic2c = vertex_tree_bnb_cosmic->GetEntries((cut2+all_cut).c_str());
	double total_scaled_sp2c = total_sp2c * run_pot / pot_sp;
	double total_scaled_sp_cosmic2c = total_sp_cosmic2c * run_pot / pot_sp_cosmic;
	double total_scaled_bnb_cosmic2c = total_bnb_cosmic2c * run_pot / pot_bnb_cosmic;
	std::cout << "Cut: " << all_cut << "\n"
		  << "applied - sp: " << total_sp1c + total_sp2c << " scaled: " << total_scaled_sp1c + total_scaled_sp2c << " " << double(total_sp1c + total_sp2c) / (total_sp1 + total_sp2) * 100 << " %\n"
		  << "          sp_cosmic: " << total_sp_cosmic1c + total_sp_cosmic2c << " scaled: " << total_scaled_sp_cosmic1c + total_scaled_sp_cosmic2c << " " << double(total_sp_cosmic1c + total_sp_cosmic2c) / (total_sp_cosmic1 + total_sp_cosmic2) * 100 << " %\n"
		  << "          bnb_cosmic: " << total_bnb_cosmic1c + total_bnb_cosmic2c << " scaled: " << total_scaled_bnb_cosmic1c + total_scaled_bnb_cosmic2c << " " << double(total_bnb_cosmic1c + total_bnb_cosmic2c) / (total_bnb_cosmic1 + total_bnb_cosmic2) * 100 << " %\n";
	if(!single_background) {
	  int total_cosmic1c = vertex_tree_cosmic->GetEntries((cut1+all_cut).c_str());
	  double total_scaled_cosmic1c = total_cosmic1c * run_pot * ngenbnbcosmic / ngencosmic * 10.729 / pot_bnb_cosmic;
	  int total_cosmic2c = vertex_tree_cosmic->GetEntries((cut2+all_cut).c_str());
	  double total_scaled_cosmic2c = total_cosmic2c * run_pot * ngenbnbcosmic / ngencosmic * 10.729 / pot_bnb_cosmic;
	  std::cout << "        cosmic: " << total_cosmic1c + total_cosmic2c << " scaled: " << total_scaled_cosmic1c + total_scaled_cosmic2c << "\n";
	}
      }
      std::cout << "after - sp: " << isp1 + isp2 << " scaled: " << lsp1 + lsp2 << " " << double(isp1 + isp2) / (total_sp1 + total_sp2) * 100 << " %\n"
		<< "        sp_cosmic: " << ispc1 + ispc2 << " scaled: " << lspc1 + lspc2 << " " << double(ispc1 + ispc2) / (total_sp_cosmic1 + total_cosmic2) * 100 << " %\n"
		<< "        bnb_cosmic: " << ibc1 + ibc2 << " scaled: " << lbc1 + lbc2 << " " << double(ibc1 + ibc2) / (total_bnb_cosmic1 + total_bnb_cosmic2) * 100 << " %\n";
      if(!single_background)
	std::cout << "        cosmic: " << ic1 + ic2 << " scaled: " << lc1 + lc2 << "\n";
      std::cout << "seff: " << (lsp1+lsp2) / (total_scaled_sp1+total_scaled_sp2) * 100 << " % beff: " << (lbc1 + lbc2 + lc1 + lc2) / (total_scaled_bnb_cosmic1 + total_scaled_bnb_cosmic2 + total_scaled_cosmic1 + total_scaled_cosmic2) * 100 << " %\n";
    }
    //if(br_sig1 + br_sig2) std::cout << "Largest background eliminated signal: " << br_sig1 + br_sig2 << " efficiency: " << (br_sig1 + br_sig2) / (total_scaled_sp1 + total_scaled_sp2) * 100 << " %\n";   

    std::cout << "sp: ";
    GetVerticesPerEvent(vertex_tree_sp, tfe_sp1->GetTree(), cut1, mva1, tfe_sp2->GetTree(), cut2, mva2, signal_definition);
    std::cout << "sp_cosmic: ";
    GetVerticesPerEvent(vertex_tree_sp_cosmic, tfe_sp_cosmic1->GetTree(), cut1, mva1, tfe_sp_cosmic2->GetTree(), cut2, mva2, signal_definition);
    std::cout << "bnb_cosmic: ";
    GetVerticesPerEvent(vertex_tree_bnb_cosmic, tfe_bnb_cosmic1->GetTree(), cut1, mva1, tfe_bnb_cosmic2->GetTree(), cut2, mva2);
    if(!single_background) {
      std::cout << "cosmic: ";
      GetVerticesPerEvent(vertex_tree_cosmic, tfe_cosmic1->GetTree(), cut1, mva1, tfe_cosmic2->GetTree(), cut2, mva2);
    }

    std::cout << "mva1 " << mva1 << " for cut " << cut1 << " and mva2 " << mva2 << " for cut " << cut2 << "\n"
	      << "largest sp significance: " << (lsp1+lsp2) / sqrt(lbc1+lbc2+lc1+lc2) << "\n"
	      << "largest sp_cosmic significance: " << (lspc1+lspc2) / sqrt(lbc1+lbc2+lc1+lc2) << "\n";

    /*
    vertex_tree_sp->GetListOfFriends()->Delete();
    vertex_tree_bnb_cosmic->GetListOfFriends()->Delete();
    vertex_tree_cosmic->GetListOfFriends()->Delete();
    */

    return largest_sig_nested > (lsp1+lsp2) / sqrt(lbc1+lbc2+lc1+lc2) ? largest_sig_nested : (lsp1+lsp2) / sqrt(lbc1+lbc2+lc1+lc2);

  }


  
  void runtmva_sig_all_twofiles(std::string const & all_cut, std::string const & fname1, std::string const & cut1, std::string const & fname2, std::string const & cut2, std::string const & background = "") {
    
    std::cout << "runtmva_sig_all_twofiles\n=============================================\n";

    double largest_sig = 0;
    std::string method_str;
    for(method const & m : method_v) {
      double sig = runtmva_sig(all_cut, fname1, cut1, fname2, cut2, m.method_str, background);
      if(sig > largest_sig) {
	largest_sig = sig;
	method_str = m.method_str;
      }
    }

    std::cout << "Best significance method: " << method_str << " value: " << largest_sig << "\n\n";

  }



};



void run(std::string const & dir, std::string name, std::string const option, double const run_pot, bool const weight, bool const sep = false) {

  if(weight) name += "_weight";
  if(sep) name += "_sepbackgrounds";
  tmva_helper th(name, dir, "passed_swtrigger == 1", run_pot);
  if(sep) th.set_separate_backgrounds();  

  th.add_variable("closest_asso_shower_dist_to_flashzcenter", "d");
  th.add_variable("totalpe_ibg_sum", "d");
  th.add_variable("summed_associated_reco_shower_energy", "d");
  th.add_variable("reco_nu_vtx_dist_to_closest_tpc_wall", "d");
  th.add_variable("shortest_asso_shower_to_vert_dist", "d");
  th.add_variable("most_energetic_shower_reco_thetaxz", "d");
  th.add_variable("most_energetic_shower_reco_thetayz", "d");
  th.add_variable("most_energetic_shower_bp_dist_to_tpc", "d");
  th.add_variable("reco_shower_dedx_plane2", "d");
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

  if(option == "all") {
    if(sep) {
      th.runtmva(weight, "bnb_cosmic");
      th.runtmva(weight, "cosmic");
    }
    else {
      th.runtmva(weight);
    }
    th.runtmva_app();
  }
  else if(option == "app") {
    th.runtmva_app();
  }
  else if(option == "sig") {
    if(sep) {
      std::cout << "BACKGROUND: bnb_cosmic\n"
		<< "===================================================================================\n";
      th.runtmva_sig_all("bnb_cosmic");
      std::cout << "BACKGROUND: cosmic\n"
		<< "===================================================================================\n";
      th.runtmva_sig_all("cosmic");
    }
    else {
      th.runtmva_sig_all();
    }
  }
  else {
    std::cout << "Invalid option\n";
    return;
  }

}



void run_split_track(std::string const & dir, std::string name, std::string const option, double const run_pot, std::string all_cut = "", bool const weight = false, std::string const & background_option = "") {

  if(background_option != "" && background_option != "singlebackground" && background_option != "separatebackgrounds") {
    std::cout << __LINE__ << " " << __PRETTY_FUNCTION__ << "\n"
	      << "ERROR: invalid background_option\n";
    return;
  }

  std::vector<method> methods;
  methods.push_back({TMVA::Types::kBDT, "BDTG",
	"!H:!V:NTrees=1000:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=20:MaxDepth=2"});
  methods.push_back({TMVA::Types::kBDT, "BDT",
	"!H:!V:NTrees=850:MinNodeSize=2.5%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=20"});
  methods.push_back({TMVA::Types::kBDT, "BDTB",
	"!H:!V:NTrees=400:BoostType=Bagging:SeparationType=GiniIndex:nCuts=20"});
  methods.push_back({TMVA::Types::kBDT, "BDTD",
	"!H:!V:NTrees=400:MinNodeSize=5%:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:VarTransform=Decorrelate"});
  methods.push_back({TMVA::Types::kBDT, "BDTF",
	"!H:!V:NTrees=50:MinNodeSize=2.5%:UseFisherCuts:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20"});
  methods.push_back({TMVA::Types::kRuleFit, "RuleFit",
		"H:!V:RuleFitModule=RFTMVA:Model=ModRuleLinear:MinImp=0.001:RuleMinDist=0.001:NTrees=20:fEventsMin=0.01:fEventsMax=0.5:GDTau=-1.0:GDTauPrec=0.01:GDStep=0.01:GDNSteps=10000:GDErrScale=1.02"});

  if(all_cut != "") all_cut = " && " + all_cut + " && passed_swtrigger == 1";
  else all_cut = " && passed_swtrigger == 1";

  std::vector<std::pair<std::string, std::string> > variables;
  variables.emplace_back("closest_asso_shower_dist_to_flashzcenter", "d");
  variables.emplace_back("totalpe_ibg_sum", "d");
  variables.emplace_back("summed_associated_reco_shower_energy", "d");
  //variables.emplace_back("summed_associated_helper_shower_energy", "d");
  variables.emplace_back("reco_nu_vtx_dist_to_closest_tpc_wall", "d");
  variables.emplace_back("most_energetic_shower_reco_thetaxz", "d");
  variables.emplace_back("most_energetic_shower_reco_thetayz", "d");
  variables.emplace_back("most_energetic_shower_bp_dist_to_tpc", "d");
  //variables.emplace_back("reco_shower_dedx_plane0", "d");
  //variables.emplace_back("reco_shower_dedx_plane1", "d");
  variables.emplace_back("reco_shower_dedx_plane2", "d");
  //variables.emplace_back("reco_shower_dedx_best_plane", "d");  

  std::string nameg0 = name+"_g0track";
  if(weight) nameg0 += "_weight";
  if(background_option != "") nameg0 += "_" + background_option;
  std::string const thg0_cut = "reco_asso_tracks > 0";
  tmva_helper thg0(nameg0, dir, thg0_cut + all_cut, run_pot);
  if(background_option == "separatebackgrounds") thg0.set_separate_backgrounds();
  else if(background_option == "singlebackground") thg0.set_single_background();

  for(auto const & p : variables) thg0.add_variable(p.first, p.second);
  thg0.add_variable("shortest_asso_shower_to_vert_dist", "d");
  thg0.add_variable("longest_asso_track_thetaxz", "d");
  thg0.add_variable("longest_asso_track_thetayz", "d");
  thg0.add_variable("reco_asso_tracks", "i");
  thg0.add_variable("longest_asso_track_displacement", "d");
  for(method const & m : methods) thg0.add_method(m);

  std::string name0 = name+"_0track";
  if(weight) name0 += "_weight";
  if(background_option != "") name0 += "_" + background_option;
  std::string const th0_cut = "reco_asso_tracks == 0";
  tmva_helper th0(name0, dir, th0_cut + all_cut, 6.6e20);
  if(background_option == "separatebackgrounds") th0.set_separate_backgrounds(); 
  else if(background_option == "singlebackground") th0.set_single_background(); 

  for(auto const & p : variables) th0.add_variable(p.first, p.second);
  for(method const & m : methods) th0.add_method(m);

  if(option == "all") {
    std::cout << "TRAIN\n";
    if(background_option == "separatebackgrounds") {
      thg0.runtmva(weight, "bnb_cosmic");
      th0.runtmva(weight, "bnb_cosmic");
      thg0.runtmva(weight, "cosmic");
      th0.runtmva(weight, "cosmic");
    }
    else if(background_option == "singlebackground") {
      thg0.runtmva(weight, "bnb_cosmic");
      th0.runtmva(weight, "bnb_cosmic");
    }
    else {
      thg0.runtmva(weight);
      th0.runtmva(weight);
    } 
    std::cout << "APP\n";
    thg0.runtmva_app();
    th0.runtmva_app();
  }
  else if(option == "app") {
    std::cout << "APP\n";
    thg0.runtmva_app();
    th0.runtmva_app();
  }
  else if(option == "sig") {
    std::cout << "SIG\n";
    std::cout << "Names: " << name0 << " " << nameg0 << "\n"
	      << "Split Track\n"
	      << "Background Option: " << background_option << "\n";
    if(background_option == "separatebackgrounds") {
      std::cout << "BACKGROUND: bnb_cosmic\n"
		<< "===================================================================================\n";
      thg0.runtmva_sig_all_twofiles(all_cut, nameg0, thg0_cut, name0, th0_cut, "bnb_cosmic");
      std::cout << "BACKGROUND: cosmic\n"
		<< "===================================================================================\n";
      thg0.runtmva_sig_all_twofiles(all_cut, nameg0, thg0_cut, name0, th0_cut, "cosmic");
    }
    else if(background_option == "singlebackground") {
      thg0.runtmva_sig_all_twofiles(all_cut, nameg0, thg0_cut, name0, th0_cut, "bnb_cosmic");
    }
    else {
      thg0.runtmva_sig_all_twofiles(all_cut, nameg0, thg0_cut, name0, th0_cut);
    }
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
  
  double const run_pot = 6.6e20;
  bool const weight = false; 
  //run_split_track(argv[3], std::string(argv[1]), argv[2], run_pot, "", weight, "singlebackground");
  run_split_track(argv[3], std::string(argv[1]), argv[2], run_pot, "closest_asso_shower_dist_to_flashzcenter <= 40 && totalpe_ibg_sum > 140", weight, "singlebackground");
  //run_split_track(argv[3], std::string(argv[1]), argv[2], run_pot, "", weight);

  return 0;

}
